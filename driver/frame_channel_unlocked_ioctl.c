#include "include/main.h"


  void* frame_channel_unlocked_ioctl(void* arg1, int32_t arg2, void* arg3)

{
    void* const $s0 = *(arg1 + 0x70);
    void* $s2 = arg3;
    void* var_78_3;
    int32_t var_74_3;
    void* var_34_4;
    int32_t $v0_7;
    void* $s1_1;
    
    if (!$s0 || $s0 >= 0xfffff001)
    {
        if (arg2 == 0x800456c5)
        {
            $s0 = nullptr;
        label_1b910:
            var_78_4 = nullptr;
            
            if (!$s0 || $s0 >= 0xfffff001)
                return 0xffffffea;
            
            if (!$s2 || $s2 >= 0xfffff001)
            {
            label_1b988:
                isp_printf(2, "Err [VIC_INT] : dma syfifo ovf!!!\\n", arg3);
                return 0xffffffea;
            }
            
            int32_t $v0_4;
            $v0_4 = private_copy_from_user(&var_78_5, $s2, 4);
            
            if ($v0_4)
            {
            label_1b9c8:
                isp_printf(2, "\\t\\t\\t "snapraw"  is cmd; \\n", arg3);
                return 0xfffffff4;
            }
            
            void* $v0_5;
            int32_t $a2_1;
            $v0_5 = tx_isp_send_event_to_remote(*($s0 + 0x2bc), 0x3000008, &var_78_6);
            
            if (!$v0_5)
                return $v0_5;
            
            if ($v0_5 == 0xfffffdfd)
                return nullptr;
            
            isp_printf(2, "VIC_ADDR_DMA_CONTROL : 0x%x\\n", $a2_1);
            return $v0_5;
        }
        
        if (arg2 < 0x800456c6)
        {
            if (arg2 == 0x407056c4)
            {
                $s0 = nullptr;
            label_1bab4:
                return frame_channel_vidioc_get_fmt($s0, $s2);
            }
            
            if (arg2 >= 0x407056c5)
            {
                if (arg2 != 0x80045612 && arg2 != 0x80045613)
                    return 0xfffffdfd;
                
                return 0xffffffea;
            }
            
            $s0 = nullptr;
        label_1c41c:
            
            if (arg2 != 0x400456bf)
                return 0xfffffdfd;
            
            var_78_7 = nullptr;
            void* $v0_89 = private_wait_for_completion_interruptible($s0 + 0x2d4);
            $s1_1 = $v0_89;
            
            if ($v0_89 >= 0)
                var_78_8 = *($s0 + 0x2d4) + 1;
            else
                var_78_9 = $v0_89;
            
            $v0_7 = private_copy_to_user($s2, &var_78_10, 4);
            goto label_1ba4c;
        }
        
        if (arg2 == 0xc044560f)
        {
            $s0 = nullptr;
        label_1bcd0:
            var_34_5 = nullptr;
            
            if (!$s0 || $s0 >= 0xfffff001)
                return 0xffffffea;
            
            if (!$s2 || $s2 >= 0xfffff001)
                goto label_1b988;
            
            int32_t $v0_28;
            $v0_28 = private_copy_from_user(&var_78_11, $s2, 0x44);
            
            if ($v0_28)
                goto label_1b9c8;
            
            arg3 = var_78_12;
            
            if (var_74_4 != *($s0 + 0x24))
            {
                isp_printf(2, "Err [VIC_INT] : frame asfifo ovf!!!!!\\n", arg3);
                return 0xffffffea;
            }
            
            if (arg3 >= *($s0 + 0x20c))
            {
                isp_printf(2, "Err [VIC_INT] : hor err ch0 !!!!! 0x3a8 = 0x%08x\\n", arg3);
                return 0xffffffea;
            }
            
            arg3 = $s0 + ((arg3 + 0x3a) << 2);
            void* $s1_5 = *(arg3 + 0x24);
            
            if (!$s1_5)
            {
                isp_printf(2, "Err [VIC_INT] : hor err ch1 !!!!!\\n", arg3);
                return 0xffffffea;
            }
            
            *($s1_5 + 0x4c) = 1;
            int32_t var_48_13;
            void* var_40_14;
            
            if (var_48_14 != *($s0 + 0x3c))
            {
                isp_printf(2, "Err [VIC_INT] : hor err ch2 !!!!!\\n", var_40_15);
                return 0xffffffea;
            }
            
            if (var_40_16 != *($s0 + 0x58))
            {
                isp_printf(2, "Err [VIC_INT] : hor err ch3 !!!!!\\n", var_40_17);
                return 0xffffffea;
            }
            
            if (*($s1_5 + 0x48))
            {
                isp_printf(2, "Err [VIC_INT] : ver err ch0 !!!!!\\n", var_40_18);
                return 0xffffffea;
            }
            
            int32_t $s4_1 = *($s0 + 0x2c0);
            int32_t var_44_6;
            *($s1_5 + 0x34) = var_44_7;
            *($s1_5 + 0x38) = var_40_19;
            int32_t var_68_6;
            *($s1_5 + 0x10) = var_68_7;
            int32_t var_64_6;
            *($s1_5 + 0x14) = var_64_7;
            int32_t var_60_4;
            *($s1_5 + 0x18) = var_60_5;
            int32_t var_6c_5;
            *($s1_5 + 0xc) = var_6c_6 & 0xffff1bb8;
            private_dma_sync_single_for_device(nullptr, var_44_8, var_40_20, 2);
            void* $v0_39 = $s1_5 + 0x68;
            
            if (isp_ch0_pre_dequeue_time)
            {
                if (!$s4_1)
                    *(var_44_9 | 0xa0000000) = 0x12345678;
                
                $v0_39 = $s1_5 + 0x68;
            }
            
            *($s1_5 + 0x68) = $v0_39;
            *($s1_5 + 0x6c) = $v0_39;
            *($s1_5 + 0x70) = var_44_10;
            *($s1_5 + 0x48) = 2;
            *($s1_5 + 0x4c) = 2;
            private_mutex_lock($s0 + 0x28);
            __private_spin_lock_irqsave($s0 + 0x2c4, &var_34_6);
            void** $v1_9 = *($s0 + 0x214);
            *($s0 + 0x214) = $s1_5 + 0x58;
            *($s1_5 + 0x58) = $s0 + 0x210;
            *($s1_5 + 0x5c) = $v1_9;
            *$v1_9 = $s1_5 + 0x58;
            *($s1_5 + 0x48) = 1;
            *($s0 + 0x218) += 1;
            private_spin_unlock_irqrestore($s0 + 0x2c4, var_34_7);
            
            if (*($s0 + 0x230) & 1)
                __enqueue_in_driver($s1_5);
            
            *($s1_5 + 0x4c) = 0x1e;
            __fill_v4l2_buffer($s1_5, &var_78_13);
            private_mutex_unlock($s0 + 0x28);
            goto label_1ba40;
        }
        
        if (arg2 < 0xc0445610)
        {
            if (arg2 != 0xc0145608 && arg2 != 0xc0445609)
                return 0xfffffdfd;
            
            return 0xffffffea;
        }
        
        if (arg2 == 0xc0445611)
            return 0xffffffea;
        
        $s0 = nullptr;
    }
    else
    {
        if (arg2 == 0x800456c5)
            goto label_1b910;
        
        if (arg2 < 0x800456c6)
        {
            if (arg2 == 0x407056c4)
                goto label_1bab4;
            
            if (arg2 < 0x407056c5)
                goto label_1c41c;
            
            if (arg2 != 0x80045612)
            {
                if (arg2 != 0x80045613)
                    return 0xfffffdfd;
                
                if (!$s2 || $s2 >= 0xfffff001)
                    goto label_1b988;
                
                int32_t $v0_24;
                $v0_24 = private_copy_from_user(&var_78_14, $s2, 4);
                
                if ($v0_24)
                    goto label_1b9c8;
                
                return __frame_channel_vb2_streamoff($s0, var_78_15);
            }
            
            if (*($s0 + 0x2d0) != 3)
            {
                isp_printf(2, "Err [VIC_INT] : ver err ch3 !!!!!\\n", *($s0 + 0x2c0));
                return 0xffffffff;
            }
            
            if (!$s2 || $s2 >= 0xfffff001)
                goto label_1b988;
            
            int32_t $v0_13;
            $v0_13 = private_copy_from_user(&var_78_16, $s2, 4);
            
            if ($v0_13)
                goto label_1b9c8;
            
            if (*($s0 + 0x24) != var_78_17)
            {
                isp_printf(2, "snapraw timeout!\\n", arg3);
                return 0xffffffea;
            }
            
            if (*($s0 + 0x230) & 1)
            {
                isp_printf(2, "Err [VIC_INT] : hvf err !!!!!\\n", arg3);
                return 0xfffffff0;
            }
            
            void* $s1_3 = *($s0 + 0x210) - 0x58;
            
            while ($s1_3 + 0x58 != $s0 + 0x210)
            {
                __enqueue_in_driver($s1_3);
                $s1_3 = *($s1_3 + 0x58) - 0x58;
            }
            
            *($s0 + 0x230) |= 1;
            void* $v0_20;
            int32_t $a2_3;
            $v0_20 = tx_isp_send_event_to_remote(*($s0 + 0x2bc), 0x3000003, 0);
            
            if (!$v0_20 || $v0_20 == 0xfffffdfd)
            {
                *($s0 + 0x2d0) = 4;
                isp_printf(0, "Err [VIC_INT] : dvp hcomp err!!!!\\n", $a2_3);
                return nullptr;
            }
            
            isp_printf(2, "VIC_ADDR_DMA_CONTROL : 0x%x\\n", $a2_3);
            __vb2_queue_cancel($s0 + 0x24);
            *($s0 + 0x230) &= 0xfe;
            return $v0_20;
        }
        
        if (arg2 == 0xc044560f)
            goto label_1bcd0;
        
        if (arg2 < 0xc0445610)
        {
            if (arg2 != 0xc0145608)
            {
                if (arg2 != 0xc0445609)
                    return 0xfffffdfd;
                
                if (!$s2 || $s2 >= 0xfffff001)
                    goto label_1b988;
                
                int32_t $v0_72;
                $v0_72 = private_copy_from_user(&var_78_18, $s2, 0x44);
                
                if ($v0_72)
                    goto label_1b9c8;
                
                void* $v0_74 = var_78_19;
                
                if (var_74_5 != *($s0 + 0x24))
                    goto label_1c210;
                
                if ($v0_74 >= *($s0 + 0x20c))
                {
                    isp_printf(2, "Info[VIC_MDAM_IRQ] : channel[%d] frame done\\n", arg3);
                    return 0xffffffea;
                }
                
                __fill_v4l2_buffer(*($s0 + (($v0_74 + 0x3a) << 2) + 0x24), &var_78_20);
            label_1ba40:
                $v0_7 = private_copy_to_user($s2, &var_78_21, 0x44);
                $s1_1 = nullptr;
            label_1ba4c:
                
                if (!$v0_7)
                    return $s1_1;
                
                isp_printf(2, "Can\'t ops the node!\\n", arg3);
                return 0xfffffff4;
            }
            
            if (!$s2 || $s2 >= 0xfffff001)
                goto label_1b988;
            
            int32_t $v0_46;
            $v0_46 = private_copy_from_user(&var_78_22, $s2, 0x14);
            
            if ($v0_46)
                goto label_1b9c8;
            
            if (*($s0 + 0x230) & 1)
            {
                isp_printf(2, "streamoff", arg3);
                return 0xfffffff0;
            }
            
            int32_t $a1_5 = *($s0 + 0x20c);
            int32_t var_70_8;
            int32_t $v1_4;
            
            if (var_78_23 && !$a1_5 && *($s0 + 0x3c) == var_70_9)
                $v1_4 = *($s0 + 0x3c);
            else
            {
                arg3 = __vb2_queue_free($s0 + 0x24, $a1_5);
                
                if (!var_78_24)
                    return nullptr;
                
                $v1_4 = *($s0 + 0x3c);
            }
            
            void* $v0_51 = var_78_25;
            
            if ($v1_4 != var_70_10)
            {
                isp_printf(2, "%s[%d]: invalid parameter\\n", arg3);
                return 0xffffffea;
            }
            
            void* $v1_11 = 0x40;
            
            if ($v0_51 < 0x41)
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
                        isp_printf(2, "%s[%d]: %s\\n", nullptr);
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
                    isp_printf(2, "%s[%d] SET ERR GPIO(%d),STATE(%d),%d", $a2_8);
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
            
            int32_t $a2_10 = isp_printf(0, "line : %d; bank_addr:0x%x; addr:0x%x\\n", $s1_6);
            
            if (!$s1_6)
            {
                isp_printf(2, "line = %d, i=%d ;num = %d;busy_buf_count %d\\n", $a2_10);
                return 0xfffffff4;
            }
            
            var_34_8 = $s1_6;
            
            if ($s1_6 < $v1_11)
            {
                __vb2_queue_free($s0 + 0x24, $s1_6);
                isp_printf(2, "function: %s ; vic dma addrrss error!!!\\n", $v1_11);
                return 0xfffffff4;
            }
            
            *($s0 + 0x20c) = $s1_6;
            var_78_26 = $s1_6;
            void* $v0_66;
            int32_t $a2_13;
            $v0_66 = tx_isp_send_event_to_remote(*($s0 + 0x2bc), 0x3000008, &var_34_9);
            
            if ($v0_66 && $v0_66 != 0xfffffdfd)
            {
                isp_printf(2, "VIC_ADDR_DMA_CONTROL : 0x%x\\n", $a2_13);
                return $v0_66;
            }
            
            int32_t $v0_67;
            int32_t $a2_14;
            $v0_67 = private_copy_to_user($s2, &var_78_27, 0x14);
            
            if ($v0_67)
            {
                isp_printf(2, "Can\'t ops the node!\\n", $a2_14);
                return 0xfffffff4;
            }
            
            if ($s0 + 0x23c >= 0xfffff001 || $s0 + 0x40 >= 0xfffff001)
                return nullptr;
            
            *($s0 + 0x40) = *($s0 + 0x23c);
            memcpy($s0 + 0x44, $s0 + 0x240, 0x30);
            return nullptr;
        }
        
        if (arg2 == 0xc0445611)
        {
            if (!$s2 || $s2 >= 0xfffff001)
                goto label_1b988;
            
            int32_t $v0_77;
            $v0_77 = private_copy_from_user(&var_78_28, $s2, 0x44);
            
            if ($v0_77)
                goto label_1b9c8;
            
            if (var_74_6 != *($s0 + 0x24))
            {
            label_1c210:
                isp_printf(2, "busy_buf null; busy_buf_count= %d\\n", arg3);
                return 0xffffffea;
            }
            
            var_34_10 = nullptr;
            void* $s4_4 = nullptr;
            void* $s3_6 = nullptr;
            int32_t $v0_79 = *($s0 + 0x230);
            
            while (true)
            {
                if (!($v0_79 & 1))
                {
                    isp_printf(2, "Err [VIC_INT] : ver err ch1 !!!!!\\n", *($s0 + 0x2c0));
                    return 0xffffffea;
                }
                
                __private_spin_lock_irqsave($s0 + 0x224, &var_34_11);
                int32_t $fp_1;
                int32_t* $v0_83;
                
                if (*($s0 + 0x2c0))
                {
                    $v0_83 = *($s0 + 0x21c);
                label_1c310:
                    $fp_1 = 0;
                    
                    if ($v0_83 != $s0 + 0x21c)
                    {
                        void** $v1_20 = $v0_83[1];
                        void* $a0_29 = *$v0_83;
                        $s4_4 = &$v0_83[-0x18];
                        *($a0_29 + 4) = $v1_20;
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
                    
                    if (*data_b2c68_2 != 2)
                    {
                        $v0_83 = *($s0 + 0x21c);
                        goto label_1c310;
                    }
                    
                    *($s0 + 0x238) = 0;
                    $s4_4 = &ch0_pre_dequeue_vb;
                    $fp_1 = 1;
                }
                private_spin_unlock_irqrestore($s0 + 0x224, var_34_12);
                
                if ($fp_1)
                    break;
                
                void* $v0_87;
                int32_t $a2_19;
                $v0_87 = private_wait_event_interruptible($s0 + 0x228, check_state, $s0 + 0x24);
                $s3_6 = $v0_87;
                
                if ($s3_6 == 0xfffffe00)
                    $v0_79 = *($s0 + 0x230);
                else
                {
                    if ($s3_6)
                    {
                        isp_printf(2, "Err [VIC_INT] : ver err ch2 !!!!!\\n", $a2_19);
                        break;
                    }
                    
                    $v0_79 = *($s0 + 0x230);
                }
            }
            
            if ($s3_6 < 0)
                return $s3_6;
            
            __fill_v4l2_buffer($s4_4, &var_78_29);
            *($s4_4 + 0x48) = 0;
            goto label_1ba40;
        }
    }
    
    if (arg2 != 0xc07056c3)
        return 0xfffffdfd;
    
    return frame_channel_vidioc_set_fmt($s0, $s2);
}

