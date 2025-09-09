#include "include/main.h"


  int32_t tisp_ae_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_179 = 0;
    
    for (int32_t i = 1; i != 0x23; )
    {
        tisp_ae_param_array_set(i, $s1, &var_20_180);
        i += 1;
        $s1 += var_20_181;
    }
    
    return 0;
}

