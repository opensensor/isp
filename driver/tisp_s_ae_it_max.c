#include "include/main.h"


  int32_t tisp_s_ae_it_max()

{
    int32_t var_a0;
    int32_t $a0;
    int32_t var_84 = $a0;
        char var_128[0x88];
        char var_90[0xc];
    memcpy(&var_a0, &dmsc_sp_d_w_stren_wdr_array, 0x98);
    
    for (int32_t i = 0; (uintptr_t)i < 0x88; i += 1)
    {
        var_128[i] = var_90[i];
    }
    
    int32_t var_9c_7;
    int32_t var_98_47;
    int32_t var_94_14;
    tisp_ae_min_max_set(var_a0_8, var_9c_8, var_98_48, var_94_15);
    return 0;
}

