#include "include/main.h"


  int32_t fix_point_mult2_32(int32_t arg1, int32_t arg2, int32_t arg3) __attribute__((pure))

{
    uint32_t $v1 = 0xffffffff >> (-(arg1) & 0x1f);
    uint32_t $a3 = arg2 >> (arg1 & 0x1f);
    uint32_t $t0 = arg3 >> (arg1 & 0x1f);
    int32_t $a1 = $v1 & arg2;
    int32_t $a2 = $v1 & arg3;
    int32_t $lo_1;
    int32_t $hi_1;
    $hi_1 = HIGHD($a1 * $t0 + $a3 * $a2);
    $lo_1 = LOWD($a1 * $t0 + $a3 * $a2);
    return $lo_1 + (($a3 * $t0) << (arg1 & 0x1f)) + (($a1 * $a2) >> (arg1 & 0x1f));
}

