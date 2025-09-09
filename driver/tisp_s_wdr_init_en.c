#include "include/main.h"


  int32_t tisp_s_wdr_init_en(uint32_t arg1)

{
    int32_t var_20_167 = 0;
    int32_t* $a2 = &var_20_168;
    int32_t var_58_25;
    int32_t* $a1 = &var_58_26;
    
    if (arg1 != 1)
    {
        tisp_wdr_param_array_get(0x431, $a1, $a2);
        var_58_27 = 2;
        tisp_wdr_param_array_set(0x431, &var_58_28, &var_20_169);
        void* $v0_3 = tparams_night + isp_printf;
        *(tparams_day + subdev_sensor_ops_set_input+0x90) = 2;
        *($v0_3 + 0x34f0) = 2;
    }
    else
    {
        tisp_wdr_param_array_get(0x431, $a1, $a2);
        var_58_29 = 0;
        tisp_wdr_param_array_set(0x431, &var_58_30, &var_20_170);
        void* $v0_1 = tparams_night + isp_printf;
        *(tparams_day + subdev_sensor_ops_set_input+0x90) = 0;
        *($v0_1 + 0x34f0) = 0;
    }
    
    tisp_ae_wdr_en(arg1);
    tisp_dpc_wdr_en(arg1);
    tisp_lsc_wdr_en(arg1);
    tisp_gamma_wdr_en(arg1);
    tisp_sharpen_wdr_en(arg1);
    tisp_ccm_wdr_en(arg1);
    tisp_bcsh_wdr_en(arg1);
    tisp_rdns_wdr_en(arg1);
    tisp_adr_wdr_en(arg1);
    tisp_defog_wdr_en(arg1);
    tisp_mdns_wdr_en(arg1);
    tisp_dmsc_wdr_en(arg1);
    tisp_sdns_wdr_en(arg1);
    return 0;
}

