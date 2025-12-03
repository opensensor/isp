# VIC Core S_Stream Type Mismatch Fix

## Problem

The system was crashing (kernel panic with speaker beep) when streaming started, likely in the `vic_core_s_stream` function.

## Root Cause

**CRITICAL TYPE MISMATCH** in `vic_core_s_stream` function at line 2731:

```c
ret = ispvic_frame_channel_s_stream(sd, 1);  // WRONG! Passing subdev pointer
```

But the function signature expects a `vic_dev` pointer:

```c
int ispvic_frame_channel_s_stream(struct tx_isp_vic_device *vic_dev, int enable)
```

### What Was Happening

1. `vic_core_s_stream` is called with a `tx_isp_subdev *sd` pointer
2. It correctly gets `vic_dev` from the subdev at line 2666:
   ```c
   vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdev_hostdata(sd);
   ```
3. But then at line 2731, it calls `ispvic_frame_channel_s_stream(sd, 1)` - **passing the wrong pointer!**
4. `ispvic_frame_channel_s_stream` interprets the `sd` pointer as a `vic_dev` pointer
5. When it tries to access `vic_dev->stream_state`, `vic_dev->vic_regs`, etc., it's actually accessing:
   - Wrong structure offsets
   - Invalid memory locations
   - NULL pointers
6. **Result: Kernel panic** 💥

### Memory Layout Mismatch

```
struct tx_isp_subdev {
    // offset 0x00: different members
    // offset 0x210: NOT stream_state!
    ...
}

struct tx_isp_vic_device {
    struct tx_isp_subdev sd;  // offset 0x00
    ...
    int stream_state;         // offset 0x210
    void __iomem *vic_regs;   // offset 0xb8
    ...
}
```

When `ispvic_frame_channel_s_stream` receives an `sd` pointer instead of `vic_dev`:
- `vic_dev->stream_state` reads from wrong offset in `sd` structure
- `vic_dev->vic_regs` reads garbage or NULL
- `vic_dev->buffer_mgmt_lock` accesses invalid memory
- **Crash when trying to use these values**

## Solution

Changed line 2731 to pass the correct pointer type:

**Before:**
```c
printk(KERN_ALERT ENABLE) to start MDMA before enabling IRQ ***\n");
ret = ispvic_frame_channel_s_stream(sd, 1);  // WRONG TYPE!
if (ret != 0) {
    printk(KERN_ALERT "*** vic_core_s_stream: ispvic_frame_channel_s_stream FAILED: %d ***\n", ret);
    return ret;
}
```

**After:**
```c
printk(KERN_ALERT ENABLE) to start MDMA before enabling IRQ ***\n");
/* CRITICAL FIX: Pass vic_dev, not sd! Function signature expects vic_dev pointer */
ret = ispvic_frame_channel_s_stream(vic_dev, 1);
if (ret != 0) {
    printk(KERN_ALERT "*** vic_core_s_stream: ispvic_frame_channel_s_stream FAILED: %d ***\n", ret);
    return ret;
}
```

### Why This Fixes the Crash

1. `vic_dev` is the correct pointer type that `ispvic_frame_channel_s_stream` expects
2. `vic_dev` was already retrieved correctly at line 2666
3. Now `ispvic_frame_channel_s_stream` can access:
   - `vic_dev->stream_state` at correct offset
   - `vic_dev->vic_regs` at correct offset
   - `vic_dev->buffer_mgmt_lock` at correct offset
4. No more invalid memory access
5. No more kernel panic

## Verification

Checked all other calls to `ispvic_frame_channel_s_stream`:

✅ **tx-isp-module.c line 3330**: `ispvic_frame_channel_s_stream(vic, 1)` - Correct (vic is vic_dev)
✅ **tx-isp-module.c line 6372**: `ispvic_frame_channel_s_stream(vic_dev, 1)` - Correct
✅ **tx_isp_vic.c line 2732**: `ispvic_frame_channel_s_stream(vic_dev, 1)` - **FIXED**

All other calls are now using the correct pointer type.

## Impact

This was likely the **primary cause** of the kernel panic when streaming started:

1. Workqueue initialization was fixed (no longer NULL)
2. Work function pointer validation was added
3. **But the actual crash was from this type mismatch** when `vic_core_s_stream` called `ispvic_frame_channel_s_stream`

The crash sequence was probably:
1. Frame channel STREAMON called
2. `vic_core_s_stream` called
3. `ispvic_frame_channel_s_stream(sd, 1)` called with wrong type
4. Function tries to access `vic_dev->vic_regs` but gets garbage
5. `readl(garbage_pointer + 0x300)` causes bus error
6. **Kernel panic with speaker beep** 🔊

## Files Modified

- `driver/tx_isp_vic.c`:
  - Line 2732: Changed `ispvic_frame_channel_s_stream(sd, 1)` to `ispvic_frame_channel_s_stream(vic_dev, 1)`

## Testing

After this fix:
1. Streaming should start without kernel panic
2. VIC MDMA should be configured correctly
3. Frame interrupts should fire properly
4. No more speaker beep crashes

