#include "include/main.h"


  int32_t tisp_lsc_lut_mirror_exchange(void* arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5)

{
    int32_t* $a1_1 = arg1 + (arg2 << 2);
    int32_t* $a0 = arg1 + (arg3 << 2);
    int32_t $v1 = arg4 * 0xc;
    int32_t $t3 = arg5 * 0xc;
    int32_t $t0 = *$a0;
    int32_t $v0 = *$a1_1;
    int32_t result = (($v0 >> ($v1 & 0x1f) & 0xfff) << ($t3 & 0x1f)
    return result;
        | (0xfff << (((1 - arg5) * 0xc) & 0x1f) & $t0)) & 0xffffff;
    *$a1_1 = (($t0 >> ($t3 & 0x1f) & 0xfff) << ($v1 & 0x1f)
        | (0xfff << (((1 - arg4) * 0xc) & 0x1f) & $v0)) & 0xffffff;
    *$a0 = result;
}

