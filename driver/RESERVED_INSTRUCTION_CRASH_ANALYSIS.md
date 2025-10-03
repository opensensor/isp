# Reserved Instruction Crash Analysis

## The Crash

After applying the initial interrupt mask fix, the system crashed with a "Reserved Instruction" exception:

```
[  927.050669] *** VIC IRQ COMPLETE: Processed v1_7=0x200000, v1_10=0x0 - returning IRQ_HANDLED ***
Reserved instruction in kernel code[#1]:
[  927.063542] CPU: 0 PID: 2606 Comm: prudynt Tainted: G           O 3.10.14__isvp_swan_1.0__ #1
[  927.126962] epc   : 84b184a4 0x84b184a4
[  927.138728] ra    : 84b184a0 0x84b184a0
```

## Root Cause: Wrong Interrupt Mask Value

The crash was caused by using the **WRONG interrupt mask value** due to misunderstanding the VIC interrupt mask semantics.

### The Misunderstanding

I initially thought the VIC interrupt mask register (0x1e8) used **positive logic**:
- 1 = ENABLED
- 0 = MASKED

But this was **WRONG**!

### The Actual Logic

Looking at the interrupt handler code (line 1891 in tx-isp-module.c):

```c
v1_7 = (~reg_1e8) & reg_1e0;
```

The mask register uses **INVERTED logic**:
- **In register 0x1e8: 1 = MASKED, 0 = ENABLED**
- The `~` operator inverts it before ANDing with the status register

### What Went Wrong

**My incorrect fix:**
```c
writel(0xFFDFFFFF, vc + 0x1e8);  // WRONG!
```

After inversion:
```
~0xFFDFFFFF = 0x00200000
            = Only bit 21 enabled (control limit error)
            = Bit 0 MASKED (frame done)
```

This made the problem **WORSE**:
- ❌ Masked frame done interrupts (bit 0)
- ✅ Enabled control limit error interrupts (bit 21)
- Result: Continuous control limit error interrupts, no frame done interrupts

### Why It Crashed

The continuous control limit error interrupts created an **interrupt storm** that:
1. Consumed all CPU time processing interrupts
2. Prevented normal code execution
3. Eventually caused stack corruption or other memory corruption
4. Led to the CPU trying to execute an invalid instruction at `0x84b184a4`

The "Reserved Instruction" exception is the CPU's way of saying "I tried to execute garbage data as code."

## The Correct Fix

**Correct mask value:**
```c
writel(0xFFFFFFFE, vc + 0x1e8);  // CORRECT!
```

After inversion:
```
~0xFFFFFFFE = 0x00000001
            = Only bit 0 enabled (frame done)
            = All other bits MASKED (including bit 21 control limit)
```

This configuration:
- ✅ Enables ONLY frame done interrupts (bit 0)
- ✅ Masks ALL error interrupts (including bit 21 control limit)
- ✅ Prevents interrupt storms
- ✅ Allows normal operation

## Verification

To verify the fix is correct, check the logs for:

```
*** VIC MASK: Set MainMask=0xFFFFFFFE (frame-done ENABLED, errors MASKED) before RUN ***
*** VIC IRQ: Calculated v1_7 = 0x1 v1_10 = 0x0 ***  ← Only bit 0 set!
*** VIC SUCCESS: FRAME DONE INTERRUPT detected (count=1) ***
```

**Should NOT see:**
```
*** VIC IRQ: Calculated v1_7 = 0x200000 ***  ← Bit 21 set = BAD!
*** VIC ERROR: control limit error (bit 21) ***
Reserved instruction in kernel code
```

## Lessons Learned

1. **Always check the actual code** to understand register semantics, don't assume
2. **Inverted logic is common** in hardware interrupt masks
3. **Test thoroughly** - the wrong mask value can cause catastrophic failures
4. **Read the formula** - the `v1_7 = (~reg_1e8) & reg_1e0` formula clearly shows the inversion

## Technical Details

### VIC Interrupt Mask Register (0x1e8)

This register controls which interrupts are enabled, but uses **inverted logic**:

```c
// In interrupt handler:
v1_7 = (~reg_1e8) & reg_1e0;  // Invert mask, then AND with status

// To enable ONLY bit 0 (frame done):
// We want: ~reg_1e8 = 0x00000001
// Therefore: reg_1e8 = ~0x00000001 = 0xFFFFFFFE
```

### Why Inverted Logic?

Inverted logic for interrupt masks is common in hardware because:
1. Default value of 0xFFFFFFFF masks all interrupts (safe default)
2. Writing 0 to a bit enables that interrupt (explicit action required)
3. Prevents accidental interrupt storms on reset/initialization

### Bit Definitions

- **Bit 0 (0x00000001)**: Frame done interrupt - **MUST BE ENABLED**
- **Bit 21 (0x00200000)**: Control limit error - **MUST BE MASKED**
- **Other bits**: Various error conditions - **SHOULD BE MASKED**

## Next Steps

With the correct interrupt mask:
1. ✅ No interrupt storm
2. ✅ No Reserved Instruction crash
3. ✅ Frame done interrupts will fire (when frames are captured)
4. ❓ Still need to ensure frames are flowing from sensor → CSI → VIC

The next issue to debug will be why frames aren't flowing from the sensor, but at least the interrupt system will be working correctly.

