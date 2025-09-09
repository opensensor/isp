#include "include/main.h"


  int32_t tisp_bcsh_set_par_cfg(int32_t* arg1)

{
    int32_t* $s1 = arg1;
    int32_t var_20_149 = 0;
    
    for (int32_t i = 0x3c0; i != 0x3e6; )
    {
        tisp_bcsh_param_array_set(i, $s1, &var_20_150);
        i += 1;
        $s1 += var_20_151;
    }
    
    return 0;
}

