#include "include/main.h"


  int32_t apical_isp_core_ops_s_ctrl(int32_t* arg1, int32_t* arg2, int32_t arg3)

{
    int32_t $a0 = *arg2;
    int32_t (* $v0_19)(int32_t* arg1);
    uint32_t* $a0_11;
    uint32_t var_b0;
    
    if ($a0 == 0x8000039)
    {
        private_copy_from_user(&var_b0_1, arg2[1], 1);
        $a0_11 = &var_b0_2;
        $v0_19 = tisp_set_defog_strength;
    }
    else
    {
        int32_t var_4d0;
        int32_t var_ac;
        int32_t var_a8_4;
        int32_t var_a4;
        int32_t var_a0;
        int32_t $v0_5;
        int32_t (* $v0_7)(char arg1);
        int32_t (* $v0_12)(int32_t arg1);
        void* $a0_1;
        uint32_t $a0_3;
        int32_t $a0_5;
        int32_t $a1_20;
        char const* const $a1_26;
        
        if ($a0 < 0x800003a)
        {
            if ($a0 == 0x800000d)
            {
                var_b0_3 = *arg2[1];
                tisp_s_wb_ct(&var_b0_4);
                return 0;
            }
            
            int32_t var_9c;
            
            if ($a0 >= 0x800000e)
            {
                if ($a0 == 0x800002b)
                    return apical_isp_gamma_s_attr.isra.34(arg2[1]);
                
                if ($a0 < 0x800002c)
                {
                    if ($a0 == 0x8000024)
                        return apical_isp_ae_s_roi.isra.36(&arg2[1]);
                    
                    if ($a0 >= 0x8000025)
                    {
                        if ($a0 == 0x8000028)
                        {
                            $a0_3 = arg2[1];
                            $v0_7 = tisp_s_max_again;
                            goto label_16ca0;
                        }
                        
                        if ($a0 < 0x8000029)
                        {
                            if ($a0 == 0x8000025)
                                return apical_isp_expr_s_ctrl.isra.35(*arg1, arg2[1]);
                            
                            return 0xffffffff;
                        }
                        
                        if ($a0 == 0x8000029)
                        {
                            $a0_3 = arg2[1];
                            $v0_7 = tisp_s_max_isp_dgain;
                            goto label_16ca0;
                        }
                        
                        if ($a0 != 0x800002a)
                            return 0xffffffff;
                        
                        tisp_s_Hilightdepress(arg2[1]);
                        return 0;
                    }
                    
                    if ($a0 == 0x800000f)
                    {
                        private_copy_from_user(&var_b0_5, arg2[1], 0x18);
                        uint32_t $a0_40 = var_b0_6;
                        var_4d0_1 = var_a0_6;
                        int32_t var_4cc_2 = var_9c_5;
                        tisp_s_awb_ct_trend($a0_40, var_ac_1, var_a8_5);
                        return 0;
                    }
                    
                    if ($a0 < 0x800000f)
                    {
                        private_copy_from_user(&var_b0_7, arg2[1], 0x28);
                        
                        for (int32_t i = 0; i < 0x18; i += 1)
                            *(&var_4d0_2 + i) = *(&var_a0_7 + i);
                        
                        tisp_s_awb_cluster(var_b0_8, var_ac_2, var_a8_6);
                        return 0;
                    }
                    
                    if ($a0 == 0x8000022)
                        return 0;
                    
                    if ($a0 == 0x8000023)
                    {
                        $a0_3 = arg2[1];
                        
                        if ($a0_3 < 0x100)
                        {
                            $v0_7 = tisp_set_ae_comp;
                            goto label_16ca0;
                        }
                        
                        $a1_26 = "qbuffer null\\n";
                        goto label_16c94;
                    }
                    
                    return 0xffffffff;
                }
                
                if ($a0 == 0x8000032)
                {
                    tisp_s_ae_it_max();
                    return 0;
                }
                
                if ($a0 >= 0x8000033)
                {
                    if ($a0 == 0x8000035)
                    {
                        private_copy_from_user(&var_b0_9, arg2[1], 0x98);
                        tisp_set_ae_attr(&var_b0_10);
                        return 0;
                    }
                    
                    if ($a0 < 0x8000036)
                    {
                        if ($a0 != 0x8000034)
                            return 0xffffffff;
                        
                        $a0_5 = arg2[1];
                        $v0_12 = tisp_set_ae_freeze;
                        goto label_16b78;
                    }
                    
                    if ($a0 == 0x8000037)
                    {
                        tisp_s_BacklightComp(arg2[1]);
                        return 0;
                    }
                    
                    if ($a0 != 0x8000038)
                        return 0xffffffff;
                    
                    private_copy_from_user(&var_b0_11, arg2[1], 0x28);
                    
                    for (int32_t i_1 = 0; i_1 < 0x18; i_1 += 1)
                        *(&var_4d0_3 + i_1) = *(&var_a0_8 + i_1);
                    
                    tisp_s_ae_at_list(var_b0_12);
                    return 0;
                }
                
                if ($a0 == 0x800002d)
                    return apical_isp_ae_zone_weight_s_attr.isra.47(&arg2[1]);
                
                if ($a0 < 0x800002d)
                    return 0;
                
                if ($a0 != 0x800002e)
                {
                    if ($a0 != 0x800002f)
                        return 0xffffffff;
                    
                    int32_t $a1_6 = arg2[1];
                    
                    if ($a1_6)
                    {
                        private_copy_from_user(&var_b0_13, $a1_6, 0x10);
                        tisp_s_ae_min(var_b0_14, var_ac_3, var_a8_7, var_a4_2);
                        return 0;
                    }
                    
                    return 0xffffffff;
                }
                
                uint32_t $v0_51;
                $v0_51 = private_kmalloc(0x42c, 0xd0);
                
                if (!$v0_51)
                {
                    $a1_26 = "Failed to allocate vic device\\n";
                    goto label_169ec;
                }
                
                private_copy_from_user(&var_b0_15, arg2[1], 0x10);
                *($v0_51 + 0x414) = var_b0_16;
                *($v0_51 + 0x418) = *var_b0_17[1];
                *($v0_51 + 0x41c) = *var_b0_18[2];
                *($v0_51 + 0x420) = *var_b0_19[3];
                
                for (int32_t i_2 = 0; i_2 < 0x41c; i_2 += 1)
                    *(&var_4d0_4 + i_2) = *($v0_51 + 0x10 + i_2);
                
                tisp_s_ae_hist();
                $a0_3 = $v0_51;
                $v0_7 = private_kfree;
                goto label_16ca0;
            }
            
            if ($a0 == 0x9a0914)
                return 0;
            
            if ($a0 >= 0x9a0915)
            {
                if ($a0 >= 0x8000008)
                {
                    if ($a0 == 0x800000a)
                    {
                        private_copy_from_user(&var_b0_20, arg2[1], 8);
                        tisp_s_awb_start(var_b0_21, var_ac_4);
                        return 0;
                    }
                    
                    if ($a0 == 0x800000c)
                    {
                        tisp_s_awb_algo(arg2[1]);
                        return 0;
                    }
                    
                    if ($a0 != 0x8000008)
                        return 0xffffffff;
                    
                    private_copy_from_user(&var_b0_22, arg2[1], 6);
                    $a0_3 = &var_b0_23;
                    $v0_7 = tisp_s_rgb_coefft;
                    goto label_16ca0;
                }
                
                if ($a0 >= 0x8000006)
                    return 0;
                
                if ($a0 < 0x8000003)
                {
                    if ($a0 >= 0x8000001)
                        return 0;
                    
                    $v0_5 = 0x9a091a;
                label_15f54:
                    
                    if ($a0 == $v0_5)
                        return 0;
                    
                    return 0xffffffff;
                }
                
                if ($a0 != 0x8000004)
                    return 0xffffffff;
                
                int32_t var_18_31;
                private_copy_from_user(&var_18_32, arg2[1], 8);
                int32_t $v0_44 = var_18_33;
                
                if ($v0_44 >= 0xa)
                {
                    isp_printf(1, "%s:%d::wdr mode\\n", "apical_isp_wb_s_ctrl");
                    return 0xffffffff;
                }
                
                uint32_t $a0_26;
                
                switch ($v0_44)
                {
                    case 0:
                    {
                        $a0_26 = 0;
                        break;
                    }
                    case 1:
                    {
                        $a0_26 = 1;
                        break;
                    }
                    case 2:
                    {
                        $a0_26 = 2;
                        break;
                    }
                    case 3:
                    {
                        $a0_26 = 3;
                        break;
                    }
                    case 4:
                    {
                        $a0_26 = 4;
                        break;
                    }
                    case 5:
                    {
                        $a0_26 = 5;
                        break;
                    }
                    case 6:
                    {
                        $a0_26 = 6;
                        break;
                    }
                    case 7:
                    {
                        $a0_26 = 7;
                        break;
                    }
                    case 8:
                    {
                        $a0_26 = 8;
                        break;
                    }
                    case 9:
                    {
                        $a0_26 = 9;
                        break;
                    }
                }
                
                int16_t var_14_7;
                uint32_t $a1_25 = var_14_8;
                var_4d0_5 = var_a0_9;
                int16_t var_12_2;
                uint32_t $a2_5 = var_12_3;
                int32_t var_98_6;
                int32_t var_4c8_1 = var_98_7;
                var_b0_24 = $a0_26;
                uint32_t var_ac_1_1 = $a1_25;
                uint32_t var_a8_1_1 = $a2_5;
                tisp_s_wb_attr($a0_26, $a1_25, $a2_5, var_a4_3, var_4d0_6, var_9c_6);
                return 0;
            }
            
            int32_t $v0_4;
            int32_t $a1_1;
            
            if ($a0 == 0x980914)
            {
                int32_t $a1_21 = arg2[1];
                $a0_1 = *(*arg1 + 0xd4);
                arg1[0x3ad] = $a1_21;
                $v0_4 = *($a0_1 + 0x168);
                *($a0_1 + 0x170) = $a1_21;
                $a1_1 = 0 < $a1_21 ? 1 : 0;
            label_1656c:
                
                if ($v0_4)
                    $v0_4 = 2;
                
                $a1_20 = $v0_4 | $a1_1;
            label_16580:
                apical_isp_hvflip_update($a0_1, $a1_20);
                return 0;
            }
            
            if ($a0 < 0x980915)
            {
                if ($a0 == 0x980901)
                {
                    if (*(*(*arg1 + 0xd4) + 0xf4) == 0x2011)
                        return 0;
                    
                    $a0_3 = arg2[1];
                    arg1[0x1023] = $a0_3;
                    $v0_7 = tisp_set_contrast;
                    goto label_16ca0;
                }
                
                if ($a0 == 0x980902)
                {
                    if (*(*(*arg1 + 0xd4) + 0xf4) == 0x2011)
                        return 0;
                    
                    $a0_3 = arg2[1];
                    arg1[0x1024] = $a0_3;
                    $v0_7 = tisp_set_saturation;
                    goto label_16ca0;
                }
                
                if ($a0 != 0x980900)
                    return 0xffffffff;
                
                if (*(*(*arg1 + 0xd4) + 0xf4) == 0x2011)
                    return 0;
                
                $a0_3 = arg2[1];
                arg1[0x1025] = $a0_3;
                $v0_7 = tisp_set_brightness;
                goto label_16ca0;
            }
            
            if ($a0 == 0x980918)
            {
                arg1[0x3bf] = arg2[1];
                int32_t $v0_30 = arg2[1];
                int32_t $a0_23 = 0xffffffea;
                
                if ($v0_30 < 3)
                    $a0_23 = *($v0_30 + &CSWTCH.84);
                
                int32_t $v0_32 = tisp_s_antiflick($a0_23);
                
                if (!$v0_32)
                    return $v0_32;
                
                isp_printf(1, "flags = 0x%08x, jzflags = %p,0x%08x", &__func__.34235);
                return $v0_32;
            }
            
            if ($a0 < 0x980919)
            {
                if ($a0 != 0x980915)
                    return 0xffffffff;
                
                $a0_1 = *(*arg1 + 0xd4);
                $v0_4 = arg2[1];
                arg1[0x3ac] = $v0_4;
                int32_t $a1 = *($a0_1 + 0x170);
                *($a0_1 + 0x168) = $v0_4;
                $a1_1 = 0 < $a1 ? 1 : 0;
                goto label_1656c;
            }
            
            $v0_5 = 0x98091f;
            
            if ($a0 != 0x98091b)
                goto label_15f54;
            
            if (*(*(*arg1 + 0xd4) + 0xf4) == 0x2011)
                return 0;
            
            $a0_3 = arg2[1];
            arg1[0xfdb] = $a0_3;
            $v0_7 = tisp_set_sharpness;
            goto label_16ca0;
        }
        
        if ($a0 == 0x80000e1)
        {
            int32_t $v0_43 = arg2[1];
            
            if ($v0_43 != arg1[0x1029])
            {
                void* $v1_36 = *(*arg1 + 0xd4);
                arg1[0x1029] = $v0_43;
                *($v1_36 + 0x178) = 1;
            }
            
            return 0;
        }
        
        int32_t $a0_32;
        
        if ($a0 >= 0x80000e2)
        {
            if ($a0 == 0x8000100)
            {
                private_copy_from_user(&var_b0_25, arg2[1], 0x28);
                tisp_s_ccm_attr(&var_b0_26);
                return 0;
            }
            
            if ($a0 >= 0x8000101)
            {
                if ($a0 == 0x8000161)
                    return 0;
                
                if ($a0 >= 0x8000162)
                {
                    if ($a0 == 0x8000164)
                    {
                        void* $v1_26 = *(*arg1 + 0xd4);
                        int32_t (* $a1_23)();
                        
                        if (arg2[1] != 1)
                        {
                            *($v1_26 + 0x15c) = 1;
                            $a1_23 = "%s[%d] do not support this interface\\n";
                        }
                        else
                        {
                            *($v1_26 + 0x15c) = 0;
                            $a1_23 = tisp_wdr_process;
                        }
                        
                        isp_printf(0, $a1_23, arg3);
                        return 0;
                    }
                    
                    if ($a0 < 0x8000165)
                    {
                        $v0_5 = 0x8000163;
                        goto label_15f54;
                    }
                    
                    if ($a0 < 0x800016a)
                        return 0;
                    
                    return 0xffffffff;
                }
                
                if ($a0 != 0x8000102)
                {
                    if ($a0 >= 0x8000102)
                    {
                        if ($a0 != 0x8000120)
                        {
                            if ($a0 == 0x8000140)
                                return 0;
                            
                            return 0xffffffff;
                        }
                        
                        return 0;
                    }
                    
                    $a0_3 = arg2[1];
                    
                    if ($a0_3 < 0x100)
                    {
                        $v0_7 = tisp_set_bcsh_hue;
                        goto label_16ca0;
                    }
                    
                    $a1_26 = "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\\n";
                label_16c94:
                    $a0_32 = 2;
                    goto label_169fc;
                }
                
                private_copy_from_user(&var_b0_27, arg2[1], 3);
                $a0_11 = &var_b0_28;
                $v0_19 = tisp_set_bcsh_fixed_contrast;
            }
            else
            {
                if ($a0 == 0x80000e6)
                {
                    $a0_3 = arg2[1];
                    $v0_7 = tisp_s_ev_start;
                label_16ca0:
                    $v0_7($a0_3);
                    return 0;
                }
                
                if ($a0 < 0x80000e7)
                {
                    if ($a0 == 0x80000e3)
                    {
                        private_copy_from_user(&var_b0_29, arg2[1], 0x14);
                        uint32_t $a0_16 = var_b0_30;
                        var_4d0_7 = var_a0_10;
                        tisp_s_fcrop_control($a0_16, var_ac_5, var_a8_8, var_a4_4, var_4d0_8);
                        return 0;
                    }
                    
                    if ($a0 < 0x80000e3)
                    {
                        private_copy_from_user(&var_b0_31, arg2[1], 4);
                        $a0_3 = var_b0_32;
                        $v0_7 = tisp_s_module_control;
                        goto label_16ca0;
                    }
                    
                    if ($a0 != 0x80000e4)
                    {
                        if ($a0 != 0x80000e5)
                            return 0xffffffff;
                        
                        return apical_isp_mask_s_attr.isra.29(arg2[1]);
                    }
                    
                    $a1_20 = arg2[1];
                    $a0_1 = *(*arg1 + 0xd4);
                    uint32_t $v0_26 = ($a1_20 & 2) >> 1;
                    arg1[0x3ac] = $v0_26;
                    *($a0_1 + 0x168) = $v0_26;
                    int32_t $v0_27 = $a1_20 & 1;
                    arg1[0x3ad] = $v0_27;
                    *($a0_1 + 0x170) = $v0_27;
                    goto label_16580;
                }
                
                if ($a0 == 0x80000e8)
                {
                    private_copy_from_user(&var_b0_33, arg2[1], 0x24);
                    
                    for (int32_t i_3 = 0; i_3 < 0x14; i_3 += 1)
                        *(&var_4d0_9 + i_3) = *(&var_a0_11 + i_3);
                    
                    tisp_s_autozoom_control(var_b0_34, var_ac_6, var_a8_9, var_a4_5);
                    return 0;
                }
                
                if ($a0 < 0x80000e8)
                {
                    int32_t $s1_1 = arg2[1];
                    void* $s0_3 = *(*arg1 + 0xd4);
                    int32_t $v0_65;
                    $v0_65 = tiziano_ae_dump();
                    
                    if ($v0_65)
                    {
                        $a1_26 = "The parameter is invalid!\\n";
                        goto label_16c94;
                    }
                    
                    if ($s1_1 != 1)
                    {
                        if ($s1_1)
                            return 0;
                        
                        int32_t $v0_68 = 0xfffffffd & arg1[0x1029];
                        
                        if ($v0_68)
                        {
                            if ($v0_68 == 1)
                                *($s0_3 + 0x178) = 3;
                        }
                        else
                            *($s0_3 + 0x178) = 2;
                    }
                    else
                    {
                        int32_t $v0_66 = 0xfffffffd & arg1[0x1029];
                        int32_t $v0_67;
                        
                        if ($v0_66)
                        {
                            if ($v0_66 != $s1_1)
                                return 0;
                            
                            *($s0_3 + 0x178) = 2;
                            $v0_67 = 3;
                        }
                        else
                        {
                            $v0_67 = 2;
                            *($s0_3 + 0x178) = 2;
                        }
                        
                        arg1[0x1029] = $v0_67;
                    }
                    
                    return 0;
                }
                
                if ($a0 == 0x80000e9)
                {
                    private_copy_from_user(&var_b0_35, arg2[1], 0xc);
                    tisp_s_scaler_level_control(var_b0_36, var_ac_7, var_a8_10);
                    return 0;
                }
                
                if ($a0 != 0x80000ea)
                    return 0xffffffff;
                
                private_copy_from_user(&var_b0_37, arg2[1], 4);
                $a0_11 = &var_b0_38;
                $v0_19 = tisp_set_wdr_output_mode;
            }
        }
        else
        {
            int32_t (* $v0_58)(int32_t arg1);
            int32_t $a0_9;
            
            if ($a0 == 0x8000086)
            {
                $a0_9 = arg2[1];
                
                if ($a0_9 >= 0x100)
                {
                label_16ac8:
                    $a1_26 = "Failed to init isp module(%d.%d)\\n";
                label_169ec:
                    $a0_32 = 1;
                label_169fc:
                    isp_printf($a0_32, $a1_26, arg3);
                    return 0xffffffff;
                }
                
                $v0_58 = tisp_s_2dns_ratio;
            label_16ae8:
                int32_t $v0_60;
                int32_t $a2_9;
                $v0_60 = $v0_58($a0_9);
                
                if ($v0_60 < 0)
                    isp_printf(1, "&vsd->mlock", $a2_9);
                
                return $v0_60;
            }
            
            int32_t $v1_14;
            
            if ($a0 < 0x8000087)
            {
                if ($a0 == 0x8000062)
                {
                    tisp_s_dpc_strength(arg2[1]);
                    return 0;
                }
                
                if ($a0 < 0x8000063)
                {
                    if ($a0 == 0x8000044)
                        return apical_isp_af_weight_s_attr.isra.51(&arg2[1]);
                    
                    if ($a0 >= 0x8000045)
                    {
                        $v1_14 = 0x8000060;
                        goto label_16220;
                    }
                    
                    if ($a0 != 0x8000042)
                        return 0xffffffff;
                    
                    return apical_isp_af_hist_s_attr.isra.50(arg2[1]);
                }
                
                if ($a0 < 0x8000084)
                {
                    if ($a0 >= 0x8000081)
                        return 0;
                    
                    if ($a0 == 0x8000080)
                    {
                        private_copy_from_user(&arg1[0xfe0], arg2[1], 0x106);
                        return 0;
                    }
                    
                    return 0xffffffff;
                }
                
                if ($a0 != 0x8000085)
                    return 0xffffffff;
                
                $a0_9 = arg2[1];
                
                if ($a0_9 >= 0x100)
                    goto label_16ac8;
                
                $v0_58 = tisp_s_3dns_ratio;
                goto label_16ae8;
            }
            
            if ($a0 == 0x80000a4)
            {
                $a0_5 = arg2[1];
                $v0_12 = tisp_s_defog_enable;
            label_16b78:
                $v0_12($a0_5);
                return 0;
            }
            
            if ($a0 < 0x80000a5)
            {
                if ($a0 == 0x80000a2)
                {
                    tisp_s_drc_strength(arg2[1]);
                    return 0;
                }
                
                if ($a0 >= 0x80000a3)
                {
                    $a0_5 = arg2[1];
                    $v0_12 = tisp_s_adr_enable;
                    goto label_16b78;
                }
                
                $v1_14 = 0x80000a0;
            label_16220:
                
                if ($a0 >= $v1_14)
                    return 0;
                
                return 0xffffffff;
            }
            
            if ($a0 >= 0x80000c3)
            {
                if ($a0 != 0x80000e0)
                    return 0xffffffff;
                
                void* $a0_12 = *arg1;
                uint32_t $v0_40 = arg2[1];
                void* $v1_32 = *($a0_12 + 0xd4);
                var_b0_39 = $v0_40;
                
                if ($v0_40 == *($v1_32 + 0x12c))
                    return 0;
                
                int32_t $s0_1;
                
                if (!$a0_12)
                    $s0_1 = 0xfffffffe;
                else
                {
                    int32_t $v0_41 = *($a0_12 + 0x7c);
                    
                    if (!$v0_41)
                        $s0_1 = 0xfffffdfd;
                    else
                    {
                        int32_t $v0_42 = $v0_41($a0_12, 0x200000a, &var_b0_40);
                        $s0_1 = $v0_42;
                        
                        if (!$v0_42)
                        {
                            uint32_t $a0_24 = var_b0_41;
                            arg1[0x1028] = $a0_24;
                            tisp_set_fps($a0_24);
                            return $s0_1;
                        }
                    }
                }
                
                isp_printf(1, "%s:%d::linear mode\\n", var_b0_42);
                return $s0_1;
            }
            
            if ($a0 >= 0x80000c0)
                return 0;
            
            if ($a0 != 0x80000a6)
                return 0xffffffff;
            
            private_copy_from_user(&var_b0_43, arg2[1], 0x40);
            $a0_11 = &var_b0_44;
            $v0_19 = tisp_set_csc_attr;
        }
    }
    return $v0_19($a0_11);
}

