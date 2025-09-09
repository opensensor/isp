#include "include/main.h"


  int32_t tisp_adr_set_par_cfg(int32_t* arg1)

{
    int32_t* $s1 = arg1;
    int32_t var_20_113 = 0;
    
    for (int32_t i = 0x380; i != 0x3ac; )
    {
        tisp_adr_param_array_set(i, $s1, &var_20_114);
        i += 1;
        $s1 += var_20_115;
    }
    
    return 0;
}

