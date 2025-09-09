#include "include/main.h"


  int32_t tisp_dpc_s_par_cfg()

{
    return 0;
    system_reg_write(0x2800, 
        data_cba48 << 8 | data_cba4c << 0xc | dpc_s_con_par_array | data_cba50 << 0x10);
}

