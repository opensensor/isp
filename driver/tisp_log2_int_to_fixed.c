#include "include/main.h"


  int32_t tisp_log2_int_to_fixed(uint32_t arg1, char arg2, char arg3) __attribute__((pure))

{
    uint32_t $a1 = arg2;
    uint32_t $a2 = arg3;
    
    if (!arg1)
        return 0;
    
    uint32_t $v1_1;
    uint32_t $t0_1;
    
    if (arg1 < isp_printf)
    {
        $t0_1 = arg1;
        $v1_1 = 0;
    }
    else
    {
        $t0_1 = arg1 >> 0x10;
        $v1_1 = 0x10;
    }
    
    int32_t $v0_2 = $t0_1 < 0x10 ? 1 : 0;
    
    if ($t0_1 >= 0x100)
    {
        $t0_1 u>>= 8;
        $v1_1 = $v1_1 + 8;
        $v0_2 = $t0_1 < 0x10 ? 1 : 0;
    }
    
    int32_t $v0_3 = $t0_1 < 4 ? 1 : 0;
    
    if (!$v0_2)
    {
        $t0_1 u>>= 4;
        $v1_1 = $v1_1 + 4;
        $v0_3 = $t0_1 < 4 ? 1 : 0;
    }
    
    if (!$v0_3)
    {
        $t0_1 u>>= 2;
        $v1_1 = $v1_1 + 2;
    }
    
    if ($t0_1 != 1)
        $v1_1 = $v1_1 + 1;
    
    uint32_t $a0;
    
    $a0 = $v1_1 >= 0x10 ? arg1 >> (($v1_1 - 0xf) & 0x1f) : arg1 << ((0xf - $v1_1) & 0x1f);
    
    int32_t $v0_6 = 0;
    
    for (int32_t i = 0; i < $a1; i += 1)
    {
        int32_t $a0_1 = $a0 * $a0;
        $v0_6 <<= 1;
        
        if ($a0_1 >= 0)
            $a0 = $a0_1 >> 0xf;
        else
        {
            $v0_6 += 1;
            $a0 = $a0_1 >> 0x10;
        }
    }
    
    return (($v1_1 << ($a1 & 0x1f)) + $v0_6) << ($a2 & 0x1f)
        | ($a0 & 0x7fff) >> ((0xf - $a2) & 0x1f);
}

