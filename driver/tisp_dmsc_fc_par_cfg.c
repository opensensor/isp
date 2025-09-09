#include "include/main.h"


  int32_t tisp_dmsc_fc_par_cfg()

{
    int32_t dmsc_fc_par_array_1 = dmsc_fc_par_array;
    int32_t $a1_20 = data_c47e8 + dmsc_fc_t1_thres_intp;
    int32_t $v1_4 = 0xfff;
    int32_t $v0_23 = data_c47e8 + data_c47ec + dmsc_fc_t1_thres_intp + data_c47f0;
    system_reg_write(0x4880, 
        dmsc_fc_alias_stren_intp << 0x15 | dmsc_fc_t1_stren_intp << 0xe | dmsc_fc_t2_stren_intp << 7
            | data_c47f8);
    system_reg_write(0x4884, data_c47dc << 0x10 | dmsc_fc_t1_thres_intp);
    system_reg_write(0x4888, data_c47e0 << 0x10 | data_c47e4);
    system_reg_write(0x488c, 
        (data_c47e8 + dmsc_fc_t1_thres_intp) << 0x11 | data_c47f4 << 0xc | data_c47fc);
    system_reg_write(0x4890, 
        dmsc_fc_par_array_1 << 0x10 | dmsc_fc_par_array_1 << 6 | dmsc_fc_par_array_1);
    system_reg_write(0x4894, dmsc_fc_t3_stren_intp);
    system_reg_write(0x4898, 
        (data_c47e8 + data_c47ec + dmsc_fc_t1_thres_intp) | dmsc_fc_par_array << 0x10);
    system_reg_write(0x489c, ($a1_20 + data_c47ec) << 0x10 | $a1_20);
    
    if ($(uintptr_t)v0_23 < 0x1000)
        $v1_4 = $v0_23;
    
    system_reg_write(0x48a0, $v1_4);
    system_reg_write(0x4980, dmsc_fc_lum_thres_intp << 0x10 | dmsc_fc_lum_stren_intp);
    return 0;
}

