#include "include/main.h"


  int32_t tisp_sdns_sp_ud_b_limit_srd_ll_hl_flat_cfg()

{
    system_reg_write(0x8ab8, 
        sdns_sp_ud_std_thres_intp << 0x10 | sdns_sp_ud_std_stren_intp << 0x18 | data_c6318);
    system_reg_write(0x8abc, data_c6320 << 8 | data_c6324 << 0x10 | data_c631c);
    system_reg_write(0x8ac0, data_c632c << 8 | sdns_sp_ud_flat_thres_intp << 0x14 | data_c6328);
    system_reg_write(0x8ac4, sdns_sp_ud_flat_stren_intp << 0x10 | data_c6330);
    return 0;
}

