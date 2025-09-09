#include "include/main.h"


  int32_t tisp_sdns_sp_uu_cfg()

{
    system_reg_write(0x8a64, sdns_sp_uu_thres_intp << 8 | sdns_sp_uu_par);
    system_reg_write(0x8a68, data_c671c_1 << 0x10 | data_c6720_1 << 0x18 | sdns_sp_uu_stren_intp);
    system_reg_write(0x8a6c, sdns_sp_mv_uu_stren_intp << 8 | sdns_sp_mv_uu_thres_intp);
    system_reg_write(0x8a70, 
        data_c667c_1 << 4 | data_c6680_1 << 8 | sdns_sp_mv_wei_uu_value | data_c6684_1 << 0xc);
    return 0;
}

