#include "include/main.h"


  int32_t tisp_dpc_s_par_cfg()

{
    system_reg_write(0x2800, 
        data_cba48_1 << 8 | data_cba4c_1 << 0xc | dpc_s_con_par_array | data_cba50_1 << 0x10);
    return 0;
}

