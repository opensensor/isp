#include "include/main.h"


  int32_t tisp_dpc_d_m3_par_cfg()

{
    int32_t $a1_2 = data_cb800;
    int32_t $v0_3 = dpc_d_m3_dthres_intp - $a1_2;
    int32_t $a0_1 = dpc_d_m3_fthres_intp - $a1_2;
    system_reg_write(0x2850, dpc_d_m3_dthres_intp << 0x10 | dpc_d_m3_fthres_intp);
    
    if ($v0_3 < 0)
        $v0_3 = 0;
    
    if ($a0_1 < 0)
        $a0_1 = 0;
    
    system_reg_write(0x2834, $v0_3 << 0x10 | $a0_1);
    system_reg_write(0x2814, data_cb7f8 << 0x10 | data_cb7fc << 0x18 | data_cb9f0);
    return 0;
}

