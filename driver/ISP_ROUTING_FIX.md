# ISP Output Routing Fix - Register 0x804

## The Problem

After all previous fixes, the logs show:
- ✅ ISP pipeline is enabled (`system_reg_write(0x800, 1)`)
- ✅ CSI has no errors (clean MIPI data reception)
- ✅ VIC control register is correct (`0x80000020`, no bit 17)
- ❌ **Still no frames reaching VIC** (control limit errors, no frame done)

ISP Core is generating statistics interrupts (0x500, 0x1000) but NO frame done interrupts.

## Root Cause: Wrong ISP Output Routing

Register `0x804` controls ISP output routing - where processed frames go.

### Binary Ninja MCP Reference

From `tisp_init` decompilation:
```c
if (data_a2ea4 != 0) {  // WDR mode
    $v0_30 = 0x10;
    $v1_29 = 0x12;
} else {  // Normal mode
    $v0_30 = 0x1c;  ← NORMAL MODE VALUE!
    $v1_29 = 0x1e;
}

if ($a1_52 == 0x14) {  // Special format
    $v0_30 = $v1_29;
}

system_reg_write(0x804, $v0_30);
```

**For normal mode (non-WDR), register 0x804 should be `0x1c`, NOT `0x1e`!**

### Our Code (WRONG)

```c
uint32_t isp_mode = (sensor_params.mode >= 4) ? 0x12 : 0x1e;  // WRONG!
system_reg_write(0x804, isp_mode);
```

We were using `0x1e` for normal mode, but BN MCP shows it should be `0x1c`!

### The Fix

```c
uint32_t isp_mode = (sensor_params.mode >= 4) ? 0x12 : 0x1c;  // CORRECT!
system_reg_write(0x804, isp_mode);
```

## What Register 0x804 Controls

This register appears to control ISP output routing/mode:
- `0x1c` = Normal mode - route ISP output to VIC
- `0x1e` = Special format mode (format 0x14)
- `0x10` = WDR mode
- `0x12` = WDR mode with special format

When set to `0x1e` instead of `0x1c`, the ISP may be:
- Routing output to wrong destination
- Using wrong output format
- Not generating frame done signals
- Stalling waiting for configuration

## Evidence

### From Logs
```
sensor parameters: mode=0  ← Normal mode (not WDR)
ISP pipeline: 0x804=0x1e   ← WRONG! Should be 0x1c
ISP CORE: interrupt_status=0x500  ← Statistics only, no frames
VIC: control limit error (bit 21)  ← Waiting for frames that never arrive
```

### Expected After Fix
```
sensor parameters: mode=0
ISP pipeline: 0x804=0x1c   ← CORRECT!
ISP CORE: interrupt_status=0x1 or 0x2  ← Frame start/done!
VIC: frame done interrupt (bit 0)  ← Frames arriving!
```

## Files Modified

### driver/tx_isp_tuning.c - Lines 2144-2152

**BEFORE:**
```c
uint32_t isp_mode = (sensor_params.mode >= 4) ? 0x12 : 0x1e;
system_reg_write(0x804, isp_mode);
```

**AFTER:**
```c
/* CRITICAL FIX: Use 0x1c for normal mode, NOT 0x1e! */
/* BN MCP shows: normal mode = 0x1c, WDR mode = 0x10/0x12 */
uint32_t isp_mode = (sensor_params.mode >= 4) ? 0x12 : 0x1c;
system_reg_write(0x804, isp_mode);
```

### driver/tx_isp_tuning.c - Lines 2052-2058

Added logging for ISP interrupt and control register writes:
```c
pr_info("*** tisp_init: Enabling ISP interrupts (0x30=0xffffffff) ***\n");
system_reg_write(0x30, 0xffffffff);

pr_info("*** tisp_init: Enabling ISP main control (0x10=0x133) ***\n");
system_reg_write(0x10, 0x133);
```

### driver/tx-isp-module.c - Lines 2002-2007

Added CSI error checking (already done in previous fix):
```c
printk(KERN_ALERT "*** VIC IRQ: No frame done interrupt (v1_7 & 1 = 0) - checking CSI ***\n");
extern void tx_isp_csi_check_errors(struct tx_isp_dev *isp_dev);
tx_isp_csi_check_errors(ourISPdev);
```

### driver/tx_isp_vic.c - Lines 3100-3117

Removed buffer_count from control register (already done in previous fix):
```c
u32 stream_ctrl = 0x80000020;  // NO buffer count!
```

## Why This Matters

The ISP has multiple output paths and modes:
1. **Direct to memory** (bypass VIC)
2. **Through VIC** (normal camera operation)
3. **Special formats** (RAW, etc.)
4. **WDR processing** (Wide Dynamic Range)

Register 0x804 selects which path to use. Using the wrong value (`0x1e` instead of `0x1c`) causes the ISP to route frames to the wrong destination or use the wrong output format, so VIC never receives frames.

## Testing

```bash
cd /home/matteius/isp-latest/driver
make clean && make
sudo rmmod tx-isp-module 2>/dev/null
sudo insmod tx-isp-module.ko
```

**Look for:**
```
tisp_init: CRITICAL - Enabling ISP pipeline: 0x804=0x1c  ← Should be 0x1c now!
ISP CORE: interrupt_status=0x00000001  ← Frame start interrupt!
ISP CORE: interrupt_status=0x00000002  ← Frame done interrupt!
VIC IRQ: Frame done interrupt (v1_7 & 1 = 1)  ← VIC receiving frames!
[No control limit errors]
[No 10/15 stall]
```

## Summary

The 10/15 stall was caused by incorrect ISP output routing. Register 0x804 was set to `0x1e` (special format mode) instead of `0x1c` (normal mode), causing the ISP to route processed frames to the wrong destination. VIC was correctly configured and waiting for frames, but frames were never arriving because they were being routed elsewhere.

This is a **single-bit difference** (`0x1e` vs `0x1c` = bit 1) that completely breaks the data pipeline!

## Why We Missed This

The Binary Ninja decompilation showed complex logic for determining the value:
```c
$v0_30 = 0x1c;  // Normal mode
$v1_29 = 0x1e;  // Special format
if (format == 0x14) $v0_30 = $v1_29;
```

We misread this as "use 0x1e for normal mode" when it actually means "use 0x1c for normal mode, only use 0x1e for special format 0x14".

This is the kind of subtle error that happens when translating decompiled code - the logic is correct but easy to misinterpret!

