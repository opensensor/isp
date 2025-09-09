#include "include/main.h"


  int32_t tisp_gib_get_par_cfg(int32_t arg1, int32_t* arg2)

{
    int32_t $s2 = arg1;
    int32_t var_20_55 = 0;
    *arg2 = 0;
    
    for (int32_t i = 0x3e; i != 0x54; )
    {
        tisp_gib_param_array_get(i, $s2, &var_20_56);
        int32_t $v1_1 = var_20_57;
        i += 1;
        $s2 += $v1_1;
        *arg2 += $v1_1;
    }
    
    return 0;
}

