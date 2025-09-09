#include "include/main.h"


  int32_t tisp_lsc_upside_down_lut(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t $s2 = arg3 * 6;
    int32_t $s3 = arg1;
    int32_t $v0 = private_vmalloc($s2);
    int32_t i = 0;
    int32_t $s0_2 = (arg3 * 3 / 2) << 2;
    int32_t $s1_1 = (arg2 + 0x3fffffff) * $s0_2 + $s3;
    memset($v0, 0, $s2);
    
    while (i != arg2 >> 1)
    {
        memcpy($v0, $s3, $s2);
        memmove($s3, $s1_1, $s2);
        memcpy($s1_1, $v0, $s2);
        i += 1;
        $s3 += $s0_2;
        $s1_1 -= $s0_2;
    }
    
    private_vfree($v0);
    return 0;
}

