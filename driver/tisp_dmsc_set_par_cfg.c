#include "include/main.h"


  int32_t tisp_dmsc_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_119 = 0;
    
    for (int32_t i = 0x5f; i != 0xa9; )
    {
        tisp_dmsc_param_array_set(i, $s1, &var_20_120);
        i += 1;
        $s1 += var_20_121;
    }
    
    return 0;
}

