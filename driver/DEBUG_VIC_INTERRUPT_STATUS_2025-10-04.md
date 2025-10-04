# Debug: VIC Interrupt Status Analysis

## Date: 2025-10-04

## Current Situation

After fixing the premature VIC start issue, we now get:
- ✅ VIC starts at the correct time (after `vic_start_ok = 1`)
- ✅ ONE VIC hardware interrupt fires
- ❌ That interrupt has "control limit err" (bit 21)
- ❌ No more interrupts after that
- ⚠️ Software generates fake frames to keep streaming alive

## Key Observation from Logs

From `driver/logs.txt` line 1708-1713:

```
[  578.720571] *** CRITICAL: isp_irq_handle: IRQ 38 received, dev_id=8209c000 ***
[  578.728029] *** isp_irq_handle: IRQ 38 received, dev_id=8209c000 ***
[  578.728035] *** INTERRUPT HANDLER CALLED - THIS PROVES THE HANDLER IS WORKING ***
[  578.735759] Err2 [VIC_INT] : control limit err!!!
[  578.735767] *** FRAME SYNC: Frame done count = 5 ***
[  578.735773] *** ISP FRAME DONE WAKEUP: Frame 5 ready for processing ***
```

**Missing**: No "VIC FRAME DONE:" messages from `vic_framedone_irq_function()`!

## Hypothesis

The ISR only calls `vic_framedone_irq_function()` if bit 0 (frame done) is set:

```c
if ((v1_7 & 1) != 0) {
    vic_framedone_irq_function(vic_dev);
}
```

**Possible scenarios:**

### Scenario 1: Frame Done Bit NOT Set
- v1_7 = 0x200000 (only bit 21 - control limit error)
- Frame done bit (bit 0) is NOT set
- `vic_framedone_irq_function()` is NOT called
- No buffer rotation happens
- VIC runs out of buffers
- No more interrupts

### Scenario 2: Frame Done Bit IS Set But Function Returns Early
- v1_7 = 0x200001 (bit 0 + bit 21)
- Frame done bit IS set
- `vic_framedone_irq_function()` IS called
- But function returns early due to some check
- No buffer rotation happens
- VIC runs out of buffers
- No more interrupts

## Debug Changes Added

### File: `driver/tx-isp-module.c`

**Added logging to determine which scenario is happening:**

```c
if (vic_start_ok != 0) {
    pr_info("*** VIC HARDWARE INTERRUPT: vic_start_ok=1, processing (v1_7=0x%x, v1_10=0x%x) ***\n", v1_7, v1_10);

    if ((v1_7 & 1) != 0) {
        pr_info("*** VIC ISR: Frame done bit SET (v1_7 & 1 = 1) ***\n");
        vic_dev->frame_count++;
        ...
        vic_framedone_irq_function(vic_dev);
    } else {
        pr_info("*** VIC ISR: Frame done bit NOT set (v1_7 & 1 = 0), v1_7=0x%x ***\n", v1_7);
    }
```

## Expected Test Results

### If Scenario 1 (Frame Done NOT Set):

```
[  X.XXX] *** VIC HARDWARE INTERRUPT: vic_start_ok=1, processing (v1_7=0x200000, v1_10=0x0) ***
[  X.XXX] *** VIC ISR: Frame done bit NOT set (v1_7 & 1 = 0), v1_7=0x200000 ***
[  X.XXX] Err2 [VIC_INT] : control limit err!!!
```

**This means**: VIC is generating control limit error WITHOUT frame done. This suggests:
- VIC tried to capture a frame
- But ran out of buffer addresses
- Never completed the frame
- Never set frame done bit

**Root cause**: Buffer addresses not properly configured in VIC registers

### If Scenario 2 (Frame Done IS Set):

```
[  X.XXX] *** VIC HARDWARE INTERRUPT: vic_start_ok=1, processing (v1_7=0x200001, v1_10=0x0) ***
[  X.XXX] *** VIC ISR: Frame done bit SET (v1_7 & 1 = 1) ***
[  X.XXX] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=1) ***
[  X.XXX] Err2 [VIC_INT] : control limit err!!!
```

**This means**: VIC completed the frame BUT also hit control limit error. This suggests:
- VIC captured first frame successfully
- But buffer rotation in `vic_framedone_irq_function()` failed
- VIC has no new buffer for next frame
- Control limit error on NEXT frame attempt

**Root cause**: Buffer rotation logic in `vic_framedone_irq_function()` not working

## Next Steps Based on Results

### If Frame Done NOT Set (Scenario 1):

Need to check VIC buffer configuration:
1. Verify VIC[0x318-0x328] have valid buffer addresses
2. Check VIC[0x300] buffer count field
3. Verify buffer addresses are in valid memory range
4. Check if VBM allocation is working

### If Frame Done IS Set (Scenario 2):

Need to debug `vic_framedone_irq_function()`:
1. Check if `vic_dev->processing` is set
2. Check if `vic_dev->stream_state` is set
3. Verify buffer list is not empty
4. Check if buffer rotation logic is working
5. Verify new buffer address is written to VIC registers

## Testing Instructions

1. **Build and deploy** the updated driver with debug logging
2. **Start streaming** with prudynt
3. **Check dmesg** for the new debug messages:
   ```bash
   dmesg | grep -E "VIC HARDWARE INTERRUPT|VIC ISR|Frame done bit"
   ```

4. **Determine which scenario** is happening based on the logs

5. **Report back** with the exact v1_7 value and whether frame done bit is set

## Files Modified

- `driver/tx-isp-module.c`: Added debug logging to ISR (lines 1706, 1710, 1723)

---

**This debug logging will tell us exactly what's happening with the VIC interrupt status!**

