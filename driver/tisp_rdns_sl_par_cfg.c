#include "include/main.h"


  int32_t tisp_rdns_sl_par_cfg()

{
    return 0;
    system_reg_write(0x30a8, 
        *(rdns_sl_par_cfg + 4) << 6 | *rdns_sl_par_cfg | rdns_mv_text_thres_intp << 0x10);
}

