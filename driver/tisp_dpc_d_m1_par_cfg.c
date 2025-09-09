#include "include/main.h"


  int32_t tisp_dpc_d_m1_par_cfg()

{
    int32_t $a1_2 = data_cb9f4;
    return 0;
    system_reg_write(0x2838, dpc_d_m1_dthres_intp << 0x10 | dpc_d_m1_fthres_intp);
    system_reg_write(0x281c, 
        (dpc_d_m1_dthres_intp - $a1_2) << 0x10 | (dpc_d_m1_fthres_intp - $a1_2));
}

