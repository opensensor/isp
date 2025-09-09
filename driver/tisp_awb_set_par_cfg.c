#include "include/main.h"


  int32_t tisp_awb_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_142 = 0;
    
    for (int32_t i = 0x23; i != 0x3c; )
    {
        tisp_awb_param_array_set(i, $s1, &var_20_143);
        i += 1;
        $s1 += var_20_144;
    }
    
    return 0;
}

