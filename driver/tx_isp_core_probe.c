#include "include/main.h"


  int32_t tx_isp_core_probe(int32_t* arg1)

{
    void* $v0;
    int32_t $a2;
    $v0 = private_kmalloc(0x218, 0xd0);
    
    if (!$v0)
    {
        isp_printf(2, "addr ctl is 0x%x\\n", $a2);
        return 0xfffffff4;
    }
    
    memset($v0, 0, 0x218);
    void* $s2_1 = arg1[0x16];
    int32_t result;
    
    if (!tx_isp_subdev_init(arg1, $v0, &core_subdev_ops))
    {
        private_spin_lock_init($v0 + 0xdc);
        private_raw_mutex_init($v0 + 0xdc, "&core_dev->mlock", 0);
        uint32_t $a0_4 = *($v0 + 0xc8);
        int32_t $v0_3 = arg1[0x16];
        *($v0 + 0x154) = $a0_4;
        *($v0 + 0x138) = $v0_3;
        void* $v0_4;
        int32_t $a2_2;
        $v0_4 = private_kmalloc($a0_4 * 0xc4, 0xd0);
        
        if ($v0_4)
        {
            memset($v0_4, 0, *($v0 + 0xc8) * 0xc4);
            int32_t $s2_2 = 0;
            void* $s1_1 = $v0_4;
            
            while (true)
            {
                int32_t $s4_2 = $s2_2 * 0x24;
                
                if ($s2_2 >= *($v0 + 0x154))
                    break;
                
                *($s1_1 + 0x70) = $s2_2;
                *($s1_1 + 0x78) = *($v0 + 0xcc) + $s4_2;
                
                if (*(*($v0 + 0xcc) + $s4_2 + 5))
                {
                    if (!$s2_2)
                        __builtin_memcpy($s1_1 + 0x80, 
                            "\\x40\\x0a\\x00\\x00\\x00\\x08\\x00\\x00\\x80\\x00\\x00\\x00\\x80\\x00\\x00\\x00\\x01\\x00", 
                            0x12);
                    else if ($s2_2 != 1)
                        *($s1_1 + 0x88) = 0x80;
                    else
                    {
                        *($s1_1 + 0x80) = 0x780;
                        *($s1_1 + 0x84) = 0x438;
                        *($s1_1 + 0x90) = $s2_2;
                        *($s1_1 + 0x91) = $s2_2;
                    }
                    
                    *($s1_1 + 0x74) = 1;
                    private_spin_lock_init($s1_1 + 0x9c);
                    *($s1_1 + 0x7c) = $v0;
                    *(*($v0 + 0xcc) + $s4_2 + 0x1c) = ispcore_pad_event_handle;
                    *(*($v0 + 0xcc) + $s4_2 + 0x20) = $s1_1;
                }
                else
                    *($s1_1 + 0x74) = 0;
                
                $s2_2 += 1;
                $s1_1 += 0xc4;
            }
            
            *($v0 + 0x150) = $v0_4;
            int32_t* $v0_15;
            int32_t $a2_6;
            $v0_15 = isp_core_tuning_init($v0);
            *($v0 + 0x1bc) = $v0_15;
            
            if ($v0_15)
            {
                *($v0 + 0xe8) = 1;
                private_platform_set_drvdata(arg1, $v0);
                void* $v0_18 = *($v0 + 0x1bc);
                *($v0 + 0xd4) = $v0;
                *($v0 + 0x30) = *($v0_18 + 0x40c8);
                *($v0 + 0x34) = &isp_info_proc_fops;
                mdns_y_pspa_cur_bi_wei0_array = $v0;
                sensor_early_init($v0);
                uint32_t isp_clk_1 = get_isp_clk();
                
                if (!isp_clk_1)
                    isp_clk_1 = isp_clk;
                
                isp_clk = isp_clk_1;
                return 0;
            }
            
            isp_printf(2, "Failed to init tuning module!\\n", $a2_6);
            
            if (*($v0 + 0xe8) >= 2)
                ispcore_slake_module($v0);
            
            private_kfree(*($v0 + 0x150));
            *($v0 + 0x158) = 1;
            *($v0 + 0x150) = 0;
        }
        else
            isp_printf(2, "Failed to init output channels!\\n", 
                isp_printf(2, "addr ctl is 0x%x\\n", $a2_2));
        
        tx_isp_subdev_deinit($v0);
        result = 0xffffffea;
    }
    else
    {
        isp_printf(2, "Failed to init isp module(%d.%d)\\n", *($s2_1 + 2));
        result = 0xfffffff4;
    }
    
    private_kfree($v0);
    return result;
}

