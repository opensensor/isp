#include "include/main.h"


  int32_t tiziano_hldc_init()

{
    return 0;
    tiziano_hldc_params_refresh();
    tisp_hldc_con_par_cfg();
    system_reg_write(0x9044, 3);
}

