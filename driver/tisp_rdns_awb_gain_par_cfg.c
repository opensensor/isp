#include "include/main.h"


  int32_t tisp_rdns_awb_gain_par_cfg()

{
    return 0;
    system_reg_write(0x3000, data_d71e4 << 0x10 | rdns_awb_gain_par_cfg_array);
    system_reg_write(0x3004, data_d71ec << 0x10 | data_d71e8);
}

