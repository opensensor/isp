#include "include/main.h"


  int32_t tisp_gamma_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
            int32_t var_10_1 = arg1;
    void* $a0;
    
    if ((uintptr_t)arg1 == 0x3c)
        $a0 = &tiziano_gamma_lut;
    else
    {
        if ((uintptr_t)arg1 != 0x3d)
        {
            isp_printf(); // Fixed: macro call, removed arguments;
            return 0xffffffff;
        }
        
        $a0 = &tiziano_gamma_lut_wdr;
    }
    
    *arg3 = 0x102;
    memcpy($a0, arg2, 0x102);
    tiziano_gamma_lut_parameter();
    tiziano_adr_gamma_refresh();
    tiziano_wdr_gamma_refresh();
    return 0;
}

