# Revert Summary - Back to Original "Latest" Driver

## What Was Reverted

I reverted the interrupt handling changes that were incorrectly brought over from the "was-better" driver:

### 1. **tx_vic_enable_irq()** - REVERTED to original with kernel IRQ calls
- **Was**: Simplified version that only set `irq_enabled` flag
- **Now**: Original version that calls `enable_irq()` at kernel level
- **Why**: The "latest" driver architecture requires kernel-level IRQ management

### 2. **tx_vic_disable_irq()** - REVERTED to original with kernel IRQ calls  
- **Was**: Simplified version that only cleared `irq_enabled` flag
- **Now**: Original version that calls `disable_irq()` at kernel level
- **Why**: The "latest" driver architecture requires kernel-level IRQ management

### 3. **vic_core_ops_init()** - REVERTED to original with IRQ enable/disable calls
- **Was**: Simplified version that only managed state transitions
- **Now**: Original version that calls `tx_vic_enable_irq()` / `tx_vic_disable_irq()`
- **Why**: The "latest" driver needs these calls as part of the proper initialization flow

### 4. **VIC Format Configuration** - ADDED sensor format to register 0x14
- **Was**: Writing `0x0` to VIC register 0x14 (format register)
- **Now**: Writing actual sensor format (RAW10 = 1) to register 0x14
- **Why**: VIC needs to know the sensor output format to process data correctly

## Current Status

### ✅ **FIXED: Continuous VIC Interrupts**
The driver now gets continuous VIC interrupts instead of stalling at frame 4/4:
```
[  171.822723] *** VIC IRQ: Got vic_dev=85fd0000 ***
[  171.933297] *** VIC IRQ: vic_start_ok=1, v1_7=0x0, v1_10=0x8 ***
```

### ❌ **CURRENT ISSUE: DMA Channel ID Overflow**
```
[  171.939754] *** VIC ERROR: dma chid overflow (bit 3) ***
```

This error means:
- **v1_10 = 0x8** (bit 3 set in VIC interrupt status register 0x1e4)
- **DMA channel ID overflow** - MDMA channel configuration is incorrect
- **No frame done interrupts** - v1_7 = 0x0 (bit 0 not set)

## Root Cause Analysis

The DMA channel ID overflow suggests:

1. **Buffer addresses not programmed correctly** - MDMA doesn't know where to write frames
2. **MDMA channel count mismatch** - Hardware expects different number of channels
3. **MDMA not properly enabled** - `vic_pipo_mdma_enable()` may not be called or configured correctly

## Next Steps

### Priority 1: Fix MDMA Configuration

Check the diff for `vic_pipo_mdma_enable()` differences:
- Buffer address programming
- Channel count configuration  
- MDMA enable sequence

### Priority 2: Verify Buffer Management

The working driver has specific buffer management:
- Uses VBM (Video Buffer Manager) buffers
- Programs buffer addresses to VIC registers 0x318-0x328
- Manages buffer ring properly

### Priority 3: Check MDMA Enable Sequence

The working driver calls `vic_pipo_mdma_enable()` in `ispvic_frame_channel_s_stream()`:
```c
/* Stream ON */
vic_pipo_mdma_enable(vic_dev);
```

The current driver may not be calling this at the right time or with the right parameters.

## Key Insight

The **control flow** differences between "was-better" and "latest" are ARCHITECTURAL:
- **"was-better"**: Simple interrupt management, just flags
- **"latest"**: Complex interrupt management with kernel IRQ calls

We can't just copy the simple approach from "was-better" to "latest" - they have different architectures. Instead, we need to:

1. Keep the "latest" driver's control flow (kernel IRQ calls, etc.)
2. Fix the MDMA configuration to match what the hardware expects
3. Ensure buffer addresses are programmed correctly

## Files Modified

1. `driver/tx_isp_vic.c`:
   - Reverted `tx_vic_enable_irq()` to original
   - Reverted `tx_vic_disable_irq()` to original
   - Reverted `vic_core_ops_init()` to original
   - Added sensor format write to register 0x14

## Test Results

**Before revert**: Stalled at frame 4/4, no interrupts
**After revert**: Continuous interrupts, but DMA channel ID overflow

This is PROGRESS - we went from "no interrupts" to "continuous interrupts with DMA error". The next step is fixing the DMA configuration.

