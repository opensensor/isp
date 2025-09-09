#include "include/main.h"


  int32_t tisp_clm_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_112 = 0;
    
    for (int32_t i = 0x357; i != 0x35a; )
    {
        tisp_clm_param_array_set(i, $s1, &var_20_113);
        i += 1;
        $s1 += var_20_114;
    }
    
    return 0;
}

