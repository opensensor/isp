#include "include/main.h"


  uint32_t tiziano_gib_deir_interpolate(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t* arg5, int32_t* arg6)

{
    int32_t* i = arg6;
    int32_t* $t1 = arg5;
    int32_t $a2_1 = arg3 + ((arg3 ^ arg4) < 1 ? 1 : 0) - arg4;
        int32_t $v0_1 = *i;
    uint32_t result;
    
    do
    {
        int32_t $lo_2;
        int32_t $hi_2;
        $hi_2 = HIGHD($v0_1 * $a2_1 + (*$t1 - $v0_1) * (arg2 - arg4));
        $lo_2 = LOWD($v0_1 * $a2_1 + (*$t1 - $v0_1) * (arg2 - arg4));
        i = &i[1];
        $t1 = &$t1[1];
        result = $lo_2 / $a2_1;
        arg1 += 4;
        *(arg1 - 4) = result;
    } while (&i[0x21] != i);
    
    return result;
}

