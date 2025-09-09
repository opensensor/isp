#include "include/main.h"


  int32_t tisp_g_BacklightComp(int32_t* arg1)

{
    int32_t var_c_12 = 0;
    int32_t var_38_59;
    tisp_ae_param_array_get(0xc, &var_38_60, &var_c_13);
    
    if (var_c_14 != 0x2c)
    {
        isp_printf(2, "%s:%d::wdr mode\\n", "tisp_g_BacklightComp");
        return 0xffffffff;
    }
    
    int32_t var_24_10;
    
    if (var_38_61 && var_24_11 != 1)
        *arg1 = var_24_12 - 1;
    else
        *arg1 = 0;
    
    return 0;
}

