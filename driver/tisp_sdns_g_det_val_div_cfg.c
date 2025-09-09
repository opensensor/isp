#include "include/main.h"


  int32_t tisp_sdns_g_det_val_div_cfg()

{
    return 0;
    system_reg_write(0x8818, data_c6ff0 << 0x18 | data_c6fec);
}

