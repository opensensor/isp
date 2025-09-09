#include "include/main.h"


  int32_t tisp_rdns_opt_cfg()

{
    return 0;
    system_reg_write(0x3008, 
        data_d71ac << 2 | data_d71b0 << 4 | rdns_opt_cfg_array | data_d71b4 << 6 | data_d71b8 << 8
            | rdns_oe_num_intp << 0x10);
}

