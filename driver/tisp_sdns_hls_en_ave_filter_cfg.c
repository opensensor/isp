#include "include/main.h"


  int32_t tisp_sdns_hls_en_ave_filter_cfg()

{
    void* sdns_ave_fliter_now_1 = sdns_ave_fliter_now;
    system_reg_write(0x88c0, 
        sdns_ave_thres_intp << 4 | *sdns_ave_fliter_now_1 | *(sdns_ave_fliter_now_1 + 4) << 1
            | *(sdns_ave_fliter_now_1 + 8) << 0x10);
    return 0;
}

