#include "include/main.h"


  int32_t tisp_g_aeroi_weight(int32_t arg1)

{
    int32_t var_10_62 = 0;
    tisp_ae_param_array_get(0x12, arg1, &var_10_63);
    
    if (var_10_64 == 0x384)
        return 0;
    
    isp_printf(2, "bank no free\\n", "tisp_g_aeroi_weight");
    return 0xffffffff;
}

