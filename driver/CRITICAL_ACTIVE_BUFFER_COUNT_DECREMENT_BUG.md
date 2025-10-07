# CRITICAL BUG: active_buffer_count Decrement in IRQ Handler

## The Bug

**Location**: `tx_isp_vic.c` line 664

**Code**:
```c
completed_buffer = list_first_entry(&vic_dev->busy_head, struct vic_buffer_entry, list);
list_del(&completed_buffer->list);
vic_dev->active_buffer_count--;  // ❌ BUG: This decrements the ring size!
```

## Impact

This bug causes `active_buffer_count` to **decrease by 1 on every interrupt**:

```
REQBUFS sets active_buffer_count = 4
  ↓
First interrupt:  4 → 3
  ↓
Second interrupt: 3 → 2
  ↓
Third interrupt:  2 → 1
  ↓
Fourth interrupt: 1 → 0
  ↓
VIC register 0x300 gets wrong buffer count!
```

## Evidence from Logs

**Line 641**: REQBUFS sets `active_buffer_count=4`
```
[ 1389.585151] *** Channel 0: VIC active_buffer_count set to 4 (memory=2) ***
```

**Line 649**: VIC event handler confirms `active_buffer_count=4`
```
[ 1389.585204] *** vic_core_ops_ioctl: VIC active_buffer_count set to 4 (Binary Ninja EXACT) ***
```

**Line 678**: First QBUF shows `active_buffer_count=4`
```
[ 1389.585437] ispvic_frame_channel_qbuf: Buffer queued (active_buffer_count=4 from REQBUFS)
```

**Line 1330**: After 2 interrupts, `active_buffer_count=2`
```
[ 1389.989613] ispvic_frame_channel_qbuf: Buffer queued (active_buffer_count=2 from REQBUFS)
```

**Line 1731**: Second PIPO call shows `active_buffer_count=2` (preserved from previous value)
```
[ 1390.029923] *** tx_isp_subdev_pipo: Buffer entries allocated, active_buffer_count=2 (preserved) ***
```

## Root Cause

The IRQ handler was treating `active_buffer_count` as a **busy queue counter** (number of buffers currently in use), when it should be a **constant ring size** (total number of buffer slots).

### What active_buffer_count Should Be

- **Constant value** set once by REQBUFS via 0x3000008 event
- **Ring size** (max 5 for VIC hardware)
- **Never modified** after REQBUFS sets it

### What It Was Being Used As

- **Busy queue counter** that incremented/decremented with each buffer operation
- Decremented in IRQ handler when buffer completed
- Incremented in QBUF handler when buffer queued

## The Fix

**Removed the decrement** in the IRQ handler (line 664):

```c
completed_buffer = list_first_entry(&vic_dev->busy_head, struct vic_buffer_entry, list);
list_del(&completed_buffer->list);
/* CRITICAL FIX: Do NOT decrement active_buffer_count! */
/* active_buffer_count is the ring size, not a busy queue counter */
```

## All Locations Fixed

1. ✅ **Line 664**: Removed `vic_dev->active_buffer_count--` in IRQ handler (THIS FIX)
2. ✅ **Line 695**: Already had comment preventing decrement in search loop
3. ✅ **Line 740**: Already had comment preventing increment when queuing next buffer
4. ✅ **Line 3737** (tx-isp-module.c): Already had comment preventing increment in QBUF handler
5. ✅ **Line 3881**: Already removed `active_buffer_count = 0` in PIPO setup
6. ✅ **Line 4319** (tx-isp-module.c): Already removed `active_buffer_count = 2` in TX_ISP_SET_BUF

## Expected Behavior After Fix

```
REQBUFS(count=4)
  ↓
active_buffer_count = 4 (set once)
  ↓
QBUF(buf[0..3])
  ↓
active_buffer_count = 4 (unchanged)
  ↓
STREAMON
  ↓
active_buffer_count = 4 (unchanged)
  ↓
Interrupt 1: active_buffer_count = 4 (unchanged) ✅
  ↓
Interrupt 2: active_buffer_count = 4 (unchanged) ✅
  ↓
Interrupt 3: active_buffer_count = 4 (unchanged) ✅
  ↓
VIC register 0x300 = 0x80040020 (4 buffers) ✅
```

## Why This Bug Was Hard to Find

1. **Multiple decrements**: The value was being decremented in the IRQ handler, so it changed gradually over time
2. **Log gaps**: The logs had gaps where interrupts occurred, making it hard to see the exact sequence
3. **Multiple PIPO calls**: prudynt calls PIPO multiple times (during tisp_init), which "preserved" the already-decremented value
4. **Previous fixes**: We had already fixed other increment/decrement locations, but missed this one in the IRQ handler

## Binary Ninja Confirmation

The reference driver's IRQ handler does NOT decrement `active_buffer_count`. It only:
1. Removes completed buffer from busy queue
2. Calls pipe callback to deliver buffer
3. Moves buffer to free queue
4. Queues next buffer if available

The `active_buffer_count` field is **read-only** in the IRQ handler - it's only used to determine the loop count when searching for matching buffers.

## Files Modified

- `driver/tx_isp_vic.c` line 664: Removed `vic_dev->active_buffer_count--`

## Success Criteria

✅ `active_buffer_count` remains constant at 4 throughout streaming
✅ No more gradual decrease from 4 → 3 → 2 → 1 → 0
✅ VIC register 0x300 always written with correct buffer count
✅ No control limit errors
✅ Continuous interrupts without stalls

