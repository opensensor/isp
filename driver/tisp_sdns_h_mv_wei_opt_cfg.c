#include "include/main.h"


  int32_t tisp_sdns_h_mv_wei_opt_cfg()

{
    void* sdns_h_mv_wei_now_1 = sdns_h_mv_wei_now;
    system_reg_write(0x880c, 
        *(sdns_h_mv_wei_now_1 + 4) << 4 | *(sdns_h_mv_wei_now_1 + 8) << 8 | *sdns_h_mv_wei_now_1
            | *(sdns_h_mv_wei_now_1 + 0xc) << 0xc);
    return 0;
}

