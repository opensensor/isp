#include "include/main.h"


  int32_t tisp_sdns_sp_d_w_wei_np_array_cfg()

{
    system_reg_write(0x8adc, 
        data_c6230 << 8 | data_c6234 << 0x10 | sdns_sp_d_w_wei_np_array | data_c6238 << 0x18);
    system_reg_write(0x8ae0, 
        data_c6240 << 8 | data_c6244 << 0x10 | data_c623c | data_c6248 << 0x18);
    system_reg_write(0x8ae4, 
        data_c6250 << 8 | data_c6254 << 0x10 | data_c624c | data_c6258 << 0x18);
    system_reg_write(0x8ae8, 
        data_c6260 << 8 | data_c6264 << 0x10 | data_c625c | data_c6268 << 0x18);
    system_reg_write(0x8aec, 
        data_c6270 << 8 | data_c6274 << 0x10 | data_c626c | data_c6278 << 0x18);
    system_reg_write(0x8af0, data_c6280 << 8 | data_c627c);
    return 0;
}

