#include "include/main.h"


  int32_t tisp_dmsc_awb_gain_par_cfg()

{
    system_reg_write(0x4984, data_c4720_1 << 0x1c | data_c471c_1 << 0x10 | dmsc_awb_gain);
    return 0;
}

