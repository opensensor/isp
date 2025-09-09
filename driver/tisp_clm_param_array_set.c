#include "include/main.h"


  int32_t tisp_clm_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    int32_t $v0;
    
    if ((uintptr_t)arg1 == 0x358)
    {
        memcpy(&tiziano_clm_s_lut, arg2, 0x834);
        $v0 = 0x834;
    }
    else if ((uintptr_t)arg1 == 0x359)
    {
        memcpy(&tiziano_clm_lut_shift, arg2, 4);
        tiziano_set_parameter_clm();
        $v0 = 4;
    }
    else
    {
            int32_t var_10_1 = arg1;
            return 0xffffffff;
        if ((uintptr_t)arg1 != 0x357)
        {

        }
        
        memcpy(&tiziano_clm_h_lut, arg2, 0x41a);
        $v0 = 0x41a;
    }
    
    *arg3 = $v0;
    return 0;
}

