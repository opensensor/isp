#include "include/main.h"


  int32_t tisp_y_sp_w_b_ll_cfg()

{
    system_reg_write(0x702c, 
        data_c5984_1 << 8 | data_c5988_1 << 0x10 | y_sp_w_b_ll_par_cfg_array | data_c598c_1 << 0x18);
    system_reg_write(0x7030, data_c5994_1 << 8 | data_c5998_1 << 0x10 | data_c5990_1);
    system_reg_write(0x7034, data_c59a0_1 << 0x10 | data_c599c_1);
    return 0;
}

