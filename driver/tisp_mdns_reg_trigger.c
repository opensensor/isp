#include "include/main.h"


  int32_t tisp_mdns_reg_trigger()

{
    system_reg_write(0x7804, 0x111);
    return 0;
}

