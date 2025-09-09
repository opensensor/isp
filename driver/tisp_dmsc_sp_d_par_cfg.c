#include "include/main.h"


  int32_t tisp_dmsc_sp_d_par_cfg()

{
    int32_t $a2 = data_c4b24_1;
    system_reg_write(0x4844, 
        dmsc_sp_d_par_array << 0x11 | data_c4b1c_1 << 7 | $a2 | data_c4b20_1 << 6 | $a2 << 4
            | $a2 << 2);
    system_reg_write(0x4848, dmsc_sp_d_w_stren_intp << 0x10 | dmsc_sp_d_b_stren_intp);
    system_reg_write(0x484c, data_c4b28_1 << 0x10 | data_c4b2c_1);
    system_reg_write(0x4850, dmsc_sp_d_brig_thres_intp << 0x10 | dmsc_sp_d_dark_thres_intp);
    system_reg_write(0x4854, data_c4b30_1);
    system_reg_write(0x4858, data_c4b34_1);
    system_reg_write(0x48a4, dmsc_sp_d_v2_win5_thres_intp << 0x10 | data_c4b38_1);
    system_reg_write(0x48a8, 
        dmsc_sp_d_flat_thres_intp << 0x14 | dmsc_sp_d_flat_stren_intp | data_c4b3c_1 << 0xb);
    system_reg_write(0x48ac, data_c4b38_2 << 0x10 | dmsc_sp_d_oe_stren_intp);
    return 0;
}

