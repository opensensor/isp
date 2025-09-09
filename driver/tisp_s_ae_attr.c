#include "include/main.h"


  int32_t tisp_s_ae_attr(int32_t arg1, int32_t arg2)

{
    int32_t $a1;
    int32_t arg_4 = $a1;
    int32_t $a2;
    int32_t arg_8 = $a2;
    int32_t $a3;
    int32_t arg_c = $a3;
    int32_t var_a0;
    int32_t var_6c = arg2;
        char var_128[0x88];
        char var_90[0x24];
    memset(&var_a0, 0, 0x98);
    memcpy(&var_a0, &dmsc_sp_d_w_stren_wdr_array, 0x98);
    
    for (int32_t i = 0; (uintptr_t)i < 0x88; i += 1)
    {
        var_128[i] = var_90[i];
    }
    
    int32_t var_9c_5;
    int32_t var_98_45;
    tisp_ae_manual_set(var_a0_7, var_9c_6, var_98_46, arg1);
    return 0;
}

