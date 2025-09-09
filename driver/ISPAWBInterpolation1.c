#include "include/main.h"


  int32_t ISPAWBInterpolation1(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6)

{
    int32_t $s1 = arg5 << (arg1 & 0x1f);
    int32_t $t1 = arg3 << (arg1 & 0x1f);
    int32_t $s2 = arg4 - arg3;
    
    if (arg6 < arg5)
        return $s1 - fix_point_div_32(arg1, 
            fix_point_mult2_32(arg1, (arg5 - arg6) << (arg1 & 0x1f), arg2 - $t1), 
            $s2 << (arg1 & 0x1f));
    
    return $s1 + fix_point_div_32(arg1, 
        fix_point_mult2_32(arg1, (arg6 - arg5) << (arg1 & 0x1f), arg2 - $t1), $s2 << (arg1 & 0x1f));
}

