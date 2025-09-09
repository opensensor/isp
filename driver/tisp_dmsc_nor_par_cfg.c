#include "include/main.h"


  int32_t tisp_dmsc_nor_par_cfg()

{
    return 0;
    system_reg_write(0x483c, dmsc_nor_par_array << 0x10 | dmsc_nor_alias_thres_intp);
    system_reg_write(0x4840, data_c4c68 << 0x10 | data_c4c6c << 6 | data_c4c70);
}

