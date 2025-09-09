#include "include/main.h"


  int32_t tisp_rdns_gray_np_par_cfg()

{
    system_reg_write(0x3028, data_d701c_1 << 0x10 | rdns_gray_np_array);
    system_reg_write(0x302c, data_d7024_1 << 0x10 | data_d7020_1);
    system_reg_write(0x3030, data_d702c_1 << 0x10 | data_d7028_1);
    system_reg_write(0x3034, data_d7034_1 << 0x10 | data_d7030_1);
    return 0;
}

