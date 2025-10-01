# CRITICAL: CSI Initialization Bug Found

## Problem

The first write to isp-csi register offset 0x0 is **WRONG**:

- **Reference trace (working)**: `0x7d`
- **Our trace (broken)**: `0x1`

This is the very first CSI PHY Control write and it's critical for MIPI lane configuration.

## Location in Code

`driver/tx_isp_csi.c` line 457:

```c
/* Binary Ninja: *$v0_8 = 0x7d */
writel(0x7d, v0_8);
```

This should write `0x7d` to `isp_csi_regs` (ISP Core CSI registers at 0x13300000).

## Why It's Wrong

Three possible causes:

### 1. Code Path Not Reached
The `csi_core_ops_init` function may not be executing the MIPI initialization path that contains line 457.

### 2. Wrong Register Pointer
The `isp_csi_regs` pointer may be pointing to the wrong address (CSI PHY instead of ISP Core).

This was previously fixed in `CSI_ISP_REGISTER_FIX.md`:
```c
// CORRECT:
csi_dev->csi_regs = csi_dev->sd.regs;       // 0x10022000 (CSI PHY)
csi_dev->isp_csi_regs = ourISPdev->core_regs;  // 0x13300000 (ISP Core)

// WRONG (both pointing to same address):
csi_dev->csi_regs = csi_dev->sd.regs;        // 0x10022000
csi_dev->isp_csi_regs = csi_dev->sd.regs;   // 0x10022000 ← WRONG!
```

### 3. Overwritten Later
Something may be writing `0x1` to offset 0x0 after the correct `0x7d` write.

## Impact

Without the correct `0x7d` write to ISP Core CSI registers:
- CSI PHY timing configuration is wrong
- MIPI lane configuration fails
- Data doesn't flow from sensor → CSI → VIC
- VIC times out waiting for frames
- No video output

## Next Steps

1. **Capture dmesg BEFORE starting streamer** to see CSI initialization logs
2. Check if `csi_core_ops_init` is being called
3. Verify `isp_csi_regs` pointer is correct (should be 0x13300000, not 0x10022000)
4. Check if MIPI interface_type is detected correctly (should be 1)
5. Trace execution path through `csi_core_ops_init` to see which branch is taken

## Test Script Changes Needed

The current test script clears dmesg AFTER module loading, so we miss the critical initialization logs. Need to:

1. Remove `/opt/trace.txt` at start
2. Always upload trace module
3. Capture dmesg BEFORE starting streamer
4. Reduce iterations to 10 (not 30 seconds)

## Reference

- Reference trace line 115: `ISP isp-csi: [CSI PHY Control] write at offset 0x0: 0x0 -> 0x7d`
- Our trace line 101: `ISP isp-csi: [CSI PHY Control] write at offset 0x0: 0x0 -> 0x1`
- Code: `driver/tx_isp_csi.c:457`
- Previous fix: `driver/CSI_ISP_REGISTER_FIX.md`

