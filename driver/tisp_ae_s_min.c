#include "include/main.h"


  int32_t tisp_ae_s_min(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)

{
    int32_t arg_4 = arg2;
    int32_t arg_c = arg4;
    int32_t var_1c_14 = arg4;
    int32_t arg_0 = arg1;
    int32_t arg_8 = arg3;
    int32_t var_20_197 = arg3;
    isp_printf(0, "%s[%d] do not support this interface\\n", arg1);
    
    if (!arg1 || **&IspAeExp < arg1)
        isp_printf(1, "%s:%d::linear mode\\n", arg1);
    else
        *dmsc_sp_d_ud_ns_opt = arg1;
    
    if (arg2 < 0x400 || **&data_d04b8_1 < arg2)
        isp_printf(1, "%s:%d::wdr mode\\n", arg2);
    else
        data_c471c_2 = arg2;
    
    if (data_b2e74_7 == 1)
    {
        int32_t $v0_11;
        
        if (!arg3 || **&data_d04d4_1 < arg3)
        {
            isp_printf(1, "qbuffer null\\n", arg3);
            $v0_11 = arg4 < 0x400 ? 1 : 0;
        }
        else
        {
            data_c4730_2 = arg3;
            $v0_11 = arg4 < 0x400 ? 1 : 0;
        }
        
        if ($v0_11 || **&data_d04d8_1 < arg4)
            isp_printf(1, "bank no free\\n", arg4);
        else
            dmsc_sp_ud_ns_thres_array = arg4;
    }
    
    data_b0e0c_4 = 0;
    return 0;
}

