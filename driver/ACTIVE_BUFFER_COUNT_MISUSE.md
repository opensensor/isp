# Active Buffer Count Misuse - Root Cause Analysis

## Problem

VIC `active_buffer_count` is being used as a **counter** instead of a **ring size constant**.

## What `active_buffer_count` Should Be

**Ring Size Constant:**
- Set ONCE by REQBUFS via 0x3000008 event
- Represents the TOTAL number of buffers in the VIC hardware ring (e.g., 4 or 5)
- Used to configure VIC register 0x300: `(buffer_count << 16) | 0x80000020`
- NEVER changes during streaming

## What It's Actually Being Used As

**Busy Queue Counter:**
- Incremented in `ispvic_frame_channel_qbuf` when buffer added to busy queue
- Decremented in `vic_framedone_irq_function` when buffer removed from busy queue
- Changes constantly during streaming (1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 10, 11, 12...)
- Causes VIC register 0x300 to be written with wrong buffer count

## Evidence from Logs

```
[  641.198284] *** vic_core_ops_ioctl: VIC active_buffer_count set to 4 (Binary Ninja EXACT) ***
... (REQBUFS sets it to 4)

[  641.199816] ispvic_frame_channel_qbuf: Incremented active_buffer_count to 12
... (QBUF keeps incrementing it!)

[  641.199829] *** STREAMON: Sending 0x3000008 event to VIC with buffer_count=5 ***
[  641.199852] *** vic_core_ops_ioctl: VIC active_buffer_count set to 5 (Binary Ninja EXACT) ***
... (STREAMON resets it to 5)

[  641.822624] *** VIC MDMA: Using active_buffer_count=4 from REQBUFS (Binary Ninja EXACT) ***
[  641.822631] *** Binary Ninja EXACT: Wrote 0x80040020 to reg 0x300 (4 buffers) ***
... (But ispvic_frame_channel_s_stream uses 4, not 5!)
```

## The Flow

### REQBUFS:
```
REQBUFS(count=4)
    ↓
Send 0x3000008 event to VIC with buffer_count=4
    ↓
vic_core_ops_ioctl: active_buffer_count = 4 ✅
```

### QBUF (called 5 times):
```
QBUF(buffer[0])
    ↓
ispvic_frame_channel_qbuf: active_buffer_count++ = 5 ❌

QBUF(buffer[1])
    ↓
ispvic_frame_channel_qbuf: active_buffer_count++ = 6 ❌

QBUF(buffer[2])
    ↓
ispvic_frame_channel_qbuf: active_buffer_count++ = 7 ❌

QBUF(buffer[3])
    ↓
ispvic_frame_channel_qbuf: active_buffer_count++ = 8 ❌

QBUF(buffer[4])
    ↓
ispvic_frame_channel_qbuf: active_buffer_count++ = 9 ❌
```

### STREAMON:
```
STREAMON
    ↓
Read vic_dev->active_buffer_count = 9 ❌
    ↓
Cap to 5: active_buffer_count = 5
    ↓
Send 0x3000008 event to VIC with buffer_count=5
    ↓
vic_core_ops_ioctl: active_buffer_count = 5 ✅
    ↓
Call tx_isp_video_s_stream(1)
    ↓
Call ispvic_frame_channel_s_stream(1)
    ↓
Read vic_dev->active_buffer_count = 5 ✅
    ↓
Write (5 << 16) | 0x80000020 to VIC[0x300]... 
    ↓
BUT WAIT! ispvic_frame_channel_s_stream is called MULTIPLE TIMES!
    ↓
Second call: active_buffer_count = 4 (from REQBUFS, not STREAMON!) ❌
    ↓
Write (4 << 16) | 0x80000020 to VIC[0x300] ❌
```

## The Real Problem

There are **TWO** different code paths setting `active_buffer_count`:

1. **REQBUFS → 0x3000008 event → vic_core_ops_ioctl**: Sets it to 4
2. **STREAMON → 0x3000008 event → vic_core_ops_ioctl**: Sets it to 5

But then `ispvic_frame_channel_s_stream` is called multiple times, and it uses whatever value is in `active_buffer_count` at that moment, which could be 4 or 5 depending on timing!

## The Fix

### Fix 1: Remove Increment/Decrement in QBUF and IRQ Handler ✅

**File: `driver/tx_isp_vic.c`**

Line 3737 in `ispvic_frame_channel_qbuf`:
```c
/* CRITICAL FIX: Do NOT increment active_buffer_count here! */
/* active_buffer_count is the TOTAL buffer ring size, set by REQBUFS via 0x3000008 event */
/* It should NOT be incremented on each QBUF - that's a counter, not a ring size! */
pr_info("ispvic_frame_channel_qbuf: Buffer queued (active_buffer_count=%d from REQBUFS)\n",
        vic_dev->active_buffer_count);
```

Line 695 and 739 in `vic_framedone_irq_function`:
```c
/* CRITICAL FIX: Do NOT decrement/increment active_buffer_count! */
/* Use a separate counter for busy queue depth if needed */
```

### Fix 2: Use Consistent Buffer Count Source

The buffer count should come from **ONE** authoritative source:
- **REQBUFS** if called (sets it to 4)
- **STREAMON** if REQBUFS not called (sets it to 5 based on actual QBUF calls)

But currently both are being called, and they set different values!

### Fix 3: Don't Send 0x3000008 from STREAMON if REQBUFS Already Set It

**File: `driver/tx-isp-module.c`**

In STREAMON handler (line 3836-3874):
```c
/* Only send 0x3000008 if REQBUFS didn't already set it */
if (ourISPdev->vic_dev->active_buffer_count == 0) {
    /* REQBUFS wasn't called, use actual queued count */
    int actual_queued = /* count from QBUF calls */;
    
    int32_t buffer_count_for_vic = actual_queued;
    tx_isp_send_event_to_remote(&ourISPdev->vic_dev->sd, 0x3000008, &buffer_count_for_vic);
} else {
    /* REQBUFS already set it, don't override */
    pr_info("STREAMON: Using buffer_count=%d from REQBUFS (not overriding)\n",
            ourISPdev->vic_dev->active_buffer_count);
}
```

## Expected Behavior After Fix

```
REQBUFS(count=4)
    ↓
vic_core_ops_ioctl: active_buffer_count = 4 ✅
    ↓
QBUF(buffer[0])
    ↓
ispvic_frame_channel_qbuf: active_buffer_count = 4 (unchanged) ✅
    ↓
QBUF(buffer[1])
    ↓
ispvic_frame_channel_qbuf: active_buffer_count = 4 (unchanged) ✅
    ↓
... (more QBUFs, count stays 4)
    ↓
STREAMON
    ↓
Check: active_buffer_count = 4 (from REQBUFS)
    ↓
Skip sending 0x3000008 (already set by REQBUFS) ✅
    ↓
Call ispvic_frame_channel_s_stream(1)
    ↓
Write (4 << 16) | 0x80000020 to VIC[0x300] ✅
    ↓
VIC configured for 4 buffers ✅
    ↓
No control limit errors! ✅
```

## Files to Modify

1. **driver/tx_isp_vic.c**:
   - Line 3737: Remove `active_buffer_count++` in `ispvic_frame_channel_qbuf`
   - Line 695: Remove `active_buffer_count--` in `vic_framedone_irq_function`
   - Line 739: Remove `active_buffer_count++` in `vic_framedone_irq_function`

2. **driver/tx-isp-module.c**:
   - Line 3836-3874: Only send 0x3000008 from STREAMON if REQBUFS didn't set it

## Key Insight

The `active_buffer_count` field has **dual purpose** in the reference driver:
1. **Ring size** for VIC hardware configuration
2. **Busy queue depth** for buffer management

Our driver needs to **separate these two concepts**:
- `active_buffer_count` = ring size (set once, never changed)
- `busy_buffer_count` = busy queue depth (incremented/decremented)

Or just remove the increment/decrement logic entirely and use `list_count_nodes()` to count busy buffers when needed.

