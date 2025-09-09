#include "include/main.h"


  int32_t tisp_s_wdr_init_en(uint32_t arg1)

{
    int32_t var_20 = 0;
    int32_t* $a2 = &var_20;
    int32_t* $a1 = &var_58;
        void* $v0_3 = tparams_night + isp_printf;
    int32_t var_58;
    
    if (arg1 != 1)
    {
        tisp_wdr_param_array_get(0x431, $a1, $a2);
        var_58 = 2;
        tisp_wdr_param_array_set(0x431, &var_58, &var_20);
        *(((int32_t*)((char*)tparams_day + subdev_sensor_ops_set_input+0x90))) = 2; // Fixed void pointer dereference
        *(((int32_t*)((char*)$v0_3 + 0x34f0))) = 2; // Fixed void pointer dereference
    }
    else
    {
        void* $v0_1 = tparams_night + isp_printf;
        tisp_wdr_param_array_get(0x431, $a1, $a2);
        var_58 = 0;
        tisp_wdr_param_array_set(0x431, &var_58, &var_20);
        *(((int32_t*)((char*)tparams_day + subdev_sensor_ops_set_input+0x90))) = 0; // Fixed void pointer dereference
        *(((int32_t*)((char*)$v0_1 + 0x34f0))) = 0; // Fixed void pointer dereference
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

