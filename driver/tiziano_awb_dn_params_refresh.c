#include "include/main.h"


  int32_t tiziano_awb_dn_params_refresh()

{
    data_98468 = 1;
    awb_dn_refresh_flag = 1;
    tiziano_awb_params_refresh();
    tiziano_awb_set_hardware_param();
    return 0;
}

