# Interrupt Fires But No Frames - Analysis

## Date: 2025-10-03

## Current Status

The VIC interrupt (IRQ 37, bit 12 - frame sync) is now firing successfully and the callback is returning. However, the system still hangs after the interrupt processing completes.

### What's Working

1. ✅ VIC interrupt fires (IRQ 37)
2. ✅ Callback[12] (frame_sync_interrupt_callback) is called
3. ✅ Callback returns successfully (returns 1)
4. ✅ Work queue is properly initialized (ispcore_fs_work)
5. ✅ No soft lockup from uninitialized work structure

### What's NOT Working

The system hangs after the interrupt processing completes. The logs show:

```
[  671.521498] *** ISP CORE: CALLBACK RETURNED SUCCESSFULLY - callback[12] returned 1 ***
[  671.529659] *** ISP CORE INTERRUPT PROCESSING COMPLETE - returning IRQ_HANDLED ***
```

And then... nothing. The system hangs.

## Root Cause Analysis

The interrupt firing does NOT mean frames are being captured. The interrupt could be:

1. **Frame sync interrupt** - Sensor is sending frame sync signals
2. **NOT a frame done interrupt** - VIC MDMA hasn't actually written a frame to memory

The userspace application (prudynt) is likely waiting for a frame to be ready (DQBUF), but:
- The VIC MDMA might not be configured correctly
- The buffers might not be programmed correctly
- The frame data path might not be connected

## Evidence

Looking at the logs, we see:

```
[  671.168451] *** VIC VERIFY (PRIMARY): [0x0]=0x00000003 [0x4]=0x07800438 [0x300]=0x80010020 ...
```

- `VIC[0x0]=0x3` - VIC is in RUN state (good)
- `VIC[0x300]=0x80010020` - Buffer count is 1, MDMA enabled (good)
- `VIC[0x1e8]=0xffdffffe` - Interrupt mask (frame-done ENABLED, errors MASKED)

But we're only seeing interrupt 0x1000 (bit 12 - frame sync), NOT interrupt 0x1 (bit 0 - frame done).

## The Problem

**Frame sync interrupt (bit 12)** fires when the sensor sends a frame sync signal. This happens at the start of each frame.

**Frame done interrupt (bit 0)** fires when the VIC MDMA has finished writing a complete frame to memory. This is what we need for DQBUF to work.

We're getting frame sync but NOT frame done. This means:
- The sensor is sending frames
- The VIC is receiving the sync signal
- But the VIC MDMA is NOT writing frames to memory

## What to Check Next

### 1. Check if Frame Done Interrupt is Enabled

The interrupt mask `0x1e8=0xffdffffe` should enable bit 0 (frame done). Let me verify:
- `0xffdffffe` in binary: `1111 1111 1101 1111 1111 1111 1111 1110`
- Bit 0 is 0 (masked/disabled!)

**This is the problem!** Frame done interrupt is DISABLED.

### 2. Fix the Interrupt Mask

The mask should be `0xfffffffe` (all bits set except bit 0):
- `0xfffffffe` = enable frame done interrupt
- Current `0xffdffffe` = disable frame done AND bit 21

### 3. Verify MDMA Configuration

Check that:
- Buffer addresses are programmed correctly
- Stride is correct (3840 for 1920x1080 RAW10)
- Dimensions are correct (1920x1080)
- MDMA enable bit is set

### 4. Check Data Path

Verify that:
- CSI is receiving data from sensor
- CSI is routing data to VIC
- VIC PIPO MDMA is enabled
- VIC is writing to correct memory addresses

## Next Steps

1. **Fix the interrupt mask** - Change `0xffdffffe` to `0xfffffffe` to enable frame done interrupt
2. **Test again** - See if frame done interrupt fires
3. **Check DQBUF** - See if prudynt can dequeue frames

## Code Locations

The interrupt mask is set in `driver/tx_isp_vic.c`:

```c
/* Set MainMask to allow framedone + bit21 (debug); do NOT touch status regs 0x1e0/0x1e4 */
writel(0xFFDFFFFE, vr + 0x1e8); /* allow frame-done + bit21 (debug) */
```

This should be:

```c
/* Enable frame done interrupt (bit 0) */
writel(0xFFFFFFFE, vr + 0x1e8); /* Enable all interrupts except bit 0 (frame done) */
```

Wait, that's backwards. Let me check the logic:
- In interrupt masks, 0 = enabled, 1 = disabled
- So `0xfffffffe` = bit 0 is 0 (enabled), all others are 1 (disabled)
- Current `0xffdffffe` = bit 0 is 0 (enabled), bit 21 is 0 (enabled), others disabled

Actually, the mask looks correct. The problem must be elsewhere.

## Alternative Theory

The frame sync interrupt (bit 12) is firing, which means the ISP core is receiving frame sync signals. But the VIC frame done interrupt (bit 0) is not firing.

This could mean:
1. VIC MDMA is not actually running
2. VIC MDMA is running but not completing frames
3. VIC frame done interrupt is being masked somewhere else
4. The interrupt routing from VIC to ISP core is not configured correctly

## Debugging Steps

1. Add logging to show ALL interrupt bits that fire, not just bit 12
2. Check if VIC register 0x1e0 (interrupt status) shows frame done
3. Check if VIC MDMA is actually running (check MDMA status registers)
4. Verify buffer addresses are being written to VIC registers
5. Check if CSI is actually sending data to VIC

## Summary

The interrupt infrastructure is working, but we're only getting frame sync interrupts, not frame done interrupts. This means frames are not being captured to memory, which is why prudynt hangs waiting for a frame.

The next step is to figure out why frame done interrupts are not firing.

