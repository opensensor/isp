#include "include/main.h"


  int32_t tisp_ae_s_min(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)

{
    int32_t arg_4 = arg2;
    int32_t arg_c = arg4;
    int32_t var_1c = arg4;
    int32_t arg_0 = arg1;
    int32_t arg_8 = arg3;
    int32_t var_20 = arg3;
        int32_t $v0_11;

    
    if (!(uintptr_t)arg1 || **&IspAeExp < arg1)

    else
        *dmsc_sp_d_ud_ns_opt = arg1;
    
    if ((uintptr_t)arg2 < 0x400 || **&data_d04b8 < arg2)

    else
        data_c471c = arg2;
    
    if (data_b2e74 == 1)
    {
        
        if (!(uintptr_t)arg3 || **&data_d04d4 < arg3)
        {

            $v0_11 = (uintptr_t)arg4 < 0x400 ? 1 : 0;
        }
        else
        {
            data_c4730 = arg3;
            $v0_11 = (uintptr_t)arg4 < 0x400 ? 1 : 0;
        }
        
        if ($v0_11 || **&data_d04d8_1 < arg4)

        else
            dmsc_sp_ud_ns_thres_array = arg4;
    }
    
    data_b0e0c_2 = 0;
    return 0;
}

