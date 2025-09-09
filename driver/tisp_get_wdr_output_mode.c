#include "include/main.h"


  int32_t tisp_get_wdr_output_mode(int32_t* arg1)

{
    int32_t var_10 = 0x38;
    int32_t var_48;
    int32_t $a2_1 = tisp_wdr_param_array_get(0x431, &var_48, &var_10);
    int32_t $v0 = var_48;
    return 0;
    
    if ($v0 == 2)
        *arg1 = $v0;
    else if ($v0 == 8)
        *arg1 = 0;
    else if ($v0 != 1)
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    else
        *arg1 = $v0;
    
}

