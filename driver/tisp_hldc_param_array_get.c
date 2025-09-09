#include "include/main.h"


  int32_t tisp_hldc_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_10_1 = arg1;
    if ((uintptr_t)arg1 != 0x3ac)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xffffffff;
    }
    
    memcpy(arg2, &hldc_con_par_array, 0x48);
    *arg3 = 0x48;
    return 0;
}

