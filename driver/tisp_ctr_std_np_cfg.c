#include "include/main.h"


  int32_t tisp_ctr_std_np_cfg()

{
    system_reg_write(0x2874, 
        data_cba58 << 8 | data_cba5c << 0x10 | ctr_std_np_array | data_cba60 << 0x18);
    system_reg_write(0x2878, 
        data_cba68 << 8 | data_cba6c << 0x10 | data_cba64 | data_cba70 << 0x18);
    system_reg_write(0x287c, 
        data_cba78 << 8 | data_cba7c << 0x10 | data_cba74 | data_cba80 << 0x18);
    system_reg_write(0x2880, 
        data_cba88 << 8 | data_cba8c << 0x10 | data_cba84 | data_cba90 << 0x18);
    return 0;
}

