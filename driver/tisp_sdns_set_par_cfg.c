#include "include/main.h"


  int32_t tisp_sdns_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20 = 0;
    
    for (int32_t i = 0x105; (uintptr_t)i != 0x180; )
    {
        tisp_sdns_param_array_set(i, $s1, &var_20);
        i += 1;
        $s1 += var_20;
    }
    
    return 0;
}

