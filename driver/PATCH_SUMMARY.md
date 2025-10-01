# ISP Driver Patch: Port Critical Initialization from isp-was-better

## Date: 2025-10-01
## Author: AI Assistant with Smart-Diff MCP Analysis

---

## Executive Summary

This patch ports critical ISP core register initialization from the `isp-was-better` version (which had continuous working VIC/ISP interrupts) to the `isp-latest` version (which only gets 4/4 control errors).

**Key Changes:**
1. Added comprehensive ISP core register initialization in `tx_isp_vic_start()`
2. Explicitly skip interrupt-killing registers (0xb018-0xb024)
3. Configure VIC routing registers (0x9a00-0x9ac8) for frame done interrupts
4. Add detailed CSI PHY configuration sequence
5. Call `ispcore_slake_module` when VIC reaches state 4

---

## Analysis Method

Used **Smart-Diff MCP** to compare:
- Source: `/home/matteius/isp-was-better/driver`
- Target: `/home/matteius/isp-latest/driver`
- Comparison ID: `0633842f-0788-4e94-bafe-30b040152bf9`

**Key Findings:**
- 859 total functions analyzed
- 255 modified functions
- 291 added functions
- 141 deleted functions

**Critical differences identified:**
- ISP core register initialization at 0x13300000
- VIC routing register configuration
- Interrupt-preserving register handling
- CSI PHY configuration sequence

---

## Detailed Changes

### File: `driver/tx_isp_vic.c`

#### Change 1: ISP Core Register Initialization (Lines 1406-1572)

**Location:** End of `tx_isp_vic_start()` function, before `vic_start_ok = 1`

**Purpose:** Initialize ISP core registers that enable VIC→ISP pipeline and frame done interrupts

**Code Structure:**
```c
/* Map main ISP base (0x13300000) */
void __iomem *main_isp_base = ioremap(0x13300000, 0x100000);

/* STEP 1: Main ISP Core Registers */
// Configure core control registers (0x0-0x114)
// Set up ISP pipeline registers (0x9804-0x98a8)

/* STEP 2: VIC Routing Registers (0x9a00-0x9ac8) - CRITICAL */
// These enable VIC frame done interrupts
writel(0x50002d0, main_isp_base + 0x9a00);  // Width
writel(0x3000300, main_isp_base + 0x9a04);  // Height
writel(0x50002d0, main_isp_base + 0x9a2c);  // Stride
writel(0x1, main_isp_base + 0x9a34);        // Enable
writel(0x1, main_isp_base + 0x9a70);        // Clear status (W1C)
writel(0x1, main_isp_base + 0x9a7c);        // Clear status (W1C)
writel(0x500, main_isp_base + 0x9a80);      // Stride value
writel(0x1, main_isp_base + 0x9a88);        // Route/enable latch
writel(0x1, main_isp_base + 0x9a94);        // Enable
writel(0x500, main_isp_base + 0x9a98);      // Width-related
writel(0x200, main_isp_base + 0x9ac0);      // VIC IRQ gate - CRITICAL
writel(0x200, main_isp_base + 0x9ac8);      // VIC IRQ gate - CRITICAL

/* STEP 3: Core Control Registers - INTERRUPT PRESERVING */
// Configure 0xb000-0xb088 BUT skip 0xb018-0xb024
writel(0xf001f001, main_isp_base + 0xb004);
writel(0x40404040, main_isp_base + 0xb008);
// ... more registers ...

/* CRITICAL: SKIP these interrupt-killing registers */
/* writel(0x40404040, main_isp_base + 0xb018); // SKIPPED */
/* writel(0x40404040, main_isp_base + 0xb01c); // SKIPPED */
/* writel(0x40404040, main_isp_base + 0xb020); // SKIPPED */
/* writel(0x404040, main_isp_base + 0xb024);   // SKIPPED */

/* STEP 4: 280ms Delta Register Changes */
// Post-sensor-detection updates
writel(0x0, main_isp_base + 0x9804);
writel(0x0, main_isp_base + 0x9ac0);  // Clear gate
writel(0x0, main_isp_base + 0x9ac8);  // Clear gate
// ... more delta updates (still skipping 0xb018-0xb024) ...

/* STEP 5: CSI PHY Configuration */
void __iomem *csi_phy_base = main_isp_base + 0x10000;
writel(0x7d, csi_phy_base + 0x0);
writel(0xe3, csi_phy_base + 0x4);
// ... complete CSI PHY sequence ...

/* STEP 6: Re-assert VIC IRQ Gates */
writel(0x1, main_isp_base + 0x9a70);   // W1C clear
writel(0x1, main_isp_base + 0x9a7c);   // W1C clear
writel(0x00000001, main_isp_base + 0x9ac0);  // Re-assert gate
writel(0x00000000, main_isp_base + 0x9ac8);  // Re-assert gate

iounmap(main_isp_base);
```

**Why This Matters:**
- The VIC routing registers (0x9a00-0x9ac8) are **essential** for frame done interrupts
- The interrupt-killing registers (0xb018-0xb024) must be **skipped** to preserve interrupts
- The CSI PHY configuration ensures proper MIPI data flow
- The gate re-assertion enables hardware interrupt generation

---

#### Change 2: Call ispcore_slake_module (Lines 3172-3190)

**Location:** In `vic_core_s_stream()`, during VIC state 3→4 transition

**Purpose:** Initialize ISP core pipeline when VIC enters streaming state

**Code:**
```c
} else if (vic_dev->state == 3) {
    vic_dev->state = 4;
    pr_info("*** vic_core_s_stream: VIC state transition 3 → 4 (STREAMING) ***\n");

    /* PORTED FROM WAS-BETTER: Call ispcore_slake_module */
    pr_info("*** VIC STATE 4: Calling ispcore_slake_module ***\n");
    extern int ispcore_slake_module(struct tx_isp_subdev *sd);
    if (ourISPdev && ourISPdev->core_dev) {
        struct tx_isp_subdev *core_sd = &ourISPdev->core_dev->sd;
        int slake_ret = ispcore_slake_module(core_sd);
        if (slake_ret == 0) {
            pr_info("*** ispcore_slake_module SUCCESS ***\n");
        } else {
            pr_warn("*** ispcore_slake_module returned %d ***\n", slake_ret);
        }
    }
}
```

**Why This Matters:**
- The was-better version that had working interrupts DID call this
- Initializes the ISP core pipeline properly
- Should be safe now that critical registers are pre-configured

---

## Critical Register Reference

### VIC Routing Registers (0x9a00-0x9ac8)

| Offset | Value | Purpose | Critical? |
|--------|-------|---------|-----------|
| 0x9a00 | 0x50002d0 | Width configuration | Yes |
| 0x9a04 | 0x3000300 | Height configuration | Yes |
| 0x9a2c | 0x50002d0 | Stride configuration | Yes |
| 0x9a34 | 0x1 | Enable bit | Yes |
| 0x9a70 | 0x1 (W1C) | Frame done status clear | Yes |
| 0x9a7c | 0x1 (W1C) | Frame done status clear | Yes |
| 0x9a80 | 0x500 | Stride value | Yes |
| 0x9a88 | 0x1 | Route/enable latch | Yes |
| 0x9a94 | 0x1 | Enable bit | Yes |
| 0x9a98 | 0x500 | Width-related | Yes |
| **0x9ac0** | **0x200→0x1** | **VIC IRQ gate** | **CRITICAL** |
| **0x9ac8** | **0x200→0x0** | **VIC IRQ gate** | **CRITICAL** |

### Interrupt-Killing Registers (MUST SKIP)

| Offset | Would Write | Effect | Action |
|--------|-------------|--------|--------|
| 0xb018 | 0x40404040→0x24242424 | Kills VIC interrupts | **SKIP** |
| 0xb01c | 0x40404040→0x24242424 | Kills VIC interrupts | **SKIP** |
| 0xb020 | 0x40404040→0x24242424 | Kills VIC interrupts | **SKIP** |
| 0xb024 | 0x404040→0x242424 | Kills VIC interrupts | **SKIP** |

---

## Expected Behavior After Patch

### Before Patch (isp-latest)
- ❌ Only 4/4 control errors
- ❌ No continuous VIC interrupts
- ❌ No ISP frame done interrupts
- ❌ VIC routing registers not initialized
- ❌ Interrupt-killing registers written

### After Patch (with was-better initialization)
- ✅ Continuous VIC frame done interrupts
- ✅ Proper ISP core pipeline initialization
- ✅ VIC routing registers configured
- ✅ Interrupt-killing registers skipped
- ✅ CSI PHY properly configured
- ✅ ISP core initialized via ispcore_slake_module

---

## Testing Instructions

### 1. Build the Driver
```bash
cd /home/matteius/isp-latest/driver
# Use appropriate cross-compilation setup for MIPS target
make
```

### 2. Load the Driver
```bash
sudo rmmod tx_isp_t31 2>/dev/null
sudo insmod tx-isp-t31.ko
```

### 3. Check for Success Indicators

**Look for ISP core initialization:**
```bash
dmesg | grep "ISP CORE INIT"
```
Expected: "ISP CORE INIT COMPLETE: All critical registers configured"

**Look for VIC IRQ gate assertion:**
```bash
dmesg | grep "VIC IRQ GATES"
```
Expected: "[9ac0]=0x00000001 [9ac8]=0x00000000 (re-asserted for frame done)"

**Look for continuous interrupts:**
```bash
dmesg | grep -E "VIC FRAME DONE|vic_framedone_irq"
```
Expected: Multiple frame done interrupt messages

**Check for ispcore_slake_module success:**
```bash
dmesg | grep "ispcore_slake_module"
```
Expected: "ispcore_slake_module SUCCESS - ISP core initialized"

### 4. Verify No Control Errors
```bash
dmesg | grep -i "control limit"
```
Expected: No control limit errors (or very few during initialization)

---

## Rollback Instructions

If the patch causes issues, restore from backup:
```bash
cd /home/matteius/isp-latest/driver
cp tx_isp_vic.c.backup tx_isp_vic.c
```

---

## References

- Smart-Diff Analysis: Comparison ID `0633842f-0788-4e94-bafe-30b040152bf9`
- Binary Ninja MCP: Decompiled reference driver functions
- Source: `/home/matteius/isp-was-better/driver/tx_isp_vic.c` (lines 2280-2650)
- Target: `/home/matteius/isp-latest/driver/tx_isp_vic.c`

---

## Notes

1. The ISP core register initialization is done in `tx_isp_vic_start()` because that's where the Binary Ninja reference driver does complex VIC configuration.

2. The interrupt-killing registers (0xb018-0xb024) appear to control interrupt routing/masking at the core level. Writing to them disrupts the VIC→ISP interrupt path.

3. The VIC IRQ gates (0x9ac0/0x9ac8) must be cleared (0x0) during initialization, then re-asserted (0x1/0x0) to enable frame done interrupts.

4. The 280ms delta represents register changes that occur after sensor detection in the reference driver.

5. The CSI PHY configuration sequence is critical for proper MIPI data flow from sensor to VIC.

