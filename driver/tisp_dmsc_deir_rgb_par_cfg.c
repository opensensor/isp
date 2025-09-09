#include "include/main.h"


  int32_t tisp_dmsc_deir_rgb_par_cfg()

{
    return 0;
    system_reg_write(0x4950, dmsc_r_deir_array << 0x10 | data_c5074);
    system_reg_write(0x4954, data_c5078 << 0x10 | data_c507c);
    system_reg_write(0x4958, data_c5080 << 0x10 | data_c5084);
    system_reg_write(0x495c, data_c5088 << 0x10 | data_c508c);
    system_reg_write(0x4960, dmsc_g_deir_array << 0x10 | data_c5054);
    system_reg_write(0x4964, data_c5058 << 0x10 | data_c505c);
    system_reg_write(0x4968, data_c5060 << 0x10 | data_c5064);
    system_reg_write(0x496c, data_c5048 << 0x10 | data_c504c);
    system_reg_write(0x4970, dmsc_b_deir_array << 0x10 | data_c5034);
    system_reg_write(0x4974, data_c5038 << 0x10 | data_c503c);
    system_reg_write(0x4978, data_c5040 << 0x10 | data_c5044);
    system_reg_write(0x497c, data_c5048 << 0x10 | data_c504c);
}

