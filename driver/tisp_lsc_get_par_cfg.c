#include "include/main.h"


  int32_t tisp_lsc_get_par_cfg(int32_t arg1, int32_t* arg2)

{
    int32_t $s0 = arg1;
    int32_t var_28 = 0;
        int32_t $v1_1 = var_28;
    *arg2 = 0;
    
    for (int32_t i = 0x54; (uintptr_t)i != 0x59; )
    {
        tisp_lsc_param_array_get(i, $s0, &var_28);
        i += 1;
        $s0 += $v1_1;
        *arg2 += $v1_1;
    }
    
    for (int32_t i_1 = 0x5c; (uintptr_t)i_1 != 0x5f; )
    {
        int32_t $v1_2 = var_28;
        tisp_lsc_param_array_get(i_1, $s0, &var_28);
        i_1 += 1;
        $s0 += $v1_2;
        *arg2 += $v1_2;
    }
    
    return 0;
}

