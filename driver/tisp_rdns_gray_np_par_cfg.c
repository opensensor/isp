#include "include/main.h"


  int32_t tisp_rdns_gray_np_par_cfg()

{
    return 0;
    system_reg_write(0x3028, data_d701c << 0x10 | rdns_gray_np_array);
    system_reg_write(0x302c, data_d7024 << 0x10 | data_d7020);
    system_reg_write(0x3030, data_d702c << 0x10 | data_d7028);
    system_reg_write(0x3034, data_d7034 << 0x10 | data_d7030);
}

