#include "include/main.h"


  int32_t tx_isp_unlocked_ioctl(void* arg1, int32_t arg2, int32_t arg3)

{
    void* $s7 = *(arg1 + 0x70);
    int32_t $s6_1;
    uint32_t var_98_64;
    int32_t var_94_10;
    
    if (arg2 == 0x800856d7)
    {
        void* $v1_22 = *(*($s7 + 0x2c) + 0xd4);
        var_98_65 = 0;
        var_94_11 = 0;
        void* $v0_96 = *($v1_22 + 0x120);
        int32_t $a0_41 = *($v0_96 + 0x90);
        
        if ($a0_41 == 1)
            var_94_12 = (*($v1_22 + 0x124) * *($v1_22 + 0x128)) << 1;
        else if ($a0_41 != 2)
            isp_printf(1, "%s:%d::wdr mode\\n", arg3);
        else
            var_94_13 = *($v0_96 + 0xe8);
        
        int32_t var_a8_3_1 = var_94_14;
        isp_printf(0, "qbuffer null\\n", "tx_isp_wdr_get_buf");
        $s6_1 = 0;
        
        if (private_copy_to_user(arg3, &var_98_66, 8))
        {
            isp_printf(2, "sensor type is BT601!\\n", "tx_isp_wdr_get_buf");
            return 0xfffffff2;
        }
    }
    else
    {
        int32_t var_90_17;
        char const* const $a1_3;
        char const* const $a2_1;
        
        if (arg2 >= 0x800856d8)
        {
            if (arg2 == 0xc00456e2)
            {
                if (private_copy_from_user(&var_98_67, arg3, 0x18))
                {
                    $a2_1 = "tx_isp_set_awb_algo_handle";
                label_1f3d0:
                    $a1_3 = "bank no free\\n";
                label_1df8c:
                    isp_printf(2, $a1_3, $a2_1);
                    return 0xfffffff2;
                }
                
                $s6_1 = 0;
                
                if (var_90_18 == 1)
                    tisp_awb_algo_handle(&var_98_68);
            }
            else
            {
                int32_t var_70_11;
                int32_t var_6c_7;
                int32_t* var_48_15;
                
                if (arg2 >= 0xc00456e3)
                {
                    if (arg2 == 0xc00456e7)
                    {
                        for (int32_t i = 0; i != 3; )
                        {
                            tisp_get_frame_drop(i, &(&var_98_69)[i * 3]);
                            i += 1;
                            $s6_1 = 0;
                        }
                        
                        if (private_copy_to_user(arg3, &var_98_70, 0x24))
                        {
                            $a2_1 = "tx_isp_get_frame_drop";
                            $a1_3 = "sensor type is BT656!\\n";
                            goto label_1df8c;
                        }
                    }
                    else if (arg2 >= 0xc00456e8)
                    {
                        int32_t $v0_103;
                        void* $s0_12;
                        
                        if (arg2 == 0xc00456e9)
                        {
                            $s0_12 = *($s7 + 0x2c);
                            
                            if (private_copy_from_user(&var_98_71, arg3, 0x2a))
                            {
                                $a2_1 = "tx_isp_set_gpio_sta";
                                $a1_3 = "sensor type is BT601!\\n";
                                goto label_1df8c;
                            }
                            
                            $s6_1 = 0xfffffffe;
                            
                            if ($s0_12)
                            {
                                $v0_103 = *($s0_12 + 0x7c);
                                $s6_1 = 0xfffffdfd;
                                
                                if ($v0_103)
                                    return $v0_103($s0_12, 0x2000018, &var_98_72);
                            }
                        }
                        else if (arg2 < 0xc00456e9)
                        {
                            $s0_12 = *($s7 + 0x2c);
                            
                            if (private_copy_from_user(&var_98_73, arg3, 0x2a))
                            {
                                $a2_1 = "tx_isp_set_gpio_init";
                                $a1_3 = "sensor type is BT601!\\n";
                                goto label_1df8c;
                            }
                            
                            $s6_1 = 0xfffffffe;
                            
                            if ($s0_12)
                            {
                                $v0_103 = *($s0_12 + 0x7c);
                                $s6_1 = 0xfffffdfd;
                                
                                if ($v0_103)
                                    return $v0_103($s0_12, 0x2000017, &var_98_74);
                            }
                        }
                        else if (arg2 == 0xc0385650)
                        {
                            int32_t $v0_57 = private_copy_from_user(&var_98_75, arg3, 0x38);
                            int32_t $v0_65;
                            
                            if (!$v0_57)
                            {
                                var_48_16 = &var_94_15;
                                int32_t var_40_2_2 = var_70_12;
                                int32_t var_3c_2_1 = var_6c_8;
                                void* $s0_7 = $s7 + 0x2c;
                                void* $a0_23 = *$s0_7;
                                
                                while (true)
                                {
                                    if ($a0_23)
                                    {
                                        void* $v0_61 = *(*($a0_23 + 0xc4) + 0xc);
                                        
                                        if (!$v0_61)
                                            $s0_7 += 4;
                                        else
                                        {
                                            int32_t $v0_62 = *($v0_61 + 8);
                                            
                                            if (!$v0_62)
                                                $s0_7 += 4;
                                            else
                                            {
                                                int32_t $v0_63 = $v0_62();
                                                
                                                if (!$v0_63)
                                                    $s0_7 += 4;
                                                else
                                                {
                                                    $s0_7 += 4;
                                                    
                                                    if ($v0_63 != 0xfffffdfd)
                                                        return $v0_63;
                                                }
                                            }
                                        }
                                    }
                                    else
                                        $s0_7 += 4;
                                    
                                    if ($s0_7 == $s7 + 0x6c)
                                        break;
                                    
                                    $a0_23 = *$s0_7;
                                }
                                
                                int32_t var_38_18;
                                int32_t var_68_1_2 = var_38_19;
                                int32_t var_44_13;
                                int32_t var_74_1_1 = var_44_14;
                                int32_t var_34_13;
                                int32_t var_64_1_2 = var_34_14;
                                $v0_65 = private_copy_to_user(arg3, &var_98_76, 0x38);
                                $s6_1 = 0;
                            }
                            
                            if ($v0_57 || $v0_65)
                            {
                                $a2_1 = "tx_isp_sensor_g_register";
                                $a1_3 = "sensor type is BT601!\\n";
                                goto label_1df8c;
                            }
                        }
                        else
                        {
                            if (arg2 != 0xc050561a)
                                return 0;
                            
                            void* $s0_3 = $s7 + 0x2c;
                            
                            if (private_copy_from_user(&var_98_77, arg3, 0x50))
                            {
                                $a2_1 = "tx_isp_sensor_enum_input";
                                $a1_3 = "sensor type is BT601!\\n";
                                goto label_1df8c;
                            }
                            
                            void* $a0_2 = *$s0_3;
                            
                            while (true)
                            {
                                if ($a0_2)
                                {
                                    void* $v0_6 = *(*($a0_2 + 0xc4) + 0xc);
                                    
                                    if (!$v0_6)
                                        $s0_3 += 4;
                                    else
                                    {
                                        int32_t $v0_7 = *($v0_6 + 8);
                                        
                                        if (!$v0_7)
                                            $s0_3 += 4;
                                        else
                                        {
                                            int32_t $v0_8 = $v0_7();
                                            
                                            if (!$v0_8)
                                                $s0_3 += 4;
                                            else
                                            {
                                                $s0_3 += 4;
                                                
                                                if ($v0_8 != 0xfffffdfd)
                                                    return $v0_8;
                                            }
                                        }
                                    }
                                }
                                else
                                    $s0_3 += 4;
                                
                                if ($s7 + 0x6c == $s0_3)
                                    break;
                                
                                $a0_2 = *$s0_3;
                            }
                            
                            $s6_1 = 0;
                            
                            if (private_copy_to_user(arg3, &var_98_78, 0x50))
                            {
                                $a2_1 = "tx_isp_sensor_enum_input";
                                $a1_3 = "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\\n";
                                goto label_1df8c;
                            }
                        }
                    }
                    else if (arg2 == 0xc00456e4)
                    {
                        if (private_copy_from_user(&var_98_79, arg3, 8))
                        {
                            $a2_1 = "tx_isp_set_awb_algo_close";
                            goto label_1f3d0;
                        }
                        
                        tisp_awb_algo_deinit();
                        tisp_awb_algo_init(0);
                        $s6_1 = 0;
                        private_kfree(awb_info_mine);
                    }
                    else
                    {
                        if (arg2 < 0xc00456e4)
                        {
                            tisp_awb_algo_init(1);
                            awb_info_mine = kmem_cache_alloc(0, 0xd0);
                            awb_algo_comp = 0;
                            __init_waitqueue_head(0xb2cb8, "Failed to init isp module(%d.%d)\\n", 0);
                            return 0;
                        }
                        
                        if (arg2 != 0xc00456e6)
                            return 0;
                        
                        int32_t $v0_147;
                        int32_t $a2_18;
                        $v0_147 = private_copy_from_user(&var_98_80, arg3, 0x24);
                        int32_t i_1 = 0;
                        
                        if ($v0_147)
                        {
                            $a2_1 = "tx_isp_set_frame_drop";
                            goto label_1f3d0;
                        }
                        
                        do
                        {
                            int32_t $v0_148;
                            $v0_148 = tisp_set_frame_drop(i_1, &(&var_98_81)[i_1 * 3], $a2_18);
                            i_1 += 1;
                            $s6_1 = $v0_148;
                        } while (i_1 != 3);
                    }
                }
                else if (arg2 == 0xc0045627)
                {
                    void* $s0_4 = $s7 + 0x2c;
                    
                    if (private_copy_from_user(&var_98_82, arg3, 4))
                    {
                        $a2_1 = "tx_isp_sensor_set_input";
                        $a1_3 = "sensor type is BT601!\\n";
                        goto label_1df8c;
                    }
                    
                    void* $a0_7 = *$s0_4;
                    
                    while (true)
                    {
                        if ($a0_7)
                        {
                            void* $v0_17 = *(*($a0_7 + 0xc4) + 0xc);
                            
                            if (!$v0_17)
                                $s0_4 += 4;
                            else
                            {
                                int32_t $v0_18 = *($v0_17 + 8);
                                
                                if (!$v0_18)
                                    $s0_4 += 4;
                                else
                                {
                                    int32_t $v0_19 = $v0_18();
                                    
                                    if (!$v0_19)
                                        $s0_4 += 4;
                                    else
                                    {
                                        $s0_4 += 4;
                                        
                                        if ($v0_19 != 0xfffffdfd)
                                            return $v0_19;
                                    }
                                }
                            }
                        }
                        else
                            $s0_4 += 4;
                        
                        if ($s0_4 == $s7 + 0x6c)
                            break;
                        
                        $a0_7 = *$s0_4;
                    }
                    
                    $s6_1 = 0;
                    
                    if (private_copy_to_user(arg3, &var_98_83, 4))
                    {
                        $a2_1 = "tx_isp_sensor_set_input";
                        $a1_3 = "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\\n";
                        goto label_1df8c;
                    }
                }
                else if (arg2 >= 0xc0045628)
                {
                    if (arg2 == 0xc00456c8)
                    {
                        sprintf(&var_98_84, *(*($s7 + 0x2c) + 0xd4) + 0x1d8);
                        $s6_1 = 0;
                        
                        if (private_copy_to_user(arg3, &var_98_85, 0x40))
                        {
                            $a2_1 = "tx_isp_get_default_bin_path";
                            $a1_3 = "sensor type is BT656!\\n";
                            goto label_1df8c;
                        }
                    }
                    else if (arg2 == 0xc00456e1)
                    {
                        private_wait_for_completion_interruptible(&awb_algo_comp);
                        memset(awb_info_mine + 8, 0, 0x2bb);
                        tisp_g_wb_zone();
                        tisp_g_wb_attr(&var_98_86);
                        uint32_t awb_info_mine_1 = awb_info_mine;
                        int32_t var_8c_8;
                        *(awb_info_mine_1 + 0x10) = var_8c_9;
                        int32_t var_88_7;
                        *(awb_info_mine_1 + 0x14) = var_88_8;
                        int32_t var_84_5;
                        *(awb_info_mine_1 + 0x18) = var_84_6;
                        int32_t var_80_16;
                        *(awb_info_mine_1 + 0x1c) = var_80_17;
                        *(awb_info_mine_1 + 8) = var_94_16;
                        *(awb_info_mine_1 + 0xc) = var_90_19;
                        $s6_1 = 0;
                        
                        if (private_copy_to_user(arg3, awb_info_mine_1, 0x2c3))
                        {
                            $a2_1 = "tx_isp_get_awb_algo_handle";
                            $a1_3 = "sensor type is BT656!\\n";
                            goto label_1df8c;
                        }
                    }
                    else
                    {
                        $s6_1 = 0;
                        
                        if (arg2 == 0xc00456c7)
                        {
                            if (!private_copy_from_user(&var_98_87, arg3, 0x40))
                            {
                                memcpy(*(*($s7 + 0x2c) + 0xd4) + 0x1d8, &var_98_88, 0x40);
                                return 0;
                            }
                            
                            $a2_1 = "tx_isp_set_default_bin_path";
                            goto label_1f3d0;
                        }
                    }
                }
                else if (arg2 == 0x805056c1)
                {
                    void* i_2 = $s7 + 0x2c;
                    
                    if (private_copy_from_user(&var_98_89, arg3, 0x50))
                    {
                        $a2_1 = "tx_isp_sensor_register_sensor";
                        $a1_3 = "sensor type is BT601!\\n";
                        goto label_1df8c;
                    }
                    
                    do
                    {
                        void* $a0_10 = *i_2;
                        
                        if ($a0_10)
                        {
                            void* $v0_22 = *(*($a0_10 + 0xc4) + 0xc);
                            
                            if ($v0_22)
                            {
                                int32_t $v0_23 = *($v0_22 + 8);
                                
                                if (!$v0_23)
                                    i_2 += 4;
                                else
                                {
                                    int32_t $v0_25 = $v0_23($a0_10, 0x2000000, &var_98_90);
                                    $s6_1 = $v0_25;
                                    
                                    if (!$v0_25)
                                        i_2 += 4;
                                    else
                                    {
                                        i_2 += 4;
                                        
                                        if ($v0_25 != 0xfffffdfd)
                                            break;
                                    }
                                }
                            }
                            else
                                i_2 += 4;
                        }
                        else
                            i_2 += 4;
                        
                        $s6_1 = 0;
                    } while ($s7 + 0x6c != i_2);
                }
                else if (arg2 == 0x805056c2)
                {
                    void* $s0_5 = $s7 + 0x2c;
                    
                    if (private_copy_from_user(&var_98_91, arg3, 0x50))
                    {
                        $a2_1 = "tx_isp_sensor_release_sensor";
                        $a1_3 = "sensor type is BT601!\\n";
                        goto label_1df8c;
                    }
                    
                    void* $a0_12 = *$s0_5;
                    
                    while (true)
                    {
                        if ($a0_12)
                        {
                            void* $v0_28 = *(*($a0_12 + 0xc4) + 0xc);
                            
                            if (!$v0_28)
                                $s0_5 += 4;
                            else
                            {
                                int32_t $v0_29 = *($v0_28 + 8);
                                
                                if (!$v0_29)
                                    $s0_5 += 4;
                                else
                                {
                                    int32_t $v0_30 = $v0_29();
                                    $s6_1 = $v0_30;
                                    
                                    if (!$v0_30)
                                        $s0_5 += 4;
                                    else
                                    {
                                        $s0_5 += 4;
                                        
                                        if ($v0_30 != 0xfffffdfd)
                                            break;
                                    }
                                }
                            }
                        }
                        else
                            $s0_5 += 4;
                        
                        if ($s7 + 0x6c == $s0_5)
                            return 0;
                        
                        $a0_12 = *$s0_5;
                    }
                }
                else
                {
                    $s6_1 = 0;
                    
                    if (arg2 == 0x8038564f)
                    {
                        if (private_copy_from_user(&var_98_92, arg3, 0x38))
                        {
                            $a2_1 = "tx_isp_sensor_s_register";
                            $a1_3 = "sensor type is BT601!\\n";
                            goto label_1df8c;
                        }
                        
                        var_48_17 = &var_94_17;
                        int32_t var_3c_1_3 = var_6c_9;
                        int32_t var_40_1_5 = var_70_13;
                        int32_t var_68_10;
                        int32_t var_38_1_2 = var_68_11;
                        int32_t var_64_10;
                        int32_t var_34_1_5 = var_64_11;
                        void* $s0_6 = $s7 + 0x2c;
                        void* $a0_21 = *$s0_6;
                        
                        while (true)
                        {
                            if ($a0_21)
                            {
                                void* $v0_54 = *(*($a0_21 + 0xc4) + 0xc);
                                
                                if (!$v0_54)
                                    $s0_6 += 4;
                                else
                                {
                                    int32_t $v0_55 = *($v0_54 + 8);
                                    
                                    if (!$v0_55)
                                        $s0_6 += 4;
                                    else
                                    {
                                        int32_t $v0_56 = $v0_55();
                                        $s6_1 = $v0_56;
                                        
                                        if (!$v0_56)
                                            $s0_6 += 4;
                                        else
                                        {
                                            $s0_6 += 4;
                                            
                                            if ($v0_56 != 0xfffffdfd)
                                                break;
                                        }
                                    }
                                }
                            }
                            else
                                $s0_6 += 4;
                            
                            if ($s7 + 0x6c == $s0_6)
                                return 0;
                            
                            $a0_21 = *$s0_6;
                        }
                    }
                }
            }
        }
        else if (arg2 == 0x800456d8)
        {
            void* $s0_13 = *($s7 + 0x2c);
            var_98_93 = 1;
            void* $s2_24 = *($s0_13 + 0xd4);
            $s6_1 = 0;
            
            if (*($s2_24 + 0x17c) != 1)
            {
                *($s2_24 + 0x17c) = 1;
                
                if (wdr_switch)
                {
                    if ($s0_13)
                    {
                        int32_t $v0_106 = *($s0_13 + 0x7c);
                        
                        if ($v0_106)
                            $v0_106($s0_13, 0x2000013, &var_98_94);
                    }
                    
                    tisp_s_wdr_en(var_98_95);
                    void* $v0_107;
                    
                    if (!$s0_13)
                        $v0_107 = *($s2_24 + 0x120);
                    else
                    {
                        int32_t $v0_108 = *($s0_13 + 0x7c);
                        
                        if ($v0_108)
                            $v0_108($s0_13, 0x200000c, &var_98_96);
                        
                        $v0_107 = *($s2_24 + 0x120);
                    }
                    
                    *($v0_107 + 0xac) = 0;
                    *($v0_107 + 0xdc) = 0;
                    *($v0_107 + 0x9c) = 0;
                    *($v0_107 + 0xe4) = 0;
                    *($v0_107 + 0xa0) = 0;
                }
                
                wdr_switch = 1;
                return 0;
            }
        }
        else if (arg2 >= 0x800456d9)
        {
            if (arg2 == 0x800456dd)
            {
                int32_t* $v0_114 = kmem_cache_alloc(0, 0xd0);
                void* $s6_5 = *(*($s7 + 0x2c) + 0xd4);
                
                if (private_copy_from_user($v0_114, arg3, 0x80))
                {
                    $a2_1 = "tx_isp_set_ae_algo_open";
                    goto label_1f3d0;
                }
                
                if (*$v0_114 != 0x336ac)
                {
                    isp_printf(2, "Failed to allocate vic device\\n", "tx_isp_set_ae_algo_open");
                    return 0xffffffff;
                }
                
                void* $s5_1 = *($s6_5 + 0x120);
                tisp_ae_algo_init(1, $v0_114);
                $v0_114[0x1b] = *($s6_5 + 0x12c);
                $v0_114[3] = *($s5_1 + 0xac);
                $v0_114[4] = private_math_exp2(*($s5_1 + 0x9c), 0x10, 0xa);
                $v0_114[5] = 0x400;
                $v0_114[0xf] = *($s5_1 + 0xdc);
                $v0_114[0x10] = private_math_exp2(*($s5_1 + 0xe4), 0x10, 0xa);
                $v0_114[0x11] = 0x400;
                
                if (*($s6_5 + 0x17c))
                {
                    $v0_114[6] = system_reg_read(0x1000);
                    $v0_114[0x12] = system_reg_read(0x100c);
                }
                else
                    $v0_114[6] = system_reg_read(0x1030);
                
                ae_info_mine = kmem_cache_alloc(0, 0xd0);
                ae_statis_mine = kmem_cache_alloc(0, 0xd0);
                ae_algo_comp = 0;
                __init_waitqueue_head(0xb2cc4, "Failed to init isp module(%d.%d)\\n", 0);
                
                if (private_copy_to_user(arg3, $v0_114, 0x80))
                {
                    $a2_1 = "tx_isp_set_ae_algo_open";
                    $a1_3 = "sensor type is BT656!\\n";
                    goto label_1df8c;
                }
                
                $s6_1 = 0;
                private_kfree($v0_114);
            }
            else if (arg2 < 0x800456de)
            {
                if (arg2 == 0x800456db)
                    return tx_isp_get_ae_algo_handle.isra.16(*($s7 + 0x2c), arg3);
                
                if (arg2 >= 0x800456dc)
                {
                    if (private_copy_from_user(&var_98_97, arg3, 0x38))
                    {
                        $a2_1 = "tx_isp_set_ae_algo_handle";
                        goto label_1f3d0;
                    }
                    
                    $s6_1 = 0;
                    
                    if (var_90_20 == 1)
                        tisp_ae_algo_handle(&var_98_98);
                }
                else
                {
                    if (arg2 != 0x800456d9)
                        return 0;
                    
                    void* $s0_2 = *($s7 + 0x2c);
                    var_98_99 = 0;
                    void* $v0_109 = *($s0_2 + 0xd4);
                    $s6_1 = 0;
                    
                    if (*($v0_109 + 0x17c))
                    {
                        void* $s1_16 = *($v0_109 + 0x120);
                        *($v0_109 + 0x17c) = 0;
                        
                        if (wdr_switch)
                        {
                            if ($s0_2)
                            {
                                int32_t $v0_110 = *($s0_2 + 0x7c);
                                
                                if ($v0_110)
                                    $v0_110($s0_2, 0x2000013, &var_98_100);
                            }
                            
                            tisp_s_wdr_en(var_98_101);
                            
                            if (!$s0_2)
                                *($s1_16 + 0xac) = 0;
                            else
                            {
                                int32_t $v0_111 = *($s0_2 + 0x7c);
                                
                                if ($v0_111)
                                    $v0_111($s0_2, 0x200000c, &var_98_102);
                                
                                *($s1_16 + 0xac) = 0;
                            }
                            
                            *($s1_16 + 0xdc) = 0;
                            *($s1_16 + 0x9c) = 0;
                            *($s1_16 + 0xe4) = 0;
                            *($s1_16 + 0xa0) = 0;
                        }
                        
                        wdr_switch = 1;
                        return 0;
                    }
                }
            }
            else if (arg2 == 0x800856d4)
            {
                void* $s4_5 = *(*($s7 + 0x2c) + 0xd4);
                
                if (private_copy_from_user(&var_98_103, arg3, 8))
                {
                    $a2_1 = "tx_isp_set_buf";
                    $a1_3 = "sensor type is BT601!\\n";
                    goto label_1df8c;
                }
                
                uint32_t $s2_6 = (*($s4_5 + 0xec) + 7) >> 3 << 3;
                int32_t $s3_3 = $s2_6 * *($s4_5 + 0xf0);
                system_reg_write(0x7820, var_98_104);
                system_reg_write(0x7824, $s2_6);
                
                if (var_94_18 < $s3_3)
                {
                label_1e95c:
                    isp_printf(1, "%s[%d] do not support this interface\\n", "tx_isp_set_buf");
                    return 0xfffffff2;
                }
                
                uint32_t $s2_10 = (*($s4_5 + 0xec) + 7) >> 3 << 3;
                int32_t $s1_4 = $s2_10 * *($s4_5 + 0xf0);
                system_reg_write(0x7828, $s3_3 + var_98_105);
                system_reg_write(0x782c, $s2_10);
                int32_t $s1_6 = $s3_3 + ($s1_4 >> 1);
                
                if (var_94_19 < $s1_6)
                    goto label_1e95c;
                
                int32_t $s3_4 = *($s4_5 + 0xf0);
                uint32_t $s2_16 = (((*($s4_5 + 0xec) + 0x1f) >> 5) + 7) >> 3 << 3;
                system_reg_write(0x7830, $s1_6 + var_98_106);
                system_reg_write(0x7834, $s2_16);
                int32_t $s3_8 = ((($s3_4 + 0xf) >> 4) + 1) * $s2_16;
                uint32_t $a1_24 = var_98_107;
                
                if (isp_memopt)
                {
                    system_reg_write(0x7840, $s1_6 + $a1_24);
                    system_reg_write(0x7844, 0);
                    system_reg_write(0x7848, $s1_6 + var_98_108);
                    system_reg_write(0x784c, 0);
                    system_reg_write(0x7850, $s1_6 + var_98_109);
                    system_reg_write(0x7854, 0);
                }
                else
                {
                    system_reg_write(0x7840, $s1_6 + $s3_8 + $a1_24);
                    system_reg_write(0x7844, $s2_16);
                    int32_t $s6_3 = $s3_8 << 1;
                    system_reg_write(0x7848, $s1_6 + var_98_110 + $s6_3);
                    system_reg_write(0x784c, $s2_16);
                    system_reg_write(0x7850, $s1_6 + var_98_111 + $s6_3 + $s3_8);
                    system_reg_write(0x7854, $s2_16);
                    $s3_8 <<= 2;
                }
                
                system_reg_write(0x7838, 0);
                system_reg_write(0x783c, 1);
                int32_t $s1_7 = $s1_6 + $s3_8;
                
                if (var_94_20 < $s1_7)
                    goto label_1e95c;
                
                uint32_t $a1_40 = var_98_112;
                
                if (!isp_memopt)
                {
                    uint32_t $s3_13 = ((*($s4_5 + 0xec) >> 1) + 7) >> 3 << 3;
                    int32_t $s2_18 = $s3_13 * *($s4_5 + 0xf0);
                    system_reg_write(0x7858, $s1_7 + $a1_40);
                    system_reg_write(0x785c, $s3_13);
                    int32_t $s1_8 = $s1_7 + $s2_18;
                    
                    if (var_94_21 < $s1_8)
                        goto label_1e95c;
                    
                    uint32_t $s3_18 = ((*($s4_5 + 0xec) >> 1) + 7) >> 3 << 3;
                    int32_t $s2_20 = $s3_18 * *($s4_5 + 0xf0);
                    system_reg_write(0x7860, $s1_8 + var_98_113);
                    system_reg_write(0x7864, $s3_18);
                    int32_t $s2_22 = $s1_8 + ($s2_20 >> 1);
                    
                    if (var_94_22 < $s2_22)
                        goto label_1e95c;
                    
                    uint32_t $s3_23 = ((*($s4_5 + 0xec) >> 5) + 7) >> 3 << 3;
                    int32_t $s1_10 = $s3_23 * *($s4_5 + 0xf0);
                    system_reg_write(0x7868, $s2_22 + var_98_114);
                    system_reg_write(0x786c, $s3_23);
                    $s1_7 = $s2_22 + ($s1_10 >> 5);
                    goto label_1e94c;
                }
                
                system_reg_write(0x7858, $a1_40);
                system_reg_write(0x785c, 0);
                system_reg_write(0x7860, var_98_115);
                system_reg_write(0x7864, 0);
                system_reg_write(0x7868, var_98_116);
                system_reg_write(0x786c, 0);
            label_1e94c:
                $s6_1 = 0;
                
                if (var_94_23 < $s1_7)
                    goto label_1e95c;
            }
            else
            {
                if (arg2 < 0x800856d5)
                {
                    if (arg2 != 0x800456de)
                        return 0;
                    
                    if (private_copy_from_user(&var_98_117, arg3, 8))
                    {
                        $a2_1 = "tx_isp_set_ae_algo_close";
                        goto label_1f3d0;
                    }
                    
                    tisp_ae_algo_deinit();
                    tisp_ae_algo_init(0, nullptr);
                    private_kfree(ae_info_mine);
                    private_kfree(ae_statis_mine);
                    return 0;
                }
                
                if (arg2 != 0x800856d5)
                {
                    if (arg2 != 0x800856d6)
                        return 0;
                    
                    void* $s2_23 = *(*($s7 + 0x2c) + 0xd4);
                    
                    if (private_copy_from_user(&var_98_118, arg3, 8))
                    {
                        isp_printf(2, "sensor type is BT601!\\n", "tx_isp_wdr_set_buf");
                        return 0xfffffff2;
                    }
                    
                    uint32_t var_a0_1_2 = var_98_119;
                    int32_t var_a4_1_1 = var_94_24;
                    int32_t var_a8_1_2 = 0;
                    int32_t $a2_12 = isp_printf(0, "%s:%d::linear mode\\n", "tx_isp_wdr_set_buf");
                    void* $v0_91 = *($s2_23 + 0x120);
                    int32_t $v1_21 = *($v0_91 + 0x90);
                    int32_t $s0_10;
                    uint32_t $s1_13;
                    
                    if ($v1_21 != 1)
                    {
                        if ($v1_21 != 2)
                        {
                            isp_printf(2, "%s:%d::wdr mode\\n", $a2_12);
                            return 0xffffffff;
                        }
                        
                        $s0_10 = *($v0_91 + 0xe8);
                        $s1_13 = $s0_10 / (*($s2_23 + 0x124) << 1);
                    }
                    else
                    {
                        $s1_13 = *($s2_23 + 0x128);
                        $s0_10 = ($s1_13 * *($s2_23 + 0x124)) << 1;
                    }
                    
                    int32_t var_a8_2_1 = $s0_10;
                    uint32_t var_a0_2_4 = var_98_120;
                    int32_t var_a4_2_1 = var_94_25;
                    isp_printf(0, "%s:%d::linear mode\\n", "tx_isp_wdr_set_buf");
                    
                    if (var_94_26 >= $s0_10)
                    {
                        system_reg_write(0x2004, var_98_121);
                        system_reg_write(0x2008, *($s2_23 + 0x124) << 1);
                        system_reg_write(0x200c, $s1_13);
                        return 0;
                    }
                    
                    isp_printf(0, "%s[%d] do not support this interface\\n", "tx_isp_wdr_set_buf");
                    *($s2_23 + 0x17c) = 0;
                    tisp_s_wdr_en(0);
                    return 0xfffffff2;
                }
                
                void* $v1_14 = *(*($s7 + 0x2c) + 0xd4);
                int32_t $v0_83 = *($v1_14 + 0xec);
                int32_t $a2_9 = *($v1_14 + 0xf0);
                int32_t $t0_3 = $a2_9 << 3;
                int32_t $a0_29 = (($v0_83 + 7) >> 3) * $t0_3;
                int32_t $a3_2 = ($a0_29 >> 1) + $a0_29;
                int32_t $a0_37 =
                    (((($v0_83 + 0x1f) >> 5) + 7) >> 3) * (((($a2_9 + 0xf) >> 4) + 1) << 3);
                int32_t $a2_10 = $a3_2 + $a0_37;
                
                if (!isp_memopt)
                {
                    int32_t $a1_55 = ((($v0_83 >> 1) + 7) >> 3) * $t0_3;
                    $a2_10 = ($a0_37 << 2) + ((((($v0_83 >> 5) + 7) >> 3) * $t0_3) >> 5)
                        + ($a1_55 >> 1) + $a3_2 + $a1_55;
                }
                
                var_94_27 = $a2_10;
                var_98_122 = 0;
                $s6_1 = 0;
                
                if (private_copy_to_user(arg3, &var_98_123, 8))
                {
                    $a2_1 = "tx_isp_get_buf";
                    $a1_3 = "sensor type is BT601!\\n";
                    goto label_1df8c;
                }
            }
        }
        else if (arg2 == 0x800456d0)
        {
            if (private_copy_from_user(&var_98_124, arg3, 4))
            {
                $a2_1 = "tx_isp_video_link_setup";
                $a1_3 = "sensor type is BT601!\\n";
                goto label_1df8c;
            }
            
            uint32_t $a2_4 = var_98_125;
            
            if ($a2_4 >= 2)
            {
                isp_printf(2, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n", $a2_4);
                return 0xffffffea;
            }
            
            $s6_1 = 0;
            
            if ($a2_4 != *($s7 + 0x10c))
            {
                int32_t $v0_36 = *(($a2_4 << 3) + 0x7ad50);
                void* $s1_1 = (&configs)[$a2_4 * 2];
                int32_t $s2_2 = 0;
                
                while ($s2_2 < $v0_36)
                {
                    int32_t (* var_2c_1_2)(void* arg1, int32_t* arg2) = find_subdev_link_pad;
                    void* $v0_39 = find_subdev_link_pad($s7 - 0xc, $s1_1);
                    void* $v0_40;
                    int32_t $a2_7;
                    $v0_40 = var_2c_1_3($s7 - 0xc, $s1_1 + 8);
                    void* $t0_2 = $v0_40;
                    
                    if (!$v0_39)
                        $s2_2 += 1;
                    else if (!$v0_40)
                        $s2_2 += 1;
                    else
                    {
                        int32_t $t1_1 = *($s1_1 + 0x10);
                        
                        if (!(*($v0_39 + 6) & *($v0_40 + 6) & $t1_1))
                        {
                            isp_printf(2, "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n", 
                                $a2_7);
                            return 0xffffffff;
                        }
                        
                        uint32_t $v0_44 = *($v0_39 + 7);
                        
                        if ($v0_44 == 4 || *($t0_2 + 7) == 4)
                        {
                            isp_printf(2, "%s[%d] VIC do not support this format %d\\n", 0xd9);
                            return 0xffffffff;
                        }
                        
                        uint32_t $v0_46;
                        
                        if ($v0_44 == 3 && $t0_2 != *($v0_39 + 0xc))
                        {
                            $t0_2 = subdev_video_destroy_link($v0_39 + 8);
                            $v0_46 = *($t0_2 + 7);
                        }
                        else
                            $v0_46 = *($t0_2 + 7);
                        
                        void* $t2_1 = $t0_2 + 8;
                        int32_t $t1_2;
                        
                        if ($v0_46 != 3 || $v0_39 == *($t0_2 + 0xc))
                            $t1_2 = $t1_1 | 1;
                        else
                        {
                            int32_t $t1_3;
                            $t0_2 = subdev_video_destroy_link($t2_1);
                            $t1_2 = $t1_3 | 1;
                        }
                        
                        *($v0_39 + 8) = $v0_39;
                        *($v0_39 + 0xc) = $t0_2;
                        *($v0_39 + 0x10) = $t2_1;
                        *($v0_39 + 0x14) = $t1_2;
                        *($v0_39 + 7) = 3;
                        *($t0_2 + 8) = $t0_2;
                        *($t0_2 + 0xc) = $v0_39;
                        *($t0_2 + 0x10) = $v0_39 + 8;
                        *($t0_2 + 0x14) = $t1_2;
                        *($t0_2 + 7) = 3;
                        $s2_2 += 1;
                    }
                    
                    $s1_1 += 0x14;
                }
                
                *($s7 + 0x10c) = var_98_126;
                return 0;
            }
        }
        else
        {
            int32_t $a1;
            
            if (arg2 >= 0x800456d1)
            {
                if (arg2 == 0x800456d2)
                    $a1 = 1;
                else
                {
                    if (arg2 < 0x800456d2)
                        return tx_isp_video_link_destroy.isra.5($s7 - 0xc);
                    
                    $a1 = 0;
                    
                    if (arg2 != 0x800456d3)
                        return 0;
                }
                
                return tx_isp_video_link_stream($s7 - 0xc, $a1);
            }
            
            if (arg2 == 0x80045612)
            {
                $a1 = 1;
            label_1e228:
                return tx_isp_video_s_stream($s7 - 0xc, $a1);
            }
            
            if (arg2 == 0x80045613)
            {
                $a1 = 0;
                goto label_1e228;
            }
            
            $s6_1 = 0;
            
            if (arg2 == 0x40045626)
            {
                int32_t* i_3 = $s7 + 0x2c;
                
                do
                {
                    void* $a0_4 = *i_3;
                    
                    if ($a0_4)
                    {
                        void* $v0_10 = *(*($a0_4 + 0xc4) + 0xc);
                        
                        if ($v0_10)
                        {
                            int32_t $v0_11 = *($v0_10 + 8);
                            
                            if (!$v0_11)
                                i_3 = &i_3[1];
                            else
                            {
                                int32_t $v0_13 = $v0_11($a0_4, 0x2000003, &var_98_127);
                                
                                if (!$v0_13)
                                    i_3 = &i_3[1];
                                else
                                {
                                    i_3 = &i_3[1];
                                    
                                    if ($v0_13 != 0xfffffdfd)
                                        return $v0_13;
                                }
                            }
                        }
                        else
                            i_3 = &i_3[1];
                    }
                    else
                        i_3 = &i_3[1];
                } while ($s7 + 0x6c != i_3);
                
                $s6_1 = 0;
                
                if (private_copy_to_user(arg3, &var_98_128, 4))
                {
                    $a2_1 = "tx_isp_sensor_get_input";
                    $a1_3 = "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\\n";
                    goto label_1df8c;
                }
            }
        }
    }
    return $s6_1;
}

