#include "include/main.h"


  int32_t tiziano_adr_init(uint32_t arg1, uint32_t arg2)

{
    uint32_t $lo = arg1 / 6;
    uint32_t $s7 = arg2 >> 2;
    int32_t $s7_1 = $s7 - ($s7 & 1);
    uint32_t $s2 = $s7_1 >> 2;
    int32_t $s2_1 = $s2 - ($s2 & 1);
    int32_t $t4 = $s7_1 << 1;
    int32_t $t5 = $s7_1 << 0x10;
    int32_t $s0_1 = $lo - ($lo & 1);
    uint32_t $s1 = $s0_1 >> 2;
    int32_t $t3 = $s0_1 << 1;
    int32_t $s1_1 = $s1 - ($s1 & 1);
    int32_t $v1_2 = $s0_1 + $t3;
    int32_t $t2 = $s0_1 + $v1_2;
    uint32_t $s0_3 = ($s0_1 + 1) >> 1;
    uint32_t $v0_3 = ($s7_1 + 1) >> 1;
    uint32_t width_def_1 = width_def;
                    uint32_t height_def_1 = height_def;
                            int32_t $a3_9 = *(&param_adr_centre_w_dis_array_tmp_4M + i);
                            void* $a2_5 = &param_adr_centre_w_dis_array_tmp + i;
    data_af158 = arg1;
    data_af15c = arg2;
    
    if ($(uintptr_t)s2_1 < 0x14)
        $s2_1 = 0x14;
    
    width_def = arg1;
    height_def = arg2;
    
    if ($(uintptr_t)s1_1 < 0x14)
        $s1_1 = 0x14;
    
    system_reg_write(0x4000, $s0_1 | $t5);
    system_reg_write(0x4010, $t5);
    system_reg_write(0x4014, ($s7_1 + $t4) << 0x10 | $t4);
    system_reg_write(0x4018, arg2);
    system_reg_write(0x401c, $s0_1 << 0x10);
    system_reg_write(0x4020, $v1_2 << 0x10 | $t3);
    system_reg_write(0x4024, ($s0_1 + $t2) << 0x10 | $t2);
    system_reg_write(0x4028, arg1);
    system_reg_write(0x4454, (arg2 - $s2_1) << 0x10 | $s2_1);
    system_reg_write(0x4458, (arg1 - $s1_1) << 0x10 | $s1_1);
    tiziano_adr_params_refresh();
    tisp_adr_set_params();
    
    if ($s0_3 >= $v0_3)
        data_ace54 = ($v0_3 * 3 + 1) >> 1;
    else
        data_ace54 = ($s0_3 * 3 + 1) >> 1;
    
    int32_t param_adr_tool_control_array_1;
    
    if ((uintptr_t)width_def_1 != 0x780)
    {
        if ((uintptr_t)width_def_1 != 0x900)
        {
            if ((uintptr_t)width_def_1 != 0xa20)
            {
                if ((uintptr_t)width_def_1 == 0xa00)
                {
                    
                    if ((uintptr_t)height_def_1 == 0x5a0)
                    {
                        for (int32_t i = 0; (uintptr_t)i != 0x7c; )
                        {
                            i += 4;
                            *$a2_5 = $a3_9;
                        }
                        
                        for (int32_t i_1 = 0; (uintptr_t)i_1 != 0x80; )
                        {
                            int32_t $t7_40 = *(&param_adr_weight_21_lut_array_tmp_4M + i_1);
                            void* $t6_20 = &param_adr_weight_21_lut_array_tmp + i_1;
                            *(&param_adr_weight_20_lut_array_tmp + i_1) =
                                *(&param_adr_weight_20_lut_array_tmp_4M + i_1);
                            *(&param_adr_weight_02_lut_array_tmp + i_1) =
                                *(&param_adr_weight_02_lut_array_tmp_4M + i_1);
                            *(&param_adr_weight_12_lut_array_tmp + i_1) =
                                *(&param_adr_weight_12_lut_array_tmp_4M + i_1);
                            *(&param_adr_weight_22_lut_array_tmp + i_1) =
                                *(&param_adr_weight_22_lut_array_tmp_4M + i_1);
                            i_1 += 4;
                            *$t6_20 = $t7_40;
                        }
                        
                        param_adr_tool_control_array_1 = param_adr_tool_control_array;
                    }
                    else if ((uintptr_t)height_def_1 != 0x780)
                    {
                        tiziano_adr_5x5_param();
                        param_adr_tool_control_array_1 = param_adr_tool_control_array;
                    }
                    else
                    {
                            int32_t $a3_11 = *(&param_adr_centre_w_dis_array_tmp_5MA + i_2);
                            void* $a2_6 = &param_adr_centre_w_dis_array_tmp + i_2;
                        for (int32_t i_2 = 0; (uintptr_t)i_2 != 0x7c; )
                        {
                            i_2 += 4;
                            *$a2_6 = $a3_11;
                        }
                        
                        for (int32_t i_3 = 0; (uintptr_t)i_3 != 0x80; )
                        {
                            int32_t $t7_50 = *(&param_adr_weight_21_lut_array_tmp_5MA + i_3);
                            void* $t6_25 = &param_adr_weight_21_lut_array_tmp + i_3;
                            *(&param_adr_weight_20_lut_array_tmp + i_3) =
                                *(&param_adr_weight_20_lut_array_tmp_5MA + i_3);
                            *(&param_adr_weight_02_lut_array_tmp + i_3) =
                                *(&param_adr_weight_02_lut_array_tmp_5MA + i_3);
                            *(&param_adr_weight_12_lut_array_tmp + i_3) =
                                *(&param_adr_weight_12_lut_array_tmp_5MA + i_3);
                            *(&param_adr_weight_22_lut_array_tmp + i_3) =
                                *(&param_adr_weight_22_lut_array_tmp_5MA + i_3);
                            i_3 += 4;
                            *$t6_25 = $t7_50;
                        }
                        
                        param_adr_tool_control_array_1 = param_adr_tool_control_array;
                    }
                }
                else if ((uintptr_t)width_def_1 != 0x500)
                {
                    if ((uintptr_t)width_def_1 != 0x6c0)
                    {
                        if ((uintptr_t)width_def_1 != 0x438)
                        {
                            if ((uintptr_t)width_def_1 != 0x430 || (uintptr_t)height_def != 0x5a0)
                            {
                                tiziano_adr_5x5_param();
                                param_adr_tool_control_array_1 = param_adr_tool_control_array;
                            }
                            else
                            {
                                memcpy(&param_adr_centre_w_dis_array_tmp, 
                                    &param_adr_centre_w_dis_array_tmp_1072_1440, 0x7c);
                                memcpy(&param_adr_weight_20_lut_array_tmp, 
                                    &param_adr_weigth_20_lut_array_tmp_1072_1440, 0x80);
                                memcpy(&param_adr_weight_02_lut_array_tmp, 
                                    &param_adr_weigth_02_lut_array_tmp_1072_1440, 0x80);
                                memcpy(&param_adr_weight_12_lut_array_tmp, 
                                    &param_adr_weigth_12_lut_array_tmp_1072_1440, 0x80);
                                memcpy(&param_adr_weight_22_lut_array_tmp, 
                                    &param_adr_weigth_22_lut_array_tmp_1072_1440, 0x80);
                                memcpy(&param_adr_weight_21_lut_array_tmp, 
                                    &param_adr_weigth_21_lut_array_tmp_1072_1440, 0x80);
                                param_adr_tool_control_array_1 = param_adr_tool_control_array;
                            }
                        }
                        else if ((uintptr_t)height_def != 0x5a0)
                        {
                            tiziano_adr_5x5_param();
                            param_adr_tool_control_array_1 = param_adr_tool_control_array;
                        }
                        else
                        {
                            memcpy(&param_adr_centre_w_dis_array_tmp, 
                                &param_adr_centre_w_dis_array_tmp_1080_1440, 0x7c);
                            memcpy(&param_adr_weight_20_lut_array_tmp, 
                                &param_adr_weigth_20_lut_array_tmp_1080_1440, 0x80);
                            memcpy(&param_adr_weight_02_lut_array_tmp, 
                                &param_adr_weigth_02_lut_array_tmp_1080_1440, 0x80);
                            memcpy(&param_adr_weight_12_lut_array_tmp, 
                                &param_adr_weigth_12_lut_array_tmp_1080_1440, 0x80);
                            memcpy(&param_adr_weight_22_lut_array_tmp, 
                                &param_adr_weigth_22_lut_array_tmp_1080_1440, 0x80);
                            memcpy(&param_adr_weight_21_lut_array_tmp, 
                                &param_adr_weigth_21_lut_array_tmp_1080_1440, 0x80);
                            param_adr_tool_control_array_1 = param_adr_tool_control_array;
                        }
                    }
                    else if ((uintptr_t)height_def != 0x3cc)
                    {
                        tiziano_adr_5x5_param();
                        param_adr_tool_control_array_1 = param_adr_tool_control_array;
                    }
                    else
                    {
                        memcpy(&param_adr_centre_w_dis_array_tmp, 
                            &param_adr_centre_w_dis_array_tmp_1728_972, 0x7c);
                        memcpy(&param_adr_weight_20_lut_array_tmp, 
                            &param_adr_weigth_20_lut_array_tmp_1728_972, 0x80);
                        memcpy(&param_adr_weight_02_lut_array_tmp, 
                            &param_adr_weigth_02_lut_array_tmp_1728_972, 0x80);
                        memcpy(&param_adr_weight_12_lut_array_tmp, 
                            &param_adr_weigth_12_lut_array_tmp_1728_972, 0x80);
                        memcpy(&param_adr_weight_22_lut_array_tmp, 
                            &param_adr_weigth_22_lut_array_tmp_1728_972, 0x80);
                        memcpy(&param_adr_weight_21_lut_array_tmp, 
                            &param_adr_weigth_21_lut_array_tmp_1728_972, 0x80);
                        param_adr_tool_control_array_1 = param_adr_tool_control_array;
                    }
                }
                else if ((uintptr_t)height_def != 0x2d0)
                {
                    tiziano_adr_5x5_param();
                    param_adr_tool_control_array_1 = param_adr_tool_control_array;
                }
                else
                {
                        int32_t $a3_13 = *(&param_adr_centre_w_dis_array_tmp_720P + i_4);
                        void* $a2_7 = &param_adr_centre_w_dis_array_tmp + i_4;
                    for (int32_t i_4 = 0; (uintptr_t)i_4 != 0x7c; )
                    {
                        i_4 += 4;
                        *$a2_7 = $a3_13;
                    }
                    
                    for (int32_t i_5 = 0; (uintptr_t)i_5 != 0x80; )
                    {
                        int32_t $t7_60 = *(&param_adr_weight_21_lut_array_tmp_720P + i_5);
                        void* $t6_30 = &param_adr_weight_21_lut_array_tmp + i_5;
                        *(&param_adr_weight_20_lut_array_tmp + i_5) =
                            *(&param_adr_weight_20_lut_array_tmp_720P + i_5);
                        *(&param_adr_weight_02_lut_array_tmp + i_5) =
                            *(&param_adr_weight_02_lut_array_tmp_720P + i_5);
                        *(&param_adr_weight_12_lut_array_tmp + i_5) =
                            *(&param_adr_weight_12_lut_array_tmp_720P + i_5);
                        *(&param_adr_weight_22_lut_array_tmp + i_5) =
                            *(&param_adr_weight_22_lut_array_tmp_720P + i_5);
                        i_5 += 4;
                        *$t6_30 = $t7_60;
                    }
                    
                    param_adr_tool_control_array_1 = param_adr_tool_control_array;
                }
            }
            else if ((uintptr_t)height_def != 0x798)
            {
                tiziano_adr_5x5_param();
                param_adr_tool_control_array_1 = param_adr_tool_control_array;
            }
            else
            {
                    int32_t $a3_7 = *(&param_adr_centre_w_dis_array_tmp_5M + i_6);
                    void* $a2_4 = &param_adr_centre_w_dis_array_tmp + i_6;
                for (int32_t i_6 = 0; (uintptr_t)i_6 != 0x7c; )
                {
                    i_6 += 4;
                    *$a2_4 = $a3_7;
                }
                
                for (int32_t i_7 = 0; (uintptr_t)i_7 != 0x80; )
                {
                    int32_t $t7_30 = *(&param_adr_weight_21_lut_array_tmp_5M + i_7);
                    void* $t6_15 = &param_adr_weight_21_lut_array_tmp + i_7;
                    *(&param_adr_weight_20_lut_array_tmp + i_7) =
                        *(&param_adr_weight_20_lut_array_tmp_5M + i_7);
                    *(&param_adr_weight_02_lut_array_tmp + i_7) =
                        *(&param_adr_weight_02_lut_array_tmp_5M + i_7);
                    *(&param_adr_weight_12_lut_array_tmp + i_7) =
                        *(&param_adr_weight_12_lut_array_tmp_5M + i_7);
                    *(&param_adr_weight_22_lut_array_tmp + i_7) =
                        *(&param_adr_weight_22_lut_array_tmp_5M + i_7);
                    i_7 += 4;
                    *$t6_15 = $t7_30;
                }
                
                param_adr_tool_control_array_1 = param_adr_tool_control_array;
            }
        }
        else if ((uintptr_t)height_def != 0x510)
        {
            tiziano_adr_5x5_param();
            param_adr_tool_control_array_1 = param_adr_tool_control_array;
        }
        else
        {
                int32_t $a3_5 = *(&param_adr_centre_w_dis_array_tmp_3M + i_8);
                void* $a2_3 = &param_adr_centre_w_dis_array_tmp + i_8;
            for (int32_t i_8 = 0; (uintptr_t)i_8 != 0x7c; )
            {
                i_8 += 4;
                *$a2_3 = $a3_5;
            }
            
            for (int32_t i_9 = 0; (uintptr_t)i_9 != 0x80; )
            {
                int32_t $t7_20 = *(&param_adr_weight_21_lut_array_tmp_3M + i_9);
                void* $t6_10 = &param_adr_weight_21_lut_array_tmp + i_9;
                *(&param_adr_weight_20_lut_array_tmp + i_9) =
                    *(&param_adr_weight_20_lut_array_tmp_3M + i_9);
                *(&param_adr_weight_02_lut_array_tmp + i_9) =
                    *(&param_adr_weight_02_lut_array_tmp_3M + i_9);
                *(&param_adr_weight_12_lut_array_tmp + i_9) =
                    *(&param_adr_weight_12_lut_array_tmp_3M + i_9);
                *(&param_adr_weight_22_lut_array_tmp + i_9) =
                    *(&param_adr_weight_22_lut_array_tmp_3M + i_9);
                i_9 += 4;
                *$t6_10 = $t7_20;
            }
            
            param_adr_tool_control_array_1 = param_adr_tool_control_array;
        }
    }
    else if ((uintptr_t)height_def != 0x438)
    {
        tiziano_adr_5x5_param();
        param_adr_tool_control_array_1 = param_adr_tool_control_array;
    }
    else
    {
            int32_t $a3_3 = *(&param_adr_centre_w_dis_array_tmp_1080P + i_10);
            void* $a2_2 = &param_adr_centre_w_dis_array_tmp + i_10;
        for (int32_t i_10 = 0; (uintptr_t)i_10 != 0x7c; )
        {
            i_10 += 4;
            *$a2_2 = $a3_3;
        }
        
        for (int32_t i_11 = 0; (uintptr_t)i_11 != 0x80; )
        {
            int32_t $t7_10 = *(&param_adr_weight_21_lut_array_tmp_1080P + i_11);
            void* $t6_5 = &param_adr_weight_21_lut_array_tmp + i_11;
            *(&param_adr_weight_20_lut_array_tmp + i_11) =
                *(&param_adr_weight_20_lut_array_tmp_1080P + i_11);
            *(&param_adr_weight_02_lut_array_tmp + i_11) =
                *(&param_adr_weight_02_lut_array_tmp_1080P + i_11);
            *(&param_adr_weight_12_lut_array_tmp + i_11) =
                *(&param_adr_weight_12_lut_array_tmp_1080P + i_11);
            *(&param_adr_weight_22_lut_array_tmp + i_11) =
                *(&param_adr_weight_22_lut_array_tmp_1080P + i_11);
            i_11 += 4;
            *$t6_5 = $t7_10;
        }
        
        param_adr_tool_control_array_1 = param_adr_tool_control_array;
    }
    
    int32_t i_12 = 0;
    
    if (!param_adr_tool_control_array_1)
    {
            int32_t $a3_15 = *(&param_adr_centre_w_dis_array_tmp + i_12);
            void* $a2_8 = &param_adr_centre_w_dis_array + i_12;
            int32_t $t7_70 = *(&param_adr_weight_21_lut_array_tmp + i_13);
            void* $t6_35 = &param_adr_weight_21_lut_array + i_13;
        do
        {
            i_12 += 4;
            *$a2_8 = $a3_15;
        } while ((uintptr_t)i_12 != 0x7c);
        
        for (int32_t i_13 = 0; (uintptr_t)i_13 != 0x80; )
        {
            *(((void**)((char*)&param_adr_weight_20_lut_array + i_13))) = *(&param_adr_weight_20_lut_array_tmp + i_13); // Fixed void pointer dereference
            *(((void**)((char*)&param_adr_weight_02_lut_array + i_13))) = *(&param_adr_weight_02_lut_array_tmp + i_13); // Fixed void pointer dereference
            *(((void**)((char*)&param_adr_weight_12_lut_array + i_13))) = *(&param_adr_weight_12_lut_array_tmp + i_13); // Fixed void pointer dereference
            *(((void**)((char*)&param_adr_weight_22_lut_array + i_13))) = *(&param_adr_weight_22_lut_array_tmp + i_13); // Fixed void pointer dereference
            i_13 += 4;
            *$t6_35 = $t7_70;
        }
    }
    
    tiziano_adr_params_init();
    system_irq_func_set(0x12, tiziano_adr_interrupt_static);
    tisp_event_set_cb(2, tisp_adr_process);
    return 0;
}

