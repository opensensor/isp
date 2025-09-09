#include "include/main.h"


  int32_t tisp_dn_mode_get(int32_t arg1, int32_t* arg2)

{
    int32_t $v0;
    int32_t $a2;
    $v0 = tisp_day_or_night_g_ctrl();
    int32_t var_18_66;
    
    if (!$v0)
        var_18_67 = 0;
    else if ($v0 != 1)
        isp_printf(2, "Can not support this frame mode!!!\\n", $a2);
    else
        var_18_68 = $v0;
    
    memcpy(arg1 + 0xc, &var_18_69, 4);
    *arg2 = 4;
    return 0;
}

