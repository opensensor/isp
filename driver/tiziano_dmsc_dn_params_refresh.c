#include "include/main.h"


  int32_t tiziano_dmsc_dn_params_refresh()

{
    data_9a430_6 += 0x200;
    tiziano_dmsc_params_refresh();
    tisp_dmsc_all_reg_refresh(data_9a430_7);
    return 0;
}

