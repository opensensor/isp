#include "include/main.h"


  int32_t tisp_dmsc_sp_ud_par_cfg()

{
    int32_t $a3 = data_c49b0_1;
    system_reg_write(0x485c, 
        dmsc_sp_ud_par_array << 0x10 | data_c49a4_1 << 0xc | $a3 | data_c49a8_1 << 8 | data_c49ac_1 << 4
            | $a3 << 2);
    system_reg_write(0x4860, dmsc_sp_ud_w_stren_intp << 0x10 | dmsc_sp_ud_b_stren_intp);
    system_reg_write(0x4864, data_c49b4_1 << 0x10 | data_c49b8_1);
    system_reg_write(0x4868, dmsc_sp_ud_brig_thres_intp << 0x10 | dmsc_sp_ud_dark_thres_intp);
    system_reg_write(0x486c, data_c49bc_1);
    system_reg_write(0x4870, data_c49c0_1);
    system_reg_write(0x48b0, 
        dmsc_sp_ud_v1_v2_par_array << 0x1b | data_c497c_1 << 0xf | data_c4984_1 | data_c4980_1 << 8);
    system_reg_write(0x48b4, 
        data_c4988_1 << 0x1b | data_c498c_1 << 0x18 | data_c499c_1 | data_c4990_1 << 0x10 | data_c4994_1 << 8
            | data_c4998_1 << 4);
    system_reg_write(0x48b8, 
        dmsc_sp_ud_std_thres_intp << 0x10 | dmsc_sp_ud_std_stren_intp | data_c49c4_1 << 8);
    system_reg_write(0x48bc, dmsc_sp_ud_flat_stren_intp << 0x10 | data_c49c8_1);
    system_reg_write(0x48c0, dmsc_sp_ud_flat_stren_intp);
    system_reg_write(0x48c4, dmsc_sp_ud_oe_stren_intp << 8 | data_c49cc_1 | data_c49d0_1 << 0x1b);
    return 0;
}

