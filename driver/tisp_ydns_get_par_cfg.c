#include "include/main.h"


  int32_t tisp_ydns_get_par_cfg(int32_t arg1, int32_t* arg2)

{
    int32_t $s2 = arg1;
    int32_t var_20_140 = 0;
    *arg2 = 0;
    
    for (int32_t i = 0x3e6; i != 0x3f5; )
    {
        tisp_ydns_param_array_get(i, $s2, &var_20_141);
        int32_t $v1_1 = var_20_142;
        i += 1;
        $s2 += $v1_1;
        *arg2 += $v1_1;
    }
    
    return 0;
}

