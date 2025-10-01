# Current Status: 4/4 Interrupt Stall

## Progress Made

✅ **Fixed VIC state machine** - VIC only starts 2 times (for ON/OFF/ON sequence)
✅ **Fixed tx_isp_subdev_pipo** - No longer calls streaming functions repeatedly  
✅ **CSI PHY configured** - MIPI PHY is enabled and configured correctly
✅ **ISP pipeline routing** - Register 0x804 = 0x1c (correct)
✅ **Sensor streaming** - gc2053 sensor is sending data

## Current Issue

❌ **VIC not receiving frames** - Control limit error (bit 21) on every interrupt
❌ **Only 4 interrupts** - System stalls after exactly 4 VIC interrupts

## Interrupt Analysis

```
/proc/interrupts shows:
 37:          4   jz-intc  isp-m0
 38:          4   jz-intc  isp-w02  ← VIC interrupts

Kernel logs show:
[   25.845689] VIC INTERRUPT HANDLER CALLED
[   25.977648] VIC IRQ: vic_start_ok=1, v1_7=0x200000, v1_10=0x0
[   26.669997] VIC INTERRUPT HANDLER CALLED
[   26.801935] VIC IRQ: vic_start_ok=1, v1_7=0x200000, v1_10=0x0
[Then no more interrupts]
```

**Every interrupt shows:**
- `v1_7 = 0x200000` (bit 21 = control limit error)
- `v1_10 = 0x0` (no MDMA interrupts)
- No frame done interrupt (bit 0 not set)

## What Control Limit Error Means

**Control limit error (bit 21) = VIC timeout waiting for frames from CSI**

The VIC hardware:
1. Expects frames to arrive from CSI within a certain time window
2. If no frame arrives within that window, sets bit 21 (control limit error)
3. Generates an interrupt
4. After some number of consecutive errors (appears to be 4), hardware stops generating interrupts

## CSI Configuration Status

✅ **CSI PHY enabled:**
```
[   25.660924] *** CSI MIPI: CRITICAL - Enabling CSI PHY: CSI[0x10] = 1 ***
[   25.660929] *** CSI MIPI: PHY ENABLED - MIPI data should now flow from sensor! ***
```

✅ **ISP_CSI registers configured:**
```
[   25.660909] *** CSI MIPI: Writing ISP_CSI[0x0] = 0x7d (timing config) ***
[   25.660919] *** CSI MIPI: Writing ISP_CSI[0x128] = 0x3f (lane config) ***
```

✅ **CSI lanes configured:**
```
CSI[0x4] = 1 (2 lanes - 1)
CSI[0xc] = 1 (MIPI mode)
```

✅ **Sensor streaming:**
```
[   26.448440] gc2053 stream on
```

## Possible Root Causes

### 1. CSI→VIC Data Path Not Connected

The CSI PHY may be receiving MIPI data from the sensor, but the data path from CSI to VIC may not be properly configured.

**Potential issues:**
- Missing CSI→VIC routing register
- VIC input source not set to CSI
- ISP core not forwarding CSI data to VIC

### 2. VIC Input Source Configuration

The VIC may need to be explicitly told to receive data from CSI (vs. other sources like DVP, memory, etc.).

**Need to check:**
- VIC register 0x8 (input source selection?)
- VIC register 0x300 (control register - currently 0x80000020)
- Any VIC routing/mux registers

### 3. ISP Core CSI Routing

The ISP core has CSI registers at offset 0x13300000 + CSI_offset. These may control how CSI data is routed to VIC.

**Already configured:**
- ISP_CSI[0x0] = 0x7d (timing)
- ISP_CSI[0x128] = 0x3f (lanes)

**May need:**
- Additional ISP_CSI routing registers
- ISP core input mux configuration

### 4. Clock/Timing Issues

The CSI PHY clock or VIC clock may not be synchronized properly, causing VIC to miss frames even though CSI is receiving them.

**Need to check:**
- CSI PHY clock frequency
- VIC clock frequency  
- Clock domain crossing configuration

### 5. MIPI CSI-2 Protocol Issues

The MIPI CSI-2 protocol has specific packet formats and timing requirements. The VIC may not be configured to properly parse the MIPI packets.

**Need to check:**
- MIPI virtual channel configuration
- MIPI data type configuration
- MIPI packet parsing settings in VIC

## Next Steps to Debug

### 1. Check VIC Input Source Register

Search Binary Ninja for VIC register writes around offset 0x8 or any "input source" or "mux" configuration.

### 2. Compare with Working Reference

Use Binary Ninja MCP to decompile the complete initialization sequence and compare with our implementation to find missing register writes.

### 3. Check ISP Core Routing

Look for ISP core registers that control CSI→VIC data path routing.

### 4. Hardware Register Dump

Add logging to dump all VIC and CSI registers after initialization to see if any critical registers are not set.

### 5. Check for Missing tisp_init Calls

The `tisp_init` function configures many ISP core registers. Verify it's being called with correct parameters and all register writes are happening.

## Files Modified in This Session

- `driver/tx_isp_vic.c` - Lines 4139-4143: Set processing = 1 (Binary Ninja EXACT)
- `driver/tx_isp_vic.c` - Lines 4214-4221: Remove streaming calls from tx_isp_subdev_pipo
- `driver/tx_isp_vic.c` - Lines 3498-3557: VIC state preservation during slake module
- `driver/tx_isp_vic.c` - Lines 3608-3618: Check stream_state in slake subdev

## Summary

We've successfully fixed the state machine issues that were causing repeated VIC starts. The driver now properly manages VIC state and only initializes VIC when needed.

However, the fundamental issue remains: **frames are not flowing from CSI to VIC**. The CSI PHY is configured and enabled, the sensor is streaming, but the VIC is not receiving any data, resulting in control limit errors and eventual stall after 4 interrupts.

The next step is to identify the missing configuration that connects the CSI data path to the VIC input, allowing frames to flow through the pipeline.

