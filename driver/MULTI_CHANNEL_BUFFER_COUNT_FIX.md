# Multi-Channel Buffer Count Fix

## The Problem

**Multiple frame channels (0, 1, 2, etc.) were overwriting the same `active_buffer_count` field in the shared VIC device!**

### Evidence from Logs

**Channel 0** (main video stream):
- Requests 4 buffers
- Sets `vic->active_buffer_count = 4`

**Channel 1** (secondary stream, e.g., substream):
- Requests 2 buffers  
- **Overwrites** `vic->active_buffer_count = 2` ❌

Result: Channel 0's buffer count (4) gets overwritten by Channel 1's buffer count (2)!

### Log Evidence

```
[  473.965929] *** Channel 1: REQBUFS - MEMORY-AWARE implementation ***
[  473.965936] Channel 1: Request 2 buffers, type=1 memory=2
...
[  473.966105] *** Channel 1: VIC active_buffer_count set to 2 (memory=2) ***
```

## Root Cause

**All channels share the same VIC device structure**, which has only **ONE** `active_buffer_count` field:

```c
struct tx_isp_vic_device {
    ...
    uint32_t active_buffer_count;  /* 0x218: Active buffer count */
    ...
};
```

When Channel 1 calls REQBUFS, it overwrites the value that Channel 0 set!

## The Architecture

### VIC Hardware

- **ONE** VIC hardware block
- **ONE** set of VIC registers (0x133e0000)
- **ONE** VIC register 0x300 that controls buffer ring size
- VIC hardware is controlled by **Channel 0 only**

### Frame Channels

- **Multiple** software channels (0, 1, 2, 3, 4)
- Channel 0: Main video stream (1920x1080, 4 buffers)
- Channel 1: Substream (640x360, 2 buffers)
- Channel 2: JPEG snapshot
- Channels 1-4 are **software-only** - they don't control VIC hardware

### The Confusion

The code was treating `vic->active_buffer_count` as if each channel had its own value, but **there's only ONE field shared by all channels**!

## The Fix

**Only Channel 0 should set `vic->active_buffer_count`!**

### Before (WRONG)

```c
/* REQBUFS handler - ALL channels */
if (ourISPdev && ourISPdev->vic_dev) {
    struct tx_isp_vic_device *vic = ourISPdev->vic_dev;
    vic->active_buffer_count = (reqbuf.count > 5) ? 5 : reqbuf.count;  // ❌ ALL channels overwrite!
    pr_info("*** Channel %d: VIC active_buffer_count set to %d ***\n",
            channel, vic->active_buffer_count);
}

/* Send 0x3000008 event - ONLY Channel 0 */
if (channel == 0 && ourISPdev && ourISPdev->vic_dev) {
    // Send event...
}
```

**Problem**: All channels set `vic->active_buffer_count`, but only Channel 0 sends the 0x3000008 event!

### After (CORRECT)

```c
/* REQBUFS handler - ONLY Channel 0 sets VIC buffer count */
if (channel == 0 && ourISPdev && ourISPdev->vic_dev) {
    struct tx_isp_vic_device *vic = ourISPdev->vic_dev;
    vic->active_buffer_count = (reqbuf.count > 5) ? 5 : reqbuf.count;  // ✅ Only Channel 0!
    pr_info("*** Channel 0: VIC active_buffer_count set to %d ***\n",
            vic->active_buffer_count);
    
    // Send 0x3000008 event...
}
```

**Solution**: Only Channel 0 sets `vic->active_buffer_count` and sends the 0x3000008 event!

## Why This Makes Sense

### Channel 0 (Main Stream)
- Controls VIC hardware
- Sets VIC register 0x300 buffer count
- Receives frames directly from VIC DMA
- Buffer count: 4 (typical for main stream)

### Channel 1-4 (Secondary Streams)
- Software-only processing
- Get frames from Channel 0 via ISP pipeline
- Don't control VIC hardware
- Buffer count: 2 (typical for substreams)
- **Should NOT modify `vic->active_buffer_count`!**

## Expected Behavior After Fix

```
Channel 0 REQBUFS(count=4)
  ↓
vic->active_buffer_count = 4 ✅
  ↓
Send 0x3000008 event to VIC
  ↓
Channel 1 REQBUFS(count=2)
  ↓
vic->active_buffer_count = 4 (unchanged) ✅
  ↓
Channel 0 STREAMON
  ↓
VIC register 0x300 = 0x80040020 (4 buffers) ✅
  ↓
Channel 1 STREAMON
  ↓
VIC register 0x300 = 0x80040020 (still 4 buffers) ✅
```

## Channel State Tracking

Each channel **already has** its own `buffer_count` field in `tx_isp_channel_state`:

```c
struct tx_isp_channel_state {
    ...
    int buffer_count;  /* Per-channel buffer count */
    ...
};
```

This is used for:
- Channel-specific buffer management
- VBM pool allocation
- Software buffer queues

But **VIC hardware** only cares about Channel 0's buffer count!

## Files Modified

- `driver/tx-isp-module.c` lines 2947-2963: Moved `vic->active_buffer_count` assignment inside the `if (channel == 0)` block

## Success Criteria

✅ Channel 0 sets `vic->active_buffer_count = 4`
✅ Channel 1 does NOT modify `vic->active_buffer_count`
✅ `vic->active_buffer_count` remains 4 throughout streaming
✅ VIC register 0x300 always written with Channel 0's buffer count
✅ No control limit errors
✅ Continuous interrupts without stalls

## Related Fixes

This fix complements the previous fix that removed the decrement in the IRQ handler:

1. ✅ **IRQ handler**: Don't decrement `active_buffer_count` (previous fix)
2. ✅ **REQBUFS**: Only Channel 0 sets `active_buffer_count` (this fix)
3. ✅ **QBUF**: Don't increment `active_buffer_count` (previous fix)
4. ✅ **PIPO**: Don't reset `active_buffer_count` (previous fix)
5. ✅ **TX_ISP_SET_BUF**: Don't modify `active_buffer_count` (previous fix)

Now `active_buffer_count` is truly a **constant value** set once by Channel 0's REQBUFS!

