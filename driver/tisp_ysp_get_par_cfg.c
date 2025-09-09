#include "include/main.h"


  int32_t tisp_ysp_get_par_cfg(int32_t arg1, int32_t* arg2)

{
    int32_t $s2 = arg1;
    int32_t var_20_115 = 0;
    *arg2 = 0;
    
    for (int32_t i = 0xb5; i != 0xe6; )
    {
        tisp_sharpen_param_array_get(i, $s2, &var_20_116);
        int32_t $v1_1 = var_20_117;
        i += 1;
        $s2 += $v1_1;
        *arg2 += $v1_1;
    }
    
    return 0;
}

