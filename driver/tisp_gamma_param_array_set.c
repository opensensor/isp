#include "include/main.h"


  int32_t tisp_gamma_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    void* $a0;
    
    if (arg1 == 0x3c)
        $a0 = &tiziano_gamma_lut;
    else
    {
        if (arg1 != 0x3d)
        {
            int32_t var_10_1_3 = arg1;
            isp_printf(2, &$LC0, "tisp_gamma_param_array_set");
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

