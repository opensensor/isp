#include "include/main.h"


  uint32_t tiziano_bcsh_StrenCal(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6)

{
    int32_t $a2_1 = arg3 - arg1;
    int32_t $t0_2 = arg2 - arg3;
    int32_t $t1_1 = arg4 - arg5;
    if (!arg6)
        /* tailcall */
        return tiziano_bcsh_StrenCal.part.0(arg1, arg2, arg3, arg4, arg5);
    
    
    if (arg2 < arg3)
        $t0_2 = arg3 - arg2;
    
    
    if (arg4 < arg5)
        $t1_1 = arg5 - arg4;
    
    if (arg1 >= arg3)
        $a2_1 = arg1 - arg3;
    
    int32_t $lo_1;
    int32_t $hi_1;
    $hi_1 = HIGHD($a2_1 * $t1_1 + $t0_2 * arg5);
    $lo_1 = LOWD($a2_1 * $t1_1 + $t0_2 * arg5);
    return $lo_1 / $t0_2;
}

