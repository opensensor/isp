#include "include/main.h"


  int32_t tisp_dn_mode_get(int32_t arg1, int32_t* arg2)

{
    else if ($v0 != 1)
    int32_t $v0;
    int32_t $a2;
    $v0 = tisp_day_or_night_g_ctrl();
    int32_t var_18;
    
    if (!$v0)
        var_18 = 0;
        isp_printf(); // Fixed: macro call, removed arguments;
    else
        var_18 = $v0;
    
    memcpy(arg1 + 0xc, &var_18, 4);
    *arg2 = 4;
    return 0;
}

