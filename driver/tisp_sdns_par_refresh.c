#include "include/main.h"


  int32_t tisp_sdns_par_refresh(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t $v0 = data_9a9c4;
        int32_t $v0_2 = $v0 - arg1;
    
    if ($(uintptr_t)v0 != 0xffffffff)
    {
        
        if (arg1 >= $v0)
            $v0_2 = arg1 - $v0;
        
        if ($v0_2 >= arg2)
        {
            data_9a9c4 = arg1;
            tisp_sdns_intp_reg_refresh();
        }
    }
    else
    {
        data_9a9c4 = arg1;
        tisp_sdns_all_reg_refresh();
    }
    
    if (arg3 == 1)
        system_reg_write(0x8b4c, 1);
    
    return 0;
}

