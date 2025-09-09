#include "include/main.h"


  int32_t fix_point_mult2(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6)

{
    int32_t $s0;
    int32_t $s6;
    
    if (!arg1)
    {
        $s0 = 0;
        $s6 = 0;
    }
    else
    {
        int32_t $v0_2;
        int32_t $v1_1;
        $v0_2 = __lshrdi3(0xffffffff, 0xffffffff, 0x40 - arg1);
        $s0 = $v0_2;
        $s6 = $v1_1;
    }
    
    int32_t $v0_3;
    int32_t $v1_2;
    $v0_3 = __lshrdi3(arg3, arg4, arg1);
    int32_t $v0_4;
    int32_t $v1_3;
    $v0_4 = __lshrdi3(arg5, arg6, arg1);
    int32_t $s5_1 = $s0 & arg3;
    int32_t $lo_1;
    int32_t $hi_1;
    $hi_1 = HIGHD($v1_3 * $v0_3 + $v0_4 * $v1_2);
    $lo_1 = LOWD($v1_3 * $v0_3 + $v0_4 * $v1_2);
    int32_t $s0_1 = $s0 & arg5;
    int32_t $lo_2;
    int32_t $hi_2;
    $hi_2 = HIGHD($v0_3 * $v0_4);
    $lo_2 = LOWD($v0_3 * $v0_4);
    int32_t $v0_8;
    int32_t $v1_4;
    $v0_8 = __ashldi3($lo_2, $lo_1 + $hi_2, arg1);
    int32_t $lo_5;
    int32_t $hi_5;
    $hi_5 = HIGHD($v0_3 * $s0_1);
    $lo_5 = LOWD($v0_3 * $s0_1);
    int32_t $lo_8;
    int32_t $hi_8;
    $hi_8 = HIGHD($s5_1 * $v0_4);
    $lo_8 = LOWD($s5_1 * $v0_4);
    int32_t $lo_10;
    int32_t $hi_10;
    $hi_10 = HIGHD(($s6 & arg6) * $s5_1 + ($s6 & arg4) * $s0_1);
    $lo_10 = LOWD(($s6 & arg6) * $s5_1 + ($s6 & arg4) * $s0_1);
    int32_t $lo_11;
    int32_t $hi_11;
    $hi_11 = HIGHD($s5_1 * $s0_1);
    $lo_11 = LOWD($s5_1 * $s0_1);
    int32_t $v0_9;
    int32_t $v1_5;
    $v0_9 = __lshrdi3($lo_11, $lo_10 + $hi_11, arg1);
    return $lo_5 + $lo_8 + $v0_8 + $v0_9;
}

