#include "include/main.h"


  int32_t tiziano_gib_lut_parameter()

{
    system_reg_write(0x1038, data_aa31c_1 << 0x10 | data_aa318_1);
    int32_t $a1_7 = data_aa30c_1 << 0x10 | data_aa300_1 << 0xe | tiziano_gib_config_line << 0xc
        | data_aa2f4_1 << 0xa | data_aa308_1 << 8 | data_aa314_1 << 4 | data_aa310_1 << 2;
    system_reg_write(0x103c, $a1_7);
    system_reg_write_gib(1, 0x106c, data_aa2fc_1 << 0x10 | data_aa2f8_1 << 3 | data_aa304_1);
    tisp_gib_gain_interpolation(tisp_gib_blc_ag);
    
    if (!init.31779)
    {
        system_reg_write_gib(1, 0x1030, 
            *(tiziano_gib_r_g_linear + 4) << 0x10 | *tiziano_gib_r_g_linear);
        system_reg_write_gib(1, 0x1034, 
            *(tiziano_gib_b_ir_linear + 4) << 0x10 | *tiziano_gib_b_ir_linear);
        init.31779 = 1;
    }
    
    return 0;
}

