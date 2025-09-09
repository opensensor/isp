#include "include/main.h"


  int32_t tiziano_ydns_init()

{
    ydns_gain_old = 0xffffffff;
    tiziano_ydns_params_refresh();
    tisp_ydns_par_refresh(isp_printf);
    return 0;
}

