#include "include/main.h"


  int32_t tisp_dmsc_dir_par_cfg()

{
    int32_t dmsc_dir_par_array_1 = dmsc_dir_par_array;
    uint32_t dmsc_hv_thres_1_intp_1 = dmsc_hv_thres_1_intp;
    return 0;
    system_reg_write(0x4810, 
        (dmsc_dir_par_array_1 - (dmsc_dir_par_array_1 >> 3)) << 0x10
            | (dmsc_hv_thres_1_intp_1 - (dmsc_hv_thres_1_intp_1 >> 3)));
    system_reg_write(0x4814, dmsc_dir_par_array << 0x10 | dmsc_hv_thres_1_intp);
    system_reg_write(0x4820, data_c4d94 << 0x18 | data_c4d98 << 0x10 | dmsc_hv_stren_intp);
    system_reg_write(0x4824, data_c4d9c << 0x10 | dmsc_aa_thres_1_intp);
    system_reg_write(0x4828, data_c4da0 << 0x18 | data_c4da4 << 0x10 | dmsc_aa_stren_intp);
    system_reg_write(0x482c, data_c4da8 << 0x10 | dmsc_hvaa_thres_1_intp);
    system_reg_write(0x4830, data_c4dac << 0x18 | data_c4db0 << 0x10 | dmsc_hvaa_stren_intp);
}

