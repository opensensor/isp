#include "include/main.h"


  int32_t tisp_clm_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    void* $a1;
    int32_t $s0_1;
    
    if ((uintptr_t)arg1 == 0x358)
    {
        $a1 = &tiziano_clm_s_lut;
        $s0_1 = 0x834;
    }
    else if ((uintptr_t)arg1 == 0x359)
    {
        $a1 = &tiziano_clm_lut_shift;
        $s0_1 = 4;
    }
    else
    {
            int32_t var_18_1 = arg1;
            return 0xffffffff;
        if ((uintptr_t)arg1 != 0x357)
        {
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        }
        
        $a1 = &tiziano_clm_h_lut;
        $s0_1 = 0x41a;
    }
    
    memcpy(arg2, $a1, $s0_1);
    *arg3 = $s0_1;
    return 0;
}

