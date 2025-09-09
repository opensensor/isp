#include "include/main.h"


  int32_t tisp_gamma_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    void* $a1;
    
    if (arg1 == 0x3c)
        $a1 = &tiziano_gamma_lut;
    else
    {
        if (arg1 != 0x3d)
        {
            int32_t var_10_1_2 = arg1;
            isp_printf(2, &$LC0, "tisp_gamma_param_array_get");
            return 0xffffffff;
        }
        
        $a1 = &tiziano_gamma_lut_wdr;
    }
    
    memcpy(arg2, $a1, 0x102);
    *arg3 = 0x102;
    return 0;
}

