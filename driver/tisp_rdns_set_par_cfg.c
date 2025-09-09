#include "include/main.h"


  int32_t tisp_rdns_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_107 = 0;
    
    for (int32_t i = 0x432; i != 0x447; )
    {
        tisp_rdns_param_array_set(i, $s1, &var_20_108);
        i += 1;
        $s1 += var_20_109;
    }
    
    return 0;
}

