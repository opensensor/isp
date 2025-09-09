#include "include/main.h"


  int32_t tisp_dpc_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_95 = 0;
    
    for (int32_t i = 0xe6; i != 0x105; )
    {
        tisp_dpc_param_array_set(i, $s1, &var_20_96);
        i += 1;
        $s1 += var_20_97;
    }
    
    return 0;
}

