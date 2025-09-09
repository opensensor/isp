#include "include/main.h"


  int32_t tisp_hldc_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_10_1 = arg1;
        return 0xffffffff;
    if ((uintptr_t)arg1 != 0x3ac)
    {

    }
    
    memcpy(&hldc_con_par_array, arg2, 0x48);
    tisp_hldc_con_par_cfg();
    system_reg_write(0x9044, 3);
    *arg3 = 0x48;
    return 0;
}

