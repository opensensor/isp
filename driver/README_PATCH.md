# ISP Driver Patch: Restore Continuous VIC/ISP Interrupts

## Quick Summary

✅ **COMPLETED**: Ported critical ISP core initialization from `isp-was-better` to `isp-latest`

**Problem:** isp-latest only gets 4/4 control errors, no continuous VIC/ISP interrupts  
**Solution:** Port ISP core register initialization that was working in isp-was-better  
**Method:** Smart-Diff MCP analysis + Binary Ninja MCP verification

---

## What Was Changed

### 1. Added ISP Core Register Initialization
**File:** `driver/tx_isp_vic.c`  
**Function:** `tx_isp_vic_start()` (end of function, ~lines 1406-1572)  
**What:** Comprehensive initialization of ISP core registers at 0x13300000

**Key Components:**
- ✅ Main ISP core registers (0x0-0x114)
- ✅ ISP pipeline registers (0x9804-0x98a8)
- ✅ **VIC routing registers (0x9a00-0x9ac8)** - CRITICAL for interrupts
- ✅ Core control registers (0xb000-0xb08c) - **SKIPPING 0xb018-0xb024**
- ✅ 280ms delta register updates
- ✅ CSI PHY configuration (0x13310000-0x1331003c)
- ✅ VIC IRQ gate re-assertion

### 2. Call ispcore_slake_module at State 4
**File:** `driver/tx_isp_vic.c`  
**Function:** `vic_core_s_stream()` (VIC state 3→4 transition, ~lines 3172-3190)  
**What:** Initialize ISP core pipeline when VIC enters streaming state

---

## Critical Registers

### Most Important: VIC IRQ Gates
```
0x9ac0: VIC IRQ gate - Must be 0x200 → 0x0 → 0x1
0x9ac8: VIC IRQ gate - Must be 0x200 → 0x0 → 0x0
```
Without proper gate initialization, **NO frame done interrupts will fire!**

### Must Skip: Interrupt-Killing Registers
```
0xb018: SKIP (writing kills VIC interrupts)
0xb01c: SKIP (writing kills VIC interrupts)
0xb020: SKIP (writing kills VIC interrupts)
0xb024: SKIP (writing kills VIC interrupts)
```
The was-better version explicitly skipped these to preserve interrupts.

---

## Files Modified

1. **driver/tx_isp_vic.c**
   - Added ISP core register initialization in `tx_isp_vic_start()`
   - Modified `vic_core_s_stream()` to call `ispcore_slake_module`

2. **Documentation Created:**
   - `PORTING_NOTES_WAS_BETTER.md` - Detailed analysis
   - `PATCH_SUMMARY.md` - Comprehensive patch documentation
   - `REGISTER_INIT_FLOW.md` - Visual register initialization flow
   - `README_PATCH.md` - This file

---

## Expected Results

### Before Patch
- ❌ Only 4/4 control errors
- ❌ No continuous VIC interrupts
- ❌ No ISP frame done interrupts

### After Patch
- ✅ Continuous VIC frame done interrupts
- ✅ Proper ISP core pipeline initialization
- ✅ VIC routing registers configured
- ✅ Interrupt-killing registers skipped

---

## Testing

### Build (on target device with MIPS cross-compiler)
```bash
cd /home/matteius/isp-latest/driver
make
```

### Load
```bash
sudo rmmod tx_isp_t31 2>/dev/null
sudo insmod tx-isp-t31.ko
```

### Verify Success

**1. Check ISP core initialization:**
```bash
dmesg | grep "ISP CORE INIT COMPLETE"
```
Expected: "ISP CORE INIT COMPLETE: All critical registers configured"

**2. Check VIC IRQ gates:**
```bash
dmesg | grep "VIC IRQ GATES"
```
Expected: "[9ac0]=0x00000001 [9ac8]=0x00000000 (re-asserted for frame done)"

**3. Check for continuous interrupts:**
```bash
dmesg | grep -c "VIC FRAME DONE"
```
Expected: Many frame done messages (not just 4)

**4. Check ispcore_slake_module:**
```bash
dmesg | grep "ispcore_slake_module SUCCESS"
```
Expected: "ispcore_slake_module SUCCESS - ISP core initialized"

**5. Verify no control errors:**
```bash
dmesg | grep -i "control limit" | wc -l
```
Expected: 0 or very few (only during initialization)

---

## Analysis Method

### Smart-Diff MCP Comparison
- **Source:** `/home/matteius/isp-was-better/driver`
- **Target:** `/home/matteius/isp-latest/driver`
- **Comparison ID:** `0633842f-0788-4e94-bafe-30b040152bf9`
- **Functions Analyzed:** 859 total
- **Key Differences:** 255 modified functions

### Binary Ninja MCP Verification
- Decompiled reference driver functions
- Verified `vic_core_s_stream` implementation
- Confirmed `tx_isp_vic_start` register sequence

---

## Key Insights

1. **VIC Routing Registers are Essential**
   - The 0x9a00-0x9ac8 range controls VIC→ISP interrupt routing
   - Without proper initialization, no frame done interrupts

2. **Interrupt-Killing Registers Must Be Skipped**
   - Registers 0xb018-0xb024 disrupt interrupt routing
   - The was-better version explicitly skipped them

3. **VIC IRQ Gates Need Specific Sequence**
   - Initial: 0x200/0x200 (setup)
   - Delta: 0x0/0x0 (clear)
   - Final: 0x1/0x0 (enable)

4. **CSI PHY Configuration is Critical**
   - Ensures proper MIPI data flow
   - Must be done before VIC enable

5. **ispcore_slake_module Initializes Pipeline**
   - Called when VIC reaches state 4
   - Initializes ISP core pipeline

---

## Rollback

If issues occur, restore from backup:
```bash
cd /home/matteius/isp-latest/driver
cp tx_isp_vic.c.backup tx_isp_vic.c
```

---

## Next Steps

1. **Build and test** on target device
2. **Monitor dmesg** for success indicators
3. **Verify continuous interrupts** (not just 4/4)
4. **Check for control errors** (should be gone)
5. **Test video capture** to confirm full functionality

---

## References

- **Smart-Diff Analysis:** `PORTING_NOTES_WAS_BETTER.md`
- **Detailed Patch Info:** `PATCH_SUMMARY.md`
- **Register Flow:** `REGISTER_INIT_FLOW.md`
- **Source Code:** `driver/tx_isp_vic.c` (lines 1406-1572, 3172-3190)

---

## Contact

If you encounter issues:
1. Check dmesg for error messages
2. Verify all success indicators above
3. Compare register values with `REGISTER_INIT_FLOW.md`
4. Use Binary Ninja MCP to verify reference driver behavior

---

## Success Criteria

The patch is successful if:
- ✅ Continuous VIC frame done interrupts (many, not just 4)
- ✅ No control limit errors
- ✅ ISP core initialization completes
- ✅ VIC IRQ gates properly asserted
- ✅ Video capture works

Good luck! 🚀

