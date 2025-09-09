#include "include/main.h"


  int32_t tisp_ae_wdr_en(uint32_t arg1)

{
    IspAeFlag = 1;
    data_b0df4_3 = 1;
    data_b0e00_4 = 1;
    data_b0e04_4 = 1;
    data_b0e08_6 = 1;
    data_b0e10_6 = arg1;
    data_b0e14_1 = 1;
    data_b0e18_3 = 1;
    data_b0e0c_10 = 0;
    ae_wdr_en = arg1;
    uint32_t $a3 = data_b2e54_3;
    int32_t $a1 = data_b2e44_2;
    uint32_t $a2 = data_b2e56_3;
    data_b0b28_2 = $a1;
    data_b0b2c_2 = $a2;
    data_b0b30_2 = $a3;
    tiziano_deflicker_expt(_flicker_t, $a1, $a2, $a3, &_deflick_lut, &_nodes_num);
    tiziano_ae_init_exp_th();
    tiziano_ae_para_addr();
    tiziano_ae_set_hardware_param(0, &_ae_parameter, 1);
    tiziano_ae_set_hardware_param(1, &_ae_parameter, 1);
    return 0;
}

