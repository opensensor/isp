#include "include/main.h"


  int32_t tisp_ydns_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_100 = 0;
    
    for (int32_t i = 0x3e6; i != 0x3f5; )
    {
        tisp_ydns_param_array_set(i, $s1, &var_20_101);
        i += 1;
        $s1 += var_20_102;
    }
    
    return 0;
}

