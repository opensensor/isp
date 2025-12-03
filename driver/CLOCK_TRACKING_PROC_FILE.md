# ISP Clock Tracking via /proc/jz/isp/clks

## Overview

Added a new `/proc/jz/isp/clks` file that provides real-time visibility into the ISP driver's clock management. This helps debug clock-related issues and verify that clocks are being enabled/disabled correctly.

## Features

### 1. Proc File: `/proc/jz/isp/clks`

Read this file to see the current status of all ISP-related clocks:

```bash
cat /proc/jz/isp/clks
```

**Output Format:**
```
ISP Clock Status
================

cgu_isp:  rate=120000000 Hz
isp_clk:  rate=200000000 Hz
ipu_clk:  rate=200000000 Hz
csi_clk:  rate=100000000 Hz

CSI Subdevice Clocks (2):
  csi[0]:   rate=100000000 Hz
  csi[1]:   rate=24000000 Hz

VIC Subdevice Clocks (1):
  vic[0]:   rate=200000000 Hz

VIN Clock:
  vin_clk:  rate=150000000 Hz

Note: Check dmesg for [CLK] tagged messages to see enable/disable events
```

### 2. Kernel Log Messages

All clock enable/disable operations are now logged with `[CLK]` prefix in dmesg:

```bash
dmesg | grep '\[CLK\]'
```

**Example Output:**
```
[CLK] Enabling CGU ISP clock (rate=120000000 Hz)
[CLK] Enabling ISP clock (rate=200000000 Hz)
[CLK] Enabling IPU clock (rate=200000000 Hz)
[CLK] Enabling CSI clock (rate=100000000 Hz)
[CLK] CSI: Enabling clock 0 (rate=100000000 Hz)
[CLK] VIC: Enabling clock 0 (rate=200000000 Hz)
[CLK] VIN: Enabling VIN clock (rate=150000000 Hz)
[CLK] Subdev: Enabling clock 'cgu_cim' (rate=24000000 Hz)
[CLK] Subdev: Clock 'cgu_cim' enabled successfully
```

## Implementation Details

### Modified Files

1. **tx_isp_proc.c**
   - Added `TX_ISP_PROC_CLKS_FILE` definition
   - Added `clks_entry` to `proc_context` structure
   - Implemented `tx_isp_proc_clks_show()` to display clock status
   - Added proc file creation/cleanup in `tx_isp_create_proc_entries()` and `tx_isp_remove_proc_entries()`

2. **tx_isp_core.c**
   - Added logging to `private_clk_enable()` and `private_clk_disable()`
   - Added logging to `tx_isp_configure_clocks()` for main ISP clocks
   - Added logging to `ispcore_slake_module()` clock disable operations

3. **tx_isp_csi.c**
   - Added logging to CSI clock enable in `tx_isp_csi_activate_subdev()`
   - Added logging to CSI clock disable in `tx_isp_csi_slake_subdev()`

4. **tx_isp_vic.c**
   - Added logging to VIC clock enable in `tx_isp_vic_activate_subdev()`

5. **tx_isp_subdev.c**
   - Added logging to subdevice clock initialization in `isp_subdev_init_clks()`
   - Added logging to clock cleanup

6. **tx-isp-module.c**
   - Added logging to module clock enable operations
   - Added logging to module cleanup clock disable

7. **tx_isp_vin.c**
   - Added logging to VIN clock enable/disable operations

### Tracked Clocks

The system tracks the following clocks:

**Main ISP Clocks:**
- `cgu_isp` - CGU ISP clock (typically 120 MHz)
- `isp_clk` - ISP core clock (typically 200 MHz)
- `ipu_clk` - IPU clock (typically 200 MHz)
- `csi_clk` - CSI clock (typically 100 MHz)

**Subdevice Clocks:**
- CSI subdevice clocks (array, typically 1-2 clocks)
- VIC subdevice clocks (array, typically 1 clock)
- VIN clock (single clock)

**Sensor Clocks:**
- Sensor-specific clocks initialized via subdev platform data

## Usage Examples

### Check Current Clock Status

```bash
cat /proc/jz/isp/clks
```

### Monitor Clock Enable/Disable Events

```bash
# Watch in real-time
dmesg -w | grep '\[CLK\]'

# Or check recent events
dmesg | grep '\[CLK\]' | tail -20
```

### Debug Clock Issues

If you suspect clock-related problems:

1. Check if clocks are acquired:
   ```bash
   cat /proc/jz/isp/clks | grep "NOT ACQUIRED"
   ```

2. Verify clock rates:
   ```bash
   cat /proc/jz/isp/clks | grep "rate="
   ```

3. Check enable/disable sequence:
   ```bash
   dmesg | grep '\[CLK\]' | grep -E 'Enabling|Disabling'
   ```

### Common Clock Event Sequences

**Driver Load:**
```
[CLK] Enabling CGU ISP clock (rate=120000000 Hz)
[CLK] Enabling ISP clock (rate=200000000 Hz)
[CLK] Enabling IPU clock (rate=200000000 Hz)
[CLK] Enabling CSI clock (rate=100000000 Hz)
```

**Stream Start:**
```
[CLK] CSI: Enabling clock 0 (rate=100000000 Hz)
[CLK] VIC: Enabling clock 0 (rate=200000000 Hz)
```

**Stream Stop:**
```
[CLK] CSI: Disabling clock 0
[CLK] ispcore_slake_module: Disabling CSI clock
[CLK] ispcore_slake_module: Disabling IPU clock
[CLK] ispcore_slake_module: Disabling ISP clock
[CLK] ispcore_slake_module: Disabling CGU ISP clock
```

## Benefits

1. **Real-time Visibility**: See current clock rates without parsing complex register dumps
2. **Debug Aid**: Quickly identify if clocks are missing or misconfigured
3. **Event Tracking**: Kernel logs show the exact sequence of clock operations
4. **Non-intrusive**: Reading the proc file doesn't affect driver operation
5. **Comprehensive**: Covers all clock domains (main ISP, CSI, VIC, VIN, sensors)

## Notes

- The proc file is read-only (mode 0444)
- Clock rates are queried in real-time using `clk_get_rate()`
- The `[CLK]` log messages use `printk(KERN_ALERT )` level, visible in dmesg
- Clock enable/disable events are logged at the point of operation, not just at the wrapper level
- This feature adds minimal overhead (only when reading the proc file or during clock operations)

## Future Enhancements

Potential improvements:
- Add enable/disable counters to track clock state transitions
- Add timestamps to show when clocks were last enabled/disabled
- Add clock parent/source information
- Add clock gating status from CPM registers

