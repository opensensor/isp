#include "include/main.h"


  int32_t tisp_g_Hilightdepress(int32_t* arg1)

{
    int32_t var_c = 0;
    int32_t var_38;
        return 0xffffffff;
    tisp_ae_param_array_get(0xc, &var_38, &var_c);
    
    if ((uintptr_t)var_c != 0x2c)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    int32_t var_20_15;
    
    if (var_38_18 && var_20_16 != 1)
        *arg1 = var_20_17 - 1;
    else
        *arg1 = 0;
    
    return 0;
}

