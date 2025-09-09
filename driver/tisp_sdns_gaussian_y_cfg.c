#include "include/main.h"


  int32_t tisp_sdns_gaussian_y_cfg()

{
    system_reg_write(0x88c4, 0x2999a);
    system_reg_write(0x88c8, 0x1999a);
    system_reg_write(0x88cc, 0x999a);
    return 0;
}

