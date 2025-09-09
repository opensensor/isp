#include "include/main.h"


  int32_t private_log2_int_to_fixed_64(uint32_t arg1, uint32_t arg2, char arg3, char arg4)

{
    uint32_t $s2 = arg3;
    
    if (!(arg1 | arg2))
        return 0;
    
    uint32_t $s1_1 = arg4;
    int32_t $v0_1;
    int32_t $a3;
    int32_t $t0_2;
    $v0_1 = private_leading_one_position_64(arg1, arg2);
    void* const $v0_3;
    int32_t $a0;
    int32_t $a1;
    int32_t $a2_1;
    
    if ($v0_1 >= 0x10)
    {
        $a2_1 = $v0_1 - 0xf;
        $a0 = $t0_2;
        $a1 = $a3;
        $v0_3 = __lshrdi3;
    }
    else
    {
        $a2_1 = 0xf - $v0_1;
        $a0 = $t0_2;
        $a1 = $a3;
        $v0_3 = __ashldi3;
    }
    
    int32_t $v0_4;
    uint32_t $v1_1;
    $v0_4 = $v0_3($a0, $a1, $a2_1);
    int32_t $s0_1 = $v0_4;
    int32_t $a0_1 = 0;
    int32_t $a2_2 = 0;
    
    for (int32_t i = 0; i < $s2; i += 1)
    {
        int32_t $lo_1;
        int32_t $hi_1;
        $hi_1 = HIGHD($s0_1 * $s0_1);
        $lo_1 = LOWD($s0_1 * $s0_1);
        int32_t $v1_3 = (($v1_1 * $s0_1) << 1) + $hi_1;
        $a2_2 = $a0_1 >> 0x1f | $a2_2 << 1;
        $a0_1 <<= 1;
        
        if (!((0x80000000 & $lo_1) | $v1_3))
        {
            $s0_1 = $v1_3 << 0x11 | $lo_1 >> 0xf;
            $v1_1 = $v1_3 >> 0xf;
        }
        else
        {
            $a2_2 += $a0_1 + 1 < $a0_1 ? 1 : 0;
            $s0_1 = $v1_3 << 0x10 | $lo_1 >> 0x10;
            $a0_1 += 1;
            $v1_1 = $v1_3 >> 0x10;
        }
    }
    
    int32_t $s2_1 = $v0_1 << ($s2 & 0x1f);
    int32_t $a0_2 = $s2_1 + $a0_1;
    return __ashldi3($a0_2, ($a0_2 < $s2_1 ? 1 : 0) + ($s2_1 >> 0x1f) + $a2_2, $s1_1)
        | __lshrdi3($s0_1 & 0x7fff, 0, 0xf - $s1_1);
}

