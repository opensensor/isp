#include "include/main.h"


  int32_t tisp_get_wdr_output_mode(int32_t* arg1)

{
    int32_t var_10_70 = 0x38;
    int32_t var_48_41;
    int32_t $a2_1 = tisp_wdr_param_array_get(0x431, &var_48_42, &var_10_71);
    int32_t $v0 = var_48_43;
    
    if ($v0 == 2)
        *arg1 = $v0;
    else if ($v0 == 8)
        *arg1 = 0;
    else if ($v0 != 1)
        isp_printf(1, "The node is busy!\\n", $a2_1);
    else
        *arg1 = $v0;
    
    return 0;
}

