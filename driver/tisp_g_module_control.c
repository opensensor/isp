#include "include/main.h"


  void* tisp_g_module_control(void** arg1)

{
    int32_t $v0 = system_reg_read(0xc);
    void* result = $v0 & &data_7ffff;
    void var_18;
    int32_t var_14;
    tisp_mdns_param_array_get(0x180, &var_14, &var_18);
    
    if (!var_14)
        result |= 0x80000000;
    
    *arg1 = result;
    return result;
}

