#include "include/main.h"


  int32_t tisp_sdns_sp_d_w_wei_np_array_cfg()

{
    system_reg_write(0x8adc, 
        data_c6230_1 << 8 | data_c6234_1 << 0x10 | sdns_sp_d_w_wei_np_array | data_c6238_1 << 0x18);
    system_reg_write(0x8ae0, 
        data_c6240_1 << 8 | data_c6244_1 << 0x10 | data_c623c_1 | data_c6248_1 << 0x18);
    system_reg_write(0x8ae4, 
        data_c6250_1 << 8 | data_c6254_1 << 0x10 | data_c624c_1 | data_c6258_1 << 0x18);
    system_reg_write(0x8ae8, 
        data_c6260_1 << 8 | data_c6264_1 << 0x10 | data_c625c_1 | data_c6268_1 << 0x18);
    system_reg_write(0x8aec, 
        data_c6270_1 << 8 | data_c6274_1 << 0x10 | data_c626c_1 | data_c6278_1 << 0x18);
    system_reg_write(0x8af0, data_c6280_1 << 8 | data_c627c_1);
    return 0;
}

