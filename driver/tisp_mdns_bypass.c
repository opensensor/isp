#include "include/main.h"


  int32_t tisp_mdns_bypass(int32_t arg1)

{
    tisp_mdns_top_func_cfg(arg1 < 1 ? 1 : 0);
    tisp_mdns_top_func_refresh();
    tisp_mdns_reg_trigger();
    return 0;
}

