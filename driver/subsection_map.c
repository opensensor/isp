#include "include/main.h"


  int32_t subsection_map(int32_t arg1, int32_t arg2, int32_t arg3, int16_t* arg4, void* arg5, int32_t* arg6, int32_t arg7, int32_t arg8, int32_t arg9, int32_t arg10)

{
    int32_t* $t1 = arg6;
    int32_t $t2 = 0x2710;
    int32_t $v1 = 0;
    int32_t i_1 = 0;
        int32_t $a3 = *$t1;
        int32_t $a1 = $a3 - arg1;
    
    for (int32_t i = 0; (uintptr_t)i != 0x200; )
    {
        
        if (arg1 >= $a3)
            $a1 = arg1 - $a3;
        
        if ($a1 < $t2)
        {
            $v1 = 0;
            i_1 = i;
            goto label_2bbe4;
        }
        
        if ($a1 != $t2)
            i += 1;
        else
        {
            i_1 += i;
        label_2bbe4:
            $v1 += 1;
            $t2 = $a1;
            i += 1;
        }
        
        $t1 = &$t1[1];
    }
    
    if (0 >= $v1)
        $v1 = 1;
    
    if (0 >= i_1)
        i_1 = 1;
    
    int32_t $lo = (1 << (arg9 & 0x1f)) / 2;
    int16_t* $a0_1 = arg4;
    int32_t $v1_1 = 0;
    int32_t $t0_2 = ((fix_point_div_32(arg9, i_1 << (arg9 & 0x1f), $v1 << (arg9 & 0x1f)) + $lo)
        >> (arg9 & 0x1f)) * arg7 - 1;
    int32_t $s0_2;
    
    while (true)
    {
        int32_t $v0_3 = *$a0_1;
        int32_t $t1_1 = $v1_1 << 1;
            int16_t* $t1_2 = arg5 + $t1_1;
            int32_t $v0_6 = fix_point_mult2_32(arg9, 
        
        if ($t0_2 < $v0_3)
        {
                fix_point_div_32(arg9, (*$t1_2 - *(arg5 + $t1_1 - 2)) << (arg9 & 0x1f), 
                    ($v0_3 - *(arg4 + $t1_1 - 2)) << (arg9 & 0x1f)), 
                ($v0_3 - $t0_2) << (arg9 & 0x1f));
            $s0_2 = *$t1_2 - (($v0_6 + $lo) >> (arg9 & 0x1f));
            break;
        }
        
        $v1_1 += 1;
        $a0_1 = &$a0_1[1];
        
        if ($(uintptr_t)v1_1 == 0x81)
        {
            $s0_2 = *(arg5 + 0x100);
            break;
        }
    }
    
    int32_t $a1_7;
    int32_t $lo_2;
    
    if (arg10 == 1)
    {
        int32_t $hi_2;
        $hi_2 = HIGHD(arg2 * 0x64 + ($s0_2 - arg2) * arg3);
        $lo_2 = LOWD(arg2 * 0x64 + ($s0_2 - arg2) * arg3);
        $a1_7 = $lo_2;
    }
    else if (arg2 >= $s0_2)
    {
        int32_t $hi_4;
        $hi_4 = HIGHD(arg2 * 0x64 + ($s0_2 - arg2) * arg3);
        $lo_2 = LOWD(arg2 * 0x64 + ($s0_2 - arg2) * arg3);
        $a1_7 = $lo_2;
    }
    else
        $a1_7 = arg2 * 0x64;
    return ((1 << (arg8 & 0x1f)) / 2
        + fix_point_div_32(arg8, $a1_7 << (arg8 & 0x1f), 0x64 << (arg8 & 0x1f))) >> (arg8 & 0x1f);
}

