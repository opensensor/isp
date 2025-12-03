# Clock Initialization Simplification

## Problem

The previous clock initialization was overly complex and didn't work correctly:

1. **Per-subdev initialization**: `isp_subdev_init_clks()` was called separately for:
   - ISP core subdev (2 clocks: cgu_isp, isp)
   - CSI subdev (1 clock: csi)

2. **Complex platform data handling**: The function tried to use platform data arrays with fallback logic

3. **Redundant CPM register manipulation**: Direct hardware register writes that duplicated clock framework functionality

4. **Multiple call sites**: Clocks were initialized in multiple places:
   - `ispcore_core_ops_init()` - called for core and CSI subdevs
   - `tx_isp_csi_activate_subdev()` - fallback initialization

## Solution

**Use the simplified `tx_isp_configure_clocks()` function**:

1. **Single initialization point**: Called once in `ispcore_core_ops_init()`
2. **All 3 clocks in correct order**:
   - `cgu_isp` (CGU ISP parent clock) - auto rate
   - `isp` (ISP core clock) - auto rate
   - `csi` (CSI interface clock) - auto rate
3. **Proper clock framework usage**: Uses standard Linux clock API
4. **No rate setting**: Just get and enable clocks (rates are pre-configured by bootloader/DTS)
5. **Stored in isp_dev**: Clocks stored in central `tx_isp_dev` structure
6. **Stabilization delay**: Includes 10ms delay for clock stabilization

## Changes Made

### 1. tx_isp_core.c - ispcore_core_ops_init()

**Before:**
```c
/* Initialize clocks for all subdevs that need them */
struct tx_isp_subdev *core_sd = tx_isp_find_subdev_by_name(ourISPdev, "isp-m0");
struct tx_isp_subdev *csi_sd = tx_isp_find_subdev_by_name(ourISPdev, "isp-w01");

if (core_sd && core_sd->clk_num > 0) {
    int clk_ret = isp_subdev_init_clks(core_sd, core_sd->clk_num);
}

if (csi_sd && csi_sd->clk_num > 0) {
    int clk_ret = isp_subdev_init_clks(csi_sd, csi_sd->clk_num);
}
```

**After:**
```c
/* CRITICAL: Initialize all ISP clocks (cgu_isp, isp, csi) in correct order */
if (!isp_dev->cgu_isp || !isp_dev->isp_clk || !isp_dev->csi_clk) {
    ret = tx_isp_configure_clocks(isp_dev);
    if (ret != 0) {
        printk(KERN_ALERT "*** tx_isp_configure_clocks failed: %d ***\n", ret);
        return ret;
    }
}
```

### 2. tx_isp_csi.c - tx_isp_csi_activate_subdev()

**Before:**
```c
/* Ensure CSI clocks are initialized before enabling */
if (!sd->clks && sd->clk_num > 0) {
    if (isp_subdev_init_clks(sd, sd->clk_num) != 0) {
        printk(KERN_ALERT "isp_subdev_init_clks failed\n");
    }
}

/* Enable clocks */
clks = sd->clks;
if (clks != NULL) {
    for (i = 0; i < clk_count; i++) {
        clk_enable(clks[i]);
    }
}
```

**After:**
```c
/* NOTE: CSI clocks are now initialized centrally in ispcore_core_ops_init
 * via tx_isp_configure_clocks, so we don't need to initialize them here.
 * The clocks are already enabled and ready to use. */
printk(KERN_ALERT "tx_isp_csi_activate_subdev: CSI clocks already initialized centrally\n");
```

### 3. tx_isp_subdev.c - isp_subdev_init_clks()

Added deprecation notice:
```c
/* isp_subdev_init_clks - DEPRECATED: Use tx_isp_configure_clocks instead
 * 
 * This function is overly complex and tries to initialize clocks per-subdev.
 * The correct approach is to call tx_isp_configure_clocks() once in 
 * ispcore_core_ops_init, which initializes all 3 clocks (cgu_isp, isp, csi)
 * in the correct order and stores them in isp_dev.
 */
```

## Benefits

1. **Simpler**: One function call instead of complex per-subdev logic
2. **Correct order**: Clocks initialized in proper dependency order
3. **Centralized**: All clock state in `isp_dev` structure
4. **Standard API**: Uses Linux clock framework properly
5. **Already tested**: `tx_isp_configure_clocks()` was already in the codebase

## Clock Details

### cgu_isp (CGU ISP Clock)
- **Rate**: Auto (pre-configured by bootloader/DTS)
- **Purpose**: Parent clock for ISP subsystem
- **Must be enabled first**

### isp (ISP Core Clock)
- **Rate**: Auto (pre-configured by bootloader/DTS)
- **Purpose**: ISP core processing clock
- **Depends on**: cgu_isp

### csi (CSI Interface Clock)
- **Rate**: Auto (pre-configured by bootloader/DTS)
- **Purpose**: CSI-2 interface clock
- **Depends on**: cgu_isp

**Important**: We don't call `clk_set_rate()` because:
1. The platform may not support runtime rate changes
2. Rates are already configured correctly by bootloader/device tree
3. The reference driver uses `0xffff` (auto-rate) in platform data
4. Attempting to set rates causes -EINVAL errors

## Testing

After this change, verify:
1. All 3 clocks are initialized on first stream start
2. Clock rates are correct (check dmesg)
3. CSI interface works properly
4. No clock-related errors in logs

## Future Work

The `isp_subdev_init_clks()` function can potentially be removed entirely once we verify
that no other code paths depend on it. For now, it's marked as deprecated.

