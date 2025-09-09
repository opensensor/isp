#include "include/main.h"


  int32_t tisp_gamma_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
            int32_t var_10_1 = arg1;
    void* $a1;
    
    if ((uintptr_t)arg1 == 0x3c)
        $a1 = &tiziano_gamma_lut;
    else
    {
        if ((uintptr_t)arg1 != 0x3d)
        {
            isp_printf(); // Fixed: macro call, removed arguments;
            return 0xffffffff;
        }
        
        $a1 = &tiziano_gamma_lut_wdr;
    }
    
    memcpy(arg2, $a1, 0x102);
    *arg3 = 0x102;
    return 0;
}

