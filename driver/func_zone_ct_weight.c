#include "include/main.h"


  int32_t func_zone_ct_weight(int32_t arg1, int32_t* arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7)

{
    int32_t $v1 = arg2[3];
    int32_t $t0_1;
    int32_t $a2_1;
    
    if (arg1 < $v1)
        $t0_1 = arg2[1];
    
    
    if (arg1 >= $v1 || $t0_1 >= arg1)
        $a2_1 = arg3 << (arg4 & 0x1f);
    else if (arg2[2] < arg1)
        $a2_1 =
            (arg3 << (arg4 & 0x1f)) + fix_point_mult2_32(arg4, ($v1 - arg1) << (arg4 & 0x1f), arg6);
    else if (arg1 < *arg2)
        $a2_1 = (arg3 << (arg4 & 0x1f))
            + fix_point_mult2_32(arg4, (arg1 - $t0_1) << (arg4 & 0x1f), arg7);
    else
        $a2_1 = 0x100 << (arg4 & 0x1f);
    
    /* tailcall */
    return fix_point_div_32(arg4, fix_point_mult2_32(arg4, arg5, $a2_1), 0x100 << (arg4 & 0x1f));
}

