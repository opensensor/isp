#include "include/main.h"


  int32_t tisp_s_antiflick(uint32_t arg1)

{
    int32_t var_28 = 0xc;
    int32_t var_2c = 0x18;
            uint32_t var_60 = arg1;
    
    if (arg1)
        flicker_hz = arg1;
    
    int32_t var_38;
    memcpy(&var_38, 0x94d68, 0xc);
    uint32_t flicker_hz_1;
    memcpy(&flicker_hz_1, 0x94d74, var_2c);
    
    if ((uintptr_t)arg1 == 0x32 || (uintptr_t)arg1 == 0x3c)
        var_38 = 1;
    else
    {
        if (arg1)
        {
            isp_printf(); // Fixed: macro call, removed arguments;
            return 0xffffffff;
        }
        
        var_38_13 = 0;
    }
    
    tiziano_deflicker_expt_tune(flicker_hz, data_b2e44_4, data_b2e56_5, data_b2e54_5);
    int32_t $a2_2 = var_28_3;
    flicker_hz_1 = flicker_hz;
    int32_t $v0_2 = data_b2e44_5;
    uint32_t $v0_3 = data_b2e56_6;
    uint32_t $v0_4 = data_b2e54_6;
    memcpy(0x94d68, &var_38_14, $a2_2);
    tisp_ae_param_array_set(0xa, &var_38_15, &var_28_4);
    memcpy(0x94d74, &flicker_hz_1, var_2c_2);
    tisp_ae_param_array_set(0xb, &flicker_hz_1, &var_2c_3);
    memcpy(tparams_day + 0x248, &var_38_16, var_28_5);
    memcpy(tparams_night + 0x248, &var_38_17, var_28_6);
    memcpy(tparams_day + 0x254, &flicker_hz_1, var_2c_4);
    memcpy(tparams_night + 0x254, &flicker_hz_1, var_2c_5);
    tisp_ae_trig();
    uint32_t var_54_1_1 = $v0_4;
    uint32_t var_58_1_1 = $v0_3;
    int32_t var_5c_1_2 = $v0_2;
    uint32_t flicker_hz_2 = flicker_hz_1;
    isp_printf(); // Fixed: macro call, removed arguments;
    return 0;
}

