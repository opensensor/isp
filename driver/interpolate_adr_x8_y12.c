#include "include/main.h"


  int32_t interpolate_adr_x8_y12(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) __attribute__((pure))

{
    int32_t $v1;
    int32_t $a3;
    
    if (arg3 >= arg4)
    {
        $a3 = arg3 - arg4;
        $v1 = 0;
    }
    else
    {
        $a3 = arg4 - arg3;
        $v1 = 1;
    }
    
    int32_t $a2 = arg3 << 0xa;
    int32_t $lo = (($a3 * (arg5 - arg1)) << 0xa) / (arg2 - arg1);
    int32_t $v0_1 = $a2 - $lo;
    
    if ($v1)
        $v0_1 = $a2 + $lo;
    
    return ($v0_1 + 0x200) / 0x400;
}

