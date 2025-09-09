#include "include/main.h"


  int32_t tisp_rdns_opt_cfg()

{
    system_reg_write(0x3008, 
        data_d71ac_1 << 2 | data_d71b0_1 << 4 | rdns_opt_cfg_array | data_d71b4_1 << 6 | data_d71b8_1 << 8
            | rdns_oe_num_intp << 0x10);
    return 0;
}

