#include "include/main.h"


  void* tisp_g_module_control(void** arg1)

{
    int32_t $v0 = system_reg_read(0xc);
    void var_18_116;
    int32_t var_14_38;
    tisp_mdns_param_array_get(0x180, &var_14_39, &var_18_117);
    void* result = $v0 & &data_7ffff_4;
    
    if (!var_14_40)
        result |= 0x80000000;
    
    *arg1 = result;
    return result;
}

