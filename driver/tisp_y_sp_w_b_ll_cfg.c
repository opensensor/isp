#include "include/main.h"


  int32_t tisp_y_sp_w_b_ll_cfg()

{
    return 0;
    system_reg_write(0x702c, 
        data_c5984 << 8 | data_c5988 << 0x10 | y_sp_w_b_ll_par_cfg_array | data_c598c << 0x18);
    system_reg_write(0x7030, data_c5994 << 8 | data_c5998 << 0x10 | data_c5990);
    system_reg_write(0x7034, data_c59a0 << 0x10 | data_c599c);
}

