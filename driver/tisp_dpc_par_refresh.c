#include "include/main.h"


  int32_t tisp_dpc_par_refresh(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t $v0 = data_9ab10_1;
    
    if ($v0 != 0xffffffff)
    {
        int32_t $v0_2 = $v0 - arg1;
        
        if (arg1 >= $v0)
            $v0_2 = arg1 - $v0;
        
        if ($v0_2 >= arg2)
        {
            data_9ab10_2 = arg1;
            tisp_dpc_intp_reg_refresh(arg1);
        }
    }
    else
    {
        data_9ab10_3 = arg1;
        tisp_dpc_all_reg_refresh(arg1);
    }
    
    if (arg3 == 1)
        system_reg_write(0x2898, 1);
    
    return 0;
}

