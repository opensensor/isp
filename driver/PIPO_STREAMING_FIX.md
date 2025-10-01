# CRITICAL FIX: tx_isp_subdev_pipo Calling vic_core_s_stream Repeatedly

## Root Cause of 6/6 Stall

After fixing the VIC state reset issue, we discovered that `tx_isp_vic_start` was still being called 5 times instead of once. Analysis of the logs revealed:

```
[  142.759597] STREAM ON  (1st - initial start)
[  143.534612] STREAM ON  (2nd - 0.775s later) ← Why?
[  145.093403] STREAM ON  (3rd - 1.559s later) ← Why?
[  145.093845] STREAM ON  (4th - 0.0004s later) ← Why?
[  145.149705] STREAM OFF (1st - expected)
[  145.149866] STREAM ON  (5th - expected after OFF)
[  146.266823] STREAM ON  (6th - 1.117s later) ← Why?
```

**`vic_core_s_stream(sd, 1)` was being called MULTIPLE times without any STREAM OFF in between!**

## The Bug

Looking at the call chain, we found:

```
tx_isp_subdev_pipo (line 4239)
  └─> vic_core_s_stream(sd, 1)  ← WRONG! Not in Binary Ninja reference!
      └─> tx_isp_vic_start (if state != 4)
          └─> 40ms delay (VIC unlock timeout)
```

**`tx_isp_subdev_pipo` was calling `vic_core_s_stream(sd, 1)` which was NOT in the Binary Ninja reference!**

This was added as a "fix" but it caused:
1. `tx_isp_subdev_pipo` to be called multiple times (from various IOCTLs)
2. Each call triggered `vic_core_s_stream(sd, 1)`
3. Each call to `vic_core_s_stream` called `tx_isp_vic_start` (if state != 4)
4. Multiple VIC starts → multiple 40ms delays → stall

## Binary Ninja Reference

Decompiled `tx_isp_subdev_pipo` from Binary Ninja MCP:

```c
void* $s0 = nullptr

if (arg1 != 0 && arg1 u< 0xfffff001)
    $s0 = *(arg1 + 0xd4)

*($s0 + 0x20c) = 1
raw_pipe = arg2

if (arg2 == 0)
    *($s0 + 0x214) = 0
else
    *($s0 + 0x204) = $s0 + 0x204  // INIT_LIST_HEAD
    *($s0 + 0x208) = $s0 + 0x204
    *($s0 + 0x1f4) = $s0 + 0x1f4
    *($s0 + 0x1f8) = $s0 + 0x1f4
    *($s0 + 0x1fc) = $s0 + 0x1fc
    *($s0 + 0x200) = $s0 + 0x1fc
    private_spin_lock_init()
    *raw_pipe = 0xbd4              // ispvic_frame_channel_qbuf
    *(raw_pipe + 8) = 0xd04         // ispvic_frame_channel_clearbuf
    *(raw_pipe + 0xc) = ispvic_frame_channel_s_stream
    *(raw_pipe + 0x10) = arg1
    
    // Initialize 5 buffer structures
    for (int i = 0; i < 5; i++) {
        // Add buffer to free list
        // Clear VIC register at offset (i + 0xc6) << 2
    }
    
    *($s0 + 0x214) = 1

return 0
```

**KEY OBSERVATION: The Binary Ninja reference does NOT call:**
- `vic_core_s_stream`
- `ispvic_frame_channel_s_stream`
- `ispvic_frame_channel_qbuf`

**It ONLY:**
1. Sets up function pointers in raw_pipe structure
2. Initializes buffer structures
3. Sets processing flag to 1

## The Fix

### Before (WRONG):

```c
/* tx_isp_subdev_pipo - lines 4232-4249 */
pr_info("*** tx_isp_subdev_pipo: CALLING ispvic_frame_channel_s_stream to start VIC streaming ***\n");
int stream_ret = ispvic_frame_channel_s_stream(sd, 1);
if (stream_ret == 0) {
    pr_info("*** tx_isp_subdev_pipo: CALLING vic_core_s_stream to enable VIC interrupts ***\n");
    stream_ret = vic_core_s_stream(sd, 1);  // ← WRONG! Not in Binary Ninja!
}
```

### After (CORRECT - Binary Ninja EXACT):

```c
/* tx_isp_subdev_pipo - lines 4214-4221 */
/* Binary Ninja EXACT: *($s0 + 0x214) = 1 */
vic_dev->processing = 1;
pr_info("tx_isp_subdev_pipo: set processing = 1 (Binary Ninja EXACT - offset 0x214)\n");

/* Binary Ninja EXACT: The reference does NOT call any streaming functions! */
/* NO vic_core_s_stream, NO ispvic_frame_channel_s_stream, NO ispvic_frame_channel_qbuf */
/* Streaming is started later by tx_isp_video_link_stream when userspace calls STREAM_ON */
pr_info("*** tx_isp_subdev_pipo: Binary Ninja EXACT - only buffer setup, no streaming calls ***\n");
```

## Why This Fixes the Issue

### Before Fix:
1. Userspace calls `TX_ISP_VIDEO_LINK_SETUP` → calls `tx_isp_subdev_pipo`
2. `tx_isp_subdev_pipo` calls `vic_core_s_stream(sd, 1)`
3. `vic_core_s_stream` calls `tx_isp_vic_start` (if state != 4)
4. VIC unlock times out (40ms delay)
5. Userspace calls another IOCTL → calls `tx_isp_subdev_pipo` again
6. Repeat steps 2-4 multiple times
7. After 5-6 calls: system stalls

### After Fix:
1. Userspace calls `TX_ISP_VIDEO_LINK_SETUP` → calls `tx_isp_subdev_pipo`
2. `tx_isp_subdev_pipo` only sets up buffers and function pointers
3. No streaming calls, no VIC start
4. Userspace calls `TX_ISP_VIDEO_LINK_STREAM_ON` → calls `tx_isp_video_link_stream(1)`
5. `tx_isp_video_link_stream` calls `vic_core_s_stream(sd, 1)` ONCE
6. `vic_core_s_stream` calls `tx_isp_vic_start` ONCE
7. VIC starts properly, interrupts continue

## Call Flow (Correct)

```
Userspace IOCTL 0x800456d0 (TX_ISP_VIDEO_LINK_SETUP)
  └─> tx_isp_subdev_pipo(sd, raw_pipe)
      └─> Set up function pointers
      └─> Initialize buffer structures
      └─> Set processing = 1
      └─> DONE (no streaming calls)

Userspace IOCTL 0x800456d2 (TX_ISP_VIDEO_LINK_STREAM_ON)
  └─> tx_isp_video_link_stream(isp_dev, 1)
      └─> Loop through all subdevs
          └─> Call subdev->ops->video->link_stream(subdev, 1)
              └─> vic_core_s_stream(sd, 1)
                  └─> if (state != 4) tx_isp_vic_start()  ← Called ONCE!
```

## Expected Behavior After Fix

```bash
dmesg | grep "tx_isp_vic_start: Following EXACT Binary Ninja flow"
# Should show ONLY ONE call during initial setup

dmesg | grep "vic_core_s_stream: STREAM ON"
# Should show:
# - 1st call: Initial setup (state 2 → 4)
# - 2nd call: After OFF/ON reconfiguration (state 3 → 4)
# - 3rd+ calls: State already 4, no action needed

dmesg | grep -c "VIC IRQ: vic_start_ok=1"
# Should show MORE than 10 interrupts (no stall!)
```

## Files Modified

- `driver/tx_isp_vic.c` - Lines 4139-4143: Set processing = 1 (Binary Ninja EXACT)
- `driver/tx_isp_vic.c` - Lines 4214-4221: Remove streaming calls from tx_isp_subdev_pipo

## Summary

The 6/6 stall was caused by `tx_isp_subdev_pipo` calling `vic_core_s_stream` which was not in the Binary Ninja reference. This caused VIC to be restarted multiple times (once per IOCTL that called `tx_isp_subdev_pipo`), leading to repeated 40ms delays and eventual stall.

The fix removes all streaming calls from `tx_isp_subdev_pipo` to match the Binary Ninja reference exactly. Streaming is now started only by `tx_isp_video_link_stream` when userspace explicitly calls STREAM_ON, ensuring VIC is started exactly once.

