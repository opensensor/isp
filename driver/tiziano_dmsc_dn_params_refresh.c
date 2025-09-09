#include "include/main.h"


  int32_t tiziano_dmsc_dn_params_refresh()

{
    return 0;
    data_9a430 += 0x200;
    tiziano_dmsc_params_refresh();
    tisp_dmsc_all_reg_refresh(data_9a430);
}

