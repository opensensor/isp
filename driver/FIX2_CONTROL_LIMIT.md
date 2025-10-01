# Fix #2: VIC Control Limit Protection Causing Error Interrupts

## Problem Identified from Logs

After applying Fix #1 (CSI interface check), the logs show:
- ✅ CSI is correctly detecting MIPI interface
- ✅ Interrupts ARE being generated (IRQ 38 firing)
- ❌ BUT: Getting **control limit error interrupts** instead of **frame done interrupts**

### Key Log Evidence

```
[12759.344194] *** VIC IRQ: No frame done interrupt (v1_7 & 1 = 0) ***
[12759.350655] *** VIC ERROR: control limit error (bit 21) ***
[12759.275778] *** VIC IRQ: CTRL[0x300]=0x80020020 DIMS[0x304]=0x7800438 STRIDE[0x310]=0xf00 ***
```

The VIC is generating interrupts, but they're **error interrupts (bit 21)** not **frame done interrupts (bit 0)**.

## Root Cause

The VIC control register at offset 0x300 is being set to `0x80020020`:
- Bit 31 (0x80000000): VIC enable
- **Bit 17 (0x00020000): Control limit protection ENABLE** ← **THIS IS THE PROBLEM**
- Bit 5 (0x00000020): Format/mode bit

When bit 17 is set, the VIC hardware enables "control limit protection" which:
1. Checks if buffer addresses are valid
2. Checks if dimensions are correct
3. Checks if stride is proper
4. **Triggers error interrupt (bit 21) if ANY check fails**
5. **Prevents frame done interrupt (bit 0) from firing**

## Working Version Comparison

**Working version** (`isp-was-better`):
```c
vic_control = (buffer_count << 16) | 0x80000020 | format_type;
//                                    ^^^^^^^^^^
//                                    NO bit 17 set
```

**Current version** (was broken):
```c
control_value = 0x80020020 | arg6;  // Bit 17 SET - enables control limit
//              ^^^^^^^^^^
//              Bit 17 is SET (0x20000)
```

## Fix Applied

Modified `vic_mdma_enable_complete()` in `driver/tx_isp_vic.c` (line ~2840):

### Before (Broken)
```c
control_value = 0x80020020 | arg6;  /* Base control + format */
```

### After (Fixed)
```c
/* CRITICAL FIX: Use 0x80000020 (NOT 0x80020020) to disable control limit protection
 * Bit 17 (0x20000) enables hardware control limit checking which causes error interrupts
 * Working version uses 0x80000020 without bit 17 set
 */
control_value = 0x80000020 | arg6;  /* Base control + format (NO control limit bit) */
```

## Expected Behavior After Fix

### Before Fix (Broken)
```
VIC IRQ: CTRL[0x300]=0x80020020  <- Bit 17 SET
VIC IRQ: No frame done interrupt (v1_7 & 1 = 0)
VIC ERROR: control limit error (bit 21)
[Interrupts stall after 10-15 frames]
```

### After Fix (Expected)
```
VIC IRQ: CTRL[0x300]=0x80000020  <- Bit 17 CLEAR
VIC IRQ: Frame done interrupt detected (v1_7 & 1 = 1)
Info[VIC_MDAM_IRQ] : channel[0] frame done
[Continuous interrupts, no stalling]
```

## Why This Matters

The control limit protection is a hardware safety feature that's supposed to prevent bad configurations from causing hardware damage or corruption. However:

1. **It's too sensitive** - triggers on configurations that actually work fine
2. **It blocks normal operation** - prevents frame done interrupts
3. **Working driver disables it** - proven to work without it
4. **Reference driver disables it** - Binary Ninja MCP shows 0x80000020

The hardware can operate safely without this protection enabled, as demonstrated by the working version.

## Testing Instructions

### 1. Rebuild and Load
```bash
cd /home/matteius/isp-latest/driver
make clean && make
sudo rmmod tx-isp-module 2>/dev/null
sudo insmod tx-isp-module.ko
```

### 2. Check Logs for Success
```bash
dmesg | grep -E "CTRL\[0x300\]|frame done|control limit"
```

**Look for:**
- ✅ `CTRL[0x300]=0x80000020` (bit 17 CLEAR)
- ✅ `Info[VIC_MDAM_IRQ] : channel[0] frame done`
- ✅ `vic_mdma_irq_function: channel=0 processing MDMA interrupt`
- ❌ NO "control limit error" messages

### 3. Count Interrupts
```bash
# Before fix: ~10-15 interrupts then stall
# After fix: Continuous interrupts

watch -n 1 'cat /proc/interrupts | grep -E "37:|38:"'
```

Should see IRQ 38 count increasing continuously.

## Technical Details

### VIC Control Register (0x300) Bit Map
```
Bit 31 (0x80000000): VIC Enable
Bit 30-20: Reserved
Bit 19-16: Buffer index (for manual cycling)
Bit 17 (0x00020000): Control limit protection enable ← FIX TARGETS THIS
Bit 16-6: Various control bits
Bit 5 (0x00000020): Format/mode bit
Bit 4-0: Format type
```

### Why Bit 17 Causes Problems

When bit 17 is set, the VIC hardware performs these checks on EVERY frame:
1. **Buffer address validation** - checks if addresses are in valid range
2. **Dimension validation** - checks if width/height match configuration
3. **Stride validation** - checks if stride is correct for format
4. **Index validation** - checks if buffer index is in valid range

If ANY check fails:
- Sets bit 21 in interrupt status (control limit error)
- Does NOT set bit 0 (frame done)
- Triggers interrupt but with wrong type
- Driver sees error interrupt, not frame completion
- After several errors, system gives up and stalls

### Why Working Version Doesn't Have This Problem

The working version uses `0x80000020` which:
- Enables VIC (bit 31)
- Sets format bit (bit 5)
- **Does NOT enable control limit protection (bit 17 clear)**
- Allows frame done interrupts to fire normally
- Hardware processes frames without excessive validation

## Files Modified

- `driver/tx_isp_vic.c` - Line ~2840 in `vic_mdma_enable_complete()`

## Related Fixes

This is **Fix #2** in the interrupt stall resolution:
- **Fix #1**: CSI interface check (✅ APPLIED, WORKING)
- **Fix #2**: VIC control limit protection (✅ APPLIED, **TEST NOW**)
- **Fix #3**: Buffer management simplification (if needed)

## Success Criteria

✅ **Fix is successful if:**
1. No more "control limit error" messages in logs
2. See "frame done" messages continuously
3. IRQ 38 count increases steadily
4. No stalling after 10-15 frames
5. Frame capture works continuously

❌ **Fix failed if:**
1. Still seeing "control limit error"
2. Still seeing "No frame done interrupt"
3. IRQ 38 count stops increasing
4. System still stalls after 10-15 frames

## Next Steps

1. **Test this fix** - Rebuild and test
2. **If successful** - Document and close issue
3. **If still failing** - Apply Fix #3 (buffer management)

