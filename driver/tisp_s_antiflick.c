#include "include/main.h"


  int32_t tisp_s_antiflick(uint32_t arg1)

{
    int32_t var_28_32 = 0xc;
    int32_t var_2c_13 = 0x18;
    
    if (arg1)
        flicker_hz = arg1;
    
    int32_t var_38_48;
    memcpy(&var_38_49, 0x94d68, 0xc);
    uint32_t flicker_hz_1;
    memcpy(&flicker_hz_1, 0x94d74, var_2c_14);
    
    if (arg1 == 0x32 || arg1 == 0x3c)
        var_38_50 = 1;
    else
    {
        if (arg1)
        {
            uint32_t var_60_23 = arg1;
            isp_printf(2, "%s[%d] VIC do not support this format %d\\n", "tisp_s_antiflick");
            return 0xffffffff;
        }
        
        var_38_51 = 0;
    }
    
    tiziano_deflicker_expt_tune(flicker_hz, data_b2e44_7, data_b2e56_9, data_b2e54_9);
    int32_t $a2_2 = var_28_33;
    flicker_hz_1 = flicker_hz;
    int32_t $v0_2 = data_b2e44_8;
    uint32_t $v0_3 = data_b2e56_10;
    uint32_t $v0_4 = data_b2e54_10;
    memcpy(0x94d68, &var_38_52, $a2_2);
    tisp_ae_param_array_set(0xa, &var_38_53, &var_28_34);
    memcpy(0x94d74, &flicker_hz_1, var_2c_15);
    tisp_ae_param_array_set(0xb, &flicker_hz_1, &var_2c_16);
    memcpy(tparams_day + 0x248, &var_38_54, var_28_35);
    memcpy(tparams_night + 0x248, &var_38_55, var_28_36);
    memcpy(tparams_day + 0x254, &flicker_hz_1, var_2c_17);
    memcpy(tparams_night + 0x254, &flicker_hz_1, var_2c_18);
    tisp_ae_trig();
    uint32_t var_54_1_5 = $v0_4;
    uint32_t var_58_1_3 = $v0_3;
    int32_t var_5c_1_4 = $v0_2;
    uint32_t flicker_hz_2 = flicker_hz_1;
    isp_printf(0, "%s[%d] do not support this interface\\n", "tisp_s_antiflick");
    return 0;
}

