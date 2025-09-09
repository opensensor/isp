#include "include/main.h"


  int32_t tisp_blc_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_75 = 0;
    
    for (int32_t i = 0x3f5; i != 0x3ff; )
    {
        tisp_gb_param_array_set(i, $s1, &var_20_76);
        i += 1;
        $s1 += var_20_77;
    }
    
    return 0;
}

