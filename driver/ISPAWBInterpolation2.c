#include "include/main.h"


  int32_t ISPAWBInterpolation2(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6)

{
    int32_t $s2 = arg4 - arg3;
    int32_t $a2 = arg2 - (arg3 << (arg1 & 0x1f));
    
    if (arg6 < arg5)
        return arg5 - fix_point_div_32(arg1, fix_point_mult2_32(arg1, arg5 - arg6, $a2), 
            $s2 << (arg1 & 0x1f));
    
    return arg5
        + fix_point_div_32(arg1, fix_point_mult2_32(arg1, arg6 - arg5, $a2), $s2 << (arg1 & 0x1f));
}

