# CSI PHY Configuration Debugging

## Correct Pipeline Flow

**You were absolutely right!** The pipeline is:

```
Sensor → MIPI CSI → VIC → ISP → Memory
         ↓           ↓     ↓
       PHY?      Waiting  Processing
```

**VIC control limit error means VIC is NOT receiving frames from CSI!**

## The Problem

VIC is correctly configured and waiting for frames, but CSI is not delivering them. The control limit error (bit 21) is a **timeout error** - VIC waits for a frame from CSI, no frame arrives within the expected time window, hardware triggers control limit error.

## CSI MIPI Configuration

The CSI needs to be properly configured to receive MIPI data from the sensor and forward it to VIC. Key steps:

### 1. Lane Configuration
```c
CSI[0x4] = lanes - 1  // Number of MIPI lanes (typically 1 for 2-lane MIPI)
```

### 2. Reset Sequence
```c
CSI[0x8] &= ~1        // Clear bit 0
CSI[0xc] = 0          // Reset
CSI[0x10] &= ~1       // PHY disable
msleep(1);
```

### 3. Enable MIPI Mode
```c
CSI[0xc] = 1          // Enable MIPI mode (interface_type)
msleep(1);
```

### 4. Timing Configuration
```c
ISP_CSI[0x0] = 0x7d       // Timing config
ISP_CSI[0x128] = 0x3f     // Lane config
ISP_CSI[0x160] = ...      // Frame rate config
```

### 5. **CRITICAL: Enable CSI PHY**
```c
CSI[0x10] = 1         // ← ENABLE PHY - MIPI data flows!
msleep(10);
```

**This is the critical step that actually enables MIPI data reception!**

## Changes Made

### driver/tx_isp_csi.c - Lines 383-419

Added detailed logging for CSI MIPI initialization:
```c
pr_info("*** CSI MIPI INIT: Configuring MIPI CSI for %d lanes ***\n", sensor_attr->mipi.lans);
pr_info("*** CSI[0x4] = %d (lanes - 1) ***\n", sensor_attr->mipi.lans - 1);
pr_info("*** CSI[0x8] = 0x%x (clear bit 0) ***\n", reg8_val);
pr_info("*** CSI[0xc] = 0 (reset) ***\n");
pr_info("*** CSI[0x10] = 0x%x (PHY disable) ***\n", reg10_val);
pr_info("*** CSI[0xc] = %d (MIPI enable) ***\n", interface_type);
```

### driver/tx_isp_csi.c - Lines 485-501

Added logging for PHY enable:
```c
pr_info("*** CSI MIPI: Writing ISP_CSI[0x0] = 0x7d (timing config) ***\n");
pr_info("*** CSI MIPI: Writing ISP_CSI[0x128] = 0x3f (lane config) ***\n");
pr_info("*** CSI MIPI: CRITICAL - Enabling CSI PHY: CSI[0x10] = 1 ***\n");
pr_info("*** CSI MIPI: PHY ENABLED - MIPI data should now flow from sensor! ***\n");
```

## Expected Output After Rebuild

### If CSI PHY is Being Configured
```
CSI MIPI INIT: Configuring MIPI CSI for 2 lanes
CSI[0x4] = 1 (lanes - 1)
CSI[0x8] = 0x... (clear bit 0)
CSI[0xc] = 0 (reset)
CSI[0x10] = 0x... (PHY disable)
CSI[0xc] = 1 (MIPI enable)
CSI MIPI: Writing ISP_CSI[0x0] = 0x7d (timing config)
CSI MIPI: Writing ISP_CSI[0x128] = 0x3f (lane config)
CSI MIPI: CRITICAL - Enabling CSI PHY: CSI[0x10] = 1
CSI MIPI: PHY ENABLED - MIPI data should now flow from sensor!
```

### If CSI PHY Enable is Missing
```
[No CSI MIPI messages]
VIC: control limit error (bit 21)  ← VIC waiting for frames
[10/15 stall]
```

### If CSI PHY is Enabled Correctly
```
CSI MIPI: PHY ENABLED - MIPI data should now flow from sensor!
VIC IRQ: Frame done interrupt (v1_7 & 1 = 1)  ← VIC receiving frames!
ISP CORE: interrupt_status=0x1 or 0x2  ← ISP processing frames!
[No control limit errors]
[No stalling!]
```

## Possible Issues

### 1. CSI PHY Not Being Enabled
- `csi_core_ops_init` not being called
- `enable` parameter is 0 instead of 1
- Function returns early due to state check

### 2. CSI PHY Enabled But No Data
- Sensor not actually streaming
- MIPI lane count mismatch
- MIPI clock/timing issues
- Sensor power/reset issues

### 3. CSI Receiving Data But Not Forwarding to VIC
- CSI→VIC routing not configured
- VIC not ready to receive
- Buffer addresses not set

## Testing

```bash
cd /home/matteius/isp-latest/driver
make clean && make
sudo rmmod tx-isp-module 2>/dev/null
sudo insmod tx-isp-module.ko
```

**Look for:**
1. CSI MIPI initialization messages
2. CSI PHY enable message
3. VIC frame done interrupts (not control limit errors)
4. ISP frame processing interrupts

## Key Insight

The 10/15 stall is caused by VIC waiting for frames that never arrive from CSI. The CSI MIPI PHY must be properly configured and enabled for MIPI data to flow from the sensor to VIC. Without proper CSI configuration, VIC times out waiting for frames and triggers control limit errors.

The pipeline is:
1. **Sensor** generates frames
2. **CSI PHY** receives MIPI data (must be enabled!)
3. **VIC** captures frames from CSI
4. **ISP** processes frames from VIC
5. **Memory** stores processed frames

If CSI PHY is not enabled or misconfigured, the entire pipeline stalls at step 2!

## Files Modified

- `driver/tx_isp_csi.c` - Lines 383-419 (CSI MIPI init logging)
- `driver/tx_isp_csi.c` - Lines 485-501 (CSI PHY enable logging)
- `driver/tx_isp_tuning.c` - Lines 2144-2152 (ISP routing fix: 0x1c)
- `driver/tx_isp_tuning.c` - Lines 2052-2058 (ISP interrupt enable logging)
- `driver/tx-isp-module.c` - Lines 2002-2007 (CSI error checking)
- `driver/tx_isp_vic.c` - Lines 3100-3117 (removed buffer_count)

