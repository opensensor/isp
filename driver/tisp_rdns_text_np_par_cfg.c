#include "include/main.h"


  int32_t tisp_rdns_text_np_par_cfg()

{
    system_reg_write(0x3038, data_c6fdc << 0x10 | sdns_aa_mv_det_opt);
    system_reg_write(0x303c, data_c6fe4 << 0x10 | data_c6fe0);
    system_reg_write(0x3040, data_c6fec << 0x10 | data_c6fe8);
    system_reg_write(0x3044, sdns_wdr_en << 0x10 | data_c6ff0);
    system_reg_write(0x3048, data_c6ffc << 0x10 | data_c6ff8);
    system_reg_write(0x304c, data_c7004 << 0x10 | mdns_c_fiir_fus_wei8_wdr_array);
    system_reg_write(0x3050, data_c700c << 0x10 | data_c7008);
    system_reg_write(0x3054, data_c7014 << 0x10 | data_c7010);
    return 0;
}

