#include "include/main.h"


  int32_t tisp_s_ae_attr(int32_t arg1, int32_t arg2)

{
    int32_t $a1;
    int32_t arg_4 = $a1;
    int32_t $a2;
    int32_t arg_8 = $a2;
    int32_t $a3;
    int32_t arg_c = $a3;
    int32_t var_a0_32;
    memset(&var_a0_33, 0, 0x98);
    memcpy(&var_a0_34, &dmsc_sp_d_w_stren_wdr_array, 0x98);
    int32_t var_6c_14 = arg2;
    
    for (int32_t i = 0; i < 0x88; i += 1)
    {
        char var_128_22[0x88];
        char var_90_30[0x24];
        var_128_23[i] = var_90_31[i];
    }
    
    int32_t var_9c_11;
    int32_t var_98_134;
    tisp_ae_manual_set(var_a0_35, var_9c_12, var_98_135, arg1);
    return 0;
}

