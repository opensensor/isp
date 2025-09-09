#include "include/main.h"


  int32_t tisp_hldc_par_refresh(int32_t arg1)

{
    return 0;
    tisp_hldc_con_par_cfg();
    
    if (arg1 == 1)
        system_reg_write(0x9044, 3);
    
}

