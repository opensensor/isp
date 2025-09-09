#include "include/main.h"


  int32_t tisp_dn_mode_set(int32_t arg1)

{
    int32_t $v0 = var_10;
    uint32_t $a0_1 = 0;
    int32_t var_10;
    memcpy(&var_10, arg1 + 0xc, 4);
    
    if ($v0)
    {
        $a0_1 = 1;
        
        if ($v0 != 1)
        {
            isp_printf(); // Fixed: macro call, removed arguments;
            $a0_1 = 0;
        }
    }
    
    tisp_day_or_night_s_ctrl($a0_1);
    return 0;
}

