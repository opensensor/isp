#include "include/main.h"


  int32_t tisp_g_aeroi_weight(int32_t arg1)

{
    int32_t var_10 = 0;
    tisp_ae_param_array_get(0x12, arg1, &var_10);
    
    if ((uintptr_t)var_10 == 0x384)
        return 0;
    
    isp_printf(); // Fixed: macro call, removed arguments;
    return 0xffffffff;
}

