#include "include/main.h"


  int32_t tiziano_awb_set_lum_th_freq()

{
    int32_t var_10_37 = 0x400;
    uint32_t var_c_5 = 0;
    tisp_ae_mean_update(&var_c_6, &var_10_38);
    uint32_t $a2 = data_aa064_1;
    uint32_t $v0 = var_c_7;
    
    if ($v0 >= $a2)
        $v0 = $a2;
    
    uint32_t $v0_2 = ($v0 * var_10_39) >> 0xa;
    
    if (!$v0_2)
        $v0_2 = 1;
    
    system_reg_write_awb(1, 0xb038, data_aa06c_1 << 0x10 | data_aa068_1 << 8 | $v0_2);
    return 0;
}

