#include "include/main.h"


  int32_t tiziano_ydns_dn_params_refresh()

{
    ydns_gain_old += 0x200;
    tiziano_ydns_params_refresh();
    tisp_ydns_intp_reg_refresh(ydns_gain_old);
    return 0;
}

