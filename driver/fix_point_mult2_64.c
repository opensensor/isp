#include "include/main.h"


  int32_t fix_point_mult2_64(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6)

{
    int32_t $v0;
    int32_t $v1;
    int32_t $v0_1;
    int32_t $v1_1;
    int32_t $a3 = arg4 & $v1;
    int32_t $s4_1 = arg3 & $v0;
    int32_t $v0_3;
    int32_t $v1_2;
    int32_t $lo_1;
    int32_t $hi_1;
    int32_t $fp_1 = arg5 & $v0;
    int32_t $lo_2;
    int32_t $hi_2;
    int32_t $v0_6;
    int32_t $v1_3;
    int32_t $lo_5;
    int32_t $hi_5;
    int32_t $lo_8;
    int32_t $hi_8;
    int32_t $lo_10;
    int32_t $hi_10;
    int32_t $lo_11;
    int32_t $hi_11;
    int32_t $v0_7;
    int32_t $v1_4;
    $v0 = __lshrdi3(0xffffffff, 0xffffffff, 0x40 - arg1);
    $v0_1 = __lshrdi3(arg3, arg4, arg1);
    $v0_3 = __lshrdi3(arg5, arg6, arg1, $a3, $v0, $a3);
    $hi_1 = HIGHD($v1_2 * $v0_1 + $v0_3 * $v1_1);
    $lo_1 = LOWD($v1_2 * $v0_1 + $v0_3 * $v1_1);
    $hi_2 = HIGHD($v0_1 * $v0_3);
    $lo_2 = LOWD($v0_1 * $v0_3);
    $v0_6 = __ashldi3($lo_2, $lo_1 + $hi_2, arg1);
    $hi_5 = HIGHD($v0_1 * $fp_1);
    $lo_5 = LOWD($v0_1 * $fp_1);
    $hi_8 = HIGHD($s4_1 * $v0_3);
    $lo_8 = LOWD($s4_1 * $v0_3);
    $hi_10 = HIGHD((arg6 & $v1) * $s4_1 + $a3 * $fp_1);
    $lo_10 = LOWD((arg6 & $v1) * $s4_1 + $a3 * $fp_1);
    $hi_11 = HIGHD($s4_1 * $fp_1);
    $lo_11 = LOWD($s4_1 * $fp_1);
    $v0_7 = __lshrdi3($lo_11, $lo_10 + $hi_11, arg1);
    return $lo_5 + $lo_8 + $v0_6 + $v0_7;
}

