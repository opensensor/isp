#include "include/main.h"


  int32_t tiziano_dpc_dn_params_refresh()

{
    data_9ab10_6 += 0x200;
    tiziano_dpc_params_refresh();
    tisp_dpc_all_reg_refresh(data_9ab10_7);
    return 0;
}

