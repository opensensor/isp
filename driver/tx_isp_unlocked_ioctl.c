#include "include/main.h"


  int32_t tx_isp_unlocked_ioctl(void* arg1, int32_t arg2, int32_t arg3)

{
    int32_t* $s7 = (int32_t*)((char*)arg1  + 0x70); // Fixed void pointer arithmetic
    int32_t $s6_1;
    uint32_t var_98;
    int32_t var_94;
        char* $v1_22 = (char*)(*(*($s7 + 0x2c) + 0xd4)); // Fixed void pointer assignment
        int32_t* $v0_96 = (int32_t*)((char*)$v1_22  + 0x120); // Fixed void pointer arithmetic
        int32_t $a0_41 = *($v0_96 + 0x90);
        int32_t var_a8_3 = var_94;
            return 0xfffffff2;
    
    if ((uintptr_t)arg2 == 0x800856d7)
    {
        var_98 = 0;
        var_94 = 0;
        
        if ($a0_41 == 1)
            var_94 = (*($v1_22 + 0x124) * *($v1_22 + 0x128)) << 1;
        else if ($a0_41 != 2)

        else
            var_94 = *($v0_96 + 0xe8);
        

        $s6_1 = 0;
        
        if (private_copy_to_user(arg3, &var_98, 8))
        {

        }
    }
    else
    {
        int32_t var_90;
                    return 0xfffffff2;
        char const* const $a1_3;
        char const* const $a2_1;
        
        if ((uintptr_t)arg2 >= 0x800856d8)
        {
            if ((uintptr_t)arg2 == 0xc00456e2)
            {
                if (private_copy_from_user(&var_98, arg3, 0x18))
                {
                    $a2_1 = "tx_isp_set_awb_algo_handle";
                label_1f3d0:
                    $a1_3 = "bank no free\n";
                label_1df8c:

                }
                
                $s6_1 = 0;
                
                if (var_90_5 == 1)
                    tisp_awb_algo_handle(&var_98_21);
            }
            else
            {
                int32_t var_70;
                int32_t var_6c;
                int32_t* var_48;
                
                if ((uintptr_t)arg2 >= 0xc00456e3)
                {
                    if ((uintptr_t)arg2 == 0xc00456e7)
                    {
                        for (int32_t i = 0; i != 3; )
                        {
                            tisp_get_frame_drop(i, &(&var_98)[i * 3]);
                            i += 1;
                            $s6_1 = 0;
                        }
                        
                        if (private_copy_to_user(arg3, &var_98_22, 0x24))
                        {
                            goto label_1df8c;
                            $a2_1 = "tx_isp_get_frame_drop";
                            $a1_3 = "sensor type is BT656!\n";
                        }
                    }
                    else if ((uintptr_t)arg2 >= 0xc00456e8)
                    {
                        int32_t $v0_103;
                        void* $s0_12;
                                goto label_1df8c;
                        
                        if ((uintptr_t)arg2 == 0xc00456e9)
                        {
                            $s0_12 = *($s7 + 0x2c);
                            
                            if (private_copy_from_user(&var_98, arg3, 0x2a))
                            {
                                $a2_1 = "tx_isp_set_gpio_sta";
                                $a1_3 = "sensor type is BT601!\n";
                            }
                            
                            $s6_1 = 0xfffffffe;
                            
                            if ($s0_12)
                            {
                                $v0_103 = *($s0_12 + 0x7c);
                                $s6_1 = 0xfffffdfd;
                                
                                if ($v0_103)
                                    return $v0_103($s0_12, 0x2000018, &var_98);
                            }
                        }
                        else if ((uintptr_t)arg2 < 0xc00456e9)
                        {
                                goto label_1df8c;
                            $s0_12 = *($s7 + 0x2c);
                            
                            if (private_copy_from_user(&var_98, arg3, 0x2a))
                            {
                                $a2_1 = "tx_isp_set_gpio_init";
                                $a1_3 = "sensor type is BT601!\n";
                            }
                            
                            $s6_1 = 0xfffffffe;
                            
                            if ($s0_12)
                            {
                                $v0_103 = *($s0_12 + 0x7c);
                                $s6_1 = 0xfffffdfd;
                                
                                if ($v0_103)
                                    return $v0_103($s0_12, 0x2000017, &var_98);
                            }
                        }
                        else if ((uintptr_t)arg2 == 0xc0385650)
                        {
                            int32_t $v0_57 = private_copy_from_user(&var_98, arg3, 0x38);
                            int32_t $v0_65;
                                int32_t var_40_2 = var_70;
                                int32_t var_3c_2 = var_6c;
                                char* $s0_7 = (char*)($s7 + 0x2c); // Fixed void pointer assignment
                                char* $a0_23 = (char*)(*$s0_7); // Fixed void pointer assignment
                                        char* $v0_61 = (char*)(*(*($a0_23 + 0xc4) + 0xc)); // Fixed void pointer assignment
                                            int32_t $v0_62 = *($v0_61 + 8);
                                                int32_t $v0_63 = $v0_62();
                                                        return $v0_63;
                            
                            if (!$v0_57)
                            {
                                var_48 = &var_94;
                                
                                while (true)
                                {
                                    if ($a0_23)
                                    {
                                        
                                        if (!$v0_61)
                                            $s0_7 += 4;
                                        else
                                        {
                                            
                                            if (!$v0_62)
                                                $s0_7 += 4;
                                            else
                                            {
                                                
                                                if (!$v0_63)
                                                    $s0_7 += 4;
                                                else
                                                {
                                                    $s0_7 += 4;
                                                    
                                                    if ($(uintptr_t)v0_63 != 0xfffffdfd)
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
                                
                                int32_t var_38_3;
                                int32_t var_68_1_1 = var_38_4;
                                int32_t var_44_4;
                                int32_t var_74_1 = var_44_5;
                                int32_t var_34_7;
                                int32_t var_64_1_1 = var_34_8;
                                $v0_65 = private_copy_to_user(arg3, &var_98_23, 0x38);
                                $s6_1 = 0;
                            }
                            
                            if ($v0_57 || $v0_65)
                            {
                                goto label_1df8c;
                                $a2_1 = "tx_isp_sensor_g_register";
                                $a1_3 = "sensor type is BT601!\n";
                            }
                        }
                        else
                        {
                                return 0;
                            char* $s0_3 = (char*)($s7 + 0x2c); // Fixed void pointer assignment
                                goto label_1df8c;
                            if ((uintptr_t)arg2 != 0xc050561a)
                            
                            
                            if (private_copy_from_user(&var_98, arg3, 0x50))
                            {
                                $a2_1 = "tx_isp_sensor_enum_input";
                                $a1_3 = "sensor type is BT601!\n";
                            }
                            
                            char* $a0_2 = (char*)(*$s0_3); // Fixed void pointer assignment
                            
                            while (true)
                            {
                                    char* $v0_6 = (char*)(*(*($a0_2 + 0xc4) + 0xc)); // Fixed void pointer assignment
                                        int32_t $v0_7 = *($v0_6 + 8);
                                            int32_t $v0_8 = $v0_7();
                                                    return $v0_8;
                                if ($a0_2)
                                {
                                    
                                    if (!$v0_6)
                                        $s0_3 += 4;
                                    else
                                    {
                                        
                                        if (!$v0_7)
                                            $s0_3 += 4;
                                        else
                                        {
                                            
                                            if (!$v0_8)
                                                $s0_3 += 4;
                                            else
                                            {
                                                $s0_3 += 4;
                                                
                                                if ($(uintptr_t)v0_8 != 0xfffffdfd)
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
                            
                            if (private_copy_to_user(arg3, &var_98_24, 0x50))
                            {
                                goto label_1df8c;
                                $a2_1 = "tx_isp_sensor_enum_input";
                                $a1_3 = "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\n";
                            }
                        }
                    }
                    else if ((uintptr_t)arg2 == 0xc00456e4)
                    {
                            goto label_1f3d0;
                        if (private_copy_from_user(&var_98, arg3, 8))
                        {
                            $a2_1 = "tx_isp_set_awb_algo_close";
                        }
                        
                        tisp_awb_algo_deinit();
                        tisp_awb_algo_init(0);
                        $s6_1 = 0;
                        private_kfree(awb_info_mine);
                    }
                    else
                    {
                            return 0;
                        if ((uintptr_t)arg2 < 0xc00456e4)
                        {
                            tisp_awb_algo_init(1);
                            awb_info_mine = kmem_cache_alloc(0, 0xd0);
                            awb_algo_comp = 0;
                            __init_waitqueue_head(0xb2cb8, "Failed to init isp module(%d.%d)\n", 0);
                        }
                        
                        if ((uintptr_t)arg2 != 0xc00456e6)
                            return 0;
                        
                        int32_t $v0_147;
                        int32_t $a2_18;
                        $v0_147 = private_copy_from_user(&var_98_25, arg3, 0x24);
                        int32_t i_1 = 0;
                        
                        if ($v0_147)
                        {
                            goto label_1f3d0;
                            $a2_1 = "tx_isp_set_frame_drop";
                        }
                        
                        do
                        {
                            int32_t $v0_148;
                            $v0_148 = tisp_set_frame_drop(i_1, &(&var_98)[i_1 * 3], $a2_18);
                            i_1 += 1;
                            $s6_1 = $v0_148;
                        } while (i_1 != 3);
                    }
                }
                else if ((uintptr_t)arg2 == 0xc0045627)
                {
                    char* $s0_4 = (char*)($s7 + 0x2c); // Fixed void pointer assignment
                        goto label_1df8c;
                    
                    if (private_copy_from_user(&var_98, arg3, 4))
                    {
                        $a2_1 = "tx_isp_sensor_set_input";
                        $a1_3 = "sensor type is BT601!\n";
                    }
                    
                    char* $a0_7 = (char*)(*$s0_4); // Fixed void pointer assignment
                    
                    while (true)
                    {
                            char* $v0_17 = (char*)(*(*($a0_7 + 0xc4) + 0xc)); // Fixed void pointer assignment
                                int32_t $v0_18 = *($v0_17 + 8);
                                    int32_t $v0_19 = $v0_18();
                                            return $v0_19;
                        if ($a0_7)
                        {
                            
                            if (!$v0_17)
                                $s0_4 += 4;
                            else
                            {
                                
                                if (!$v0_18)
                                    $s0_4 += 4;
                                else
                                {
                                    
                                    if (!$v0_19)
                                        $s0_4 += 4;
                                    else
                                    {
                                        $s0_4 += 4;
                                        
                                        if ($(uintptr_t)v0_19 != 0xfffffdfd)
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
                    
                    if (private_copy_to_user(arg3, &var_98_26, 4))
                    {
                        goto label_1df8c;
                        $a2_1 = "tx_isp_sensor_set_input";
                        $a1_3 = "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\n";
                    }
                }
                else if ((uintptr_t)arg2 >= 0xc0045628)
                {
                            goto label_1df8c;
                    if ((uintptr_t)arg2 == 0xc00456c8)
                    {
                        sprintf(&var_98, *(*($s7 + 0x2c) + 0xd4) + 0x1d8);
                        $s6_1 = 0;
                        
                        if (private_copy_to_user(arg3, &var_98, 0x40))
                        {
                            $a2_1 = "tx_isp_get_default_bin_path";
                            $a1_3 = "sensor type is BT656!\n";
                        }
                    }
                    else if ((uintptr_t)arg2 == 0xc00456e1)
                    {
                        uint32_t awb_info_mine_1 = awb_info_mine;
                        int32_t var_8c;
                        int32_t var_88;
                        int32_t var_84;
                        int32_t var_80;
                            goto label_1df8c;
                        private_wait_for_completion_interruptible(&awb_algo_comp);
                        memset(awb_info_mine + 8, 0, 0x2bb);
                        tisp_g_wb_zone();
                        tisp_g_wb_attr(&var_98);
                        *((int32_t*)((char*)awb_info_mine_1 + 0x10)) = var_8c; // Fixed void pointer dereference
                        *((int32_t*)((char*)awb_info_mine_1 + 0x14)) = var_88; // Fixed void pointer dereference
                        *((int32_t*)((char*)awb_info_mine_1 + 0x18)) = var_84; // Fixed void pointer dereference
                        *((int32_t*)((char*)awb_info_mine_1 + 0x1c)) = var_80; // Fixed void pointer dereference
                        *((int32_t*)((char*)awb_info_mine_1 + 8)) = var_94; // Fixed void pointer dereference
                        *((int32_t*)((char*)awb_info_mine_1 + 0xc)) = var_90; // Fixed void pointer dereference
                        $s6_1 = 0;
                        
                        if (private_copy_to_user(arg3, awb_info_mine_1, 0x2c3))
                        {
                            $a2_1 = "tx_isp_get_awb_algo_handle";
                            $a1_3 = "sensor type is BT656!\n";
                        }
                    }
                    else
                    {
                                return 0;
                        $s6_1 = 0;
                        
                        if ((uintptr_t)arg2 == 0xc00456c7)
                        {
                            if (!private_copy_from_user(&var_98, arg3, 0x40))
                            {
                                memcpy(*(*($s7 + 0x2c) + 0xd4) + 0x1d8, &var_98, 0x40);
                            }
                            
                            $a2_1 = "tx_isp_set_default_bin_path";
                            goto label_1f3d0;
                        }
                    }
                }
                else if ((uintptr_t)arg2 == 0x805056c1)
                {
                    char* i_2 = (char*)($s7 + 0x2c); // Fixed void pointer assignment
                        goto label_1df8c;
                    
                    if (private_copy_from_user(&var_98, arg3, 0x50))
                    {
                        $a2_1 = "tx_isp_sensor_register_sensor";
                        $a1_3 = "sensor type is BT601!\n";
                    }
                    
                    do
                    {
                        char* $a0_10 = (char*)(*i_2); // Fixed void pointer assignment
                            char* $v0_22 = (char*)(*(*($a0_10 + 0xc4) + 0xc)); // Fixed void pointer assignment
                                int32_t $v0_23 = *($v0_22 + 8);
                                    int32_t $v0_25 = $v0_23($a0_10, 0x2000000, &var_98);
                        
                        if ($a0_10)
                        {
                            
                            if ($v0_22)
                            {
                                
                                if (!$v0_23)
                                    i_2 += 4;
                                else
                                {
                                    $s6_1 = $v0_25;
                                    
                                    if (!$v0_25)
                                        i_2 += 4;
                                    else
                                    {
                                        i_2 += 4;
                                        
                                        if ($(uintptr_t)v0_25 != 0xfffffdfd)
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
                else if ((uintptr_t)arg2 == 0x805056c2)
                {
                    char* $s0_5 = (char*)($s7 + 0x2c); // Fixed void pointer assignment
                        goto label_1df8c;
                    
                    if (private_copy_from_user(&var_98, arg3, 0x50))
                    {
                        $a2_1 = "tx_isp_sensor_release_sensor";
                        $a1_3 = "sensor type is BT601!\n";
                    }
                    
                    char* $a0_12 = (char*)(*$s0_5); // Fixed void pointer assignment
                    
                    while (true)
                    {
                            char* $v0_28 = (char*)(*(*($a0_12 + 0xc4) + 0xc)); // Fixed void pointer assignment
                                int32_t $v0_29 = *($v0_28 + 8);
                                    int32_t $v0_30 = $v0_29();
                        if ($a0_12)
                        {
                            
                            if (!$v0_28)
                                $s0_5 += 4;
                            else
                            {
                                
                                if (!$v0_29)
                                    $s0_5 += 4;
                                else
                                {
                                    $s6_1 = $v0_30;
                                    
                                    if (!$v0_30)
                                        $s0_5 += 4;
                                    else
                                    {
                                        $s0_5 += 4;
                                        
                                        if ($(uintptr_t)v0_30 != 0xfffffdfd)
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
                            goto label_1df8c;
                    $s6_1 = 0;
                    
                    if ((uintptr_t)arg2 == 0x8038564f)
                    {
                        if (private_copy_from_user(&var_98, arg3, 0x38))
                        {
                            $a2_1 = "tx_isp_sensor_s_register";
                            $a1_3 = "sensor type is BT601!\n";
                        }
                        
                        var_48_6 = &var_94_5;
                        int32_t var_3c_1_1 = var_6c_2;
                        int32_t var_40_1_1 = var_70_5;
                        int32_t var_68_4;
                        int32_t var_38_1_1 = var_68_5;
                        int32_t var_64_4;
                        int32_t var_34_1_1 = var_64_5;
                        char* $s0_6 = (char*)($s7 + 0x2c); // Fixed void pointer assignment
                        char* $a0_21 = (char*)(*$s0_6); // Fixed void pointer assignment
                        
                        while (true)
                        {
                                char* $v0_54 = (char*)(*(*($a0_21 + 0xc4) + 0xc)); // Fixed void pointer assignment
                                    int32_t $v0_55 = *($v0_54 + 8);
                                        int32_t $v0_56 = $v0_55();
                            if ($a0_21)
                            {
                                
                                if (!$v0_54)
                                    $s0_6 += 4;
                                else
                                {
                                    
                                    if (!$v0_55)
                                        $s0_6 += 4;
                                    else
                                    {
                                        $s6_1 = $v0_56;
                                        
                                        if (!$v0_56)
                                            $s0_6 += 4;
                                        else
                                        {
                                            $s0_6 += 4;
                                            
                                            if ($(uintptr_t)v0_56 != 0xfffffdfd)
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
        else if ((uintptr_t)arg2 == 0x800456d8)
        {
            int32_t* $s0_13 = (int32_t*)((char*)$s7  + 0x2c); // Fixed void pointer arithmetic
            int32_t* $s2_24 = (int32_t*)((char*)$s0_13  + 0xd4); // Fixed void pointer arithmetic
                        int32_t $v0_106 = *($s0_13 + 0x7c);
            var_98 = 1;
            $s6_1 = 0;
            
            if (*($s2_24 + 0x17c) != 1)
            {
                *((int32_t*)((char*)$s2_24 + 0x17c)) = 1; // Fixed void pointer dereference
                
                if (wdr_switch)
                {
                    if ($s0_13)
                    {
                        
                        if ($v0_106)
                            $v0_106($s0_13, 0x2000013, &var_98);
                    }
                    
                    tisp_s_wdr_en(var_98_27);
                    void* $v0_107;
                    
                    if (!$s0_13)
                        $v0_107 = *($s2_24 + 0x120);
                    else
                    {
                        int32_t $v0_108 = *($s0_13 + 0x7c);
                        
                        if ($v0_108)
                            $v0_108($s0_13, 0x200000c, &var_98);
                        
                        $v0_107 = *($s2_24 + 0x120);
                    }
                    
                    *((int32_t*)((char*)$v0_107 + 0xac)) = 0; // Fixed void pointer dereference
                    *((int32_t*)((char*)$v0_107 + 0xdc)) = 0; // Fixed void pointer dereference
                    *((int32_t*)((char*)$v0_107 + 0x9c)) = 0; // Fixed void pointer dereference
                    *((int32_t*)((char*)$v0_107 + 0xe4)) = 0; // Fixed void pointer dereference
                    *((int32_t*)((char*)$v0_107 + 0xa0)) = 0; // Fixed void pointer dereference
                }
                
                wdr_switch = 1;
                return 0;
            }
        }
        else if ((uintptr_t)arg2 >= 0x800456d9)
        {
                int32_t* $v0_114 = kmem_cache_alloc(0, 0xd0);
                char* $s6_5 = (char*)(*(*($s7 + 0x2c) + 0xd4)); // Fixed void pointer assignment
                    goto label_1f3d0;
            if ((uintptr_t)arg2 == 0x800456dd)
            {
                
                if (private_copy_from_user($v0_114, arg3, 0x80))
                {
                    $a2_1 = "tx_isp_set_ae_algo_open";
                }
                
                if (*$(uintptr_t)v0_114 != 0x336ac)
                {
                    return 0xffffffff;

                }
                
                int32_t* $s5_1 = (int32_t*)((char*)$s6_5  + 0x120); // Fixed void pointer arithmetic
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
                    goto label_1df8c;
                    $a2_1 = "tx_isp_set_ae_algo_open";
                    $a1_3 = "sensor type is BT656!\n";
                }
                
                $s6_1 = 0;
                private_kfree($v0_114);
            }
            else if ((uintptr_t)arg2 < 0x800456de)
            {
                        goto label_1f3d0;
                if ((uintptr_t)arg2 == 0x800456db)
                    return tx_isp_get_ae_algo_handle.isra.16(*($s7 + 0x2c), arg3);
                
                if ((uintptr_t)arg2 >= 0x800456dc)
                {
                    if (private_copy_from_user(&var_98, arg3, 0x38))
                    {
                        $a2_1 = "tx_isp_set_ae_algo_handle";
                    }
                    
                    $s6_1 = 0;
                    
                    if (var_90_6 == 1)
                        tisp_ae_algo_handle(&var_98_28);
                }
                else
                {
                        return 0;
                    int32_t* $s0_2 = (int32_t*)((char*)$s7  + 0x2c); // Fixed void pointer arithmetic
                    int32_t* $v0_109 = (int32_t*)((char*)$s0_2  + 0xd4); // Fixed void pointer arithmetic
                        int32_t* $s1_16 = (int32_t*)((char*)$v0_109  + 0x120); // Fixed void pointer arithmetic
                                int32_t $v0_110 = *($s0_2 + 0x7c);
                    if ((uintptr_t)arg2 != 0x800456d9)
                    
                    var_98 = 0;
                    $s6_1 = 0;
                    
                    if (*($v0_109 + 0x17c))
                    {
                        *((int32_t*)((char*)$v0_109 + 0x17c)) = 0; // Fixed void pointer dereference
                        
                        if (wdr_switch)
                        {
                            if ($s0_2)
                            {
                                
                                if ($v0_110)
                                    $v0_110($s0_2, 0x2000013, &var_98);
                            }
                            
                            tisp_s_wdr_en(var_98_29);
                            
                            if (!$s0_2)
                                *((int32_t*)((char*)$s1_16 + 0xac)) = 0; // Fixed void pointer dereference
                            else
                            {
                                int32_t $v0_111 = *($s0_2 + 0x7c);
                                
                                if ($v0_111)
                                    $v0_111($s0_2, 0x200000c, &var_98);
                                
                                *((int32_t*)((char*)$s1_16 + 0xac)) = 0; // Fixed void pointer dereference
                            }
                            
                            *((int32_t*)((char*)$s1_16 + 0xdc)) = 0; // Fixed void pointer dereference
                            *((int32_t*)((char*)$s1_16 + 0x9c)) = 0; // Fixed void pointer dereference
                            *((int32_t*)((char*)$s1_16 + 0xe4)) = 0; // Fixed void pointer dereference
                            *((int32_t*)((char*)$s1_16 + 0xa0)) = 0; // Fixed void pointer dereference
                        }
                        
                        wdr_switch = 1;
                        return 0;
                    }
                }
            }
            else if ((uintptr_t)arg2 == 0x800856d4)
            {
                char* $s4_5 = (char*)(*(*($s7 + 0x2c) + 0xd4)); // Fixed void pointer assignment
                    goto label_1df8c;
                
                if (private_copy_from_user(&var_98, arg3, 8))
                {
                    $a2_1 = "tx_isp_set_buf";
                    $a1_3 = "sensor type is BT601!\n";
                }
                
                uint32_t $s2_6 = (*($s4_5 + 0xec) + 7) >> 3 << 3;
                int32_t $s3_3 = $s2_6 * *($s4_5 + 0xf0);
                system_reg_write(0x7820, var_98_30);
                system_reg_write(0x7824, $s2_6);
                
                if (var_94_6 < $s3_3)
                {
                    return 0xfffffff2;
                label_1e95c:

                }
                
                uint32_t $s2_10 = (*($s4_5 + 0xec) + 7) >> 3 << 3;
                int32_t $s1_4 = $s2_10 * *($s4_5 + 0xf0);
                system_reg_write(0x7828, $s3_3 + var_98_31);
                system_reg_write(0x782c, $s2_10);
                int32_t $s1_6 = $s3_3 + ($s1_4 >> 1);
                
                if (var_94_7 < $s1_6)
                    goto label_1e95c;
                
                int32_t $s3_4 = *($s4_5 + 0xf0);
                uint32_t $s2_16 = (((*($s4_5 + 0xec) + 0x1f) >> 5) + 7) >> 3 << 3;
                system_reg_write(0x7830, $s1_6 + var_98_32);
                system_reg_write(0x7834, $s2_16);
                int32_t $s3_8 = ((($s3_4 + 0xf) >> 4) + 1) * $s2_16;
                uint32_t $a1_24 = var_98_33;
                
                if (isp_memopt)
                {
                    system_reg_write(0x7840, $s1_6 + $a1_24);
                    system_reg_write(0x7844, 0);
                    system_reg_write(0x7848, $s1_6 + var_98);
                    system_reg_write(0x784c, 0);
                    system_reg_write(0x7850, $s1_6 + var_98);
                    system_reg_write(0x7854, 0);
                }
                else
                {
                    int32_t $s6_3 = $s3_8 << 1;
                    system_reg_write(0x7840, $s1_6 + $s3_8 + $a1_24);
                    system_reg_write(0x7844, $s2_16);
                    system_reg_write(0x7848, $s1_6 + var_98 + $s6_3);
                    system_reg_write(0x784c, $s2_16);
                    system_reg_write(0x7850, $s1_6 + var_98 + $s6_3 + $s3_8);
                    system_reg_write(0x7854, $s2_16);
                    $s3_8 <<= 2;
                }
                
                system_reg_write(0x7838, 0);
                system_reg_write(0x783c, 1);
                int32_t $s1_7 = $s1_6 + $s3_8;
                
                if (var_94_8 < $s1_7)
                    goto label_1e95c;
                
                uint32_t $a1_40 = var_98_34;
                
                if (!isp_memopt)
                {
                    uint32_t $s3_13 = ((*($s4_5 + 0xec) >> 1) + 7) >> 3 << 3;
                    int32_t $s2_18 = $s3_13 * *($s4_5 + 0xf0);
                    int32_t $s1_8 = $s1_7 + $s2_18;
                        goto label_1e95c;
                    uint32_t $s3_18 = ((*($s4_5 + 0xec) >> 1) + 7) >> 3 << 3;
                    int32_t $s2_20 = $s3_18 * *($s4_5 + 0xf0);
                    int32_t $s2_22 = $s1_8 + ($s2_20 >> 1);
                        goto label_1e95c;
                    uint32_t $s3_23 = ((*($s4_5 + 0xec) >> 5) + 7) >> 3 << 3;
                    int32_t $s1_10 = $s3_23 * *($s4_5 + 0xf0);
                    goto label_1e94c;
                    system_reg_write(0x7858, $s1_7 + $a1_40);
                    system_reg_write(0x785c, $s3_13);
                    
                    if (var_94 < $s1_8)
                    
                    system_reg_write(0x7860, $s1_8 + var_98);
                    system_reg_write(0x7864, $s3_18);
                    
                    if (var_94 < $s2_22)
                    
                    system_reg_write(0x7868, $s2_22 + var_98);
                    system_reg_write(0x786c, $s3_23);
                    $s1_7 = $s2_22 + ($s1_10 >> 5);
                }
                
                system_reg_write(0x7858, $a1_40);
                system_reg_write(0x785c, 0);
                system_reg_write(0x7860, var_98_35);
                system_reg_write(0x7864, 0);
                system_reg_write(0x7868, var_98_36);
                system_reg_write(0x786c, 0);
            label_1e94c:
                $s6_1 = 0;
                
                if (var_94_9 < $s1_7)
                    goto label_1e95c;
            }
            else
            {
                        return 0;
                        goto label_1f3d0;
                if ((uintptr_t)arg2 < 0x800856d5)
                {
                    if ((uintptr_t)arg2 != 0x800456de)
                    
                    if (private_copy_from_user(&var_98, arg3, 8))
                    {
                        $a2_1 = "tx_isp_set_ae_algo_close";
                    }
                    
                    tisp_ae_algo_deinit();
                    tisp_ae_algo_init(0, nullptr);
                    private_kfree(ae_info_mine);
                    private_kfree(ae_statis_mine);
                    return 0;
                }
                
                if ((uintptr_t)arg2 != 0x800856d5)
                {
                        return 0;
                    char* $s2_23 = (char*)(*(*($s7 + 0x2c) + 0xd4)); // Fixed void pointer assignment
                        return 0xfffffff2;
                    if ((uintptr_t)arg2 != 0x800856d6)
                    
                    
                    if (private_copy_from_user(&var_98, arg3, 8))
                    {

                    }
                    
                    uint32_t var_a0_1_1 = var_98_37;
                    int32_t var_a4_1 = var_94_10;
                    int32_t var_a8_1_1 = 0;
                    int32_t $a2_12 =
                    int32_t* $v0_91 = (int32_t*)((char*)$s2_23  + 0x120); // Fixed void pointer arithmetic
                    int32_t $v1_21 = *($v0_91 + 0x90);
                    int32_t $s0_10;
                    uint32_t $s1_13;
                    
                    if ($v1_21 != 1)
                    {
                            return 0xffffffff;
                        if ($v1_21 != 2)
                        {

                        }
                        
                        $s0_10 = *($v0_91 + 0xe8);
                        $s1_13 = $s0_10 / (*($s2_23 + 0x124) << 1);
                    }
                    else
                    {
                        $s1_13 = *($s2_23 + 0x128);
                        $s0_10 = ($s1_13 * *($s2_23 + 0x124)) << 1;
                    }
                    
                    int32_t var_a8_2 = $s0_10;
                    uint32_t var_a0_2_1 = var_98_38;
                    int32_t var_a4_2 = var_94_11;

                    
                    if (var_94_12 >= $s0_10)
                    {
                        return 0;
                        system_reg_write(0x2004, var_98);
                        system_reg_write(0x2008, *($s2_23 + 0x124) << 1);
                        system_reg_write(0x200c, $s1_13);
                    }
                    

                    *((int32_t*)((char*)$s2_23 + 0x17c)) = 0; // Fixed void pointer dereference
                    tisp_s_wdr_en(0);
                    return 0xfffffff2;
                }
                
                char* $v1_14 = (char*)(*(*($s7 + 0x2c) + 0xd4)); // Fixed void pointer assignment
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
                
                var_94_13 = $a2_10;
                var_98_39 = 0;
                $s6_1 = 0;
                
                if (private_copy_to_user(arg3, &var_98_40, 8))
                {
                    goto label_1df8c;
                    $a2_1 = "tx_isp_get_buf";
                    $a1_3 = "sensor type is BT601!\n";
                }
            }
        }
        else if ((uintptr_t)arg2 == 0x800456d0)
        {
                goto label_1df8c;
            if (private_copy_from_user(&var_98, arg3, 4))
            {
                $a2_1 = "tx_isp_video_link_setup";
                $a1_3 = "sensor type is BT601!\n";
            }
            
            uint32_t $a2_4 = var_98_41;
            
            if ($a2_4 >= 2)
            {
                return 0xffffffea;

            }
            
            $s6_1 = 0;
            
            if ($a2_4 != *($s7 + 0x10c))
            {
                int32_t $v0_36 = *(($a2_4 << 3) + 0x7ad50);
                int32_t* $s1_1 = (&configs)[$a2_4 * 2];
                int32_t $s2_2 = 0;
                    char* $v0_39 = (char*)(find_subdev_link_pad($s7 - 0xc, $s1_1)); // Fixed void pointer assignment
                    void* $v0_40;
                    int32_t $a2_7;
                    char* $t0_2 = (char*)($v0_40); // Fixed void pointer assignment
                        int32_t $t1_1 = $s1_1[4];
                            return 0xffffffff;
                
                while ($s2_2 < $v0_36)
                {
                    int32_t (* var_2c_1)(void* arg1, int32_t* arg2) = find_subdev_link_pad;
                    $v0_40 = var_2c_1($s7 - 0xc, &$s1_1[2]);
                    
                    if (!$v0_39)
                        $s2_2 += 1;
                    else if (!$v0_40)
                        $s2_2 += 1;
                    else
                    {
                        
                        if (!(*($v0_39 + 6) & *($v0_40 + 6) & $t1_1))
                        {

                                $a2_7);
                        }
                        
                        uint32_t $v0_44 = *($v0_39 + 7);
                        
                        if ($v0_44 == 4 || *($t0_2 + 7) == 4)
                        {
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
                        
                        char* $t2_1 = (char*)($t0_2 + 8); // Fixed void pointer assignment
                        int32_t $t1_2;
                        
                        if ($v0_46 != 3 || $v0_39 == *($t0_2 + 0xc))
                            $t1_2 = $t1_1 | 1;
                        else
                        {
                            int32_t $t1_3;
                            $t0_2 = subdev_video_destroy_link($t2_1);
                            $t1_2 = $t1_3 | 1;
                        }
                        
                        *((int32_t*)((char*)$v0_39 + 8)) = $v0_39; // Fixed void pointer dereference
                        *((int32_t*)((char*)$v0_39 + 0xc)) = $t0_2; // Fixed void pointer dereference
                        *((int32_t*)((char*)$v0_39 + 0x10)) = $t2_1; // Fixed void pointer dereference
                        *((int32_t*)((char*)$v0_39 + 0x14)) = $t1_2; // Fixed void pointer dereference
                        *((int32_t*)((char*)$v0_39 + 7)) = 3; // Fixed void pointer dereference
                        *((int32_t*)((char*)$t0_2 + 8)) = $t0_2; // Fixed void pointer dereference
                        *((int32_t*)((char*)$t0_2 + 0xc)) = $v0_39; // Fixed void pointer dereference
                        *((int32_t*)((char*)$t0_2 + 0x10)) = $v0_39 + 8; // Fixed void pointer dereference
                        *((int32_t*)((char*)$t0_2 + 0x14)) = $t1_2; // Fixed void pointer dereference
                        *((int32_t*)((char*)$t0_2 + 7)) = 3; // Fixed void pointer dereference
                        $s2_2 += 1;
                    }
                    
                    $s1_1 = &$s1_1[5];
                }
                
                *((int32_t*)((char*)$s7 + 0x10c)) = var_98_42; // Fixed void pointer dereference
                return 0;
            }
        }
        else
        {
            int32_t $a1;
                        return 0;
            
            if ((uintptr_t)arg2 >= 0x800456d1)
            {
                if ((uintptr_t)arg2 == 0x800456d2)
                    $a1 = 1;
                else
                {
                    if ((uintptr_t)arg2 < 0x800456d2)
                        return tx_isp_video_link_destroy.isra.5($s7 - 0xc);
                    
                    $a1 = 0;
                    
                    if ((uintptr_t)arg2 != 0x800456d3)
                }
                
                return tx_isp_video_link_stream($s7 - 0xc, $a1);
            }
            
            if ((uintptr_t)arg2 == 0x80045612)
            {
                $a1 = 1;
            label_1e228:
                return tx_isp_video_s_stream($s7 - 0xc, $a1);
            }
            
            if ((uintptr_t)arg2 == 0x80045613)
            {
                goto label_1e228;
                $a1 = 0;
            }
            
            $s6_1 = 0;
            
            if ((uintptr_t)arg2 == 0x40045626)
            {
                int32_t* i_3 = $s7 + 0x2c;
                    char* $a0_4 = (char*)(*i_3); // Fixed void pointer assignment
                        char* $v0_10 = (char*)(*(*($a0_4 + 0xc4) + 0xc)); // Fixed void pointer assignment
                            int32_t $v0_11 = *($v0_10 + 8);
                                int32_t $v0_13 = $v0_11($a0_4, 0x2000003, &var_98);
                                        return $v0_13;
                
                do
                {
                    
                    if ($a0_4)
                    {
                        
                        if ($v0_10)
                        {
                            
                            if (!$v0_11)
                                i_3 = &i_3[1];
                            else
                            {
                                
                                if (!$v0_13)
                                    i_3 = &i_3[1];
                                else
                                {
                                    i_3 = &i_3[1];
                                    
                                    if ($(uintptr_t)v0_13 != 0xfffffdfd)
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
                
                if (private_copy_to_user(arg3, &var_98_43, 4))
                {
                    goto label_1df8c;
                    $a2_1 = "tx_isp_sensor_get_input";
                    $a1_3 = "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\n";
                }
            }
        }
    }
    return $s6_1;
}

