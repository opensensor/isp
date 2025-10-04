# ISR Simplification - 2025-10-04

## Problem

The current driver stalls at 5/5 frames with control limit errors, while the prior "isp-was-better" version achieved continuous interrupts.

## Root Cause Analysis

Comparing the function diff export revealed that the **working ISR was 3x smaller** (215 lines vs 659 lines):

### Working Version (isp-was-better)
- **Simple structure**: Direct access to `ourISPdev->vic_dev`
- **No dual-bank logic**: Just used `vic_regs` directly
- **Minimal register manipulation**: Read status, clear status, process interrupts
- **No re-arming in ISR**: All VIC configuration done outside ISR
- **Clean logging**: Used `pr_debug()` for normal flow, `pr_err()` for errors
- **Total**: ~215 lines

### Broken Version (isp-latest)
- **Complex validation**: Checked `dev_id`, NULL pointers, dual banks
- **Dual-bank fallback**: PRIMARY → CONTROL bank logic (121 lines of disabled code!)
- **Massive re-arm logic**: 194 lines trying to fix VIC state during ISR
- **Excessive logging**: `printk(KERN_ALERT ...)` on every line
- **Total**: ~659 lines

## The Fix

**Reverted ISR to match working version's simplicity:**

### Changes Made

1. **Removed dual-bank fallback logic** (lines 1714-1734)
   - Working version just used `vic_regs` directly
   - No need to check PRIMARY vs CONTROL banks

2. **Removed disabled fixup code** (lines 1736-1858)
   - 121 lines of commented-out register manipulation
   - This was fighting the hardware, not helping

3. **Removed VIC re-arm logic** (lines 1889-2052)
   - 194 lines trying to reconfigure VIC during ISR
   - Working version proved this is unnecessary
   - All VIC config should be done in streaming start/stop

4. **Simplified to use `ourISPdev` global** like working version
   - No complex `dev_id` parameter validation
   - Direct access to `vic_dev` and `vic_regs`

5. **Reduced logging to pr_debug/pr_err**
   - Removed excessive `printk(KERN_ALERT ...)` calls
   - Matches working version's logging style

### Final ISR Structure (Matching Working Version)

```c
irqreturn_t isp_vic_interrupt_service_routine(int irq, void *dev_id)
{
    // 1. Get vic_dev from global ourISPdev
    vic_dev = ourISPdev->vic_dev;
    vic_regs = vic_dev->vic_regs;
    
    // 2. Read interrupt status (Binary Ninja exact formula)
    v1_7 = (~readl(vic_regs + 0x1e8)) & readl(vic_regs + 0x1e0);
    v1_10 = (~readl(vic_regs + 0x1ec)) & readl(vic_regs + 0x1e4);
    
    // 3. Clear interrupt status (W1C)
    writel(v1_7, vic_regs + 0x1f0);
    writel(v1_10, vic_regs + 0x1f4);
    wmb();
    
    // 4. Check vic_start_ok flag
    if (vic_start_ok != 0) {
        // 5. Process frame done interrupt
        if ((v1_7 & 1) != 0) {
            vic_dev->frame_count++;
            ourISPdev->frame_count++;
            vic_framedone_irq_function(vic_dev);
        }
        
        // 6. Log all error conditions (but don't try to fix them!)
        if ((v1_7 & 0x200) != 0) pr_err("frame asfifo ovf");
        if ((v1_7 & 0x400) != 0) pr_err("hor err ch0");
        // ... etc for all error bits
        
        // 7. Error recovery ONLY for critical conditions (0xde00)
        if ((v1_7 & 0xde00) != 0 && *vic_irq_enable_flag == 1) {
            // Binary Ninja exact recovery sequence
            writel(4, vic_regs + 0x0);  // Reset
            // Wait for ready
            // Restore registers
            writel(1, vic_regs + 0x0);  // Run
        }
        
        // 8. Wake up frame channels
        for (i = 0; i < num_channels; i++) {
            if (frame_channels[i].state.streaming) {
                frame_channel_wakeup_waiters(&frame_channels[i]);
            }
        }
    } else {
        pr_warn("VIC INTERRUPT IGNORED: vic_start_ok=0");
    }
    
    return IRQ_HANDLED;
}
```

## Key Principles

1. **ISR should be MINIMAL** - Just read, clear, process, wake
2. **No VIC reconfiguration in ISR** - That belongs in streaming start/stop
3. **No fighting the hardware** - If registers are wrong, fix the init sequence
4. **Trust the hardware** - If it's configured correctly, it will work
5. **Keep it simple** - The working version proves simplicity works

## Expected Result

With this simplified ISR matching the working version:
- ✅ Continuous interrupts (no 5/5 stall)
- ✅ No control limit errors (proper init prevents them)
- ✅ Clean frame done interrupts at ~30 FPS
- ✅ Proper buffer rotation through frame channels

## Files Modified

- `driver/tx-isp-module.c`: Simplified `isp_vic_interrupt_service_routine()` from 659 lines to ~215 lines

## Testing

Build and test:
```bash
cd driver
make clean && make
sudo insmod tx-isp.ko
# Run prudynt and check for continuous interrupts
dmesg | grep -E "VIC FRAME DONE|frame_count"
```

Look for:
- Continuous "VIC FRAME DONE INTERRUPT" messages
- Incrementing frame_count values
- NO "control limit err" messages
- NO stalling at 5/5 frames

