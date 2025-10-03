# VIC Control Limit Error Fix

## Problem

Driver now has **continuous interrupts** (✅ fixed!) but gets **VIC control limit error (bit 21)**:

```
[   45.743097] *** VIC ERROR: control limit error (bit 21) ***
[   45.660624] *** VIC IRQ: No frame done interrupt (v1_7 & 1 = 0) ***
[   45.657550] *** VIC IRQ: CTRL[0x300]=0x80000020 DIMS[0x304]=0x7800438 STRIDE[0x310]=0x780 ***
```

## Analysis

### What's Configured Correctly
- ✅ VIC dimensions: 1920x1080 (0x7800438)
- ✅ VIC stride: 0x780 (1920 pixels * 2 bytes = 3840 bytes)
- ✅ Sensor configured for 1920x1080
- ✅ Interrupts are continuous (not stalling)

### What's Wrong
- ❌ **Control limit error (bit 21)** = VIC received data that doesn't match configuration
- ❌ **No frame done interrupt (bit 0)** = Frames not completing properly
- ❌ Frame index progresses (0x8000 → 0x8004) but no actual frames

## Root Cause

The **control limit error** means one of:
1. **Horizontal count mismatch**: Sensor sending different line length than VIC expects
2. **Vertical count mismatch**: Sensor sending different number of lines than VIC expects  
3. **Timing/blanking mismatch**: Sensor blanking periods don't match VIC configuration

## Key Observation from Logs

```
CTRL[0x300]=0x80000020  → Frame index 0
CTRL[0x300]=0x80040020  → Frame index 4
```

The frame index is incrementing (bits 4-7 of CTRL register), which means:
- VIC **is** receiving frame start/end signals
- VIC **is** trying to process frames
- But frames are **not completing** due to data mismatch

## Solution: Check Sensor vs VIC Configuration

### 1. Sensor Total Dimensions vs Active Dimensions

GC2053 sensor has:
- **Active area**: 1920x1080 (image data)
- **Total frame**: 2200x1125 (includes blanking)

VIC needs to be configured for:
- **Active dimensions** for image processing
- **Total dimensions** for timing/synchronization

### 2. Check MIPI Configuration

From logs, MIPI is configured as:
- Mode: 2 (MIPI CSI-2)
- Lanes: 2
- Data type: RAW10 or similar

VIC control limit error can occur if:
- MIPI virtual channel mismatch
- MIPI data type mismatch
- MIPI lane configuration mismatch

### 3. Check VIC Control Register Bits

```
CTRL[0x300] = 0x80000020
```

Breaking down:
- Bit 31 (0x80000000): VIC enable
- Bits 4-7 (0x00000020): Frame index = 2
- Other bits: Mode configuration

The working driver may have different mode bits set.

## Recommended Fixes

### Fix 1: Verify Sensor Blanking Configuration

Check if sensor is configured with correct blanking:
- Horizontal blanking (HTS - horizontal total size)
- Vertical blanking (VTS - vertical total size)

GC2053 should have:
- HTS = 2200 (0x898)
- VTS = 1125 (0x465)

### Fix 2: Check VIC Input Format Configuration

VIC register 0xC (config) should match sensor output:
- Bit 0-1: Input format (0=RAW8, 1=RAW10, 2=RAW12)
- Bit 2-3: MIPI mode
- Other bits: Various format settings

From logs: `VIC_CONFIG (0x0C) = 0x2` suggests RAW10 mode.

### Fix 3: Compare with Working Driver

The diff shows the working driver has simpler vic_core_s_stream.
Current driver has 300+ lines of complex register manipulation.

**Key difference**: Working driver doesn't do extensive re-configuration during streaming.

### Fix 4: Check ISP Core VIC Routing

ISP core registers 0x9ac0/0x9ac8 control VIC routing.
These may need to be set for proper frame done signaling.

From reference trace:
```
ISP isp-m0: [ISP Control] write at offset 0x9ac0: 0x0
ISP isp-m0: [ISP Control] write at offset 0x9ac8: 0x0
```

But current driver may have different values.

## Testing Plan

1. **Add debug logging** to show:
   - Sensor HTS/VTS values
   - VIC input format configuration
   - ISP core VIC routing registers

2. **Compare register dumps** between:
   - Working driver at frame done
   - Current driver at control limit error

3. **Try simplified vic_core_s_stream**:
   - Remove extensive register manipulation
   - Use minimal configuration like working driver

4. **Check sensor register dump**:
   - Verify sensor is outputting correct format
   - Verify sensor timing matches VIC expectations

## Next Steps

1. Get register dump from working driver during successful streaming
2. Get register dump from current driver at control limit error
3. Compare the two to find configuration differences
4. Apply minimal changes to match working configuration

## Expected Result

After fix:
- ✅ Continuous interrupts (already working)
- ✅ Frame done interrupts (bit 0 set)
- ✅ No control limit errors (bit 21 clear)
- ✅ Frames actually captured and available to userspace

