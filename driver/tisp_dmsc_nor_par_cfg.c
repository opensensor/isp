#include "include/main.h"


  int32_t tisp_dmsc_nor_par_cfg()

{
    system_reg_write(0x483c, dmsc_nor_par_array << 0x10 | dmsc_nor_alias_thres_intp);
    system_reg_write(0x4840, data_c4c68_1 << 0x10 | data_c4c6c_1 << 6 | data_c4c70_1);
    return 0;
}

