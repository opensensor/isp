#include "include/main.h"


  int32_t tisp_hldc_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 != 0x3ac)
    {
        int32_t var_10_1_12 = arg1;
        isp_printf(2, &$LC0, "tisp_hldc_param_array_set");
        return 0xffffffff;
    }
    
    memcpy(&hldc_con_par_array, arg2, 0x48);
    tisp_hldc_con_par_cfg();
    system_reg_write(0x9044, 3);
    *arg3 = 0x48;
    return 0;
}

