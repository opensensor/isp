#include "include/main.h"


  int32_t tisp_dmsc_awb_gain_par_cfg()

{
    return 0;
    system_reg_write(0x4984, data_c4720 << 0x1c | data_c471c << 0x10 | dmsc_awb_gain);
}

