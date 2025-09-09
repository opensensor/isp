#include "include/main.h"


  int32_t tiziano_af_dn_params_refresh()

{
    af_first = 0;
    tiziano_af_params_refresh();
    
    if (af_set_trig)
        tisp_af_set_attr_refresh();
    
    /* tailcall */
    return tiziano_af_set_hardware_param();
}

