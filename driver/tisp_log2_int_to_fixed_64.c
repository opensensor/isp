#include "include/main.h"


  int32_t tisp_log2_int_to_fixed_64(uint32_t arg1, uint32_t arg2, char arg3, char arg4)

{
    uint32_t $s3 = arg3;
    uint32_t $s2 = arg4;
    if (!(arg1 | arg2))
        return 0;
    
    uint32_t $v0;
    int32_t $s0;
    
    if (!arg2)
    {
        $v0 = arg1;
        $s0 = 0;
    }
    else
    {
        $v0 = arg2;
        $s0 = 0x20;
    }
    
    int32_t $a2_1 = $(uintptr_t)v0 < 0x100 ? 1 : 0;
    
    if ($v0 >= isp_printf)
    {
        $v0 u>>= 0x10;
        $s0 += 0x10;
        $a2_1 = $(uintptr_t)v0 < 0x100 ? 1 : 0;
    }
    
    int32_t $a2_2 = $(uintptr_t)v0 < 0x10 ? 1 : 0;
    
    if (!$a2_1)
    {
        $v0 u>>= 8;
        $s0 += 8;
        $a2_2 = $(uintptr_t)v0 < 0x10 ? 1 : 0;
    }
    
    int32_t $a2_3 = $v0 < 4 ? 1 : 0;
    
    if (!$a2_2)
    {
        $v0 u>>= 4;
        $s0 += 4;
        $a2_3 = $v0 < 4 ? 1 : 0;
    }
    
    if (!$a2_3)
    {
        $v0 u>>= 2;
        $s0 += 2;
    }
    
    int32_t $v0_2;
    
    if ($v0 != 1)
    {
        $s0 += 1;
        $v0_2 = $(uintptr_t)s0 < 0x10 ? 1 : 0;
    }
    else
        $v0_2 = $(uintptr_t)s0 < 0x10 ? 1 : 0;
    
    void* const $v0_3;
    
    $v0_3 = !$v0_2 ? __lshrdi3 : __ashldi3;
    
    int32_t $v0_4;
    uint32_t $v1;
    $v0_4 = $v0_3();
    int32_t $s1 = $v0_4;
    int32_t $a0 = 0;
    int32_t $a2_4 = 0;
    
    for (int32_t i = 0; i < $s3; i += 1)
    {
        int32_t $v1_2 = (($v1 * $s1) << 1) + $hi_1;
        int32_t $lo_1;
        int32_t $hi_1;
        $hi_1 = HIGHD($s1 * $s1);
        $lo_1 = LOWD($s1 * $s1);
        $a2_4 = $a0 >> 0x1f | $a2_4 << 1;
        $a0 <<= 1;
        
        if (!((0x80000000 & $lo_1) | $v1_2))
        {
            $s1 = $v1_2 << 0x11 | $lo_1 >> 0xf;
            $v1 = $v1_2 >> 0xf;
        }
        else
        {
            $a2_4 += $a0 + 1 < $a0 ? 1 : 0;
            $s1 = $v1_2 << 0x10 | $lo_1 >> 0x10;
            $a0 += 1;
            $v1 = $v1_2 >> 0x10;
        }
    }
    
    int32_t $s0_1 = $s0 << ($s3 & 0x1f);
    int32_t $a0_1 = $s0_1 + $a0;
    return __ashldi3($a0_1, ($a0_1 < $s0_1 ? 1 : 0) + ($s0_1 >> 0x1f) + $a2_4, $s2)
        | __lshrdi3($s1 & 0x7fff, 0, 0xf - $s2);
}

