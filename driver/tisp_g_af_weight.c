#include "include/main.h"


  int32_t tisp_g_af_weight(int32_t arg1)

{
    int32_t var_10_68 = 0;
    tisp_af_param_array_get(0x3bf, arg1, &var_10_69);
    
    if (var_10_70 == 0x384)
        return 0;
    
    isp_printf(2, "Failed to init isp module(%d.%d)\\n", "tisp_g_af_weight");
    return 0xffffffff;
}

