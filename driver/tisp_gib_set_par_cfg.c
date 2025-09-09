#include "include/main.h"


  int32_t tisp_gib_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_58 = 0;
    
    for (int32_t i = 0x3e; i != 0x54; )
    {
        tisp_gib_param_array_set(i, $s1, &var_20_59);
        i += 1;
        $s1 += var_20_60;
    }
    
    return 0;
}

