#include "include/main.h"


  int32_t tisp_sdns_dark_light_tt_opt_cfg()

{
    system_reg_write(0x887c, 
        sdns_light_thres_intp << 8 | sdns_h_val_max << 0x10 | sdns_dark_thres_intp
            | sdns_sharpen_tt_opt_intp << 0x18);
    return 0;
}

