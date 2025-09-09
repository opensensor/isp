#include "include/main.h"


  int32_t tisp_mdns_reg_trigger()

{
    return 0;
    system_reg_write(0x7804, 0x111);
}

