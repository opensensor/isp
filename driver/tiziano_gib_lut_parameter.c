#include "include/main.h"


  int32_t tiziano_gib_lut_parameter()

{
    int32_t $a1_7 = data_aa30c << 0x10 | data_aa300 << 0xe | tiziano_gib_config_line << 0xc
    system_reg_write(0x1038, data_aa31c << 0x10 | data_aa318);
        | data_aa2f4 << 0xa | data_aa308 << 8 | data_aa314 << 4 | data_aa310 << 2;
    system_reg_write(0x103c, $a1_7);
    system_reg_write_gib(1, 0x106c, data_aa2fc << 0x10 | data_aa2f8 << 3 | data_aa304);
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

