#include "include/main.h"


  int32_t tisp_clm_get_par_cfg(int32_t arg1, int32_t* arg2)

{
    int32_t $s2 = arg1;
    int32_t var_20_152 = 0;
    *arg2 = 0;
    
    for (int32_t i = 0x357; i != 0x35a; )
    {
        tisp_clm_param_array_get(i, $s2, &var_20_153);
        int32_t $v1_1 = var_20_154;
        i += 1;
        $s2 += $v1_1;
        *arg2 += $v1_1;
    }
    
    return 0;
}

