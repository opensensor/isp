#include "include/main.h"


  uint32_t tisp_math_exp2(int32_t arg1, char arg2, char arg3)

{
    uint32_t $a1_5 = arg2;
    int32_t $v1_2 = ((1 << ($a1_5 & 0x1f)) - 1) & arg1;
    uint32_t $s1 = arg3;
    uint32_t $s2 = arg1 >> ($a1_5 & 0x1f);
    
    if ($a1_5 < 6)
        return *(&__pow2_lut + ($v1_2 << ((5 - $a1_5) & 0x1f) << 2)) >> ((0x1e - $s1 - $s2) & 0x1f);
    
    uint32_t $a1_1 = $v1_2 >> (($a1_5 - 5) & 0x1f);
    int32_t $s0_1 = *(&__pow2_lut + ($a1_1 << 2));
    int32_t $lo_1;
    int32_t $hi_1;
    $hi_1 = HIGHD((*(&__pow2_lut + (($a1_1 + 1) << 2)) - $s0_1)
        * (((1 << (($a1_5 - 5) & 0x1f)) - 1) & $v1_2));
    $lo_1 = LOWD((*(&__pow2_lut + (($a1_1 + 1) << 2)) - $s0_1)
        * (((1 << (($a1_5 - 5) & 0x1f)) - 1) & $v1_2));
    return ($s0_1 + __lshrdi3($lo_1, $hi_1)) >> ((0x1e - $s1 - $s2) & 0x1f);
}

