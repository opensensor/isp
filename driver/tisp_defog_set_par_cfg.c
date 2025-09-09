#include "include/main.h"


  int32_t tisp_defog_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_88 = 0;
    
    for (int32_t i = 0x35a; i != 0x380; )
    {
        tisp_defog_param_array_set(i, $s1, &var_20_89);
        i += 1;
        $s1 += var_20_90;
    }
    
    return 0;
}

