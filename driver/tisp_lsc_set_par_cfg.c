#include "include/main.h"


  int32_t tisp_lsc_set_par_cfg(int32_t arg1, int32_t arg2)

{
    int32_t var_20 = 0;
        int32_t $s0_1 = arg2;
    
    if (arg1 == 1)
        tisp_lsc_param_array_set(0x59, arg2, &var_20);
    else if (!arg1)
    {
        
        for (int32_t i = 0x54; (uintptr_t)i != 0x59; )
        {
            tisp_lsc_param_array_set(i, $s0_1, &var_20);
            i += 1;
            $s0_1 += var_20;
        }
        
        for (int32_t i_1 = 0x5c; (uintptr_t)i_1 != 0x5f; )
        {
            tisp_lsc_param_array_set(i_1, $s0_1, &var_20);
            i_1 += 1;
            $s0_1 += var_20;
        }
    }
    else if (arg1 == 2)
        tisp_lsc_param_array_set(0x5a, arg2, &var_20_11);
    else if (arg1 == 3)
        tisp_lsc_param_array_set(0x5b, arg2, &var_20_12);
    
    return 0;
}

