#include "include/main.h"


  int32_t tisp_ae_manual_set(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7, int32_t arg8, int32_t arg9, int32_t arg10, int32_t arg11, int32_t arg12, int32_t arg13, int32_t arg14, int32_t arg15, int32_t arg16, int32_t arg17, int32_t arg18)

{
    int32_t arg_8 = arg3;
    int32_t arg_0 = arg1;
    int32_t arg_4 = arg2;
    int32_t arg_c = arg4;
    dmsc_sp_d_w_stren_wdr_array = arg1;
    
    if (!arg1)
    {
        data_c46e0 = arg6;
        
        if (arg6)
        {
            data_c46b8 = arg4;
            isp_printf(); // Fixed: macro call, removed arguments;
        }
        
        data_c46e4_1 = arg7;
        
        if (arg7)
        {
            int32_t $a0_3 = 0x400;
            
            if ((uintptr_t)arg2 >= 0x400)
                $a0_3 = arg2;
            
            data_c46b0 = $a0_3;
            isp_printf(); // Fixed: macro call, removed arguments;
        }
        
        data_c46e8_1 = arg8;
        
        if (arg8)
        {
            int32_t $a0_4 = 0x400;
            
            if ((uintptr_t)arg5 >= 0x400)
                $a0_4 = arg5;
            
            data_c46bc = $a0_4;
            isp_printf(); // Fixed: macro call, removed arguments;
        }
        
        data_c473c_1 = arg17;
        
        if (arg17)
        {
            int32_t $a0_5 = 0x400;
            
            if ((uintptr_t)arg3 >= 0x400)
                $a0_5 = arg3;
            
            data_c46b4 = $a0_5;
            isp_printf(); // Fixed: macro call, removed arguments;
        }
        
        data_c470c_2 = arg13;
    }
    else
    {
        int32_t $a1 = 0x400;
        int32_t $v1_3 = 0x400;
        int32_t $v1_4 = 0x400;
        int32_t var_50_1 = arg5;
        int32_t var_54_1 = arg3;
        int32_t var_58_1 = arg2;
        
        if ((uintptr_t)arg2 >= 0x400)
            $v1_3 = arg2;
        
        data_c46b0 = $v1_3;
        
        if ((uintptr_t)arg5 >= 0x400)
            $v1_4 = arg5;
        
        if ((uintptr_t)arg3 >= 0x400)
            $a1 = arg3;
        
        data_c46b4 = $a1;
        data_c46b8 = arg4;
        data_c46bc = $v1_4;
        isp_printf(); // Fixed: macro call, removed arguments;
        data_c470c = arg13;
    }
    
    int32_t $a0_6 = 0x400;
    
    if (!arg13)
    {
            int32_t $a0_7 = 0x400;
        data_c46ec = arg9;
        
        if (arg9)
        {
            
            if ((uintptr_t)arg12 >= 0x400)
                $a0_7 = arg12;
            
            data_c46f8 = $a0_7;
            isp_printf(); // Fixed: macro call, removed arguments!\n", arg9);
        }
        
        data_c46f0_1 = arg10;
        
        if (arg10)
        {
            int32_t $a0_8 = 0x400;
            
            if ((uintptr_t)arg11 >= 0x400)
                $a0_8 = arg11;
            
            dmsc_uu_thres_wdr_array = $a0_8;
            isp_printf(); // Fixed: macro call, removed arguments;
        }
        
        data_c4714_1 = arg15;
        
        if (arg15)
        {
            int32_t $a0_9 = 0x400;
            
            if ((uintptr_t)arg16 >= 0x400)
                $a0_9 = arg16;
            
            dmsc_awb_gain = $a0_9;
            isp_printf(); // Fixed: macro call, removed arguments;
        }
        
        data_c4740_1 = arg18;
        
        if (arg18)
        {
            int32_t $a0_10 = 0x400;
            
            if ((uintptr_t)arg14 >= 0x400)
                $a0_10 = arg14;
            
            data_c4710 = $a0_10;
            isp_printf(); // Fixed: macro call, removed arguments;
        }
    }
    else
    {
        int32_t $v1_5 = 0x400;
        int32_t $v1_6 = 0x400;
        int32_t var_50_2 = arg16;
        int32_t var_54_2 = arg14;
        int32_t var_58_2 = arg11;
        
        if ((uintptr_t)arg11 >= 0x400)
            $v1_5 = arg11;
        
        dmsc_uu_thres_wdr_array = $v1_5;
        
        if ((uintptr_t)arg16 >= 0x400)
            $v1_6 = arg16;
        
        if ((uintptr_t)arg14 >= 0x400)
            $a0_6 = arg14;
        
        data_c4710 = $a0_6;
        data_c46f8 = arg12;
        dmsc_awb_gain = $v1_6;
        isp_printf(0, 
            "width is %d, height is %d, imagesize is %d\n, snap num is %d, buf size is %d", arg13);
    }
    
    data_b0e0c_6 = 0;
    return 0;
}

