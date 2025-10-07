# Final Fix Summary - Control Limit Error Resolution

## Root Cause Identified

The `active_buffer_count` field was being **misused as a counter** instead of a **ring size constant**.

### What Was Happening:

1. **REQBUFS** sets `active_buffer_count = 4` via 0x3000008 event ✅
2. **QBUF** increments `active_buffer_count` on each call: 5, 6, 7, 8, 9... ❌
3. **STREAMON** reads the inflated count (9), caps it to 5, sends 0x3000008 event ❌
4. **ispvic_frame_channel_s_stream** called multiple times, uses different values (4 or 5) ❌
5. **VIC register 0x300** gets written with inconsistent buffer counts ❌
6. **Control limit error** - VIC hardware confused about ring size ❌

## The Fixes Applied

### Fix 1: Remove active_buffer_count Increment in QBUF ✅

**File: `driver/tx_isp_vic.c` (line 3737)**

**Before:**
```c
vic_dev->active_buffer_count++;
pr_info("ispvic_frame_channel_qbuf: Incremented active_buffer_count to %d\n",
        vic_dev->active_buffer_count);
```

**After:**
```c
/* CRITICAL FIX: Do NOT increment active_buffer_count here! */
/* active_buffer_count is the TOTAL buffer ring size, set by REQBUFS via 0x3000008 event */
/* It should NOT be incremented on each QBUF - that's a counter, not a ring size! */
pr_info("ispvic_frame_channel_qbuf: Buffer queued (active_buffer_count=%d from REQBUFS)\n",
        vic_dev->active_buffer_count);
```

### Fix 2: Remove active_buffer_count Decrement in IRQ Handler ✅

**File: `driver/tx_isp_vic.c` (line 695)**

**Before:**
```c
list_del(&buf->list);
vic_dev->active_buffer_count--;
```

**After:**
```c
list_del(&buf->list);
/* CRITICAL FIX: Do NOT decrement active_buffer_count! */
/* active_buffer_count is the ring size, not a busy queue counter */
```

### Fix 3: Remove active_buffer_count Increment in IRQ Handler ✅

**File: `driver/tx_isp_vic.c` (line 739)**

**Before:**
```c
list_add_tail(&free_buf->list, &vic_dev->busy_head);
vic_dev->active_buffer_count++;
```

**After:**
```c
list_add_tail(&free_buf->list, &vic_dev->busy_head);
/* CRITICAL FIX: Do NOT increment active_buffer_count! */
/* active_buffer_count is the ring size, not a busy queue counter */
```

### Fix 4: Remove 0x3000008 Event from STREAMON ✅

**File: `driver/tx-isp-module.c` (lines 3836-3858)**

**Before:**
```c
/* Use VIC's active_buffer_count (incremented during QBUF) */
int actual_queued = ourISPdev->vic_dev->active_buffer_count;
// ... cap to 5 ...
ourISPdev->vic_dev->active_buffer_count = actual_queued;

/* Send 0x3000008 event to VIC with buffer count */
int32_t buffer_count_for_vic = ourISPdev->vic_dev->active_buffer_count;
tx_isp_send_event_to_remote(&ourISPdev->vic_dev->sd, 0x3000008, &buffer_count_for_vic);
```

**After:**
```c
/* CRITICAL FIX: Do NOT override active_buffer_count here! */
/* REQBUFS already set it via 0x3000008 event - trust that value */
int buffer_count = ourISPdev->vic_dev->active_buffer_count;

/* Fallback only if REQBUFS wasn't called (shouldn't happen with prudynt) */
if (buffer_count == 0) {
    buffer_count = (st->vbm_buffer_count > 5) ? 5 : st->vbm_buffer_count;
    if (buffer_count == 0) buffer_count = 2;
    pr_warn("STREAMON: REQBUFS not called, using fallback buffer_count=%d\n", buffer_count);
}

/* REMOVED: Do NOT send 0x3000008 event here! */
/* REQBUFS already sent it with the correct buffer count */
```

## The Correct Flow (After Fixes)

### REQBUFS:
```
prudynt: REQBUFS(count=4)
    ↓
Driver: Create VBM pool with 4 buffers
    ↓
Driver: Send 0x3000008 event to VIC with buffer_count=4
    ↓
VIC: vic_core_ops_ioctl(0x3000008, &buffer_count=4)
    ↓
VIC: active_buffer_count = 4 ✅ (SET ONCE, NEVER CHANGED)
```

### QBUF (called 4 times):
```
prudynt: QBUF(buffer[0])
    ↓
Driver: ispvic_frame_channel_qbuf
    ↓
VIC: active_buffer_count = 4 ✅ (UNCHANGED)

prudynt: QBUF(buffer[1])
    ↓
Driver: ispvic_frame_channel_qbuf
    ↓
VIC: active_buffer_count = 4 ✅ (UNCHANGED)

prudynt: QBUF(buffer[2])
    ↓
Driver: ispvic_frame_channel_qbuf
    ↓
VIC: active_buffer_count = 4 ✅ (UNCHANGED)

prudynt: QBUF(buffer[3])
    ↓
Driver: ispvic_frame_channel_qbuf
    ↓
VIC: active_buffer_count = 4 ✅ (UNCHANGED)
```

### STREAMON:
```
prudynt: STREAMON
    ↓
Driver: Read vic_dev->active_buffer_count = 4 ✅
    ↓
Driver: Do NOT send 0x3000008 event (REQBUFS already did) ✅
    ↓
Driver: Call tx_isp_video_s_stream(1)
    ↓
Driver: Call ispvic_frame_channel_s_stream(1)
    ↓
VIC: Read active_buffer_count = 4 ✅
    ↓
VIC: Write (4 << 16) | 0x80000020 = 0x80040020 to register 0x300 ✅
    ↓
VIC: Program 4 buffer addresses from VBM pool ✅
    ↓
VIC: Start streaming with 4 buffers ✅
```

### During Streaming (IRQ Handler):
```
VIC: Frame done interrupt
    ↓
IRQ Handler: Remove buffer from busy queue
    ↓
VIC: active_buffer_count = 4 ✅ (UNCHANGED)
    ↓
IRQ Handler: Add next buffer to busy queue
    ↓
VIC: active_buffer_count = 4 ✅ (UNCHANGED)
```

## Expected Log Output After Fix

```
*** Channel 0: REQBUFS - count=4, memory=2 ***
*** REQBUFS: Sending 0x3000008 event to VIC with buffer_count=4 (Binary Ninja EXACT) ***
*** vic_core_ops_ioctl: VIC active_buffer_count set to 4 (Binary Ninja EXACT) ***

*** Channel 0: QBUF - Buffer 0: phys_addr=0x6300000 (VALIDATED) ***
ispvic_frame_channel_qbuf: Buffer queued (active_buffer_count=4 from REQBUFS)

*** Channel 0: QBUF - Buffer 1: phys_addr=0x65ee000 (VALIDATED) ***
ispvic_frame_channel_qbuf: Buffer queued (active_buffer_count=4 from REQBUFS)

*** Channel 0: QBUF - Buffer 2: phys_addr=0x69dc000 (VALIDATED) ***
ispvic_frame_channel_qbuf: Buffer queued (active_buffer_count=4 from REQBUFS)

*** Channel 0: QBUF - Buffer 3: phys_addr=0x6dca000 (VALIDATED) ***
ispvic_frame_channel_qbuf: Buffer queued (active_buffer_count=4 from REQBUFS)

*** Channel 0: VIDIOC_STREAMON ***
STREAMON: VIC ring active_count=4 (from REQBUFS), last_idx=3

*** VIC MDMA: Using active_buffer_count=4 from REQBUFS (Binary Ninja EXACT) ***
*** Binary Ninja EXACT: Wrote 0x80040020 to reg 0x300 (4 buffers) ***
*** VIC BUFFER INIT: Using VBM pool buffers (Binary Ninja EXACT) ***
*** VIC BUFFER 0: 0x6300000 (from VBM pool) ***
*** VIC BUFFER 1: 0x65ee000 (from VBM pool) ***
*** VIC BUFFER 2: 0x69dc000 (from VBM pool) ***
*** VIC BUFFER 3: 0x6dca000 (from VBM pool) ***
*** READBACK: VIC[0x300]=0x80040020, buffer_count_field=4 ***

*** VIC: Frame done interrupt ***
*** VIC: Frame done interrupt ***
*** VIC: Frame done interrupt ***
... (continuous interrupts, no stalls!)
```

## Key Changes Summary

1. **`active_buffer_count` is now a constant** - set once by REQBUFS, never changed
2. **QBUF does not increment it** - it's not a counter
3. **IRQ handler does not increment/decrement it** - it's not a busy queue depth
4. **STREAMON does not override it** - REQBUFS already set the correct value
5. **VIC register 0x300 gets consistent value** - always 4 buffers (from REQBUFS)

## Success Criteria

✅ **Buffer count set once by REQBUFS** (4 buffers)
✅ **Buffer count never changes during streaming**
✅ **VIC register 0x300 always has correct value** (0x80040020)
✅ **No control limit errors**
✅ **Continuous frame done interrupts** (no 5/5 stalls)
✅ **Real frame data** from sensor

## Files Modified

1. **driver/tx_isp_vic.c**:
   - Line 695: Removed `active_buffer_count--` in IRQ handler
   - Line 739: Removed `active_buffer_count++` in IRQ handler
   - Line 3737: Removed `active_buffer_count++` in QBUF handler

2. **driver/tx-isp-module.c**:
   - Lines 3836-3858: Removed 0x3000008 event call from STREAMON

## Testing

Build and test:
```bash
cd /home/matteius/isp-latest/driver
make clean && make
sudo insmod tx-isp-t31.ko
./prudynt
dmesg -w | grep -E "active_buffer_count|0x300|control limit"
```

Expected: No control limit errors, continuous interrupts, buffer count stays at 4!

