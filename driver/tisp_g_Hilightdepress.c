#include "include/main.h"


  int32_t tisp_g_Hilightdepress(int32_t* arg1)

{
    int32_t var_c_9 = 0;
    int32_t var_38_56;
    tisp_ae_param_array_get(0xc, &var_38_57, &var_c_10);
    
    if (var_c_11 != 0x2c)
    {
        isp_printf(2, "%s:%d::linear mode\\n", "tisp_g_Hilightdepress");
        return 0xffffffff;
    }
    
    int32_t var_20_203;
    
    if (var_38_58 && var_20_204 != 1)
        *arg1 = var_20_205 - 1;
    else
        *arg1 = 0;
    
    return 0;
}

