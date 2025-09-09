#include "include/main.h"


  void* frame_channel_unlocked_ioctl(void* arg1, int32_t arg2, void* arg3)

{
    void* const $s0 = *(arg1 + 0x70);
    void* $s2 = arg3;
    void* var_78;
    int32_t var_74;
    void* var_34;
    int32_t $v0_7;
    void* $s1_1;
    
    if (!$s0 || $(uintptr_t)s0 >= 0xfffff001)
    {
        if ((uintptr_t)arg2 == 0x800456c5)
        {
            $s0 = nullptr;
        label_1b910:
            var_78 = nullptr;
            
            if (!$s0 || $(uintptr_t)s0 >= 0xfffff001)
                return 0xffffffea;
            
            if (!$s2 || $(uintptr_t)s2 >= 0xfffff001)
            {
            label_1b988:
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            int32_t $v0_4;
            $v0_4 = private_copy_from_user(&var_78, $s2, 4);
            
            if ($v0_4)
            {
            label_1b9c8:
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xfffffff4;
            }
            
            void* $v0_5;
            int32_t $a2_1;
            $v0_5 = tx_isp_send_event_to_remote(*($s0 + 0x2bc), 0x3000008, &var_78_1);
            
            if (!$v0_5)
                return $v0_5;
            
            if ($(uintptr_t)v0_5 == 0xfffffdfd)
                return nullptr;
            
            isp_printf(); // Fixed: macro call, removed arguments;
            return $v0_5;
        }
        
        if ((uintptr_t)arg2 < 0x800456c6)
        {
            if ((uintptr_t)arg2 == 0x407056c4)
            {
                $s0 = nullptr;
            label_1bab4:
                return frame_channel_vidioc_get_fmt($s0, $s2);
            }
            
            if ((uintptr_t)arg2 >= 0x407056c5)
            {
                if ((uintptr_t)arg2 != 0x80045612 && (uintptr_t)arg2 != 0x80045613)
                    return 0xfffffdfd;
                
                return 0xffffffea;
            }
            
            $s0 = nullptr;
        label_1c41c:
            
            if ((uintptr_t)arg2 != 0x400456bf)
                return 0xfffffdfd;
            
            var_78_2 = nullptr;
            void* $v0_89 = private_wait_for_completion_interruptible($s0 + 0x2d4);
            $s1_1 = $v0_89;
            
            if ($v0_89 >= 0)
                var_78_3 = *($s0 + 0x2d4) + 1;
            else
                var_78_4 = $v0_89;
            
            $v0_7 = private_copy_to_user($s2, &var_78_5, 4);
            goto label_1ba4c;
        }
        
        if ((uintptr_t)arg2 == 0xc044560f)
        {
            $s0 = nullptr;
        label_1bcd0:
            var_34 = nullptr;
            
            if (!$s0 || $(uintptr_t)s0 >= 0xfffff001)
                return 0xffffffea;
            
            if (!$s2 || $(uintptr_t)s2 >= 0xfffff001)
                goto label_1b988;
            
            int32_t $v0_28;
            $v0_28 = private_copy_from_user(&var_78, $s2, 0x44);
            
            if ($v0_28)
                goto label_1b9c8;
            
            arg3 = var_78;
            
            if (var_74 != *($s0 + 0x24))
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            if (arg3 >= *($s0 + 0x20c))
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            arg3 = $s0 + ((arg3 + 0x3a) << 2);
            char* $s1_5 = *((char*)arg3 + 0x24); // Fixed void pointer arithmetic
            
            if (!$s1_5)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            *(((int32_t*)((char*)$s1_5 + 0x4c))) = 1; // Fixed void pointer dereference
            int32_t var_48_4;
            void* var_40_9;
            
            if (var_48_5 != *($s0 + 0x3c))
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            if (var_40_10 != *($s0 + 0x58))
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            if (*($s1_5 + 0x48))
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            int32_t $s4_1 = *($s0 + 0x2c0);
            int32_t var_44;
            *(((void**)((char*)$s1_5 + 0x34))) = var_44_1; // Fixed void pointer dereference
            *(((void**)((char*)$s1_5 + 0x38))) = var_40_11; // Fixed void pointer dereference
            int32_t var_68;
            *(((void**)((char*)$s1_5 + 0x10))) = var_68_3; // Fixed void pointer dereference
            int32_t var_64_2;
            *(((void**)((char*)$s1_5 + 0x14))) = var_64_3; // Fixed void pointer dereference
            int32_t var_60;
            *(((void**)((char*)$s1_5 + 0x18))) = var_60_1; // Fixed void pointer dereference
            int32_t var_6c;
            *(((void**)((char*)$s1_5 + 0xc))) = var_6c_1 & 0xffff1bb8; // Fixed void pointer dereference
            private_dma_sync_single_for_device(nullptr, var_44_2, var_40_12, 2);
            void* $v0_39 = $s1_5 + 0x68;
            
            if (isp_ch0_pre_dequeue_time)
            {
                if (!$s4_1)
                    *(var_44 | 0xa0000000) = 0x12345678;
                
                $v0_39 = $s1_5 + 0x68;
            }
            
            *(((void**)((char*)$s1_5 + 0x68))) = $v0_39; // Fixed void pointer dereference
            *(((void**)((char*)$s1_5 + 0x6c))) = $v0_39; // Fixed void pointer dereference
            *(((void**)((char*)$s1_5 + 0x70))) = var_44_3; // Fixed void pointer dereference
            *(((int32_t*)((char*)$s1_5 + 0x48))) = 2; // Fixed void pointer dereference
            *(((int32_t*)((char*)$s1_5 + 0x4c))) = 2; // Fixed void pointer dereference
            private_mutex_lock($s0 + 0x28);
            __private_spin_lock_irqsave($s0 + 0x2c4, &var_34);
            void** $v1_9 = *($s0 + 0x214);
            *(((void**)((char*)$s0 + 0x214))) = $s1_5 + 0x58; // Fixed void pointer dereference
            *(((void**)((char*)$s1_5 + 0x58))) = $s0 + 0x210; // Fixed void pointer dereference
            *(((void**)((char*)$s1_5 + 0x5c))) = $v1_9; // Fixed void pointer dereference
            *$v1_9 = $s1_5 + 0x58;
            *(((int32_t*)((char*)$s1_5 + 0x48))) = 1; // Fixed void pointer dereference
            *($s0 + 0x218) += 1;
            private_spin_unlock_irqrestore($s0 + 0x2c4, var_34_1);
            
            if (*($s0 + 0x230) & 1)
                __enqueue_in_driver($s1_5);
            
            *(((void**)((char*)$s1_5 + 0x4c))) = 0x1e; // Fixed void pointer dereference
            __fill_v4l2_buffer($s1_5, &var_78_6);
            private_mutex_unlock($s0 + 0x28);
            goto label_1ba40;
        }
        
        if ((uintptr_t)arg2 < 0xc0445610)
        {
            if ((uintptr_t)arg2 != 0xc0145608 && (uintptr_t)arg2 != 0xc0445609)
                return 0xfffffdfd;
            
            return 0xffffffea;
        }
        
        if ((uintptr_t)arg2 == 0xc0445611)
            return 0xffffffea;
        
        $s0 = nullptr;
    }
    else
    {
        if ((uintptr_t)arg2 == 0x800456c5)
            goto label_1b910;
        
        if ((uintptr_t)arg2 < 0x800456c6)
        {
            if ((uintptr_t)arg2 == 0x407056c4)
                goto label_1bab4;
            
            if ((uintptr_t)arg2 < 0x407056c5)
                goto label_1c41c;
            
            if ((uintptr_t)arg2 != 0x80045612)
            {
                if ((uintptr_t)arg2 != 0x80045613)
                    return 0xfffffdfd;
                
                if (!$s2 || $(uintptr_t)s2 >= 0xfffff001)
                    goto label_1b988;
                
                int32_t $v0_24;
                $v0_24 = private_copy_from_user(&var_78, $s2, 4);
                
                if ($v0_24)
                    goto label_1b9c8;
                
                return __frame_channel_vb2_streamoff($s0, var_78);
            }
            
            if (*($s0 + 0x2d0) != 3)
            {
                isp_printf(); // Fixed: macro call, removed arguments);
                return 0xffffffff;
            }
            
            if (!$s2 || $(uintptr_t)s2 >= 0xfffff001)
                goto label_1b988;
            
            int32_t $v0_13;
            $v0_13 = private_copy_from_user(&var_78_7, $s2, 4);
            
            if ($v0_13)
                goto label_1b9c8;
            
            if (*($s0 + 0x24) != var_78_8)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            if (*($s0 + 0x230) & 1)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xfffffff0;
            }
            
            char* $s1_3 = (char*)(*($s0 + 0x210) - 0x58); // Fixed void pointer assignment
            
            while ($s1_3 + 0x58 != $s0 + 0x210)
            {
                __enqueue_in_driver($s1_3);
                $s1_3 = *($s1_3 + 0x58) - 0x58;
            }
            
            *($s0 + 0x230) |= 1;
            void* $v0_20;
            int32_t $a2_3;
            $v0_20 = tx_isp_send_event_to_remote(*($s0 + 0x2bc), 0x3000003, 0);
            
            if (!$v0_20 || $(uintptr_t)v0_20 == 0xfffffdfd)
            {
                *(((int32_t*)((char*)$s0 + 0x2d0))) = 4; // Fixed void pointer dereference
                isp_printf(); // Fixed: macro call, removed arguments;
                return nullptr;
            }
            
            isp_printf(); // Fixed: macro call, removed arguments;
            __vb2_queue_cancel($s0 + 0x24);
            *($s0 + 0x230) &= 0xfe;
            return $v0_20;
        }
        
        if ((uintptr_t)arg2 == 0xc044560f)
            goto label_1bcd0;
        
        if ((uintptr_t)arg2 < 0xc0445610)
        {
                void* $v0_74 = var_78;
            if ((uintptr_t)arg2 != 0xc0145608)
            {
                if ((uintptr_t)arg2 != 0xc0445609)
                    return 0xfffffdfd;
                
                if (!$s2 || $(uintptr_t)s2 >= 0xfffff001)
                    goto label_1b988;
                
                int32_t $v0_72;
                $v0_72 = private_copy_from_user(&var_78, $s2, 0x44);
                
                if ($v0_72)
                    goto label_1b9c8;
                
                
                if (var_74 != *($s0 + 0x24))
                    goto label_1c210;
                
                if ($v0_74 >= *($s0 + 0x20c))
                {
                    isp_printf(); // Fixed: macro call, removed arguments;
                    return 0xffffffea;
                }
                
                __fill_v4l2_buffer(*($s0 + (($v0_74 + 0x3a) << 2) + 0x24), &var_78_9);
            label_1ba40:
                $v0_7 = private_copy_to_user($s2, &var_78_10, 0x44);
                $s1_1 = nullptr;
            label_1ba4c:
                
                if (!$v0_7)
                    return $s1_1;
                
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xfffffff4;
            }
            
            if (!$s2 || $(uintptr_t)s2 >= 0xfffff001)
                goto label_1b988;
            
            int32_t $v0_46;
            $v0_46 = private_copy_from_user(&var_78_11, $s2, 0x14);
            
            if ($v0_46)
                goto label_1b9c8;
            
            if (*($s0 + 0x230) & 1)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xfffffff0;
            }
            
            int32_t $a1_5 = *($s0 + 0x20c);
            int32_t var_70_2;
            int32_t $v1_4;
            
            if (var_78_12 && !$a1_5 && *($s0 + 0x3c) == var_70_3)
                $v1_4 = *($s0 + 0x3c);
            else
            {
                arg3 = __vb2_queue_free($s0 + 0x24, $a1_5);
                
                if (!var_78)
                    return nullptr;
                
                $v1_4 = *($s0 + 0x3c);
            }
            
            void* $v0_51 = var_78_13;
            
            if ($v1_4 != var_70_4)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            void* $v1_11 = 0x40;
            
            if ($(uintptr_t)v0_51 < 0x41)
                $v1_11 = $v0_51;
            
            void* $s1_6;
            
            if (isp_ch0_pre_dequeue_time <= 0)
                $s1_6 = nullptr;
            else
            {
                $s1_6 = nullptr;
                
                if (!*($s0 + 0x2c0))
                {
                    if ($v1_11 >= 2)
                    {
                        isp_printf(); // Fixed: macro call, removed arguments;
                        return 0xffffffea;
                    }
                    
                    $s1_6 = nullptr;
                }
            }
            
            while (true)
            {
                if ($v1_11 == $s1_6)
                {
                    $s1_6 = $v1_11;
                    break;
                }
                
                int32_t* $v0_64;
                int32_t $a2_8;
                $v0_64 = private_kmalloc(*($s0 + 0x34), 0xd0);
                
                if (!$v0_64)
                {
                    isp_printf(); // Fixed: macro call, removed arguments,STATE(%d),%d", $a2_8);
                    break;
                }
                
                memset($v0_64, 0, *($s0 + 0x34));
                $v0_64[0x12] = 0;
                $v0_64[0x11] = $s0 + 0x24;
                *$v0_64 = *($s0 + 0x20c) + $s1_6;
                $v0_64[1] = *($s0 + 0x24);
                $v0_64[0xc] = *($s0 + 0x3c);
                *($s0 + (($s1_6 + *($s0 + 0x20c) + 0x3a) << 2) + 0x24) = $v0_64;
                $s1_6 += 1;
            }
            
            int32_t $a2_10 = isp_printf(); // Fixed: macro call, removed arguments;
            
            if (!$s1_6)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xfffffff4;
            }
            
            var_34_2 = $s1_6;
            
            if ($s1_6 < $v1_11)
            {
                __vb2_queue_free($s0 + 0x24, $s1_6);
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xfffffff4;
            }
            
            *(((void**)((char*)$s0 + 0x20c))) = $s1_6; // Fixed void pointer dereference
            var_78_14 = $s1_6;
            void* $v0_66;
            int32_t $a2_13;
            $v0_66 = tx_isp_send_event_to_remote(*($s0 + 0x2bc), 0x3000008, &var_34_3);
            
            if ($v0_66 && $(uintptr_t)v0_66 != 0xfffffdfd)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return $v0_66;
            }
            
            int32_t $v0_67;
            int32_t $a2_14;
            $v0_67 = private_copy_to_user($s2, &var_78_15, 0x14);
            
            if ($v0_67)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xfffffff4;
            }
            
            if ($s0 + (uintptr_t)0x23c >= 0xfffff001 || $s0 + (uintptr_t)0x40 >= 0xfffff001)
                return nullptr;
            
            *(((void**)((char*)$s0 + 0x40))) = *($s0 + 0x23c); // Fixed void pointer dereference
            memcpy($s0 + 0x44, $s0 + 0x240, 0x30);
            return nullptr;
        }
        
        if ((uintptr_t)arg2 == 0xc0445611)
        {
            if (!$s2 || $(uintptr_t)s2 >= 0xfffff001)
                goto label_1b988;
            
            int32_t $v0_77;
            $v0_77 = private_copy_from_user(&var_78, $s2, 0x44);
            
            if ($v0_77)
                goto label_1b9c8;
            
            if (var_74 != *($s0 + 0x24))
            {
            label_1c210:
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
            
            var_34_4 = nullptr;
            void* $s4_4 = nullptr;
            void* $s3_6 = nullptr;
            int32_t $v0_79 = *($s0 + 0x230);
            
            while (true)
            {
                if (!($v0_79 & 1))
                {
                    isp_printf(); // Fixed: macro call, removed arguments);
                    return 0xffffffea;
                }
                
                __private_spin_lock_irqsave($s0 + 0x224, &var_34_5);
                int32_t $fp_1;
                int32_t* $v0_83;
                
                if (*($s0 + 0x2c0))
                {
                        void* $a0_29 = *$v0_83;
                    $v0_83 = *($s0 + 0x21c);
                label_1c310:
                    $fp_1 = 0;
                    
                    if ($v0_83 != $s0 + 0x21c)
                    {
                        void** $v1_20 = $v0_83[1];
                        $s4_4 = &$v0_83[-0x18];
                        *(((void**)((char*)$a0_29 + 4))) = $v1_20; // Fixed void pointer dereference
                        *$v1_20 = $a0_29;
                        *$v0_83 = 0x100100;
                        $v0_83[1] = 0x200200;
                        *($s0 + 0x224) -= 1;
                        $fp_1 = 1;
                    }
                }
                else
                {
                    if (!isp_ch0_pre_dequeue_interrupt_process)
                    {
                        $v0_83 = *($s0 + 0x21c);
                        goto label_1c310;
                    }
                    
                    if (!*($s0 + 0x238))
                    {
                        $v0_83 = *($s0 + 0x21c);
                        goto label_1c310;
                    }
                    
                    if (*data_b2c68_1 != 2)
                    {
                        $v0_83 = *($s0 + 0x21c);
                        goto label_1c310;
                    }
                    
                    *(((int32_t*)((char*)$s0 + 0x238))) = 0; // Fixed void pointer dereference
                    $s4_4 = &ch0_pre_dequeue_vb;
                    $fp_1 = 1;
                }
                private_spin_unlock_irqrestore($s0 + 0x224, var_34_6);
                
                if ($fp_1)
                    break;
                
                void* $v0_87;
                int32_t $a2_19;
                $v0_87 = private_wait_event_interruptible($s0 + 0x228, check_state, $s0 + 0x24);
                $s3_6 = $v0_87;
                
                if ($(uintptr_t)s3_6 == 0xfffffe00)
                    $v0_79 = *($s0 + 0x230);
                else
                {
                    if ($s3_6)
                    {
                        isp_printf(); // Fixed: macro call, removed arguments;
                        break;
                    }
                    
                    $v0_79 = *($s0 + 0x230);
                }
            }
            
            if ($s3_6 < 0)
                return $s3_6;
            
            __fill_v4l2_buffer($s4_4, &var_78_16);
            *(((int32_t*)((char*)$s4_4 + 0x48))) = 0; // Fixed void pointer dereference
            goto label_1ba40;
        }
    }
    
    if ((uintptr_t)arg2 != 0xc07056c3)
        return 0xfffffdfd;
    
    return frame_channel_vidioc_set_fmt($s0, $s2);
}

