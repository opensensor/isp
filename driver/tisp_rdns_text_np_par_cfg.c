#include "include/main.h"


  int32_t tisp_rdns_text_np_par_cfg()

{
    system_reg_write(0x3038, data_c6fdc_2 << 0x10 | sdns_aa_mv_det_opt);
    system_reg_write(0x303c, data_c6fe4_2 << 0x10 | data_c6fe0_2);
    system_reg_write(0x3040, data_c6fec_2 << 0x10 | data_c6fe8_2);
    system_reg_write(0x3044, sdns_wdr_en << 0x10 | data_c6ff0_2);
    system_reg_write(0x3048, data_c6ffc_1 << 0x10 | data_c6ff8_1);
    system_reg_write(0x304c, data_c7004_1 << 0x10 | mdns_c_fiir_fus_wei8_wdr_array);
    system_reg_write(0x3050, data_c700c_1 << 0x10 | data_c7008_1);
    system_reg_write(0x3054, data_c7014_1 << 0x10 | data_c7010_1);
    return 0;
}

