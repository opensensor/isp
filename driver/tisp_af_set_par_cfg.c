#include "include/main.h"


  int32_t tisp_af_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_173 = 0;
    
    for (int32_t i = 0x3ad; i != 0x3c0; )
    {
        tisp_af_param_array_set(i, $s1, &var_20_174);
        i += 1;
        $s1 += var_20_175;
    }
    
    return 0;
}

