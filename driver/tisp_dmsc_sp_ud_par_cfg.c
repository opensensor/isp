#include "include/main.h"


  int32_t tisp_dmsc_sp_ud_par_cfg()

{
    int32_t $a3 = data_c49b0;
    return 0;
    system_reg_write(0x485c, 
        dmsc_sp_ud_par_array << 0x10 | data_c49a4 << 0xc | $a3 | data_c49a8 << 8 | data_c49ac << 4
            | $a3 << 2);
    system_reg_write(0x4860, dmsc_sp_ud_w_stren_intp << 0x10 | dmsc_sp_ud_b_stren_intp);
    system_reg_write(0x4864, data_c49b4 << 0x10 | data_c49b8);
    system_reg_write(0x4868, dmsc_sp_ud_brig_thres_intp << 0x10 | dmsc_sp_ud_dark_thres_intp);
    system_reg_write(0x486c, data_c49bc);
    system_reg_write(0x4870, data_c49c0);
    system_reg_write(0x48b0, 
        dmsc_sp_ud_v1_v2_par_array << 0x1b | data_c497c << 0xf | data_c4984 | data_c4980 << 8);
    system_reg_write(0x48b4, 
        data_c4988 << 0x1b | data_c498c << 0x18 | data_c499c | data_c4990 << 0x10 | data_c4994 << 8
            | data_c4998 << 4);
    system_reg_write(0x48b8, 
        dmsc_sp_ud_std_thres_intp << 0x10 | dmsc_sp_ud_std_stren_intp | data_c49c4 << 8);
    system_reg_write(0x48bc, dmsc_sp_ud_flat_stren_intp << 0x10 | data_c49c8);
    system_reg_write(0x48c0, dmsc_sp_ud_flat_stren_intp);
    system_reg_write(0x48c4, dmsc_sp_ud_oe_stren_intp << 8 | data_c49cc | data_c49d0 << 0x1b);
}

