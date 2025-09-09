#include "include/main.h"


  int32_t tisp_ae_trig()

{
    data_b0e00_8 = 1;
    data_b0e04_8 = 1;
    data_b0e08_8 = 1;
    data_b0e0c_15 = 0;
    tiziano_ae_set_hardware_param(0, &_ae_parameter, 1);
    /* tailcall */
    return tiziano_ae_set_hardware_param(1, &_ae_parameter, 1);
}

