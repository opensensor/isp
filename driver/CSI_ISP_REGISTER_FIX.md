# CRITICAL FIX: CSI ISP Register Mapping Error

## The Problem

The CSI driver was mapping BOTH `csi_regs` and `isp_csi_regs` to the SAME address (`0x10022000`), but they should point to DIFFERENT register spaces!

### From Reference `/proc/iomem`

```
10022000-10022fff : mipi-phy          ← CSI PHY registers
10023000-10023fff : isp-device        ← VIC control space
  10023000-10023fff : isp-w01
13300000-1330ffff : isp-device        ← ISP CORE registers
  13300000-1330ffff : isp-m0
133e0000-133effff : isp-device        ← VIC primary space
  133e0000-133effff : isp-w02
```

### What We Had (WRONG)

```c
csi_dev->csi_regs = csi_dev->sd.regs;        // 0x10022000 ✓ CORRECT
csi_dev->isp_csi_regs = csi_dev->sd.regs;   // 0x10022000 ✗ WRONG!
```

Both pointing to `0x10022000` (CSI PHY space)!

### What We Need (CORRECT)

```c
csi_dev->csi_regs = csi_dev->sd.regs;       // 0x10022000 (CSI PHY)
csi_dev->isp_csi_regs = ourISPdev->core_regs;  // 0x13300000 (ISP Core)
```

## Why This Matters

The `csi_core_ops_init` function writes to BOTH register spaces:

### CSI PHY Registers (0x10022000)
```c
writel(lanes - 1, csi_regs + 0x4);      // Lane count
writel(0, csi_regs + 0xc);              // Reset
writel(1, csi_regs + 0x10);             // PHY enable ← CRITICAL!
```

### ISP Core CSI Registers (0x13300000)
```c
writel(0x7d, isp_csi_regs + 0x0);       // Timing config
writel(0x3f, isp_csi_regs + 0x128);     // Lane config
writel(..., isp_csi_regs + 0x160);      // Frame rate config
```

**When both pointers point to the same address, the ISP Core CSI registers are NEVER configured!**

## The Impact

Without proper ISP Core CSI configuration:
1. CSI PHY may be enabled (register 0x10022010 = 1)
2. But ISP Core doesn't know how to interpret the MIPI data
3. ISP Core CSI timing/lane config is missing
4. Data doesn't flow from CSI → VIC
5. VIC times out waiting for frames
6. Control limit error (bit 21)
7. **10/15 stall!**

## The Fix

### driver/tx_isp_csi.c - Lines 808-820

**BEFORE:**
```c
/* WRONG: Both point to CSI space */
csi_dev->isp_csi_regs = csi_dev->sd.regs;
pr_info("*** CSI PROBE: isp_csi_regs (offset 0x13c) mapped to: %p ***\n", csi_dev->isp_csi_regs);
```

**AFTER:**
```c
/* CORRECT: Point to ISP Core space */
extern struct tx_isp_dev *ourISPdev;
if (ourISPdev && ourISPdev->core_regs) {
    csi_dev->isp_csi_regs = ourISPdev->core_regs;
    pr_info("*** CSI PROBE: isp_csi_regs (offset 0x13c) mapped to ISP CORE: %p (0x13300000) ***\n", csi_dev->isp_csi_regs);
} else {
    pr_err("*** CSI PROBE: ERROR - ISP Core registers not available! ***\n");
    csi_dev->isp_csi_regs = csi_dev->sd.regs;  /* Fallback */
    pr_err("*** CSI PROBE: FALLBACK - Using CSI regs for isp_csi_regs (WRONG!) ***\n");
}
```

## Expected Output After Rebuild

### Probe Phase
```
CSI PROBE: csi_regs (offset 0xb8) mapped to: b0022000
CSI PROBE: isp_csi_regs (offset 0x13c) mapped to ISP CORE: b3300000 (0x13300000)
                                                              ^^^^^^^^ DIFFERENT!
```

### CSI Init Phase
```
CSI MIPI INIT: Configuring MIPI CSI for 2 lanes
CSI[0x4] = 1 (lanes - 1)
CSI[0xc] = 1 (MIPI enable)
CSI MIPI: Writing ISP_CSI[0x0] = 0x7d (timing config)      ← ISP Core!
CSI MIPI: Writing ISP_CSI[0x128] = 0x3f (lane config)      ← ISP Core!
CSI MIPI: CRITICAL - Enabling CSI PHY: CSI[0x10] = 1       ← CSI PHY!
CSI MIPI: PHY ENABLED - MIPI data should now flow!
```

### Result
```
VIC IRQ: Frame done interrupt (v1_7 & 1 = 1)  ← Frames arriving!
ISP CORE: interrupt_status=0x1 or 0x2  ← Processing frames!
[No control limit errors]
[No 10/15 stall!]
```

## Why We Missed This

The Binary Ninja decompilation showed:
```c
csi_dev->csi_regs = ...;        // offset 0xb8
csi_dev->isp_csi_regs = ...;    // offset 0x13c
```

We assumed both should point to the same base because they're both "CSI-related", but actually:
- `csi_regs` (0xb8) = CSI PHY hardware registers (0x10022000)
- `isp_csi_regs` (0x13c) = ISP Core CSI configuration registers (0x13300000)

These are DIFFERENT register spaces that control DIFFERENT aspects of the CSI pipeline!

## Register Space Summary

| Pointer | Offset | Physical Address | Purpose |
|---------|--------|------------------|---------|
| `csi_regs` | 0xb8 | 0x10022000 | CSI PHY control (lanes, enable, reset) |
| `isp_csi_regs` | 0x13c | 0x13300000 | ISP Core CSI config (timing, frame rate) |
| `vic_regs` | varies | 0x133e0000 | VIC primary space (capture, buffers) |
| `vic_regs_control` | varies | 0x10023000 | VIC control space (gates, clocks) |

## Files Modified

- `driver/tx_isp_csi.c` - Lines 808-820 (fixed isp_csi_regs mapping)
- `driver/tx_isp_csi.c` - Lines 383-419 (added CSI MIPI init logging)
- `driver/tx_isp_csi.c` - Lines 485-501 (added CSI PHY enable logging)
- `driver/tx_isp_tuning.c` - Lines 2144-2152 (ISP routing fix: 0x1c)
- `driver/tx_isp_tuning.c` - Lines 2052-2058 (ISP interrupt enable logging)
- `driver/tx-isp-module.c` - Lines 2002-2007 (CSI error checking)
- `driver/tx_isp_vic.c` - Lines 3100-3117 (removed buffer_count)

## Testing

```bash
cd /home/matteius/isp-latest/driver
make clean && make
sudo rmmod tx-isp-module 2>/dev/null
sudo insmod tx-isp-module.ko
```

**Critical check:**
```bash
dmesg | grep "isp_csi_regs.*mapped"
```

Should show:
```
CSI PROBE: isp_csi_regs (offset 0x13c) mapped to ISP CORE: b3300000 (0x13300000)
```

NOT:
```
CSI PROBE: isp_csi_regs (offset 0x13c) mapped to: b0022000  ← WRONG!
```

## Conclusion

This is a **critical register mapping error** that prevented the ISP Core from being properly configured to receive MIPI data from the CSI PHY. Even though the CSI PHY was enabled, the ISP Core didn't know how to interpret the data because its CSI configuration registers were never written.

This is the kind of subtle bug that's easy to miss when translating from decompiled code - two pointers with similar names pointing to completely different register spaces!

