#include "include/main.h"


  int32_t tisp_bcsh_set_par_cfg(int32_t* arg1)

{
    int32_t* $s1 = arg1;
    int32_t var_20 = 0;
    
    for (int32_t i = 0x3c0; (uintptr_t)i != 0x3e6; )
    {
        tisp_bcsh_param_array_set(i, $s1, &var_20);
        i += 1;
        $s1 += var_20;
    }
    
    return 0;
}

