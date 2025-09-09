#include "include/main.h"


  int32_t tisp_set_ae_freeze(int32_t arg1)

{
    int32_t var_a0_41;
    memset(&var_a0_42, 0, 0x98);
    memcpy(&var_a0_43, &dmsc_sp_d_w_stren_wdr_array, 0x98);
    
    if (arg1 != 1)
    {
        if (arg1)
        {
            isp_printf(2, "Can\'t output the width(%d)!\\n", arg1);
            return 0xffffffff;
        }
        
        var_a0_44 = 0;
        int32_t var_40_2_3 = 0;
    }
    else
    {
        var_a0_45 = arg1;
        int32_t var_40_1_8 = arg1;
    }
    
    for (int32_t i = 0; i < 0x88; i += 1)
    {
        char var_128_26[0x88];
        char var_90_34[0x50];
        var_128_27[i] = var_90_35[i];
    }
    
    int32_t var_9c_15;
    int32_t var_98_138;
    int32_t var_94_32;
    tisp_ae_manual_set(var_a0_46, var_9c_16, var_98_139, var_94_33);
    return 0;
}

