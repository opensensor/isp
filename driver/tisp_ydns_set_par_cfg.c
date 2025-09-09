#include "include/main.h"


  int32_t tisp_ydns_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20 = 0;
    
    for (int32_t i = 0x3e6; (uintptr_t)i != 0x3f5; )
    {
        tisp_ydns_param_array_set(i, $s1, &var_20);
        i += 1;
        $s1 += var_20;
    }
    
    return 0;
}

