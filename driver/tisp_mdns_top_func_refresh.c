#include "include/main.h"


  int32_t tisp_mdns_top_func_refresh()

{
    system_reg_write(0x7818, 1);
    return 0;
}

