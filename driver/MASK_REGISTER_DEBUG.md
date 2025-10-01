# Mask Register Debugging

## Current Status

We're setting the mask register to `0xFFDFFFFF` (frame done enabled, control limit masked), but when the interrupt fires, the mask register reads back as `0xffdffffe` (frame done MASKED, control limit enabled).

## The Problem

```
We write:    0xFFDFFFFF = ...1111 1111 1111 (bit 0 = 1, bit 21 = 0)
We read:     0xffdffffe = ...1111 1111 1110 (bit 0 = 0, bit 21 = 0)
                                          ^
                                    Bit 0 cleared!
```

Bit 0 is being cleared somewhere between when we write it and when the interrupt handler reads it.

## Possible Causes

1. **Hardware clears bit 0 on RUN state transition**
   - Writing 1 to register 0x0 (RUN) might reset the mask
   
2. **IMR/IMCR writes affect the mask**
   - The IMR (0x04) and IMCR (0x0c) routing registers might control which interrupts can be enabled
   
3. **Mask register is write-only or has special semantics**
   - Maybe bit 0 has inverted logic or special behavior
   
4. **Wrong register bank**
   - We're writing to PRIMARY (0x133e0000) but maybe need to write to CONTROL (0x10023000)

## Debug Changes Made

Added readback checks in `tx_isp_vic.c`:

### After writing mask (line 3379)
```c
writel(0xFFDFFFFF, vr + 0x1e8);
wmb();
u32 readback_mask = readl(vr + 0x1e8);
pr_info("*** VIC MASK: Set MainMask=0xFFDFFFFF, readback=0x%08x ***\n", readback_mask);
```

### After RUN state (line 3388)
```c
writel(1, vr + 0x0);
wmb();
u32 mask_after_run = readl(vr + 0x1e8);
pr_info("*** VIC CONTROL (PRIMARY): WROTE 1 to [0x0], mask after RUN=0x%08x ***\n", mask_after_run);
```

## What To Look For In Logs

### Scenario 1: Write doesn't stick
```
VIC MASK: Set MainMask=0xFFDFFFFF, readback=0xffdffffe
```
→ Hardware doesn't accept bit 0 = 1, or register has special semantics

### Scenario 2: RUN clears bit 0
```
VIC MASK: Set MainMask=0xFFDFFFFF, readback=0xffdfffff  ← Good
VIC CONTROL: WROTE 1 to [0x0], mask after RUN=0xffdffffe  ← Bit 0 cleared!
```
→ RUN state transition clears the frame done mask bit

### Scenario 3: Something else clears it
```
VIC MASK: Set MainMask=0xFFDFFFFF, readback=0xffdfffff  ← Good
VIC CONTROL: WROTE 1 to [0x0], mask after RUN=0xffdfffff  ← Still good
[Later in interrupt handler]
VIC IRQ: Using base ... [1e8]=0xffdffffe  ← Cleared later!
```
→ Something between RUN and interrupt clears bit 0

## Possible Solutions

### If write doesn't stick:
- Check if bit 0 has inverted logic (0 = enabled, 1 = masked)
- Check if we need to write to a different register first
- Check if IMR/IMCR must be configured before mask

### If RUN clears it:
- Write mask AFTER entering RUN state
- Or re-write mask after RUN
- Or use a different initialization sequence

### If something else clears it:
- Find what's clearing it and fix that code
- Or re-write mask in interrupt handler (not ideal)

## Register Documentation Needed

We need to understand:
1. VIC register 0x1e8 (interrupt mask) - exact semantics
2. VIC register 0x0 (control/state) - what happens on state transitions
3. IMR (0x04) and IMCR (0x0c) - how they interact with 0x1e8
4. Whether PRIMARY and CONTROL banks have different behavior

## Next Steps

1. **Rebuild and check logs** for the readback values
2. **Compare with working version** - what mask value does it use?
3. **Check Binary Ninja reference** - how does it set the mask?
4. **Try alternative approaches** based on what we learn

## Files Modified

- `driver/tx_isp_vic.c` - Lines 3370-3389 (added readback debugging)

