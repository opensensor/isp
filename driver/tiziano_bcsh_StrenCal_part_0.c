#include "include/main.h"


  uint32_t tiziano_bcsh_StrenCal.part.0(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) __attribute__((pure))

{
    int32_t $a0 = arg2 - arg1;
    int32_t $a1 = arg2 - arg3;
    int32_t $t0 = arg4 - arg5;
    
    if (arg2 < arg1)
        $a0 = arg1 - arg2;
    
    
    if (arg2 < arg3)
        $a1 = arg3 - arg2;
    
    
    if (arg4 < arg5)
        $t0 = arg5 - arg4;
    
    int32_t $lo_1;
    int32_t $hi_1;
    $hi_1 = HIGHD($a0 * $t0 + $a1 * arg4);
    $lo_1 = LOWD($a0 * $t0 + $a1 * arg4);
    return $lo_1 / $a1;
}

