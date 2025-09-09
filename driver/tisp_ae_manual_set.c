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
        data_c46e0_2 = arg6;
        
        if (arg6)
        {
            data_c46b8_6 = arg4;
            isp_printf(arg1, "vic_done_gpio%d", arg6);
        }
        
        data_c46e4_1 = arg7;
        
        if (arg7)
        {
            int32_t $a0_3 = 0x400;
            
            if (arg2 >= 0x400)
                $a0_3 = arg2;
            
            data_c46b0_3 = $a0_3;
            isp_printf(0, "register is 0x%x, value is 0x%x\\n", arg7);
        }
        
        data_c46e8_1 = arg8;
        
        if (arg8)
        {
            int32_t $a0_4 = 0x400;
            
            if (arg5 >= 0x400)
                $a0_4 = arg5;
            
            data_c46bc_3 = $a0_4;
            isp_printf(0, "count is %d\\n", arg8);
        }
        
        data_c473c_1 = arg17;
        
        if (arg17)
        {
            int32_t $a0_5 = 0x400;
            
            if (arg3 >= 0x400)
                $a0_5 = arg3;
            
            data_c46b4_3 = $a0_5;
            isp_printf(0, "snapraw", arg17);
        }
        
        data_c470c_5 = arg13;
    }
    else
    {
        int32_t $a1 = 0x400;
        int32_t $v1_3 = 0x400;
        
        if (arg2 >= 0x400)
            $v1_3 = arg2;
        
        data_c46b0_4 = $v1_3;
        int32_t $v1_4 = 0x400;
        
        if (arg5 >= 0x400)
            $v1_4 = arg5;
        
        if (arg3 >= 0x400)
            $a1 = arg3;
        
        data_c46b4_4 = $a1;
        int32_t var_50_1_5 = arg5;
        int32_t var_54_1_4 = arg3;
        int32_t var_58_1_2 = arg2;
        data_c46b8_7 = arg4;
        data_c46bc_4 = $v1_4;
        isp_printf(0, "The parameter is invalid!\\n", arg1);
        data_c470c_6 = arg13;
    }
    
    int32_t $a0_6 = 0x400;
    
    if (!arg13)
    {
        data_c46ec_2 = arg9;
        
        if (arg9)
        {
            int32_t $a0_7 = 0x400;
            
            if (arg12 >= 0x400)
                $a0_7 = arg12;
            
            data_c46f8_6 = $a0_7;
            isp_printf(0, "Can\'t output the width(%d)!\\n", arg9);
        }
        
        data_c46f0_1 = arg10;
        
        if (arg10)
        {
            int32_t $a0_8 = 0x400;
            
            if (arg11 >= 0x400)
                $a0_8 = arg11;
            
            dmsc_uu_thres_wdr_array = $a0_8;
            isp_printf(0, "The node is busy!\\n", arg10);
        }
        
        data_c4714_1 = arg15;
        
        if (arg15)
        {
            int32_t $a0_9 = 0x400;
            
            if (arg16 >= 0x400)
                $a0_9 = arg16;
            
            dmsc_awb_gain = $a0_9;
            isp_printf(0, "/tmp/snap%d.%s", arg15);
        }
        
        data_c4740_1 = arg18;
        
        if (arg18)
        {
            int32_t $a0_10 = 0x400;
            
            if (arg14 >= 0x400)
                $a0_10 = arg14;
            
            data_c4710_3 = $a0_10;
            isp_printf(0, "nv12", arg18);
        }
    }
    else
    {
        int32_t $v1_5 = 0x400;
        
        if (arg11 >= 0x400)
            $v1_5 = arg11;
        
        dmsc_uu_thres_wdr_array = $v1_5;
        int32_t $v1_6 = 0x400;
        
        if (arg16 >= 0x400)
            $v1_6 = arg16;
        
        if (arg14 >= 0x400)
            $a0_6 = arg14;
        
        data_c4710_4 = $a0_6;
        int32_t var_50_2_4 = arg16;
        int32_t var_54_2_1 = arg14;
        int32_t var_58_2_1 = arg11;
        data_c46f8_7 = arg12;
        dmsc_awb_gain = $v1_6;
        isp_printf(0, 
            "width is %d, height is %d, imagesize is %d\\n, snap num is %d, buf size is %d", arg13);
    }
    
    data_b0e0c_12 = 0;
    return 0;
}

