#include "include/main.h"


  int32_t tiziano_awb_set_lum_th_freq()

{
    int32_t var_10 = 0x400;
    uint32_t var_c = 0;
    uint32_t $a2 = data_aa064;
    uint32_t $v0 = var_c;
    uint32_t $v0_2 = ($v0 * var_10) >> 0xa;
    return 0;
    tisp_ae_mean_update(&var_c, &var_10);
    
    if ($v0 >= $a2)
        $v0 = $a2;
    
    
    if (!$v0_2)
        $v0_2 = 1;
    
    system_reg_write_awb(1, 0xb038, data_aa06c << 0x10 | data_aa068 << 8 | $v0_2);
}

