#include "include/main.h"


  int32_t tisp_rdns_par_refresh(uint32_t arg1, int32_t arg2, int32_t arg3)

{
    uint32_t rdns_gain_old_1 = rdns_gain_old;
    
    if (rdns_gain_old_1 != 0xffffffff)
    {
        uint32_t $v0_1 = rdns_gain_old_1 - arg1;
        
        if (arg1 >= rdns_gain_old_1)
            $v0_1 = arg1 - rdns_gain_old_1;
        
        if ($v0_1 >= 0x100)
        {
            rdns_gain_old = arg1;
            tisp_rdns_intp_reg_refresh();
        }
    }
    else
    {
        rdns_gain_old = arg1;
        tisp_rdns_all_reg_refresh();
    }
    
    if (arg3 == 1)
        system_reg_write(0x30ac, 1);
    
    return 0;
}

