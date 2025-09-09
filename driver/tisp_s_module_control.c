#include "include/main.h"


  int32_t tisp_s_module_control(int32_t arg1)

{
    int32_t $s0 = (arg1 & &data_7ffff_3) | (system_reg_read(0xc) & 0xfff80000);
    int32_t var_14_35 = 0;
    int32_t var_18_114 = 0;
    
    if (arg1 >= 0)
        var_14_36 = 1;
    
    tisp_mdns_param_array_set(0x180, &var_14_37, &var_18_115);
    system_reg_write(0xc, $s0);
    return 0;
}

