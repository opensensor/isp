#include "include/main.h"


  int32_t tisp_g_aezone_weight(int32_t arg1)

{
    int32_t var_10_59 = 0;
    tisp_ae_param_array_get(0x10, arg1, &var_10_60);
    
    if (var_10_61 == 0x384)
        return 0;
    
    isp_printf(2, "Failed to allocate vic device\\n", "tisp_g_aezone_weight");
    return 0xffffffff;
}

