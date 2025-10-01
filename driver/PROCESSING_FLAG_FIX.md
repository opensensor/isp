# CRITICAL FIX: Processing Flag Still Being Set to 1

## The Problem

Even though we changed `vic_mdma_enable_complete()` to set `processing = 0`, the `tx_isp_subdev_pipo()` function was **still setting it back to 1**!

From the logs:
```
[51.764048] tx_isp_subdev_pipo: set processing = 1 (Binary Ninja offset 0x20c)
[51.764163] tx_isp_subdev_pipo: set processing = 1 (pipe enabled, safe struct access)
```

This function is called AFTER `vic_mdma_enable_complete`, so it was overwriting our fix!

## Why This Causes The Problem

When `processing = 1`:
1. Frame done interrupt handler enters buffer management code
2. Updates VIC control register with buffer index
3. When buffer index = 2: `(2 << 16) = 0x00020000` = **bit 17 set!**
4. Bit 17 enables "control limit protection"
5. Hardware triggers control limit error (bit 21)
6. No frame done interrupt → **10/15 stall!**

From logs:
```
CTRL[0x300]=0x80020020  ← Bit 17 set! (0x00020000)
VIC STAT: v1_7=0x00200000  ← Control limit error (bit 21)
VIC IRQ: No frame done interrupt (v1_7 & 1 = 0)
```

## The Call Sequence

1. `vic_mdma_enable_complete()` sets `processing = 0` ✓
2. `tx_isp_subdev_pipo()` called (line 4115) sets `processing = 1` ✗
3. `tx_isp_subdev_pipo()` called again (line 4188) sets `processing = 1` ✗
4. Frame done interrupt fires
5. Handler sees `processing == 1`, updates control register
6. Buffer index 2 sets bit 17 → control limit error → stall!

## The Fix

Changed `tx_isp_subdev_pipo()` to set `processing = 0` instead of 1:

### Location 1: Line 4114-4117
```c
// OLD (broken):
vic_dev->processing = 1;
pr_info("tx_isp_subdev_pipo: set processing = 1 (Binary Ninja offset 0x20c)\n");

// NEW (fixed):
vic_dev->processing = 0;
pr_info("tx_isp_subdev_pipo: set processing = 0 (FIXED - matching working version)\n");
```

### Location 2: Line 4186-4191
```c
// OLD (broken):
vic_dev->processing = 1;
pr_info("tx_isp_subdev_pipo: set processing = 1 (pipe enabled, safe struct access)\n");

// NEW (fixed):
vic_dev->processing = 0;
pr_info("tx_isp_subdev_pipo: set processing = 0 (FIXED - pipe enabled, matching working version)\n");
```

## What Processing Flag Controls

### When processing = 0 (Working Version)
- Frame done handler **skips** buffer management code
- Control register **never updated** with buffer index
- Stays at initial value (no bit 17)
- Hardware manages buffers automatically
- **Continuous frame done interrupts!**

### When processing = 1 (Broken)
- Frame done handler **enters** buffer management code
- Control register **updated** with buffer index
- Buffer index 2 sets bit 17
- Hardware triggers control limit error
- **10/15 stall!**

## Files Modified

- `driver/tx_isp_vic.c` - Lines 4114-4117 (first processing = 1)
- `driver/tx_isp_vic.c` - Lines 4186-4191 (second processing = 1)

## Expected Behavior After Fix

### Before Fix
```
tx_isp_subdev_pipo: set processing = 1
[Frame done interrupt]
VIC FRAME DONE: Binary Ninja - Updated VIC[0x300]=0x80020020 (buffer_pos=2)
VIC IRQ: CTRL[0x300]=0x80020020  ← Bit 17 set!
VIC ERROR: control limit error (bit 21)
VIC IRQ: No frame done interrupt
[10/15 stall]
```

### After Fix
```
tx_isp_subdev_pipo: set processing = 0 (FIXED)
[Frame done interrupt]
VIC FRAME DONE: processing=0, skipping buffer index update
VIC IRQ: CTRL[0x300]=0x80000020  ← No bit 17!
VIC IRQ: Frame done interrupt (v1_7 & 1 = 1)
[Continuous interrupts!]
```

## Why This Is The Root Cause

1. ✅ **Explains the control register alternating** - Buffer index updates when processing=1
2. ✅ **Explains bit 17 being set** - Buffer index 2 = 0x00020000
3. ✅ **Explains control limit errors** - Bit 17 enables protection
4. ✅ **Explains 10/15 stall** - Happens when buffer cycling reaches index 2
5. ✅ **Matches working version** - Working version has processing=0

## Testing

After rebuilding:

```bash
cd /home/matteius/isp-latest/driver
make clean && make
sudo rmmod tx-isp-module 2>/dev/null
sudo insmod tx-isp-module.ko
```

**Look for in logs:**
```
tx_isp_subdev_pipo: set processing = 0 (FIXED - matching working version)
[Then continuous frame done interrupts with no control limit errors]
```

**Check control register:**
```
VIC IRQ: CTRL[0x300]=0x80000020  ← Should stay at this value (no bit 17)
VIC IRQ: Frame done interrupt (v1_7 & 1 = 1)  ← Should see this!
```

## Conclusion

The 10/15 stall was caused by `tx_isp_subdev_pipo()` setting `processing = 1`, which enabled buffer management code in the frame done handler. This code updated the VIC control register with buffer index values, and when the index reached 2, it accidentally set bit 17 (control limit enable), causing hardware to trigger control limit errors instead of frame done interrupts.

The fix is to set `processing = 0` in ALL locations, matching the working version's behavior where the frame done handler skips buffer management and lets hardware manage buffers automatically.

