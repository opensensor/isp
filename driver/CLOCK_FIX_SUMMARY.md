# Clock Initialization Fix - Summary

## Problem
The clock initialization was failing with error -22 (EINVAL) when trying to set clock rates:
```
Failed to set ISP clock rate
*** tx_isp_configure_clocks failed: -22 ***
```

## Root Cause
The `tx_isp_configure_clocks()` function was trying to call `clk_set_rate()` on clocks that:
1. Don't support runtime rate changes on this platform
2. Are already configured correctly by bootloader/device tree
3. Should use auto-rate (0xffff) according to the reference driver's platform data

## Solution
Simplified `tx_isp_configure_clocks()` to:
1. **Get clocks** using `clk_get()`
2. **Set cgu_isp rate to 100MHz** - required for proper ISP operation
3. **Enable clocks** using `clk_prepare_enable()` in correct order
4. **Auto-rate for isp and csi clocks** - use pre-configured rates

This matches the reference driver's platform data: `cgu_isp` at 100MHz, others at `0xffff` (auto-rate).

## Changes Made

### 1. tx_isp_core.c - tx_isp_configure_clocks()
- Removed all `clk_set_rate()` calls
- Removed `ipu_clk` (not needed - only 3 clocks required)
- Simplified to just get + enable clocks
- Added logging to show actual clock rates

### 2. tx_isp_core.c - ispcore_slake_module()
- Removed `ipu_clk` disable code

### 3. tx_isp_proc.c - tx_isp_proc_clks_show()
- Removed `ipu_clk` from proc output

### 4. include/tx_isp.h
- Removed `ipu_clk` from `struct tx_isp_dev`
- Updated comments to clarify only 3 clocks are used

## The 3 Clocks

### 1. cgu_isp (CGU ISP Clock)
- Parent clock for ISP subsystem
- Must be enabled first
- **Rate: 100MHz (explicitly set)** - REQUIRED for proper ISP operation

### 2. isp (ISP Core Clock)
- ISP core processing clock
- Depends on cgu_isp
- Rate: Auto (pre-configured, typically 200MHz)

### 3. csi (CSI Interface Clock)
- CSI-2 interface clock
- Depends on cgu_isp
- Rate: Auto (pre-configured, typically 100MHz)

## Initialization Flow

```
ispcore_core_ops_init()
  └─> tx_isp_configure_clocks()
      ├─> clk_get("cgu_isp")
      ├─> clk_get("isp")
      ├─> clk_get("csi")
      ├─> clk_set_rate(cgu_isp, 100MHz)  ← CRITICAL: Set parent clock rate
      ├─> clk_prepare_enable(cgu_isp)    ← Parent first
      ├─> clk_prepare_enable(isp)        ← Auto-rate
      ├─> clk_prepare_enable(csi)        ← Auto-rate
      └─> msleep(10)  ← Stabilization delay
```

## Testing
After this fix, verify:
1. ✓ No clock initialization errors
2. ✓ All 3 clocks enabled successfully
3. ✓ Clock rates logged correctly
4. ✓ CSI interface works

## Expected Log Output
```
[CLK] Configuring ISP system clocks
[CLK] Setting CGU ISP clock rate to 100MHz (current=75000000 Hz)
[CLK] CGU ISP clock rate set to 100000000 Hz
[CLK] Enabling CGU ISP clock (rate=100000000 Hz)
[CLK] Enabling ISP clock (rate=200000000 Hz)
[CLK] Enabling CSI clock (rate=100000000 Hz)
[CLK] All ISP clocks enabled successfully
[CLK]   cgu_isp: 100000000 Hz  ← Must be 100MHz
[CLK]   isp:     200000000 Hz
[CLK]   csi:     100000000 Hz
```

## Key Insights
1. **cgu_isp must be 100MHz** - This is explicitly set via `clk_set_rate()`
2. **isp and csi use auto-rate** - Platform data uses `0xffff` meaning "don't set rate"
3. **Original error was trying to set all rates** - Only cgu_isp needs explicit rate setting

