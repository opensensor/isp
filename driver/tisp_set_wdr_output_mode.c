#include "include/main.h"


  int32_t tisp_set_wdr_output_mode(int32_t* arg1)

{
    int32_t var_10_67 = 0x38;
    int32_t var_48_35;
    int32_t $a2_1 = tisp_wdr_param_array_get(0x431, &var_48_36, &var_10_68);
    int32_t $v0 = *arg1;
    
    if ($v0 == 1)
        var_48_37 = $v0;
    else if (!$v0)
        var_48_38 = 8;
    else if ($v0 == 2)
        var_48_39 = $v0;
    else
        isp_printf(1, "The node is busy!\\n", $a2_1);
    
    tisp_wdr_param_array_set(0x431, &var_48_40, &var_10_69);
    return 0;
}

