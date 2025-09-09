#include "include/main.h"


  int32_t tiziano_mdns_dn_params_refresh()

{
    data_9a9d0 += 0x200;
    tiziano_mdns_params_refresh();
    tisp_mdns_all_reg_refresh(data_9a9d0);
    tisp_mdns_top_func_refresh();
    tisp_mdns_reg_trigger();
    return 0;
}

