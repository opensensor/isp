#include "include/main.h"


  int32_t tisp_lsc_set_par_cfg(int32_t arg1, int32_t arg2)

{
    int32_t var_20_78 = 0;
    
    if (arg1 == 1)
        tisp_lsc_param_array_set(0x59, arg2, &var_20_79);
    else if (!arg1)
    {
        int32_t $s0_1 = arg2;
        
        for (int32_t i = 0x54; i != 0x59; )
        {
            tisp_lsc_param_array_set(i, $s0_1, &var_20_80);
            i += 1;
            $s0_1 += var_20_81;
        }
        
        for (int32_t i_1 = 0x5c; i_1 != 0x5f; )
        {
            tisp_lsc_param_array_set(i_1, $s0_1, &var_20_82);
            i_1 += 1;
            $s0_1 += var_20_83;
        }
    }
    else if (arg1 == 2)
        tisp_lsc_param_array_set(0x5a, arg2, &var_20_84);
    else if (arg1 == 3)
        tisp_lsc_param_array_set(0x5b, arg2, &var_20_85);
    
    return 0;
}

