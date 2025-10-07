# Missing STREAMON Event - Root Cause of Control Limit Error

## The Problem

**VIC register 0x300 was NEVER being written with the buffer count!**

This caused:
- VIC[0x300] = 0x00000000 (0 buffers configured)
- VIC hardware expected buffers but got none
- Control limit error (bit 21) after 5 interrupts
- 5/5 interrupt stall

## Root Cause

**STREAMON was NOT sending the 0x3000003 event!**

The 0x3000003 event (TX_ISP_EVENT_FRAME_STREAMON) triggers `ispvic_frame_channel_s_stream`, which:
1. Calls `vic_pipo_mdma_enable` (MDMA configuration)
2. Writes VIC[0x300] = (buffer_count << 16) | 0x80000020

Without this event, VIC[0x300] was never written, so VIC thought it had 0 buffers!

## Binary Ninja Analysis

**Reference driver's STREAMON handler:**

```c
*($s0 + 0x230) |= 1  // Set streaming flag
tx_isp_send_event_to_remote(*($s0 + 0x2bc), 0x3000003, 0)  // Send STREAMON event
*($s0 + 0x2d0) = 4  // Set state to streaming
```

**Key finding:** The reference driver sends 0x3000003 event during STREAMON!

## The Fix

**Added 0x3000003 event to STREAMON handler:**

```c
// Binary Ninja EXACT: Send 0x3000003 event (TX_ISP_EVENT_FRAME_STREAMON)
if (ourISPdev && ourISPdev->vic_dev) {
    struct tx_isp_vic_device *vic_dev = ourISPdev->vic_dev;
    
    pr_info("*** Channel %d: STREAMON - Sending 0x3000003 event (Binary Ninja EXACT) ***\n", channel);
    
    // Send event to frame channel's subdev (routes to VIC via pipe callback)
    if (fcd->subdev) {
        event_result = tx_isp_send_event_to_remote(fcd->subdev, 0x3000003, NULL);
    } else {
        // Fallback: send directly to VIC subdev
        event_result = tx_isp_send_event_to_remote(&vic_dev->sd, 0x3000003, NULL);
    }
    
    if (event_result == 0xfffffdfd) {
        // Fallback: Call ispvic_frame_channel_s_stream directly
        ispvic_frame_channel_s_stream(&vic_dev->sd, 1);
    }
}
```

## Event Flow

### Before Fix (BROKEN):
```
STREAMON
  ↓
tx_isp_video_s_stream(1)
  ↓
vic_core_s_stream
  ↓
tx_isp_vic_start (VIC config only)
  ↓
VIC[0x300] = 0x00000000 (never written!) ❌
  ↓
Control limit error after 5 interrupts ❌
```

### After Fix (CORRECT):
```
STREAMON
  ↓
Send 0x3000003 event
  ↓
Pipe callback → ispvic_frame_channel_s_stream
  ↓
vic_pipo_mdma_enable (MDMA config)
  ↓
Write VIC[0x300] = 0x80040020 (4 buffers) ✅
  ↓
tx_isp_video_s_stream(1)
  ↓
vic_core_s_stream
  ↓
tx_isp_vic_start (VIC config)
  ↓
VIC starts streaming with 4 buffers ✅
```

## Why This Was Missed

1. **Previous fixes** removed the direct call to `ispvic_frame_channel_s_stream` from `vic_core_s_stream` (which was correct!)
2. **But forgot** to add the 0x3000003 event to STREAMON (which triggers it via pipe callback)
3. **Result**: `ispvic_frame_channel_s_stream` was never called at all!

## Files Modified

- **driver/tx-isp-module.c** lines 3863-3894: Added 0x3000003 event to STREAMON handler

## Expected Behavior

### Logs:
```
Channel 0: STREAMON - Sending 0x3000003 event (Binary Ninja EXACT)
Channel 0: STREAMON - 0x3000003 event SUCCESS (ispvic_frame_channel_s_stream called)
VIC MDMA: Using active_buffer_count=4 from REQBUFS
Binary Ninja EXACT: Wrote 0x80040020 to reg 0x300 (4 buffers)
```

### VIC Registers:
```
VIC[0x300] = 0x80040020  (4 buffers, streaming enabled)
VIC[0x318] = 0x6300000   (buffer 0 address)
VIC[0x31c] = 0x65f7600   (buffer 1 address)
VIC[0x320] = 0x68eec00   (buffer 2 address)
VIC[0x324] = 0x6be6200   (buffer 3 address)
```

### Result:
✅ VIC configured with 4 buffers
✅ No control limit errors
✅ Continuous interrupts
✅ Video streaming works!

## Success Criteria

✅ STREAMON sends 0x3000003 event
✅ `ispvic_frame_channel_s_stream` is called
✅ VIC[0x300] = 0x80040020 (4 buffers)
✅ No control limit errors
✅ Continuous interrupts without stalls
✅ Video frames captured successfully

This was the missing piece! 🎉

