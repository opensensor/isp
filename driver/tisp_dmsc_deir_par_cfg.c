#include "include/main.h"


  int32_t tisp_dmsc_deir_par_cfg()

{
    system_reg_write(0x4944, *dmsc_deir_oe_en << 8 | *(dmsc_deir_oe_en + 4));
    system_reg_write(0x4948, dmsc_deir_rgb_ir_oe_slope << 0x10 | data_c47c0);
    system_reg_write(0x494c, data_c47c4 << 0x10 | data_c47c8);
    system_reg_write(0x4988, 
        dmsc_deir_fusion_thres_intp << 0x10 | dmsc_deir_fusion_stren_intp << 8 | data_c47cc);
    return 0;
}

