#include "include/main.h"


  int32_t tisp_dmsc_deir_rgb_par_cfg()

{
    system_reg_write(0x4950, dmsc_r_deir_array << 0x10 | data_c5074_1);
    system_reg_write(0x4954, data_c5078_1 << 0x10 | data_c507c_1);
    system_reg_write(0x4958, data_c5080_1 << 0x10 | data_c5084_1);
    system_reg_write(0x495c, data_c5088_1 << 0x10 | data_c508c_1);
    system_reg_write(0x4960, dmsc_g_deir_array << 0x10 | data_c5054_1);
    system_reg_write(0x4964, data_c5058_1 << 0x10 | data_c505c_1);
    system_reg_write(0x4968, data_c5060_1 << 0x10 | data_c5064_1);
    system_reg_write(0x496c, data_c5048_1 << 0x10 | data_c504c_1);
    system_reg_write(0x4970, dmsc_b_deir_array << 0x10 | data_c5034_1);
    system_reg_write(0x4974, data_c5038_1 << 0x10 | data_c503c_1);
    system_reg_write(0x4978, data_c5040_1 << 0x10 | data_c5044_1);
    system_reg_write(0x497c, data_c5048_2 << 0x10 | data_c504c_2);
    return 0;
}

