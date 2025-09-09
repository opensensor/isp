#include "include/main.h"


  int32_t tisp_sdns_g_det_val_div_cfg()

{
    system_reg_write(0x8818, data_c6ff0_1 << 0x18 | data_c6fec_1);
    return 0;
}

