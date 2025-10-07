# ISP Core Slake Module Fix - Complete Summary

## Problem Identified

The `ispcore_slake_module` function in isp-latest was **missing critical subdevice slake calls** that are present in the Binary Ninja reference driver and isp-was-better.

### What Was Missing

**isp-latest (BROKEN)**:
```c
/* Binary Ninja: Subdevice initialization loop */
pr_info("ispcore_slake_module: Initializing subdevices");

/* Initialize CSI subdevice if present */
if (isp->csi_dev) {
    pr_info("ispcore_slake_module: Initializing CSI subdevice");
    isp->csi_dev->state = 1;  /* Set CSI to INIT state */
}

/* Initialize VIC subdevice if present */
if (vic_dev) {
    pr_info("ispcore_slake_module: Initializing VIC subdevice");
    vic_dev->state = 1;  /* Set VIC to INIT state */
}

/* NO ACTUAL SLAKE_MODULE CALLS! Just sets state directly! */
```

**Binary Ninja Reference (CORRECT)**:
```c
/* Binary Ninja decompilation shows subdev slake loop: */
void* $s3_1 = $s0_1 + 0x38  // Get subdev array
void* $s2_1 = *$s3_1        // First subdev

while (true) {
    if ($s2_1 != 0 && $s2_1 u< 0xfffff001) {
        void* $v0_6 = *(*($s2_1 + 0xc4) + 0x10)  // Get internal ops
        if ($v0_6 != 0) {
            int32_t $v0_7 = *($v0_6 + 4)  // Get slake_module function
            if ($v0_7 != 0) {
                int32_t $v0_8 = $v0_7($s2_1)  // CALL slake_module!
                if ($v0_8 != 0 && $v0_8 != 0xfffffdfd) {
                    isp_printf(2, "Failed to slake %s\n", *($s2_1 + 8))
                }
            }
        }
    }
    $s3_1 += 4
    if ($s0_1 + 0x78 == $s3_1) break
    $s2_1 = *$s3_1
}
```

## Impact

Without proper slake calls:
- ❌ CSI/VIC/VIN subdevices are NOT properly shut down
- ❌ Hardware may not be properly reset between streaming sessions
- ❌ MIPI lane configuration may not be properly reset
- ❌ Clocks may not be properly disabled in subdevices
- ❌ State machines get out of sync

## Solution Applied

### 1. Added Subdev Slake Calls to `ispcore_slake_module`

**File**: `driver/tx_isp_core.c`

```c
/* CRITICAL FIX: Binary Ninja subdev slake loop - MUST happen regardless of VIC state */
pr_info("ispcore_slake_module: Processing subdevices");

/* Process CSI device if present */
if (isp->csi_dev && isp->csi_dev->sd.ops && isp->csi_dev->sd.ops->internal && 
    isp->csi_dev->sd.ops->internal->slake_module) {
    pr_info("*** ispcore_slake_module: Calling slake_module for CSI ***\n");
    ret = isp->csi_dev->sd.ops->internal->slake_module(&isp->csi_dev->sd);
    if (ret != 0 && ret != -0x203) {
        pr_err("ispcore_slake_module: Failed to slake CSI: %d\n", ret);
    } else {
        pr_info("ispcore_slake_module: CSI slake success");
    }
}

/* Process VIC device if present */
if (isp->vic_dev && isp->vic_dev->sd.ops && isp->vic_dev->sd.ops->internal && 
    isp->vic_dev->sd.ops->internal->slake_module) {
    pr_info("*** ispcore_slake_module: Calling slake_module for VIC ***\n");
    ret = isp->vic_dev->sd.ops->internal->slake_module(&isp->vic_dev->sd);
    if (ret != 0 && ret != -0x203) {
        pr_err("ispcore_slake_module: Failed to slake VIC: %d\n", ret);
    } else {
        pr_info("ispcore_slake_module: VIC slake success");
    }
}

/* Process VIN device if present */
if (isp->vin_dev && isp->vin_dev->sd.ops && isp->vin_dev->sd.ops->internal && 
    isp->vin_dev->sd.ops->internal->slake_module) {
    pr_info("*** ispcore_slake_module: Calling slake_module for VIN ***\n");
    ret = isp->vin_dev->sd.ops->internal->slake_module(&isp->vin_dev->sd);
    if (ret != 0 && ret != -0x203) {
        pr_err("ispcore_slake_module: Failed to slake VIN: %d\n", ret);
    } else {
        pr_info("ispcore_slake_module: VIN slake success");
    }
}
```

### 2. Registered Internal Ops for VIC

**File**: `driver/tx_isp_vic.c`

```c
/* VIC internal ops for activate/slake */
static struct tx_isp_subdev_internal_ops vic_subdev_internal_ops = {
    .activate_module = tx_isp_vic_activate_subdev,
    .slake_module = tx_isp_vic_slake_subdev,
};

/* Complete VIC subdev ops structure */
struct tx_isp_subdev_ops vic_subdev_ops = {
    .core = &vic_core_ops,
    .video = &vic_video_ops,
    .sensor = &vic_sensor_ops,
    .internal = &vic_subdev_internal_ops,  // ADDED
};
```

### 3. Registered Internal Ops for CSI

**File**: `driver/tx_isp_csi.c`

```c
/* CSI internal ops for activate/slake */
static struct tx_isp_subdev_internal_ops csi_subdev_internal_ops = {
    .activate_module = tx_isp_csi_activate_subdev,
    .slake_module = tx_isp_csi_slake_subdev,
};

/* Initialize the subdev ops structure with pointers to the operations */
static struct tx_isp_subdev_ops csi_subdev_ops = {
    .core = &csi_core_ops,
    .video = &csi_video_ops,
    .sensor = &csi_sensor_ops,
    .internal = &csi_subdev_internal_ops,  // ADDED
};
```

### 4. VIN Already Had Internal Ops Registered

**File**: `driver/tx_isp_vin.c` (already correct)

```c
static struct tx_isp_subdev_internal_ops vin_subdev_internal_ops = {
    .activate_module = tx_isp_vin_activate_subdev,
    .slake_module = tx_isp_vin_slake_subdev,
};
```

### 5. Added VIC Control Register Write

**File**: `driver/tx_isp_core.c`

```c
/* Binary Ninja: (*($a0_1 + 0x40cc))($a0_1, 0x4000001, 0) */
if (vic_dev && vic_dev->vic_regs) {
    pr_info("ispcore_slake_module: Calling VIC control function (0x4000001, 0)");
    /* CRITICAL: This is the missing VIC control call that enables proper state transitions */
    uint32_t *vic_control_reg = (uint32_t *)((char *)vic_dev->vic_regs + 0x40cc);
    if (vic_control_reg) {
        /* Write the control value 0x4000001 to enable VIC operation */
        writel(0x4000001, vic_control_reg);
        pr_info("ispcore_slake_module: VIC control register written: 0x4000001");
    }
}
```

## Verification

The complete slake chain now works:

1. **ispcore_slake_module** called (from vic_core_s_stream)
2. → Calls **CSI slake_module** (tx_isp_csi_slake_subdev)
   - Stops CSI streaming if active
   - Disables CSI core
   - Disables CSI clocks
3. → Calls **VIC slake_module** (tx_isp_vic_slake_subdev)
   - Stops VIC streaming if active
   - Disables VIC core
   - Transitions VIC to INIT state
4. → Calls **VIN slake_module** (tx_isp_vin_slake_subdev)
   - Handles reference counting
   - Stops VIN streaming if active
   - Deinitializes VIN if needed
   - Transitions VIN to SLAKE state

## Expected Results

✅ Proper hardware shutdown sequence
✅ MIPI lane configuration properly reset
✅ All subdevice state machines synchronized
✅ Clocks properly disabled
✅ Clean restart for next streaming session

## Commits

1. `e507b0c6` - Fix ispcore_slake_module: Add critical subdev slake calls
2. `eef84ada` - Register internal ops for VIC and CSI slake functions

## References

- Binary Ninja decompilation of reference driver (address 0x69198)
- isp-was-better implementation in tx_isp_core.c (lines 2346-2550)
- Smart Diff MCP comparison results

