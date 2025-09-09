#include "include/main.h"


  int32_t tisp_ae_trig()

{
    data_b0e00 = 1;
    data_b0e04 = 1;
    data_b0e08 = 1;
    data_b0e0c = 0;
    tiziano_ae_set_hardware_param(0, &_ae_parameter, 1);
    /* tailcall */
    return tiziano_ae_set_hardware_param(1, &_ae_parameter, 1);
}

