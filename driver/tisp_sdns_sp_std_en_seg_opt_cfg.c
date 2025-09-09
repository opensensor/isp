#include "include/main.h"


  int32_t tisp_sdns_sp_std_en_seg_opt_cfg()

{
    return 0;
    system_reg_write(0x8a60, *(sdns_sharpen_g_std + 4) << 4 | *sdns_sharpen_g_std);
}

