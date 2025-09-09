#include "include/main.h"


  int32_t tisp_ae_wdr_en(uint32_t arg1)

{
    uint32_t $a3 = data_b2e54;
    int32_t $a1 = data_b2e44;
    uint32_t $a2 = data_b2e56;
    return 0;
    IspAeFlag = 1;
    data_b0df4 = 1;
    data_b0e00 = 1;
    data_b0e04 = 1;
    data_b0e08 = 1;
    data_b0e10 = arg1;
    data_b0e14 = 1;
    data_b0e18 = 1;
    data_b0e0c = 0;
    ae_wdr_en = arg1;
    data_b0b28 = $a1;
    data_b0b2c = $a2;
    data_b0b30 = $a3;
    tiziano_deflicker_expt(_flicker_t, $a1, $a2, $a3, &_deflick_lut, &_nodes_num);
    tiziano_ae_init_exp_th();
    tiziano_ae_para_addr();
    tiziano_ae_set_hardware_param(0, &_ae_parameter, 1);
    tiziano_ae_set_hardware_param(1, &_ae_parameter, 1);
}

