# MIPS32 64-bit Division Fix

## Problem

The driver failed to load on MIPS32 architecture with the following errors:

```
[   85.500440] tx_isp_t31: Unknown symbol __divdi3 (err 0)
[   85.500552] tx_isp_t31: Unknown symbol __udivdi3 (err 0)
```

These symbols (`__divdi3` for signed 64-bit division and `__udivdi3` for unsigned 64-bit division) are compiler-generated helper functions for 64-bit division on 32-bit architectures. They are not available in the Linux kernel module context.

## Root Cause

The code in `tx_isp_tuning.c` was performing direct division operations on 64-bit integers using the `/` operator:

```c
// Problematic code - generates __divdi3/__udivdi3 calls on MIPS32
v = (int32_t)( ((int64_t)(d - t) * w) / DEN_DT ) + t;
defog_block_sizem_work[i] = (uint32_t)((uint64_t)defog_frame_h * i / 10);
```

On 32-bit architectures like MIPS32, the compiler generates calls to `__divdi3` (signed) and `__udivdi3` (unsigned) to handle 64-bit division, but these symbols are not exported by the kernel.

## Solution

Replace all 64-bit division operations with kernel-provided helper functions:

1. **For signed 64-bit division**: Use `div64_s64(dividend, divisor)`
2. **For unsigned 64-bit division**: Use `do_div(dividend, divisor)` (modifies dividend in-place)

### Changes Made

#### 1. Added Required Header

Added `#include <linux/math64.h>` to provide `div64_s64()` and `do_div()` functions.

#### 2. Fixed Signed 64-bit Divisions

**Before:**
```c
if (d >= t)
    v = (int32_t)( ((int64_t)(d - t) * w) / DEN_DT ) + t;
else
    v = t - (int32_t)( ((int64_t)(t - d) * w) / DEN_DT );
```

**After:**
```c
if (d >= t) {
    int64_t tmp = (int64_t)(d - t) * w;
    v = (int32_t)div64_s64(tmp, DEN_DT) + t;
} else {
    int64_t tmp = (int64_t)(t - d) * w;
    v = t - (int32_t)div64_s64(tmp, DEN_DT);
}
```

#### 3. Fixed Unsigned 64-bit Divisions

**Before:**
```c
defog_block_sizem_work[i] = (uint32_t)((uint64_t)defog_frame_h * i / 10);
```

**After:**
```c
u64 tmp = (u64)defog_frame_h * i;
do_div(tmp, 10);
defog_block_sizem_work[i] = (uint32_t)tmp;
```

#### 4. Fixed 32-bit Divisions That Could Overflow

Some divisions were on 32-bit values but the intermediate multiplication could overflow:

**Before:**
```c
v0_4 = ((v0_5 - t0_1) * t6) / a1 + t0_1;
```

**After:**
```c
v0_4 = ((v0_5 - t0_1) * t6) / (uint32_t)a1 + t0_1;
```

Note: These were already 32-bit divisions, but we added explicit casts to ensure the compiler doesn't promote to 64-bit.

## Locations Fixed

All fixes were in `driver/tx_isp_tuning.c`:

1. **Lines 315-329**: CCM interpolation D->T (signed division)
2. **Lines 347-361**: CCM interpolation T->A (signed division)
3. **Lines 407-426**: BCSH hue interpolation (signed division)
4. **Lines 6533-6560**: Defog block size calculation (unsigned division)
5. **Lines 9504-9511**: AE parameter interpolation (cast to ensure 32-bit)
6. **Lines 9521-9535**: AE parameter interpolation (cast to ensure 32-bit)
7. **Lines 11021-11036**: Tiziano CCM D->T interpolation (signed division)
8. **Lines 11050-11065**: Tiziano CCM T->A interpolation (signed division)

## Verification

After these changes, the module should compile without generating `__divdi3` or `__udivdi3` symbol references.

To verify:
```bash
# After building the module
mipsel-linux-nm tx-isp-t31.ko | grep -E "__divdi3|__udivdi3"
# Should return no results
```

## Technical Notes

### Why This Happens on MIPS32

- MIPS32 is a 32-bit architecture with no native 64-bit division instruction
- The compiler generates calls to software division routines (`__divdi3`, `__udivdi3`)
- In userspace, these are provided by libgcc
- In kernel space, these symbols are not available (kernel doesn't link with libgcc)
- The kernel provides its own optimized division helpers instead

### Kernel Division Helpers

- `div64_s64(s64 dividend, s64 divisor)` → `s64` result (signed 64-bit ÷ 64-bit)
- `div64_u64(u64 dividend, u64 divisor)` → `u64` result (unsigned 64-bit ÷ 64-bit)
- `do_div(u64 &dividend, u32 divisor)` → modifies dividend in-place (unsigned 64-bit ÷ 32-bit)
- `div_s64(s64 dividend, s32 divisor)` → `s64` result (signed 64-bit ÷ 32-bit)
- `div_u64(u64 dividend, u32 divisor)` → `u64` result (unsigned 64-bit ÷ 32-bit)

### Performance Considerations

The kernel division helpers are optimized for each architecture:
- On 64-bit architectures, they compile to native division instructions
- On 32-bit architectures, they use optimized software implementations
- They are more efficient than the generic libgcc implementations

## References

- Linux kernel documentation: `Documentation/core-api/kernel-api.rst`
- `include/linux/math64.h` - Kernel 64-bit division helpers
- `lib/div64.c` - Implementation of division helpers

