#include "include/main.h"


  int32_t tiziano_rdns_dn_params_refresh()

{
    tiziano_rdns_params_refresh();
    tisp_rdns_all_reg_refresh(rdns_gain_old);
    return 0;
}

