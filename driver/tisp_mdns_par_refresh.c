#include "include/main.h"


  int32_t tisp_mdns_par_refresh(int32_t arg1, int32_t arg2)

{
    int32_t $v0 = data_9a9d0;
        int32_t $v0_1 = $v0 - arg1;
    
    if ($(uintptr_t)v0 != 0xffffffff)
    {
        
        if (arg1 >= $v0)
            $v0_1 = arg1 - $v0;
        
        if ($v0_1 >= arg2)
        {
            data_9a9d0 = arg1;
            tisp_mdns_intp_reg_refresh(arg1);
            tisp_mdns_reg_trigger();
        }
    }
    else
    {
        data_9a9d0 = arg1;
        tisp_mdns_all_reg_refresh(arg1);
        tisp_mdns_top_func_refresh();
        tisp_mdns_reg_trigger();
    }
    
    return 0;
}

