# ROOT CAUSE: ispcore_slake_module Disables VIC Interrupts

## The Actual Problem

The `ispcore_slake_module` function is being called after VIC streaming is enabled, and it **disables VIC interrupts at the kernel level** by calling `disable_irq()`.

## The Sequence

From the logs:

```
[32.599093] vic_core_s_stream SUCCESS - VIC interrupts should now be ENABLED!
[32.599143] ispcore_core_ops_init_with_sensor: SUCCESS - Core initialized and VIC streaming/IRQs armed
[32.599344] tx_vic_disable_irq: VIC interrupts disabled (irq_enabled = 0)
[32.599356] tx_isp_disable_irq: EXACT Binary Ninja - disabling IRQ 38
[32.599364] tx_isp_disable_irq: IRQ 38 DISABLED
```

**VIC interrupts are enabled, then immediately disabled!**

## Call Stack

1. `vic_core_s_stream(enable=1)` - Enables VIC streaming
2. Sets `vic_dev->state = 4` (STREAMING)
3. Calls `ispcore_slake_module(isp_dev)`
4. `ispcore_slake_module` sees `vic_state >= 3`
5. Calls `ispcore_core_ops_init_with_sensor(isp_dev, sensor_attr)`
6. This eventually calls VIC's `vic_core_ops_init(sd, enable=0)` to disable
7. `vic_core_ops_init` calls `tx_vic_disable_irq(vic_dev)`
8. `tx_vic_disable_irq` calls `disable_irq(38)` ← **INTERRUPTS DISABLED!**

## Why This Happens

The `ispcore_slake_module` function is designed to initialize the ISP core and manage state transitions. When it sees VIC state >= 3, it calls `ispcore_core_ops_init` which:

1. Initializes ISP core registers
2. Configures silicon bits and clocks
3. **Disables VIC interrupts as part of initialization**

This is correct for initial setup, but WRONG when called after VIC is already streaming!

## The Code

### ispcore_slake_module (tx_isp_core.c:2384)
```c
if (vic_state >= 3) {
    pr_info("ispcore_slake_module: ISP state >= 3, calling ispcore_core_ops_init");
    int ret = ispcore_core_ops_init_with_sensor(isp_dev, sensor_attr);
    // This disables VIC interrupts!
}
```

### vic_core_ops_init (tx_isp_vic.c:2720)
```c
if (enable == 0) {
    if (current_state != 2) {
        tx_vic_disable_irq(vic_dev);  ← DISABLES INTERRUPTS!
        vic_dev->state = 2;
    }
}
```

### tx_vic_disable_irq (tx_isp_vic.c:256)
```c
if (vic_dev->irq > 0) {
    disable_irq(vic_dev->irq);  ← KERNEL LEVEL DISABLE!
}
```

## The Fix

After `ispcore_slake_module` returns, we need to **re-enable interrupts at the kernel level**:

```c
/* CRITICAL FIX: Re-enable VIC interrupts at kernel level after slake module disabled them */
if (vic_dev->irq > 0) {
    pr_info("*** VIC STATE 4: Re-enabling VIC interrupt (IRQ %d) at kernel level ***\n", vic_dev->irq);
    enable_irq(vic_dev->irq);
    vic_dev->irq_enabled = 1;
    pr_info("*** VIC STATE 4: VIC interrupt (IRQ %d) RE-ENABLED ***\n", vic_dev->irq);
}
```

## Why Previous Fixes Didn't Work

1. **Mask register fixes**: Correct, but interrupts were disabled at kernel level
2. **Control register fixes**: Correct, but interrupts were disabled at kernel level
3. **processing flag fix**: Correct, but interrupts were disabled at kernel level

All the hardware registers were configured correctly, but the Linux kernel's interrupt subsystem had IRQ 38 **disabled** via `disable_irq()`, so no interrupts could fire!

## Files Modified

- `driver/tx_isp_vic.c` - Lines 3516-3541 (added enable_irq() call after slake module)

## Expected Behavior After Fix

### Before Fix
```
vic_core_s_stream: VIC interrupts enabled
ispcore_slake_module: Called
tx_vic_disable_irq: IRQ 38 DISABLED  ← PROBLEM!
[No interrupts fire]
```

### After Fix
```
vic_core_s_stream: VIC interrupts enabled
ispcore_slake_module: Called
tx_vic_disable_irq: IRQ 38 DISABLED
VIC STATE 4: Re-enabling VIC interrupt (IRQ 38)  ← FIX!
VIC STATE 4: VIC interrupt (IRQ 38) RE-ENABLED
[Interrupts fire continuously]
```

## Why This Is The Root Cause

1. ✅ **Explains why no interrupts fire** - Kernel has IRQ 38 disabled
2. ✅ **Explains the timing** - Happens right after streaming starts
3. ✅ **Explains why hardware looks correct** - All registers are fine, but kernel blocks interrupts
4. ✅ **Matches the logs** - "IRQ 38 DISABLED" message right after "VIC interrupts should now be ENABLED"
5. ✅ **Simple fix** - Just call `enable_irq()` to undo the `disable_irq()`

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
VIC STATE 4: Re-enabling VIC interrupt (IRQ 38) at kernel level
VIC STATE 4: VIC interrupt (IRQ 38) RE-ENABLED at kernel level
[Then continuous interrupt messages]
```

**Check /proc/interrupts:**
```bash
watch -n 1 'cat /proc/interrupts | grep 38'
```

Should see IRQ 38 count increasing continuously!

## Conclusion

The 10/15 interrupt stall was caused by `ispcore_slake_module` calling `disable_irq(38)` right after VIC streaming was enabled. The hardware was configured correctly, the mask registers were correct, but the Linux kernel's interrupt subsystem had IRQ 38 disabled, preventing any interrupts from firing.

The fix is to call `enable_irq(38)` after `ispcore_slake_module` returns to re-enable interrupts at the kernel level.

