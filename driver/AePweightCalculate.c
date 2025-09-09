#include "include/main.h"


  int32_t AePweightCalculate(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)

{
    int32_t $s1 = arg2 << (arg1 & 0x1f);
    int32_t var_4 = $ra;
    if (!arg2)
        return 0;
    
    int32_t $ra;
    fix_point_div_32(arg1, (arg4 * arg3) << (arg1 & 0x1f), $s1);
    /* tailcall */
    return fix_point_div_32(arg1, fix_point_mult2_32(arg1, $s1, tisp_log2_fixed_to_fixed()), 
        arg3 << (arg1 & 0x1f));
}

