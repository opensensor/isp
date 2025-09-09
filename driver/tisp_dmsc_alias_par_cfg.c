#include "include/main.h"


  int32_t tisp_dmsc_alias_par_cfg()

{
    uint32_t dmsc_alias_dir_thres_intp_1 = dmsc_alias_dir_thres_intp;
    system_reg_write(0x480c, 
        (dmsc_alias_dir_thres_intp_1 - data_c4ca4_1) << 0x10 | dmsc_alias_dir_thres_intp_1);
    system_reg_write(0x4834, 
        dmsc_alias_stren_intp << 0x12 | dmsc_alias_par_array | data_c4ca0_1 << 0xa | data_c4c9c_1 << 6);
    system_reg_write(0x4838, dmsc_alias_thres_2_intp << 0x10 | dmsc_alias_thres_1_intp);
    return 0;
}

