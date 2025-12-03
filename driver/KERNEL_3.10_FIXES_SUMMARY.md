# Kernel 3.10 Compatibility Fixes Summary

This document summarizes all fixes applied to make the ISP driver compatible with Linux kernel 3.10 on MIPS32 architecture.

## Issues Fixed

### 1. MIPS32 64-bit Division Symbols (Module Load Failure)

**Error:**
```
tx_isp_t31: Unknown symbol __divdi3 (err 0)
tx_isp_t31: Unknown symbol __udivdi3 (err 0)
```

**Fix:** Replaced all 64-bit division operations with kernel-provided helpers
- **File:** `driver/tx_isp_tuning.c`
- **Changes:** 8 locations fixed
- **Functions used:**
  - `div64_s64(s64, s64)` for signed 64-bit division
  - `do_div(u64, u32)` for unsigned 64-bit division
- **Header added:** `#include <linux/math64.h>`

**Kernel 3.10 Compatibility:** ✅ Both functions available since kernel 2.6.22

**Documentation:** `driver/MIPS32_DIVISION_FIX.md`

---

### 2. Event Thread Deadlock (22-Second Hang During STREAMON)

**Error:**
```
[IMP_ISP] EnableSensor: calling ioctl 0x80045612 (ISP STREAMON)
<system hangs for ~22 seconds, then reboots>
```

**Fix:** Initialize completion once at system init, then reinitialize before each wait
- **Files:** `driver/tx_isp_tuning.c` lines 12463-12483, 2148-2159, 12492-12500
- **Changes:**
  1. Added `init_completion(&tevent_info)` in `tisp_event_init()` (one-time initialization)
  2. Removed duplicate initialization from `tisp_event_push()`
  3. Removed duplicate initialization from `tisp_event_process()`
  4. Added `INIT_COMPLETION(tevent_info)` before wait in `tisp_event_process()`
- **Macro used:** `INIT_COMPLETION(x)` (kernel 3.10 compatible)

**Kernel 3.10 Compatibility:** ✅ `INIT_COMPLETION` macro available in kernel 3.10
- Note: `reinit_completion()` was introduced in kernel 3.13
- Kernel 3.10 uses `INIT_COMPLETION()` macro instead

**CRITICAL BUG FIX (2025-10-17):** Race condition from duplicate initialization
- **Problem:** TWO separate static `tevent_initialized` variables in `tisp_event_push()` and `tisp_event_process()`
- **Solution:** Initialize completion exactly once in `tisp_event_init()` during system initialization
- **Result:** No race conditions, proper initialization order guaranteed

**Documentation:** `driver/EVENT_THREAD_DEADLOCK_FIX.md`

---

## Kernel 3.10 API Compatibility Notes

### Completion API

| Function/Macro | Kernel 3.10 | Kernel 3.13+ | Used In Fix |
|----------------|-------------|--------------|-------------|
| `init_completion(&x)` | ✅ Available | ✅ Available | Yes |
| `INIT_COMPLETION(x)` | ✅ Available | ❌ Removed | **Yes** |
| `reinit_completion(&x)` | ❌ Not available | ✅ Available | No |
| `complete(&x)` | ✅ Available | ✅ Available | Yes |
| `wait_for_completion_interruptible(&x)` | ✅ Available | ✅ Available | Yes |

**Key Difference:**
- `INIT_COMPLETION(x)` - Takes completion by value (macro) - **Used in our fix**
- `reinit_completion(&x)` - Takes completion by pointer (function) - Not available in 3.10

### Division API

| Function | Kernel 3.10 | Description | Used In Fix |
|----------|-------------|-------------|-------------|
| `do_div(n, base)` | ✅ Available | Unsigned 64÷32 division (in-place) | **Yes** |
| `div64_s64(n, d)` | ✅ Available | Signed 64÷64 division | **Yes** |
| `div64_u64(n, d)` | ✅ Available | Unsigned 64÷64 division | No |
| `div_s64(n, d)` | ✅ Available | Signed 64÷32 division | No |
| `div_u64(n, d)` | ✅ Available | Unsigned 64÷32 division | No |

**Header:** `#include <linux/math64.h>` (available since kernel 2.6.22)

---

## Files Modified

### Source Code Changes

1. **driver/tx_isp_tuning.c**
   - Line 31: Added `#include <linux/math64.h>`
   - Lines 315-329: Fixed CCM interpolation D->T (signed division)
   - Lines 347-361: Fixed CCM interpolation T->A (signed division)
   - Lines 407-426: Fixed BCSH hue interpolation (signed division)
   - Lines 6533-6560: Fixed defog block size calculation (unsigned division)
   - Lines 9504-9511: Fixed AE parameter interpolation (cast to ensure 32-bit)
   - Lines 9521-9535: Fixed AE parameter interpolation (cast to ensure 32-bit)
   - Lines 11021-11036: Fixed Tiziano CCM D->T interpolation (signed division)
   - Lines 11050-11065: Fixed Tiziano CCM T->A interpolation (signed division)
   - Lines 12463-12483: Added `init_completion(&tevent_info)` in `tisp_event_init()`
   - Lines 2148-2159: Removed duplicate initialization from `tisp_event_push()`
   - Lines 12492-12500: Simplified `tisp_event_process()` with `INIT_COMPLETION(tevent_info)`

### Documentation Added

1. **driver/MIPS32_DIVISION_FIX.md** - Detailed documentation of division fix
2. **driver/EVENT_THREAD_DEADLOCK_FIX.md** - Detailed documentation of deadlock fix
3. **driver/KERNEL_3.10_FIXES_SUMMARY.md** - This file
4. **driver/verify_no_divdi3.sh** - Script to verify no problematic symbols remain

---

## Testing Checklist

After rebuilding with these fixes:

- [ ] Module loads without "Unknown symbol" errors
- [ ] No `__divdi3` or `__udivdi3` symbols in module (run `verify_no_divdi3.sh`)
- [ ] STREAMON completes without hanging
- [ ] No watchdog reboots during normal operation
- [ ] Event processing thread runs continuously
- [ ] Camera streaming works correctly

---

## Build Instructions

```bash
# Set environment variables
export CROSS_COMPILE=mipsel-linux-
export KDIR=/path/to/kernel-3.10/source
export PATH=/path/to/toolchain/bin:$PATH

# Build the module
cd external/ingenic-sdk
SENSOR_MODEL=gc2053 ./build.sh t31

# Verify no problematic symbols
cd ../../driver
./verify_no_divdi3.sh ../external/ingenic-sdk/3.10/isp/tx-isp-t31.ko

# Deploy to device
scp ../external/ingenic-sdk/3.10/isp/tx-isp-t31.ko root@192.168.50.211:/tmp/
```

---

## Verification Commands

```bash
# On the device, check for symbol errors
insmod /tmp/tx-isp-t31.ko
dmesg | grep -i "unknown symbol"

# Check event thread is running
ps | grep tisp_events

# Test STREAMON (via prudynt or test application)
./prudynt &
# Should not hang or reboot
```

---

## Technical Background

### Why These Issues Occur on MIPS32

1. **Division Issue:**
   - MIPS32 has no native 64-bit division instruction
   - Compiler generates calls to libgcc functions (`__divdi3`, `__udivdi3`)
   - Kernel modules don't link with libgcc
   - Must use kernel-provided division helpers

2. **Completion Issue:**
   - Completion API changed between kernel 3.10 and 3.13
   - `INIT_COMPLETION` macro was replaced with `reinit_completion()` function
   - Must use the correct API for target kernel version

### Why Kernel 3.10?

The Ingenic T31 SoC vendor SDK is based on Linux kernel 3.10, which is a Long-Term Support (LTS) kernel version. Many embedded systems use this kernel for stability and vendor support.

---

## References

- Linux kernel 3.10 source: https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/?h=v3.10
- Completion API: `include/linux/completion.h`
- Division helpers: `include/linux/math64.h`
- MIPS architecture: `arch/mips/`

---

## Changelog

- **2025-10-17**: Initial fixes for MIPS32 division and event thread deadlock
  - Fixed 8 locations with 64-bit division operations
  - Added `INIT_COMPLETION` to prevent event thread deadlock
  - Verified kernel 3.10 API compatibility

