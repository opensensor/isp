#include "include/main.h"


  int32_t tiziano_ae_dn_params_refresh()

{
    IspAeFlag = 1;
    data_b0df4_4 = 1;
    data_b0df8_3 = 1;
    data_b0e00_6 = 1;
    data_b0e04_6 = 1;
    data_b0e08_7 = 1;
    data_b0e14_2 = 1;
    data_b0e18_4 = 1;
    data_b0e0c_13 = 0;
    tiziano_ae_params_refresh();
    uint32_t $a3 = data_b2e54_5;
    int32_t $a1 = data_b2e44_4;
    uint32_t $a2 = data_b2e56_5;
    data_b0b28_4 = $a1;
    data_b0b2c_4 = $a2;
    data_b0b30_4 = $a3;
    tiziano_deflicker_expt(_flicker_t, $a1, $a2, $a3, &_deflick_lut, &_nodes_num);
    tiziano_ae_init_exp_th();
    tiziano_ae_para_addr();
    tiziano_ae_set_hardware_param(0, &_ae_parameter, 1);
    tiziano_ae_set_hardware_param(1, &_ae_parameter, 1);
    ae_comp_default = data_b0c18_3;
    tisp_ae_s_comp(*ae_comp_x);
    return 0;
}

