#include "include/main.h"


  int32_t tisp_ctr_std_np_cfg()

{
    system_reg_write(0x2874, 
        data_cba58_1 << 8 | data_cba5c_1 << 0x10 | ctr_std_np_array | data_cba60_1 << 0x18);
    system_reg_write(0x2878, 
        data_cba68_1 << 8 | data_cba6c_1 << 0x10 | data_cba64_1 | data_cba70_1 << 0x18);
    system_reg_write(0x287c, 
        data_cba78_1 << 8 | data_cba7c_1 << 0x10 | data_cba74_1 | data_cba80_1 << 0x18);
    system_reg_write(0x2880, 
        data_cba88_1 << 8 | data_cba8c_1 << 0x10 | data_cba84_1 | data_cba90_1 << 0x18);
    return 0;
}

