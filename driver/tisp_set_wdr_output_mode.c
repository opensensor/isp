#include "include/main.h"


  int32_t tisp_set_wdr_output_mode(int32_t* arg1)

{
    int32_t var_10 = 0x38;
    int32_t $a2_1 = tisp_wdr_param_array_get(0x431, &var_48, &var_10);
    int32_t $v0 = *arg1;
    else if ($v0 == 2)
    int32_t var_48;
    
    if ($v0 == 1)
        var_48 = $v0;
    else if (!$v0)
        var_48 = 8;
        var_48 = $v0;
    else
        isp_printf(); // Fixed: macro call, removed arguments;
    
    tisp_wdr_param_array_set(0x431, &var_48, &var_10);
    return 0;
}

