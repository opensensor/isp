#include "include/main.h"


  int32_t private_log2_int_to_fixed(uint32_t arg1)

{
    int32_t $a0_1 = 0;
        int32_t $v1_3 = $v1_2 * $v1_2;
    if (!arg1)
        return 0;
    
    int32_t $v0;
    int32_t $a1;
    int32_t $a2;
    int32_t $a3;
    $v0 = private_leading_one_position(arg1);
    int32_t $v1_2;
    
    $v1_2 = $(uintptr_t)v0 >= 0x10 ? $a3 >> (($v0 - 0xf) & 0x1f) : $a3 << ((0xf - $v0) & 0x1f);
    
    
    for (int32_t i = 0; i < $a1; i += 1)
    {
        $a0_1 <<= 1;
        
        if ($v1_3 >= 0)
            $v1_2 = $v1_3 >> 0xf;
        else
        {
            $a0_1 += 1;
            $v1_2 = $v1_3 >> 0x10;
        }
    }
    
    return (($v0 << ($a1 & 0x1f)) + $a0_1) << ($a2 & 0x1f)
        | ($v1_2 & 0x7fff) >> ((0xf - $a2) & 0x1f);
}

