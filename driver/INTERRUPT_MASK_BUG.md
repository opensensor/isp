# THE REAL BUG: Interrupt Mask Was Backwards!

## The Actual Problem

The VIC interrupt mask register was set to `0xFFFFFFFE` which:
- **MASKS frame done interrupts (bit 0 = 0)**
- **ENABLES control limit error interrupts (bit 21 = 1)**

This is the OPPOSITE of what we want!

## VIC Interrupt Mask Register Semantics

In the VIC interrupt mask register (offset 0x1e8):
- **1 = interrupt ENABLED**
- **0 = interrupt MASKED (disabled)**

This is standard interrupt controller behavior.

## What We Were Doing (WRONG)

```c
writel(0xFFFFFFFE, vic_regs + 0x1e8);
// 0xFFFFFFFE in binary:
// 1111 1111 1111 1111 1111 1111 1111 1110
//                                        ^
//                                     Bit 0 = 0 (frame done MASKED!)
// Bit 21 = 1 (control limit ENABLED!)
```

**Result:**
- Frame done interrupts: MASKED (disabled)
- Control limit error interrupts: ENABLED
- We get control limit errors, NOT frame done!

## What We Should Do (CORRECT)

```c
writel(0xFFDFFFFF, vic_regs + 0x1e8);
// 0xFFDFFFFF in binary:
// 1111 1111 1101 1111 1111 1111 1111 1111
//           ^                          ^
//        Bit 21 = 0              Bit 0 = 1
//   (control limit MASKED)  (frame done ENABLED)
```

**Result:**
- Frame done interrupts: ENABLED
- Control limit error interrupts: MASKED (disabled)
- We get frame done interrupts!

## Why This Explains Everything

### Latest Logs Show

```
[0x1e8]=0xfffffffe  ← Mask register (frame done MASKED)
[0x1e0]=0x00200000  ← Status register (bit 21 = control limit error)
v1_7 & 1 = 0        ← No frame done interrupt
VIC ERROR: control limit error (bit 21)
```

The control register `0x300` is correct (`0x80000020` - no bit 17), but we're still getting control limit errors because:
1. Control limit interrupts are ENABLED in the mask
2. Frame done interrupts are MASKED
3. Hardware generates control limit errors for some reason
4. We never get frame done interrupts

### Why Control Limit Errors Happen

The hardware is generating control limit errors (bit 21) for some validation reason. This could be:
- Buffer address validation
- Dimension validation  
- Stride validation
- Some other hardware protection check

But it doesn't matter WHY they happen - we should have them MASKED anyway!

## The Comment Was Misleading

The code had this comment:
```c
writel(0xFFFFFFFE, vr + 0x1e8); /* allow ONLY frame-done, mask control limit error */
```

The comment says "allow ONLY frame-done" but the code does the OPPOSITE!

Someone misunderstood the mask register semantics and thought:
- 0 = enabled
- 1 = masked

But it's actually:
- 1 = enabled
- 0 = masked

## Files Fixed

### driver/tx_isp_vic.c
- Line 3304: Changed `0xFFFFFFFE` → `0xFFDFFFFF`

### driver/tx-isp-module.c
- Line 1757: Changed `0xFFFFFFFE` → `0xFFDFFFFF`
- Line 1780: Changed `0xFFFFFFFE` → `0xFFDFFFFF`
- Line 1800: Changed `0xFFFFFFFE` → `0xFFDFFFFF`
- Line 1833: Changed `0xFFFFFFFE` → `0xFFDFFFFF`
- Line 2245: Changed `0xFFFFFFFE` → `0xFFDFFFFF`

## Expected Behavior After Fix

### Before Fix
```
Interrupt mask: 0xFFFFFFFE (frame done MASKED, control limit ENABLED)
Interrupt status: 0x00200000 (control limit error)
Result: No frame done interrupts, driver stalls
```

### After Fix
```
Interrupt mask: 0xFFDFFFFF (frame done ENABLED, control limit MASKED)
Interrupt status: 0x00000001 (frame done)
Result: Continuous frame done interrupts, driver works!
```

## Why Previous Fixes Didn't Work

1. **Fix #1 (CSI interface)**: Correct but unrelated
2. **Fix #2 (Clear bit 17)**: Correct but irrelevant - bit 17 wasn't the issue
3. **Fix #3 (processing = 0)**: Correct but incomplete - still had wrong mask
4. **Fix #4 (Frame handler mask)**: Correct but never executed - no frame done interrupts

All the fixes were addressing symptoms, not the root cause. The root cause was that we were **masking the interrupts we wanted and enabling the interrupts we didn't want**!

## Mask Register Bit Map

```
Bit 0:  Frame done interrupt (we want this ENABLED = 1)
Bit 1:  Unknown
...
Bit 21: Control limit error (we want this MASKED = 0)
...
Bit 31: Unknown

Correct mask: 0xFFDFFFFF
- All bits = 1 (enabled) except bit 21 = 0 (masked)
- Bit 0 = 1 (frame done enabled)
- Bit 21 = 0 (control limit masked)
```

## Testing

After rebuilding with this fix:

```bash
cd /home/matteius/isp-latest/driver
make clean && make
sudo rmmod tx-isp-module 2>/dev/null
sudo insmod tx-isp-module.ko
```

**Look for in logs:**
```
Set interrupt mask=0xFFDFFFFF (frame-done ENABLED, control limit MASKED)
[0x1e8]=0xffdfffff  ← Correct mask
[0x1e0]=0x00000001  ← Frame done interrupt!
Info[VIC_MDAM_IRQ] : channel[0] frame done
```

**Should NOT see:**
```
VIC ERROR: control limit error (bit 21)
No frame done interrupt (v1_7 & 1 = 0)
```

## Conclusion

The 10/15 interrupt stall was caused by a simple but critical bug: **the interrupt mask register was set backwards**. We were masking frame done interrupts and enabling control limit error interrupts, when we should have been doing the opposite.

This is a classic case of misunderstanding hardware register semantics. The fix is simple: change `0xFFFFFFFE` to `0xFFDFFFFF` in all places where the VIC interrupt mask is set.

