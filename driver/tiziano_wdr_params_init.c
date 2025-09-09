#include "include/main.h"


  int32_t tiziano_wdr_params_init()

{
    int32_t param_wdr_tool_control_array_1 = param_wdr_tool_control_array;
            char* $a1_1 = (char*)(U"#',17=DKS[clu~" + i); // Fixed void pointer assignment
    
    if (param_wdr_tool_control_array_1 == 1)
    {
        param_wdr_gam_y_array = 0;
        data_b240c = 3;
        data_b2350 = 0;
        
        for (int32_t i = 0; (uintptr_t)i != 0x84; )
        {
            i += 4;
            *$a1_1 = 0x100;
        }
        
        data_b1ff8_4 = 0;
        goto label_6fa38;
    }
    
    if (param_wdr_tool_control_array_1 == 2)
    {
            char* $a1_2 = (char*)(U"#',17=DKS[clu~" + i_1); // Fixed void pointer assignment
        param_wdr_gam_y_array = 0;
        data_b240c = param_wdr_tool_control_array_1;
        data_b2350 = 0;
        
        for (int32_t i_1 = 0; (uintptr_t)i_1 != 0x84; )
        {
            i_1 += 4;
            *$a1_2 = 0x100;
        }
    }
    else if (param_wdr_tool_control_array_1 == 3)
    {
            char* $a1_3 = (char*)(U"#',17=DKS[clu~" + i_2); // Fixed void pointer assignment
        param_wdr_gam_y_array = 0;
        data_b240c = param_wdr_tool_control_array_1;
        data_b2350 = 0;
        
        for (int32_t i_2 = 0; (uintptr_t)i_2 != 0x84; )
        {
            i_2 += 4;
            *$a1_3 = 0x100;
        }
    }
    else if (param_wdr_tool_control_array_1 == 8)
    {
        param_wdr_gam_y_array = 0;
        data_b240c = 0;
        data_b2350 = 1;
        memcpy(U"#',17=DKS[clu~", &fusion1_cure_y_tmp, 0x84);
    label_6fa38:
        data_b1e4c = 0;
        data_b1e50 = 0;
        data_b1e5c = 0;
        data_b1e60 = 0;
    }
    else if (param_wdr_tool_control_array_1 == 9)
    {
            int32_t $a3_2 = *(&fusion1_cure_y_tmp + i_3);
            char* $a2_1 = (char*)(U"#',17=DKS[clu~" + i_3); // Fixed void pointer assignment
        param_wdr_gam_y_array = 0;
        data_b240c = 0;
        data_b2350 = 0;
        
        for (int32_t i_3 = 0; (uintptr_t)i_3 != 0x84; )
        {
            i_3 += 4;
            *$a2_1 = $a3_2;
        }
    }
    
    int32_t $v1 = data_b1588_1;
    int32_t $v0_1;
    
    if ($v1 == 1)
    {
        int32_t $a1_4 = data_b158c;
        int32_t $lo_1 = (data_b1590 - $a1_4) / 0x21;
            char* $t0_1 = (char*)(U"#',17=DKS[clu~" + i_4); // Fixed void pointer assignment
        
        for (int32_t i_4 = 0; (uintptr_t)i_4 != 0x84; )
        {
            i_4 += 4;
            *$t0_1 = $a1_4;
            $a1_4 += $lo_1;
        }
        
        $v0_1 = data_b1594_1;
    }
    else if ($v1 != 9)
        $v0_1 = data_b1594_2;
    else
    {
            int32_t $t0_3 = *(&fusion1_cure_y_tmp + i_5);
            char* $a3_3 = (char*)(U"#',17=DKS[clu~" + i_5); // Fixed void pointer assignment
        for (int32_t i_5 = 0; (uintptr_t)i_5 != 0x84; )
        {
            i_5 += 4;
            *$a3_3 = $t0_3;
        }
        
        $v0_1 = data_b1594_3;
    }
    
    if (!$v0_1)
    {
            int32_t $a2_3 = *(&wdr_gam_y33_array + i_6);
            char* $a1_5 = (char*)(&param_wdr_gam_y_array_def + i_6); // Fixed void pointer assignment
        for (int32_t i_6 = 0; (uintptr_t)i_6 != 0x84; )
        {
            i_6 += 4;
            *$a1_5 = $a2_3;
        }
    }
    else if ($v0_1 == 1)
    {
        int32_t* $v1_4 = &param_wdr_gam_y_array_def;
        
        for (int32_t i_7 = 0; (uintptr_t)i_7 != 0x1000; )
        {
            *$v1_4 = i_7;
            i_7 += 0x80;
            $v1_4 = &$v1_4[1];
        }
        
        data_b22f8_1 = 0xfff;
    }
    
    system_reg_write(0x2030, (data_b1fe0_1 & 0xfff) << 0x10 | (data_b1fdc_1 & 0xfff));
    system_reg_write(0x2034, data_b1fe8_1 << 0x10 | (data_b1fe4_1 & 0xfff));
    system_reg_write(0x2038, (data_b1ff0_1 & 0x3ff) << 0x10 | (data_b1fec_1 & 0x1ff));
    system_reg_write(0x203c, (data_b1fd8_1 & 0x3f) << 8 | (param_wdr_degost_para_array & 0x3f));
    system_reg_write(0x236c, data_b1ff4_1);
    system_reg_write(0x223c, (data_b1f28_1 & 0xfff) << 0x10 | (param_wdr_thrLable_array & 0xfff));
    system_reg_write(0x2240, (data_b1f30_1 & 0xfff) << 0x10 | (data_b1f2c_1 & 0xfff));
    system_reg_write(0x2244, (data_b1f38_1 & 0xfff) << 0x10 | (data_b1f34_1 & 0xfff));
    system_reg_write(0x2248, (data_b1f40_1 & 0xfff) << 0x10 | (data_b1f3c_1 & 0xfff));
    system_reg_write(0x224c, (data_b1f48_1 & 0xfff) << 0x10 | (data_b1f44_1 & 0xfff));
    system_reg_write(0x2250, (data_b1f50_1 & 0xfff) << 0x10 | (data_b1f4c_1 & 0xfff));
    system_reg_write(0x2254, (data_b1f58_1 & 0xfff) << 0x10 | (data_b1f54_1 & 0xfff));
    system_reg_write(0x2258, (data_b1f60_1 & 0xfff) << 0x10 | (data_b1f5c_1 & 0xfff));
    system_reg_write(0x225c, (data_b1f68_1 & 0xfff) << 0x10 | (data_b1f64_1 & 0xfff));
    system_reg_write(0x2260, (data_b1f70_1 & 0xfff) << 0x10 | (data_b1f6c_1 & 0xfff));
    system_reg_write(0x2264, (data_b1f78_1 & 0xfff) << 0x10 | (data_b1f74_1 & 0xfff));
    system_reg_write(0x2268, (data_b1f80_1 & 0xfff) << 0x10 | (data_b1f7c_1 & 0xfff));
    system_reg_write(0x226c, (data_b1f88_1 & 0xfff) << 0x10 | (data_b1f84_1 & 0xfff));
    system_reg_write(0x2270, data_b1f8c_1 & 0xfff);
    system_reg_write(0x234c, data_b1fb8_1 << 0x10 | param_wdr_darkLable_array);
    system_reg_write(0x2350, data_b1fc0_1 << 0x10 | data_b1fbc_1);
    system_reg_write(0x2354, data_b1fc4_1);
    system_reg_write(0x2358, (data_b1fa8_1 & 7) << 0x10 | (param_wdr_darkLableN_array & 7));
    system_reg_write(0x235c, (data_b1fb0_1 & 7) << 0x10 | (data_b1fac_1 & 7));
    system_reg_write(0x2360, (data_b1f94_1 & 0x1f) << 0x10 | (param_wdr_darkWeight_array & 0x1f));
    system_reg_write(0x2364, (data_b1f9c_1 & 0x1f) << 0x10 | (data_b1f98_1 & 0x1f));
    system_reg_write(0x2368, data_b1fa0_1 & 0x1f);
    system_reg_write(0x244c, (data_b227c_1 & 0xfff) << 0x10 | (param_wdr_gam_y_array_def & 0xfff));
    system_reg_write(0x2450, (data_b2284_1 & 0xfff) << 0x10 | (data_b2280_1 & 0xfff));
    system_reg_write(0x2454, (data_b228c_1 & 0xfff) << 0x10 | (data_b2288_1 & 0xfff));
    system_reg_write(0x2458, (data_b2294_1 & 0xfff) << 0x10 | (data_b2290_1 & 0xfff));
    system_reg_write(0x245c, (data_b229c_1 & 0xfff) << 0x10 | (data_b2298_1 & 0xfff));
    system_reg_write(0x2460, (data_b22a4_1 & 0xfff) << 0x10 | (data_b22a0_1 & 0xfff));
    system_reg_write(0x2464, (data_b22ac_1 & 0xfff) << 0x10 | (data_b22a8_1 & 0xfff));
    system_reg_write(0x2468, (data_b22b4_1 & 0xfff) << 0x10 | (data_b22b0_1 & 0xfff));
    system_reg_write(0x246c, (data_b22bc_1 & 0xfff) << 0x10 | (data_b22b8_1 & 0xfff));
    system_reg_write(0x2470, (data_b22c4_1 & 0xfff) << 0x10 | (data_b22c0_1 & 0xfff));
    system_reg_write(0x2474, (data_b22cc_1 & 0xfff) << 0x10 | (data_b22c8_1 & 0xfff));
    system_reg_write(0x2478, (data_b22d4_1 & 0xfff) << 0x10 | (data_b22d0_1 & 0xfff));
    system_reg_write(0x247c, (data_b22dc_1 & 0xfff) << 0x10 | (data_b22d8_1 & 0xfff));
    system_reg_write(0x2480, (data_b22e4_1 & 0xfff) << 0x10 | (data_b22e0_1 & 0xfff));
    system_reg_write(0x2484, (data_b22ec_1 & 0xfff) << 0x10 | (data_b22e8_1 & 0xfff));
    system_reg_write(0x2488, (data_b22f4_1 & 0xfff) << 0x10 | (data_b22f0_1 & 0xfff));
    system_reg_write(0x248c, data_b22f8_2 & 0xfff);
    system_reg_write(0x2490, 
        (data_b23fc_1 & 0x1fff) << 0x10 | (param_wdr_w_point_weight_x_array & 0x1fff));
    system_reg_write(0x2494, (data_b2404_1 & 0x1fff) << 0x10 | (data_b2400_1 & 0x1fff));
    system_reg_write(0x2498, 
        (data_b23ec_1 & 0x7fff) << 0x10 | (param_wdr_w_point_weight_y_array & 0x7fff));
    system_reg_write(0x249c, (data_b23f4_1 & 0x7fff) << 0x10 | (data_b23f0_1 & 0x7fff));
    system_reg_write(0x24a0, 
        (data_b23e0_1 & 0xf) << 8 | (data_b23e4_1 & 0xf) << 0x10
            | (param_wdr_w_point_weight_pow_array & 0xf));
    system_reg_write(0x24f4, param_wdr_contrast_t_y_mux_array | data_b232c_1 << 0x10);
    system_reg_write(0x24f8, data_b2330_1 | data_b2334_1 << 0x10);
    system_reg_write(0x24fc, data_b2338_1);
    system_reg_write(0x2630, (data_b231c_1 & 0xfff) << 0x10 | (param_wdr_ct_cl_para_array & 0xfff));
    system_reg_write(0x2634, (data_b2324_1 & 0xfff) << 0x10 | (data_b2320_1 & 0xfff));
    system_reg_write(0x2600, 
        (data_b240c_1 & 3) << 4 | (data_b2414_1 & 1) << 8 | (param_wdr_para_array & 1)
            | (data_b2428_1 & 1) << 0xc | (data_b2410_1 & 0xfff) << 0x10);
    system_reg_write(0x260c, data_b242c_1 & 0xfff);
    system_reg_write(0x2610, (data_b2350_1 & 1) | data_b2354_1 << 0x10);
    system_reg_write(0x2650, (data_b2304_1 & 0xfff) << 0x10 | (data_b2300_1 & 0xfff));
    system_reg_write(0x2654, data_b2308_1 & 0xfff);
    system_reg_write(0x2658, (data_b2310_1 & 0xfff) << 0x10 | (data_b230c_1 & 0xfff));
    system_reg_write(0x265c, data_b2314_1 & 0xfff);
    system_reg_write(0x2684, 
        (*(param_wdr_dbg_out_array + 4) & 0x1fff) << 4 | (*param_wdr_dbg_out_array & 0x1fff));
    return 0;
}

