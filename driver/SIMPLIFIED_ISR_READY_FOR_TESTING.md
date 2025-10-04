# Simplified ISR - Ready for Testing

## Date: 2025-10-04

## Summary

Successfully simplified the VIC interrupt handler from **659 lines to ~215 lines** by reverting to the structure of the working "isp-was-better" version.

## What Was Changed

### File: `driver/tx-isp-module.c`

**Function**: `isp_vic_interrupt_service_routine()`

**Lines removed**: ~444 lines of complex logic that was fighting the hardware

**Changes**:

1. ✅ **Removed dual-bank fallback logic** (64 lines)
   - Working version just used `vic_regs` directly
   - No need for PRIMARY → CONTROL bank switching

2. ✅ **Removed disabled fixup code** (121 lines)
   - Massive block of commented-out register manipulation
   - Was trying to fix VIC state during ISR (wrong place!)

3. ✅ **Removed VIC re-arm logic** (194 lines)
   - Complex buffer management and register reconfiguration
   - Working version proved this is unnecessary in ISR
   - All VIC config belongs in streaming start/stop paths

4. ✅ **Simplified to use `ourISPdev` global**
   - Matches working version's approach
   - No complex `dev_id` parameter validation

5. ✅ **Reduced logging to pr_debug/pr_err**
   - Removed excessive `printk(KERN_ALERT ...)` calls
   - Cleaner logs, less interrupt latency

## The Simplified ISR Structure

```c
irqreturn_t isp_vic_interrupt_service_routine(int irq, void *dev_id)
{
    // Get VIC device from global
    vic_dev = ourISPdev->vic_dev;
    vic_regs = vic_dev->vic_regs;
    vic_irq_enable_flag = (uint32_t*)((char*)vic_dev + 0x13c);
    
    // Read interrupt status (Binary Ninja exact formula)
    v1_7 = (~readl(vic_regs + 0x1e8)) & readl(vic_regs + 0x1e0);
    v1_10 = (~readl(vic_regs + 0x1ec)) & readl(vic_regs + 0x1e4);
    
    // Clear interrupt status (W1C)
    writel(v1_7, vic_regs + 0x1f0);
    writel(v1_10, vic_regs + 0x1f4);
    wmb();
    
    // Check vic_start_ok flag
    if (vic_start_ok != 0) {
        // Process frame done interrupt
        if ((v1_7 & 1) != 0) {
            vic_dev->frame_count++;
            ourISPdev->frame_count++;
            vic_framedone_irq_function(vic_dev);
        }
        
        // Log all error conditions (but don't try to fix them!)
        if ((v1_7 & 0x200) != 0) pr_err("Err [VIC_INT] : frame asfifo ovf!!!!!");
        if ((v1_7 & 0x400) != 0) pr_err("Err [VIC_INT] : hor err ch0 !!!!!");
        // ... etc for all 32 error bits
        
        // Error recovery ONLY for critical conditions (0xde00)
        if ((v1_7 & 0xde00) != 0 && *vic_irq_enable_flag == 1) {
            // Binary Ninja exact recovery sequence
            writel(4, vic_regs + 0x0);  // Reset
            // Wait for ready
            // Restore registers 0x104, 0x108
            writel(1, vic_regs + 0x0);  // Run
        }
        
        // Wake up frame channels
        for (i = 0; i < num_channels; i++) {
            if (frame_channels[i].state.streaming) {
                frame_channel_wakeup_waiters(&frame_channels[i]);
            }
        }
    } else {
        pr_warn("*** VIC INTERRUPT IGNORED: vic_start_ok=0 ***");
    }
    
    return IRQ_HANDLED;
}
```

## Why This Should Work

### Evidence from Function Diff

The working "isp-was-better" version that achieved continuous interrupts had:
- **Simple ISR**: 215 lines total
- **No dual-bank logic**: Direct `vic_regs` access
- **No re-arming**: All VIC config outside ISR
- **Minimal logging**: pr_debug for normal flow

The broken "isp-latest" version that stalled at 5/5 had:
- **Bloated ISR**: 659 lines total (3x larger!)
- **Complex dual-bank logic**: 64 lines of fallback code
- **Disabled fixup code**: 121 lines of commented-out hacks
- **VIC re-arm logic**: 194 lines fighting the hardware
- **Excessive logging**: printk(KERN_ALERT) everywhere

### Key Insight

**The ISR should be MINIMAL** - just:
1. Read interrupt status
2. Clear interrupt status
3. Check vic_start_ok flag
4. Process frame done / errors
5. Wake up waiters
6. Return

All the complex VIC configuration should happen **outside the ISR** in:
- `tx_isp_vic_start()` - Initial VIC setup
- `vic_core_s_stream()` - Streaming start/stop
- `vic_pipo_mdma_enable()` - Buffer configuration

## Expected Results

With this simplified ISR:

✅ **Continuous interrupts** - No 5/5 stall
✅ **No control limit errors** - Proper init prevents them
✅ **Clean frame done interrupts** - At ~30 FPS
✅ **Proper buffer rotation** - Through frame channels
✅ **Lower interrupt latency** - Less code to execute

## Testing Instructions

### On Target Device

1. **Build the driver**:
   ```bash
   cd /path/to/isp-latest/driver
   # Use your normal build process
   ```

2. **Load the driver**:
   ```bash
   sudo rmmod tx-isp-t31 2>/dev/null
   sudo insmod tx-isp-t31.ko
   ```

3. **Start streaming**:
   ```bash
   # Your normal streaming command (prudynt, etc.)
   ```

4. **Check for continuous interrupts**:
   ```bash
   dmesg | grep -E "VIC FRAME DONE|frame_count" | tail -20
   ```

### What to Look For

**✅ SUCCESS indicators**:
```
[  10.123] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=1) ***
[  10.156] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=2) ***
[  10.189] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=3) ***
[  10.222] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=4) ***
[  10.255] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=5) ***
[  10.288] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=6) ***
... (continuous, ~30 FPS)
```

**❌ FAILURE indicators**:
```
[  10.123] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=1) ***
[  10.156] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=2) ***
[  10.189] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=3) ***
[  10.222] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=4) ***
[  10.255] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=5) ***
[  10.255] Err2 [VIC_INT] : control limit err!!!
... (stalls here, no more interrupts)
```

## Rollback Plan

If this doesn't work, you can revert by:

```bash
cd /path/to/isp-latest
git diff driver/tx-isp-module.c > /tmp/isr-simplification.patch
git checkout driver/tx-isp-module.c
```

## Next Steps If This Works

Once continuous interrupts are confirmed:

1. **Verify frame data** - Check that frames are actually being captured
2. **Test buffer rotation** - Ensure QBUF/DQBUF cycle works
3. **Check /proc/jz/isp/isp-w02** - Verify frame_count increments
4. **Test streaming stability** - Run for extended period

## Files Modified

- `driver/tx-isp-module.c`: Simplified `isp_vic_interrupt_service_routine()` (lines 1671-1887)

## Documentation Created

- `driver/ISR_SIMPLIFICATION_2025-10-04.md`: Detailed analysis and changes
- `driver/SIMPLIFIED_ISR_READY_FOR_TESTING.md`: This file

## Confidence Level

**HIGH** - The working "isp-was-better" version proves that this simple ISR structure achieves continuous interrupts. We've removed all the accumulated complexity that was fighting the hardware.

The key difference that allowed continuous interrupts was **simplicity** - not trying to fix VIC state during interrupt processing.

