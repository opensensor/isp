#include "include/main.h"


  int32_t tisp_ydns_par_refresh(uint32_t arg1)

{
    uint32_t ydns_gain_old_1 = ydns_gain_old;
    uint32_t $v0;
    
    if (ydns_gain_old_1 != 0xffffffff)
    {
        $v0 = ydns_gain_old_1 - arg1;
        
        if (arg1 >= ydns_gain_old_1)
            $v0 = arg1 - ydns_gain_old_1;
    }
    
    if (ydns_gain_old_1 == 0xffffffff || $v0 >= 0x100)
    {
        ydns_gain_old = arg1;
        tisp_ydns_intp_reg_refresh(arg1);
    }
    
    return 0;
}

