#include "include/main.h"


  int32_t tisp_g_Gamma(int32_t arg1)

{
    int32_t var_10_53 = 0;
    tisp_gamma_param_array_get(0x3c, arg1, &var_10_54);
    
    if (var_10_55 == 0x102)
        return 0;
    
    isp_printf(2, "qbuffer null\\n", "tisp_g_Gamma");
    return 0xffffffff;
}

