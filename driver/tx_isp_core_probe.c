#include "include/main.h"


  int32_t tx_isp_core_probe(int32_t* arg1)

{
    void* $v0;
    int32_t $a2;
    $v0 = private_kmalloc(0x218, 0xd0);
    
    if (!$v0)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xfffffff4;
    }
    
    memset($v0, 0, 0x218);
    void* $s2_1 = arg1[0x16];
    int32_t result;
    
    if (!tx_isp_subdev_init(arg1, $v0, &core_subdev_ops))
    {
        uint32_t $a0_4 = *($v0 + 0xc8);
        int32_t $v0_3 = arg1[0x16];
            int32_t $s2_2 = 0;
            void* $s1_1 = $v0_4;
                int32_t $s4_2 = $s2_2 * 0x24;
                    else if ($s2_2 != 1)
        private_spin_lock_init($v0 + 0xdc);
        private_raw_mutex_init($v0 + 0xdc, "&core_dev->mlock", 0);
        *(((void**)((char*)$v0 + 0x154))) = $a0_4; // Fixed void pointer dereference
        *(((void**)((char*)$v0 + 0x138))) = $v0_3; // Fixed void pointer dereference
        void* $v0_4;
        int32_t $a2_2;
        $v0_4 = private_kmalloc($a0_4 * 0xc4, 0xd0);
        
        if ($v0_4)
        {
            memset($v0_4, 0, *($v0 + 0xc8) * 0xc4);
            
            while (true)
            {
                
                if ($s2_2 >= *($v0 + 0x154))
                    break;
                
                *(((void**)((char*)$s1_1 + 0x70))) = $s2_2; // Fixed void pointer dereference
                *(((void**)((char*)$s1_1 + 0x78))) = *($v0 + 0xcc) + $s4_2; // Fixed void pointer dereference
                
                if (*(*($v0 + 0xcc) + $s4_2 + 5))
                {
                    if (!$s2_2)
                        __builtin_memcpy($s1_1 + 0x80, 
                            "\x40\x0a\x00\x00\x00\x08\x00\x00\x80\x00\x00\x00\x80\x00\x00\x00\x01\x00", 
                            0x12);
                        *(((void**)((char*)$s1_1 + 0x88))) = 0x80; // Fixed void pointer dereference
                    else
                    {
                        *(((void**)((char*)$s1_1 + 0x80))) = 0x780; // Fixed void pointer dereference
                        *(((void**)((char*)$s1_1 + 0x84))) = 0x438; // Fixed void pointer dereference
                        *(((void**)((char*)$s1_1 + 0x90))) = $s2_2; // Fixed void pointer dereference
                        *(((void**)((char*)$s1_1 + 0x91))) = $s2_2; // Fixed void pointer dereference
                    }
                    
                    *(((int32_t*)((char*)$s1_1 + 0x74))) = 1; // Fixed void pointer dereference
                    private_spin_lock_init($s1_1 + 0x9c);
                    *(((void**)((char*)$s1_1 + 0x7c))) = $v0; // Fixed void pointer dereference
                    *(*($v0 + 0xcc) + $s4_2 + 0x1c) = ispcore_pad_event_handle;
                    *(*($v0 + 0xcc) + $s4_2 + 0x20) = $s1_1;
                }
                else
                    *(((int32_t*)((char*)$s1_1 + 0x74))) = 0; // Fixed void pointer dereference
                
                $s2_2 += 1;
                $s1_1 += 0xc4;
            }
            
            *(((void**)((char*)$v0 + 0x150))) = $v0_4; // Fixed void pointer dereference
            int32_t* $v0_15;
            int32_t $a2_6;
            $v0_15 = isp_core_tuning_init($v0);
            *(((void**)((char*)$v0 + 0x1bc))) = $v0_15; // Fixed void pointer dereference
            
            if ($v0_15)
            {
                char* $v0_18 = *((char*)$v0 + 0x1bc); // Fixed void pointer arithmetic
                uint32_t isp_clk_1 = get_isp_clk();
                *(((int32_t*)((char*)$v0 + 0xe8))) = 1; // Fixed void pointer dereference
                private_platform_set_drvdata(arg1, $v0);
                *(((void**)((char*)$v0 + 0xd4))) = $v0; // Fixed void pointer dereference
                *(((void**)((char*)$v0 + 0x30))) = *($v0_18 + 0x40c8); // Fixed void pointer dereference
                *(((void**)((char*)$v0 + 0x34))) = &isp_info_proc_fops; // Fixed void pointer dereference
                mdns_y_pspa_cur_bi_wei0_array = $v0;
                sensor_early_init($v0);
                
                if (!isp_clk_1)
                    isp_clk_1 = isp_clk;
                
                isp_clk = isp_clk_1;
                return 0;
            }
            
            isp_printf(); // Fixed: macro call, removed arguments;
            
            if (*($v0 + 0xe8) >= 2)
                ispcore_slake_module($v0);
            
            private_kfree(*($v0 + 0x150));
            *(((int32_t*)((char*)$v0 + 0x158))) = 1; // Fixed void pointer dereference
            *(((int32_t*)((char*)$v0 + 0x150))) = 0; // Fixed void pointer dereference
        }
        else
            isp_printf(2, "Failed to init output channels!\\n", 
                isp_printf(); // Fixed: macro call, removed arguments);
        
        tx_isp_subdev_deinit($v0);
        result = 0xffffffea;
    }
    else
    {
        isp_printf(); // Fixed: macro call, removed arguments\n", *($s2_1 + 2));
        result = 0xfffffff4;
    }
    
    private_kfree($v0);
    return result;
}

