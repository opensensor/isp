#include "include/main.h"


  int32_t tiziano_wdr_5x5_param_distance(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, void* arg5)

{
    int32_t $lo;
    
    $lo = arg3 >= arg1 ? (arg3 - arg1) / 4 : (arg1 - arg3) / 4;
    
    int32_t $lo_1;
    
    $lo_1 = arg4 >= arg2 ? (arg4 - arg2) / 4 : (arg2 - arg4) / 4;
    
    int32_t* $v1_1 = arg5 + 0x78;
    int32_t i = 0x1e;
    int32_t $lo_3;
    int32_t $hi_5;
    $hi_5 = HIGHD($lo_1 * $lo_1 + $lo * $lo);
    $lo_3 = LOWD($lo_1 * $lo_1 + $lo * $lo);
    
    do
    {
        if (*$v1_1 >= $lo_3)
            return i + 1;
        
        i -= 1;
        $v1_1 = &$v1_1[-1];
    } while (i != 0xffffffff);
    
    return 0;
}

