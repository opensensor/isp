# CSI Pipeline Debugging

## Current Status

After fixing the buffer_count issue, the VIC control register now stays at `0x80000020` (no bit 17), but we're STILL getting control limit errors (bit 21) instead of frame done interrupts (bit 0).

## The Real Problem

The control limit errors are NOT caused by bit 17 in the control register. The problem is that **frames are not arriving from the sensor/CSI/ISP pipeline!**

### Evidence

From the logs:
```
VIC IRQ: CTRL[0x300]=0x80000020  ← Correct, no bit 17
VIC STAT: v1_7=0x00200000  ← Control limit error (bit 21)
VIC IRQ: No frame done interrupt (v1_7 & 1 = 0)  ← No frames!
```

ISP Core interrupts:
```
ISP CORE: interrupt_status=0x00000500  ← Bits 8 and 10 (AE stats)
ISP CORE: interrupt_status=0x00001000  ← Bit 12
```

**NO bit 0 (frame start) or bit 1 (frame done) from ISP Core!**

## What Control Limit Error Means

The VIC control limit error (bit 21) is triggered when:
- VIC is waiting for a frame from the ISP
- No frame arrives within the expected time window
- Hardware triggers a "control limit" timeout error

This is NOT a configuration error - it's a **data starvation error**!

## The Pipeline

```
Sensor (GC2053) → MIPI CSI → ISP Core → VIC → Memory
                    ↓           ↓         ↓
                  Data?     Processing? Waiting?
```

The sensor is streaming (`gc2053 stream on`), but frames are not reaching VIC.

## Possible Causes

1. **CSI not receiving MIPI data** - PHY not configured, lanes not synced
2. **ISP pipeline not enabled** - `system_reg_write(0x800, 1)` not working
3. **ISP pipeline stalled** - Waiting for something (buffers, config, etc.)
4. **ISP→VIC connection broken** - Routing registers not set correctly

## Changes Made

### 1. Added ISP Pipeline Enable Logging

**File:** `driver/tx_isp_tuning.c` Lines 2146-2154

Added debug logging around the critical ISP pipeline enable sequence:
```c
pr_info("*** tisp_init: CRITICAL - Enabling ISP pipeline: 0x804=0x%x, 0x1c=8, 0x800=1 ***\n", isp_mode);
system_reg_write(0x804, isp_mode);
system_reg_write(0x1c, 8);
system_reg_write(0x800, 1);
pr_info("*** tisp_init: ISP pipeline ENABLED - frames should now flow from CSI->ISP->VIC ***\n");
```

This will show if the ISP pipeline enable is actually being executed.

### 2. Added CSI Error Checking

**File:** `driver/tx-isp-module.c` Lines 2002-2007

Added CSI error checking when VIC doesn't get frame done interrupts:
```c
printk(KERN_ALERT "*** VIC IRQ: No frame done interrupt (v1_7 & 1 = 0) - checking CSI ***\n");

/* Check CSI for errors - frames may not be arriving from sensor */
extern void tx_isp_csi_check_errors(struct tx_isp_dev *isp_dev);
tx_isp_csi_check_errors(ourISPdev);
```

This will check CSI error registers and report:
- SOT Sync Error
- ECC errors
- CRC errors
- Frame sync errors
- Data ID errors
- PHY state

### 3. Removed Buffer Count from Control Register

**File:** `driver/tx_isp_vic.c` Lines 3100-3117

Changed from writing `buffer_count << 16` to just `0x80000020`:
```c
u32 stream_ctrl = 0x80000020;  // NO buffer count!
```

This fixed the bit 17 issue, but didn't solve the frame starvation problem.

## Expected Output After Rebuild

### If ISP Pipeline Enable is Working
```
tisp_init: CRITICAL - Enabling ISP pipeline: 0x804=0x1e, 0x1c=8, 0x800=1
tisp_init: ISP pipeline ENABLED - frames should now flow from CSI->ISP->VIC
```

### If CSI Has Errors
```
VIC IRQ: No frame done interrupt (v1_7 & 1 = 0) - checking CSI
CSI ERROR CHECK: err1=0x00000008, err2=0x00000002, phy_state=0x00000000
CSI Protocol errors (ERR1): 0x00000008
  - ECC Multi-bit Error (uncorrectable)
CSI Application errors (ERR2): 0x00000002
  - Frame Sync Error
```

### If CSI is Clean
```
VIC IRQ: No frame done interrupt (v1_7 & 1 = 0) - checking CSI
[No CSI error messages - CSI is receiving data correctly]
```

## Next Steps Based on Results

### If ISP Pipeline Enable Logs Don't Appear
- `tisp_init` is not being called or is failing early
- Need to check why ISP initialization is not completing

### If CSI Shows Errors
- MIPI PHY not configured correctly
- Lane count mismatch
- Clock/timing issues
- Sensor not actually streaming

### If CSI is Clean But No Frames
- ISP pipeline is stalled internally
- ISP→VIC routing is broken
- ISP needs additional configuration
- Buffer allocation issue

## Testing

```bash
cd /home/matteius/isp-latest/driver
make clean && make
sudo rmmod tx-isp-module 2>/dev/null
sudo insmod tx-isp-module.ko
```

**Look for:**
1. ISP pipeline enable messages
2. CSI error messages
3. ISP Core interrupt status values
4. VIC interrupt status values

## Files Modified

- `driver/tx_isp_tuning.c` - Lines 2146-2154 (ISP pipeline enable logging)
- `driver/tx-isp-module.c` - Lines 2002-2007 (CSI error checking)
- `driver/tx_isp_vic.c` - Lines 3100-3117 (removed buffer_count)

## Key Insight

The 10/15 stall is NOT a VIC configuration issue - it's a **data pipeline issue**. The VIC is correctly configured and waiting for frames, but frames are not arriving from upstream (CSI/ISP). We need to debug the CSI→ISP→VIC data flow, not the VIC interrupt configuration.

