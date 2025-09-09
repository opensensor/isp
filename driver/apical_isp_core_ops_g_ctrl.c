#include "include/main.h"


  int32_t apical_isp_core_ops_g_ctrl(int32_t* arg1, int32_t* arg2)

{
    int32_t $a1 = *arg2;
    uint32_t var_98;
    int32_t result;
        return result;
    
    if ($(uintptr_t)a1 == 0x8000032)
    {
        tisp_g_ae_it_max(&var_98);
        result = 0;
    label_1856c:
        arg2[1] = var_98;
    }
    
    char const* const $a2_4;
    int16_t var_88;
    int32_t var_18_15;
    uint8_t (* $v0_8)(uint8_t* arg1);
    int32_t* $a0_2;
    int32_t $a2_3;
    int32_t $a2_6;
    
    if ($(uintptr_t)a1 >= 0x8000033)
    {
            goto label_1856c;
        if ($(uintptr_t)a1 == 0x80000a2)
        {
            tisp_g_drc_strength(&var_98);
            result = 0;
        }
        
        int32_t $v0_11;
        int32_t (* $v0_26)(int32_t arg1);
        uint32_t* $a0_12;
        int32_t $a2_5;
        
        if ($(uintptr_t)a1 >= 0x80000a3)
        {
                    return 0;
            if ($(uintptr_t)a1 == 0x80000e5)
                return apical_isp_mask_g_attr.isra.68(&arg2[1]);
            
            if ($(uintptr_t)a1 < 0x80000e6)
            {
                if ($(uintptr_t)a1 == 0x80000e0)
                {
                    arg2[1] = *(*(*arg1 + 0xd4) + 0x12c);
                }
                
                if ($(uintptr_t)a1 >= 0x80000e1)
                {
                        goto label_180c0;
                    if ($(uintptr_t)a1 == 0x80000e2)
                    {
                        tisp_g_module_control(&var_98);
                        $a2_3 = 4;
                    }
                    
                    if ($(uintptr_t)a1 < 0x80000e2)
                    {
                        return 0;
                        arg2[1] = arg1[0x1029] & 1;
                    }
                    
                    if ($(uintptr_t)a1 == 0x80000e3)
                    {
                        goto label_180c0;
                        tisp_g_fcrop_control(&var_98);
                        $a2_3 = 0x14;
                    }
                    
                    if ($(uintptr_t)a1 != 0x80000e4)
                        return 0xffffffff;
                    
                    int32_t* $a0_3 = (int32_t*)((char*)*arg1  + 0xd4); // Fixed void pointer arithmetic
                    int32_t $v0_17 = *($a0_3 + 0x168);
                    
                    if ($v0_17)
                        $v0_17 = 2;
                    
                    arg2[1] = $v0_17 | (0 < *($a0_3 + 0x170) ? 1 : 0);
                    return 0;
                }
                
                if ($(uintptr_t)a1 == 0x80000a6)
                    return tiziano_isp_csc_g_attr.isra.108(&arg2[1]);
                
                if ($(uintptr_t)a1 >= 0x80000a7)
                {
                        return 0;
                    return 0xffffffff;
                    if ($a1 - 0x80000c0 < 3)
                    
                }
                
                if ($(uintptr_t)a1 != 0x80000a5)
                    return 0xffffffff;
                
                $a0_12 = &var_98_6;
                $v0_26 = tisp_get_blc_attr;
                goto label_18818;
            }
            
            if ($(uintptr_t)a1 == 0x8000120)
                return 0;
            
            if ($(uintptr_t)a1 >= 0x8000121)
            {
                            return 0;
                if ($(uintptr_t)a1 != 0x8000162)
                {
                    if ($(uintptr_t)a1 >= 0x8000163)
                    {
                        if ($(uintptr_t)a1 == 0x8000167)
                        {
                            arg2[1] = arg1[0x3b1];
                        }
                        
                        if ($(uintptr_t)a1 == 0x8000169)
                            return 0;
                        
                        return 0xffffffff;
                    }
                    
                    $v0_11 = $(uintptr_t)a1 < 0x8000160 ? 1 : 0;
                    goto label_18064;
                }
                
                int32_t $s1_1 = arg2[1];
                var_18_16 = 0;
                int32_t var_14_1_1 = 0;
                void* entry_gp;
                
                if (!((($s1_1 + 0x18) | $s1_1) & *(entry_gp_1 + 0x18)))
                {
                    __might_sleep("nv12", 0xc9, 0);
                    __copy_user(&var_98, $s1_1, 0x18);
                }
                
                result = isp_frame_done_wait(var_98_7, &var_18_17);
                int32_t $s0_1 = arg2[1];
                int32_t var_90_2 = var_18_18;
                int32_t var_8c_2 = var_14_1_2;
                
                if (!((($s0_1 + 0x18) | $s0_1) & *(entry_gp_2 + 0x18)))
                {
                    __might_sleep("nv12", 0xc9, 0);
                    __copy_user($s0_1, &var_98, 0x18);
                }
                
                return result;
            }
            
            if ($(uintptr_t)a1 == 0x80000ea)
            {
                return result;
                tisp_get_wdr_output_mode(&var_98);
                result = 0;
                $a2_5 = 4;
            label_18864:
                private_copy_to_user(arg2[1], &var_98, $a2_5);
            }
            
            if ($(uintptr_t)a1 >= 0x80000eb)
            {
                    goto label_180c0;
                if ($(uintptr_t)a1 == 0x8000100)
                {
                    tisp_g_ccm_attr(&var_98);
                    $a2_3 = 0x28;
                }
                
                if ($(uintptr_t)a1 != 0x8000101)
                    return 0xffffffff;
                
                $a0_2 = &var_98_8;
                $v0_8 = tisp_get_bcsh_hue;
                goto label_186f0;
            }
            
            if ($(uintptr_t)a1 != 0x80000e7)
                return 0xffffffff;
            
            uint32_t $v0_57;
            $v0_57 = tisp_cust_mode_g_ctrl();
            
            if ($v0_57 >= 0)
            {
                return 0;
                arg2[1] = $v0_57;
            }
            

            return 0xffffffff;
        }
        
        int32_t $a0_6;
        int32_t* $a1_1;
        
        if ($(uintptr_t)a1 == 0x8000043)
        {
            return 0;
            tisp_g_af_metric(&var_98);
            $a2_3 = 4;
        label_180c0:
            $a0_6 = arg2[1];
            $a1_1 = &var_98;
        label_180d4:
            private_copy_to_user($a0_6, $a1_1, $a2_3);
        }
        
        if ($(uintptr_t)a1 >= 0x8000044)
        {
                    goto label_180c0;
            if ($(uintptr_t)a1 >= 0x8000084)
            {
                if ($(uintptr_t)a1 == 0x8000084)
                {
                    $a0_12 = &var_98;
                    $v0_26 = tisp_g_ncuinfo;
                label_18818:
                    $v0_26($a0_12);
                    $a2_3 = 0x14;
                }
                
                $v0_11 = $(uintptr_t)a1 < 0x80000a0 ? 1 : 0;
            label_18064:
                
                if (!$v0_11)
                    return 0;
                
                return 0xffffffff;
            }
            
            if ($(uintptr_t)a1 >= 0x8000082)
                return 0;
            
            if ($(uintptr_t)a1 == 0x8000045)
            {
                int32_t $a3_1 = arg2[1];
                int32_t* $v0_21 = (int32_t*)((char*)*arg1  + 0xd4); // Fixed void pointer arithmetic
                int32_t* $v1_25 = (int32_t*)((char*)$v0_21  + 0x120); // Fixed void pointer arithmetic
                uint32_t var_94_1 = *($v1_25 + 0xb2);
                int32_t var_90_1 = *($v0_21 + 0x12c);
                int32_t var_8c_1 = *($v0_21 + 0x124);
                goto label_180d4;
                $a2_3 = 0x14;
                $a1_1 = &var_98;
                var_98 = *($v1_25 + 0xb0);
                $a0_6 = $a3_1;
                var_88 = *($v0_21 + 0x128);
            }
            
            if ($(uintptr_t)a1 < 0x8000045)
                return apical_isp_af_weight_g_attr.isra.96(&arg2[1]);
            
            if ($(uintptr_t)a1 == 0x8000046)
                return apical_isp_af_zone_g_ctrl.isra.85(&arg2[1]);
            
            if ($(uintptr_t)a1 != 0x8000062)
                return 0xffffffff;
            
            tisp_g_dpc_strength(&var_98_9);
            result = 0;
            goto label_1856c;
        }
        
        if ($(uintptr_t)a1 == 0x8000038)
        {
            return 0;
            tisp_g_ae_at_list(&var_98);
            private_copy_to_user(arg2[1], &var_98, 0x28);
        }
        
        if ($(uintptr_t)a1 >= 0x8000039)
        {
                return 0;
                return 0xffffffff;
            goto label_18864;
            if ($(uintptr_t)a1 >= 0x8000042)
                return apical_isp_af_hist_g_attr.isra.95(&arg2[1]);
            
            if ($(uintptr_t)a1 >= 0x8000040)
            
            if ($(uintptr_t)a1 != 0x8000039)
            
            tisp_get_defog_strength(&var_98);
            result = 0;
            $a2_5 = 1;
        }
        
        if ($(uintptr_t)a1 == 0x8000035)
            return tiziano_isp_ae_manual_attr_g_ctrl.isra.103(&arg2[1]);
        
        if ($(uintptr_t)a1 < 0x8000036)
        {
                return 0xffffffff;
            goto label_186f0;
            if ($(uintptr_t)a1 != 0x8000033)
            
            $a0_2 = &var_98;
            $v0_8 = tisp_get_ae_luma;
        }
        
        if ($(uintptr_t)a1 == 0x8000036)
        {
            goto label_180c0;
            tisp_get_ae_state(&var_98);
            $a2_3 = 0xc;
        }
        
        if ($(uintptr_t)a1 != 0x8000037)
            return 0xffffffff;
        
        int32_t result_2 = tisp_g_BacklightComp(&var_98_10);
        result = result_2;
        
        if (!result_2)
            goto label_1856c;
        
        $a2_4 = "tiziano_isp_backlight_comp_g_ctrl";
    }
    else
    {
        int16_t var_94;
        int16_t var_90;
        int32_t var_8c;
                int32_t $v0_35;
                uint32_t $v1_31;
                        int32_t $a2_1;
        if ($(uintptr_t)a1 == 0x800000b)
            return apical_isp_awb_zone_statis_g_attr.isra.94(&arg2[1]);
        
        
        if ($(uintptr_t)a1 < 0x800000c)
        {
            if ($(uintptr_t)a1 >= 0x8000002)
            {
                
                if ($(uintptr_t)a1 >= 0x8000008)
                {
                    if ($(uintptr_t)a1 != 0x8000009)
                    {
                        
                        if ($(uintptr_t)a1 >= 0x800000a)
                        {
                            tisp_g_awb_start(&var_98);
                            $a2_1 = 8;
                        }
                        else
                        {
                            tisp_g_rgb_coefft(&var_98);
                            $a2_1 = 6;
                        }
                        
                        private_copy_to_user(arg2[1], &var_98_11, $a2_1);
                        return 0;
                    }
                    
                    tisp_g_wb_attr(&var_98_12);
                    $v1_31 = var_88_1;
                    $v0_35 = var_8c << 0x10;
                }
                else
                {
                        return 0;
                        uint32_t $v0_28 = var_98;
                            return 0xffffffff;
                    if ($(uintptr_t)a1 >= 0x8000006)
                    
                    if ($(uintptr_t)a1 == 0x8000004)
                    {
                        tisp_g_wb_attr(&var_98);
                        
                        if ($(uintptr_t)v0_28 >= 0xa)
                        {

                        }
                        
                        switch ($v0_28)
                        {
                            case 0:
                            {
                                var_18 = 0;
                                break;
                            }
                            case 1:
                            {
                                var_18 = 1;
                                break;
                            }
                            case 2:
                            {
                                var_18 = 2;
                                break;
                            }
                            case 3:
                            {
                                var_18 = 3;
                                break;
                            }
                            case 4:
                            {
                                var_18 = 4;
                                break;
                            }
                            case 5:
                            {
                                var_18 = 5;
                                break;
                            }
                            case 6:
                            {
                                var_18 = 6;
                                break;
                            }
                            case 7:
                            {
                                var_18 = 7;
                                break;
                            }
                            case 8:
                            {
                                var_18 = 8;
                                break;
                            }
                            case 9:
                            {
                                var_18 = 9;
                                break;
                            }
                        }
                        
                        int32_t var_14_6;
                        var_14_7 = var_94_2;
                        *var_14_8[2] = var_90_1;
                        private_copy_to_user(arg2[1], &var_18_19, 8);
                        return 0;
                    }
                    
                    if ($(uintptr_t)a1 < 0x8000005)
                    {
                            return 0;
                        return 0xffffffff;
                        if ($(uintptr_t)a1 == 0x8000003)
                        
                    }
                    
                    tisp_g_wb_attr(&var_98_13);
                    int16_t var_80;
                    $v1_31 = var_80_1;
                    int32_t var_84;
                    $v0_35 = var_84_1 << 0x10;
                }
                
                arg2[1] = $v0_35 + $v1_31;
                return 0;
            }
            
            if ($(uintptr_t)a1 >= 0x8000000)
                return 0;
            
            if ($(uintptr_t)a1 == 0x980915)
            {
                return 0;
                arg2[1] = arg1[0x3ac];
            }
            
            if ($(uintptr_t)a1 >= 0x980916)
            {
                    return 0;
                if ($(uintptr_t)a1 == 0x98091b)
                {
                    arg2[1] = arg1[0xfdb];
                }
                
                if ($(uintptr_t)a1 < 0x98091c)
                {
                        return 0xffffffff;
                    return 0;
                    if ($(uintptr_t)a1 != 0x980918)
                    
                    arg2[1] = arg1[0x3bf];
                }
                
                if ($(uintptr_t)a1 == 0x98091f)
                {
                    return 0;
                    arg2[1] = arg1[0x1027];
                }
                
                if ($(uintptr_t)a1 != 0x9a091a)
                    return 0xffffffff;
                
                arg2[1] = arg1[0x1026];
                return 0;
            }
            
            if ($(uintptr_t)a1 == 0x980901)
            {
                return 0;
                arg2[1] = arg1[0x1023];
            }
            
            if ($(uintptr_t)a1 < 0x980902)
            {
                    return 0xffffffff;
                return 0;
                if ($(uintptr_t)a1 != 0x980900)
                
                arg2[1] = arg1[0x1025];
            }
            
            if ($(uintptr_t)a1 == 0x980902)
            {
                return 0;
                arg2[1] = arg1[0x1024];
            }
            
            if ($(uintptr_t)a1 != 0x980914)
                return 0xffffffff;
            
            arg2[1] = arg1[0x3ad];
            return 0;
        }
        
        if ($(uintptr_t)a1 == 0x8000027)
        {
            uint32_t var_7c;
            return 0;
            tisp_g_ev_attr(&var_98);
            arg2[1] = var_7c;
        }
        
        if ($(uintptr_t)a1 < 0x8000028)
        {
                    return 0;
                    return 0;
            if ($(uintptr_t)a1 < 0x8000023)
            {
                if ($(uintptr_t)a1 >= 0x8000020)
                
                if ($(uintptr_t)a1 == 0x800000e)
                {
                    tisp_g_awb_cluster(&var_98);
                    private_copy_to_user(arg2[1], &var_98, 0x28);
                }
                
                if ($(uintptr_t)a1 == 0x800000f)
                {
                    return 0;
                    tisp_g_awb_ct_trend(&var_98);
                    private_copy_to_user(arg2[1], &var_98, 0x18);
                }
                
                if ($(uintptr_t)a1 != 0x800000d)
                    return 0xffffffff;
                
                tisp_g_wb_ct(&var_98_14);
                $a2_3 = 4;
                goto label_180c0;
            }
            
            if ($(uintptr_t)a1 == 0x8000024)
                return apical_isp_ae_g_roi.isra.77(&arg2[1]);
            
            if ($(uintptr_t)a1 < 0x8000024)
            {
                return 0;
                $a0_2 = &var_98;
                $v0_8 = tisp_get_ae_comp;
            label_186f0:
                $v0_8($a0_2);
                arg2[1] = var_98;
            }
            
            if ($(uintptr_t)a1 == 0x8000025)
                return apical_isp_expr_g_ctrl.isra.72(&arg2[1]);
            
            if ($(uintptr_t)a1 != 0x8000026)
                return 0xffffffff;
            
            return apical_isp_ev_g_attr.isra.75(&arg2[1]);
        }
        
        if ($(uintptr_t)a1 == 0x800002c)
            return 0;
        
        if ($(uintptr_t)a1 >= 0x800002d)
        {
                return 0;
            if ($(uintptr_t)a1 == 0x800002f)
            {
                tisp_g_ae_min(&var_98);
                private_copy_to_user(arg2[1], &var_98, 0x10);
            }
            
            if ($(uintptr_t)a1 >= 0x8000030)
            {
                    return 0xffffffff;
                if ($(uintptr_t)a1 == 0x8000030)
                    return apical_isp_ae_zone_g_ctrl.isra.84(&arg2[1]);
                
                if ($(uintptr_t)a1 != 0x8000031)
                
                return apical_isp_ae_hist_origin_g_attr.isra.92(&arg2[1]);
            }
            
            if ($(uintptr_t)a1 == 0x800002d)
                return apical_isp_ae_zone_weight_g_attr.isra.89(&arg2[1]);
            
            if ($(uintptr_t)a1 != 0x800002e)
                return 0xffffffff;
            
            void* $v0_45;
            $v0_45 = private_kmalloc(0x42c, 0xd0);
            
            if (!$v0_45)
            {
                return 0xffffffff;

            }
            
            tisp_g_ae_hist($v0_45);
            var_98_15 = *($v0_45 + 0x414);
            *var_98_16[1] = *($v0_45 + 0x418);
            *var_98_17[2] = *($v0_45 + 0x41c);
            *var_98_18[3] = *($v0_45 + 0x420);
            var_94_3 = *($v0_45 + 0x400);
            var_94_4 = *($v0_45 + 0x404);
            var_90_3 = *($v0_45 + 0x408);
            var_90_4 = *($v0_45 + 0x40c);
            var_8c_1 = *($v0_45 + 0x410);
            *var_8c_3[2] = *($v0_45 + 0x424);
            *var_8c_4[3] = *($v0_45 + 0x428);
            private_copy_to_user(arg2[1], &var_98_19, 0x10);
            private_kfree($v0_45);
            return 0;
        }
        
        if ($(uintptr_t)a1 == 0x8000029)
            return apical_isp_max_dgain_g_ctrl.isra.74(&arg2[1]);
        
        if ($(uintptr_t)a1 < 0x8000029)
            return apical_isp_max_again_g_ctrl.isra.73(&arg2[1]);
        
        if ($(uintptr_t)a1 != 0x800002a)
        {
                return 0xffffffff;
            if ($(uintptr_t)a1 != 0x800002b)
            
            return apical_isp_gamma_g_attr.isra.76(&arg2[1]);
        }
        
        int32_t result_1 = tisp_g_Hilightdepress(&var_98_20);
        result = result_1;
        
        if (!result_1)
            goto label_1856c;
        
        $a2_4 = "apical_isp_hi_light_depress_g_ctrl";
    }

    return result;
}

