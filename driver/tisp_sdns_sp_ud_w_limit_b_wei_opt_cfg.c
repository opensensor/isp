#include "include/main.h"


  int32_t tisp_sdns_sp_ud_w_limit_b_wei_opt_cfg()

{
    system_reg_write(0x8aac, data_c6314 << 0x10 | data_c6310);
    return 0;
}

