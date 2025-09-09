#include "include/main.h"


  int32_t tisp_ccm_set_par_cfg(int32_t arg1)

{
    int32_t $s1 = arg1;
    int32_t var_20_82 = 0;
    
    for (void* i = 0xa9; i != 0xb5; )
    {
        Tiziano_awb_set_gain(i, $s1, &var_20_83);
        i += 1;
        $s1 += var_20_84;
    }
    
    return 0;
}

