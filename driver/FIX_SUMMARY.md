# ISP Interrupt Stall Fix Summary

## Problem
The current driver (`isp-latest`) gets 10-15 ISP/VIC interrupts then stalls, while the working driver (`isp-was-better`) gets continuous interrupts.

## Root Cause Identified
The issue is in `csi_video_s_stream()` function in `tx_isp_csi.c`:

### Current (Broken) Implementation
```c
// Checks csi_dev->interface_type which may not be initialized yet
if (csi_dev->interface_type != 1) {
    pr_info("csi_video_s_stream: Interface type %d != 1 (MIPI), returning 0\n", 
            csi_dev->interface_type);
    return 0;
}
```

**Problem:** `csi_dev->interface_type` is only set during `csi_core_ops_init()`, which may be called:
1. After `csi_video_s_stream()` in some scenarios
2. Not at all if initialization order is wrong
3. With a default value of 0 if not properly initialized

When the check fails (interface_type != 1), the function returns 0 without setting `csi_dev->state`, which causes:
- CSI device remains in wrong state
- Interrupts are not properly handled
- After 10-15 interrupts, the system stalls

### Working Implementation (from isp-was-better)
```c
// Checks sensor attributes directly - always available
extern struct tx_isp_sensor *tx_isp_get_sensor(void);
struct tx_isp_sensor *sensor = tx_isp_get_sensor();
if (!sensor || !sensor->video.attr || 
    sensor->video.attr->dbus_type != TX_SENSOR_DATA_INTERFACE_MIPI) {
    pr_info("csi_video_s_stream: Sensor interface type is not MIPI (1), returning 0\n");
    return 0;
}
```

**Why it works:** Sensor attributes are initialized during sensor registration, which happens before any streaming operations.

## Fix Applied

Modified `csi_video_s_stream()` in `driver/tx_isp_csi.c` to:
1. **First** check sensor attributes (like working version)
2. **Fallback** to `csi_dev->interface_type` if sensor not available
3. This ensures the check always works regardless of initialization order

### New Implementation
```c
/* CRITICAL FIX: Check sensor interface type instead of CSI device interface_type */
extern struct tx_isp_sensor *tx_isp_get_sensor(void);
struct tx_isp_sensor *sensor = tx_isp_get_sensor();

/* First try to check sensor attributes (more reliable) */
if (sensor && sensor->video.attr) {
    if (sensor->video.attr->dbus_type != TX_SENSOR_DATA_INTERFACE_MIPI) {
        pr_info("csi_video_s_stream: Sensor interface type %d != 1 (MIPI), returning 0\n", 
                sensor->video.attr->dbus_type);
        return 0;
    }
    pr_info("csi_video_s_stream: Sensor interface type is MIPI (1) - proceeding\n");
} else {
    /* Fallback to CSI device interface_type if sensor not available */
    pr_info("csi_video_s_stream: Sensor not available, checking csi_dev->interface_type=%d\n", 
            csi_dev->interface_type);
    if (csi_dev->interface_type != 1) {
        pr_info("csi_video_s_stream: CSI device interface type %d != 1 (MIPI), returning 0\n", 
                csi_dev->interface_type);
        return 0;
    }
}
```

## Files Modified
- `driver/tx_isp_csi.c` - Lines 167-232 (csi_video_s_stream function)

## Testing Instructions

### 1. Rebuild the Driver
```bash
cd /home/matteius/isp-latest/driver
make clean
make
```

### 2. Load the Driver
```bash
sudo rmmod tx-isp-module 2>/dev/null
sudo insmod tx-isp-module.ko
```

### 3. Check for Continuous Interrupts
```bash
# Monitor kernel logs for interrupt messages
dmesg -w | grep -E "ISP|VIC|interrupt|MDMA"
```

### 4. Expected Behavior
**Before fix:**
- See 10-15 interrupt messages
- Then interrupts stop
- System stalls

**After fix:**
- See continuous interrupt messages
- No stalling
- Proper frame capture

### 5. Key Log Messages to Look For
```
csi_video_s_stream: Sensor interface type is MIPI (1) - proceeding
csi_video_s_stream: EXACT Binary Ninja MCP - CSI state set to 4 (enable=1)
vic_mdma_irq_function: channel=0 processing MDMA interrupt
Info[VIC_MDAM_IRQ] : channel[0] frame done
```

## Verification

### Success Indicators
1. ✅ Continuous interrupt messages in dmesg
2. ✅ No "Interface type X != 1 (MIPI), returning 0" errors
3. ✅ CSI state properly set to 4 (streaming)
4. ✅ Frame capture works continuously

### Failure Indicators
1. ❌ Interrupts stop after 10-15 frames
2. ❌ "Interface type 0 != 1 (MIPI), returning 0" in logs
3. ❌ CSI state not set correctly
4. ❌ System stalls

## Additional Fixes (If Needed)

If the primary fix doesn't fully resolve the issue, there are two additional potential fixes identified:

### Secondary Fix: VIC Register Initialization Order
The working version writes critical interrupt-enabling registers at the END of `tx_isp_vic_start()`:
```c
writel(0x3130322a, vic_regs + 0x0);  /* CRITICAL for interrupts */
writel(0x1, vic_regs + 0x4);         /* CRITICAL for interrupts */
writel(0x200, vic_regs + 0x14);      /* CRITICAL for interrupts */
```

### Tertiary Fix: Buffer Management
The current version has more complex buffer circulation logic that may cause issues. Consider simplifying to match the working version.

## Comparison Summary

| Aspect | Working (isp-was-better) | Current (isp-latest) | Status |
|--------|-------------------------|---------------------|--------|
| CSI interface check | Sensor attributes | CSI device field | ✅ FIXED |
| VIC register order | Simple, interrupts at end | Complex, may overwrite | 🔍 Monitor |
| Buffer management | Simple | Complex | 🔍 Monitor |
| gc2053 registration | Common API | Common API | ✅ Same |

## Next Steps

1. **Test the fix** - Rebuild and test with the CSI interface check fix
2. **Monitor logs** - Look for continuous interrupts and proper state transitions
3. **Report results** - If still stalling, we'll apply secondary fixes
4. **Document findings** - Update this file with test results

## Technical Details

### Call Sequence Analysis
```
1. tx_isp_video_s_stream(enable=1)
   ├─> csi_sd->ops->core->init(csi_sd, 1)  [csi_core_ops_init]
   │   └─> Sets csi_dev->interface_type = sensor_attr->dbus_type
   │
   └─> csi_sd->ops->video->s_stream(csi_sd, 1)  [csi_video_s_stream]
       └─> Checks interface_type and sets csi_dev->state
```

**Problem:** In some scenarios, `csi_video_s_stream` may be called before `csi_core_ops_init`, or `interface_type` may not be properly initialized.

**Solution:** Check sensor attributes directly, which are always available after sensor registration.

## References
- Analysis document: `driver/INTERRUPT_STALL_ANALYSIS.md`
- Smart-diff comparison ID: `c3c2d8ed-a112-421a-b800-143149169b57`
- Source (working): `/home/matteius/isp-was-better/driver`
- Target (current): `/home/matteius/isp-latest/driver`

