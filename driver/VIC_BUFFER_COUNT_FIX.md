# VIC Buffer Count Fix - Root Cause Analysis

## Problem

VIC is configured for **1 buffer** (`active_buffer_count=1`) but prudynt is queueing **5 buffers** via QBUF, causing control limit errors.

## Root Cause

Prudynt uses a **newer allocation strategy**:

1. **Does NOT call REQBUFS** - so VBM pool is never created via REQBUFS
2. **Calls TX_ISP_SET_BUF once** - sets `vbm_buffer_count=1`
3. **Calls QBUF 5 times** - queues 5 buffers to VIC
4. **VIC gets configured for 1 buffer** - based on `vbm_buffer_count=1`
5. **Control limit error** - VIC tries to access buffer indices 1-4 that don't exist in hardware config

## The Flow

### What Happens:
```
prudynt: IMP_System_Init()
    ↓
libimp.so: Allocates 5 buffers internally
    ↓
prudynt: TX_ISP_SET_BUF(buffer[0]) - called ONCE
    ↓
Driver: vbm_buffer_count = 1
    ↓
prudynt: QBUF(buffer[0])
prudynt: QBUF(buffer[1])
prudynt: QBUF(buffer[2])
prudynt: QBUF(buffer[3])
prudynt: QBUF(buffer[4])
    ↓
Driver: 5 buffers queued to VIC
    ↓
prudynt: STREAMON
    ↓
Driver: active_buffer_count = vbm_buffer_count = 1 ❌
    ↓
Driver: Writes (1 << 16) | 0x80000020 to VIC[0x300] ❌
    ↓
VIC: Configured for 1 buffer but receives 5 buffers
    ↓
VIC: Control limit error! ❌
```

### What Should Happen:
```
prudynt: TX_ISP_SET_BUF(buffer[0])
    ↓
prudynt: QBUF(buffer[0]) - count buffers here!
prudynt: QBUF(buffer[1]) - count buffers here!
prudynt: QBUF(buffer[2]) - count buffers here!
prudynt: QBUF(buffer[3]) - count buffers here!
prudynt: QBUF(buffer[4]) - count buffers here!
    ↓
Driver: Track actual queued buffer count = 5
    ↓
prudynt: STREAMON
    ↓
Driver: active_buffer_count = actual_queued_count = 5 ✅
    ↓
Driver: Send 0x3000008 event to VIC with buffer_count=5 ✅
    ↓
Driver: Writes (5 << 16) | 0x80000020 to VIC[0x300] ✅
    ↓
VIC: Configured for 5 buffers, receives 5 buffers ✅
    ↓
VIC: No control limit error! ✅
```

## The Fix

### Option 1: Count Buffers During QBUF (BEST)

Track the maximum buffer index seen during QBUF calls, then use that as the buffer count during STREAMON.

**File: `driver/tx-isp-module.c`**

In QBUF handler, track max buffer index:
```c
/* Track maximum buffer index for buffer count calculation */
if (buffer.index >= state->max_buffer_index) {
    state->max_buffer_index = buffer.index;
    state->actual_buffer_count = buffer.index + 1;
}
```

In STREAMON handler, use actual buffer count:
```c
/* Use actual queued buffer count, not vbm_buffer_count */
int actual_count = st->actual_buffer_count;
if (actual_count == 0) actual_count = st->vbm_buffer_count;
if (actual_count == 0) actual_count = 2; /* Fallback */

ourISPdev->vic_dev->active_buffer_count = (actual_count > 5) ? 5 : actual_count;
```

### Option 2: Use VIC's Queued Buffer Count (SIMPLER)

The VIC already tracks how many buffers have been queued via `ispvic_frame_channel_qbuf`. We can use that count.

**File: `driver/tx_isp_vic.c`**

In `ispvic_frame_channel_qbuf`:
```c
/* Already increments active_buffer_count */
vic_dev->active_buffer_count++;
```

In STREAMON, VIC already has the correct count!

## Current Status

**Added 0x3000008 event call in STREAMON** (line 3845-3859):
```c
/* CRITICAL: Send 0x3000008 event to VIC with buffer count (Binary Ninja EXACT) */
int32_t buffer_count_for_vic = ourISPdev->vic_dev->active_buffer_count;
pr_info("*** STREAMON: Sending 0x3000008 event to VIC with buffer_count=%d ***\n", 
        buffer_count_for_vic);

int event_result = tx_isp_send_event_to_remote(&ourISPdev->vic_dev->sd, 0x3000008, &buffer_count_for_vic);
```

**But** `active_buffer_count` is still based on `vbm_buffer_count=1` from SET_BUF, not the actual 5 buffers queued via QBUF.

## Next Step

Need to update STREAMON to use the **actual number of buffers queued** instead of `vbm_buffer_count`.

The VIC's `active_buffer_count` is incremented in `ispvic_frame_channel_qbuf`, so by the time STREAMON is called, it should already be 5!

Let me check if that's the case by looking at the VIC qbuf logs...

Actually, looking at the logs:
```
ispvic_frame_channel_qbuf: Incremented active_buffer_count to 1
```

This shows VIC is incrementing the count, but it's only showing "to 1" which means it's being reset somewhere or only 1 buffer is being queued before STREAMON.

The issue is that **STREAMON is being called BEFORE all 5 buffers are queued**!

## Real Root Cause

Looking at the flow more carefully:

1. prudynt calls QBUF(buffer[0]) - VIC active_buffer_count = 1
2. prudynt calls STREAMON - VIC configured for 1 buffer
3. prudynt calls QBUF(buffer[1-4]) - But VIC already configured!

So the buffers are being queued **AFTER** streaming starts, which means the VIC is already configured with the wrong buffer count.

## Real Fix

We need to **defer the VIC hardware configuration** until we know the final buffer count. This means:

1. QBUF should queue buffers but NOT configure VIC hardware
2. STREAMON should count all queued buffers and THEN configure VIC
3. VIC register 0x300 should be written in STREAMON, not in individual QBUF calls

This matches the Binary Ninja reference driver behavior!

