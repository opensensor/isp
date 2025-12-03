# CSI Lane Configuration Fix for MIPI Sensors

## Problem Summary

The ISP driver was missing proper CSI lane configuration for MIPI sensors like the GC2053 (2-lane MIPI). This caused the VIC to not receive proper MIPI data, resulting in no video output.

## Root Cause Analysis

### Binary Ninja MCP Analysis

Using Binary Ninja MCP to decompile the reference driver (`tx-isp-t31.ko`), we found:

1. **`csi_core_ops_init` function** (address 0x4af0):
   - Writes `(lanes - 1)` to CSI register offset 0x4
   - Configures CSI PHY registers at offset 0x128 with lane enable mask
   - For 2-lane MIPI: writes `0x1` to CSI[0x4] (2-1=1)

2. **`csi_set_on_lanes` function** (address 0x537c):
   - Dedicated function for setting MIPI lane count
   - Uses read-modify-write pattern on CSI[0x4]
   - Masks bits 0-1 to set lane configuration

3. **`tx_isp_vic_start` function** (address 0x260):
   - Configures VIC registers for MIPI mode
   - Does NOT directly configure CSI lanes (that's CSI's job)
   - Coordinates with CSI through hardware registers

## Key Register Mappings

### CSI Registers (Base: 0x10022000)

- **Offset 0x4**: Lane configuration
  - Bits 0-1: Lane count minus 1
  - `0b00` (0): 1 lane
  - `0b01` (1): 2 lanes
  - `0b10` (2): 3 lanes
  - `0b11` (3): 4 lanes

- **Offset 0x8**: CSI control register
  - Bit 0: CSI enable

- **Offset 0xc**: CSI mode register
  - Value 0: Disabled
  - Value 1: MIPI mode

- **Offset 0x10**: CSI PHY enable
  - Value 1: PHY enabled

### CSI PHY Registers (Base: 0x10022000, accessed via isp_csi_regs)

- **Offset 0x0**: Timing configuration
  - Value 0x7d: Standard MIPI timing

- **Offset 0x128**: Lane enable mask
  - For 1-lane: 0x31 (bit 0 + control bits)
  - For 2-lane: 0x33 (bits 0-1 + control bits)
  - For 4-lane: 0x3f (bits 0-3 + control bits)

- **Offset 0x160, 0x1e0, 0x260**: Frame rate configuration
  - Calculated based on sensor total width
  - Used for MIPI timing synchronization

## Fixes Applied

### 1. CSI Lane Configuration (tx_isp_csi.c)

**Location**: `csi_core_ops_init` function, line 505-543

**Changes**:
```c
/* CRITICAL FIX: Write lane configuration to CSI register FIRST */
u32 lane_config = (sensor_attr->mipi.lans - 1) & 0x3;  /* Mask to 2 bits */
writel(lane_config, csi_dev->csi_regs + 4);
printk(KERN_ALERT lanes=%d, lanes-1=%d) ***\n", 
        lane_config, sensor_attr->mipi.lans, lane_config);

/* Read back to verify */
u32 lane_readback = readl(csi_dev->csi_regs + 4);
printk(KERN_ALERT should be 0x%x) ***\n", 
        lane_readback, lane_config);
```

**Why**: Ensures the lane count is properly written and verified before PHY initialization.

### 2. CSI PHY Lane Enable Mask (tx_isp_csi.c)

**Location**: `csi_core_ops_init` function, line 609-635

**Changes**:
```c
/* CRITICAL FIX: Configure CSI PHY lane enable mask based on actual lane count */
u32 lane_enable_mask;
if (sensor_attr->mipi.lans == 1) {
    lane_enable_mask = 0x31;  /* 1 lane: bit 0 + control bits */
} else if (sensor_attr->mipi.lans == 2) {
    lane_enable_mask = 0x33;  /* 2 lanes: bits 0-1 + control bits */
} else if (sensor_attr->mipi.lans == 4) {
    lane_enable_mask = 0x3f;  /* 4 lanes: bits 0-3 + control bits */
} else {
    lane_enable_mask = 0x3f;  /* Default to 4-lane config */
}
writel(lane_enable_mask, isp_csi_regs + 0x128);
printk(KERN_ALERT lane enable mask for %d lanes) ***\n",
        lane_enable_mask, sensor_attr->mipi.lans);
```

**Why**: The reference driver uses different lane enable masks for different lane counts. The control bits (bits 4-5) enable additional MIPI PHY features.

## Sensor Configuration

### GC2053 Sensor (sensor-src/t31/gc2053.c)

**MIPI Configuration** (line 162-170):
```c
struct tx_isp_mipi_bus sensor_mipi = {
    .mode = SENSOR_MIPI_OTHER_MODE,
    .clk = 600,
    .lans = 2,  /* ← 2-lane MIPI configuration */
    .settle_time_apative_en = 1,
    .mipi_sc.sensor_csi_fmt = TX_SENSOR_RAW10,
    .mipi_sc.hcrop_diff_en = 0,
    .mipi_sc.mipi_vcomp_en = 1,
    .mipi_sc.mipi_hcomp_en = 1,
};
```

**Interface Type** (line 206):
```c
.dbus_type = TX_SENSOR_DATA_INTERFACE_MIPI,  /* MIPI interface */
```

## Testing and Verification

### Expected Kernel Log Output

When the driver loads correctly, you should see:

```
*** CSI MIPI INIT: Configuring MIPI CSI for 2 lanes ***
*** CSI[0x4] = 0x1 (lanes=2, lanes-1=1) ***
*** CSI[0x4] READBACK = 0x1 (should be 0x1) ***
*** CSI MIPI: Writing ISP_CSI[0x0] = 0x7d (timing config) ***
*** CSI MIPI: Writing ISP_CSI[0x128] = 0x33 (lane enable mask for 2 lanes) ***
*** CSI MIPI: CRITICAL - Enabling CSI PHY: CSI[0x10] = 1 ***
*** CSI MIPI: PHY ENABLED - MIPI data should now flow! ***
```

### Register Verification

You can verify the configuration by reading the CSI registers:

```bash
# Read CSI lane configuration (should be 0x1 for 2 lanes)
devmem 0x10022004 32

# Read CSI PHY enable (should be 0x1)
devmem 0x10022010 32

# Read CSI PHY lane enable mask (should be 0x33 for 2 lanes)
devmem 0x10022128 32
```

## Hardware State Flow

1. **CSI Initialization** (`csi_core_ops_init`):
   - Write lane count to CSI[0x4]
   - Configure CSI PHY timing (CSI_PHY[0x0] = 0x7d)
   - Configure lane enable mask (CSI_PHY[0x128] = 0x33 for 2 lanes)
   - Enable CSI PHY (CSI[0x10] = 1)

2. **VIC Initialization** (`tx_isp_vic_start`):
   - Configure VIC for MIPI mode (VIC[0xc] = 2)
   - Set dimensions (VIC[0x4])
   - Configure MIPI timing registers
   - Enable VIC (VIC[0x0] = 1)

3. **Sensor Streaming** (`sensor_s_stream`):
   - Sensor starts outputting MIPI data
   - CSI PHY receives data on configured lanes
   - VIC processes the MIPI data
   - Frames are delivered to userspace

## Related Files

- **driver/tx_isp_csi.c**: CSI lane configuration
- **driver/tx_isp_vic.c**: VIC MIPI mode configuration
- **driver/sensor-src/t31/gc2053.c**: GC2053 sensor MIPI configuration
- **driver/include/tx-isp-common.h**: MIPI interface type definitions

## References

- Binary Ninja MCP decompilation of `tx-isp-t31.ko`
- Function `csi_core_ops_init` at address 0x4af0
- Function `csi_set_on_lanes` at address 0x537c
- Function `tx_isp_vic_start` at address 0x260

