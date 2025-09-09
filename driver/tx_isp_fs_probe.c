#include "include/main.h"


  int32_t tx_isp_fs_probe(int32_t* arg1)

{
    void* $v0;
    int32_t $a2;
        return 0xfffffff4;
    $v0 = private_kmalloc(0xe8, 0xd0);
    
    if (!$v0)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    memset($v0, 0, 0xe8);
    char* $s2_1 = (char*)(arg1[0x16]); // Fixed void pointer assignment
    int32_t result;
    
    if (!tx_isp_subdev_init(arg1, $v0, &fs_subdev_ops))
    {
        uint32_t $a0_2 = *($v0 + 0xc8);
            return 0;
        *((int32_t*)((char*)$v0 + 0xe0)) = $a0_2; // Fixed void pointer dereference
        
        if (!$a0_2)
        {
        label_1c670:
            *((int32_t*)((char*)$v0 + 0xe4)) = 1; // Fixed void pointer dereference
            private_platform_set_drvdata(arg1, $v0);
            *((int32_t*)((char*)$v0 + 0x34)) = &isp_framesource_fops; // Fixed void pointer dereference
            *((int32_t*)((char*)$v0 + 0xd4)) = $v0; // Fixed void pointer dereference
        }
        
        int32_t $v0_3 = private_kmalloc($a0_2 * 0x2ec, 0xd0);
        int32_t $a2_2 = *($v0 + 0xe0);
        *((int32_t*)((char*)$v0 + 0xdc)) = $v0_3; // Fixed void pointer dereference
        memset($v0_3, 0, 0x2ec * $a2_2);
        int32_t $s2_2 = 0;
        
        while (true)
        {
                goto label_1c670;
            int32_t $s4_1 = $s2_2 * 0x2ec;
            int32_t* $s0_2 = *($v0 + 0xdc) + $s4_1;
            char* $s6_1 = (char*)($s2_2 * 0x24 + *($v0 + 0xcc)); // Fixed void pointer assignment
                uint32_t $a2_4 = *($s6_1 + 4);
            if ($s2_2 >= *($v0 + 0xe0))
            
            
            if (!$s0_2)
                result = 0xffffffea;
            else if ($(uintptr_t)s0_2 >= 0xfffff001)
                result = 0xffffffea;
            else if (!$s6_1 || $(uintptr_t)s6_1 >= 0xfffff001)
                result = 0xffffffea;
            else
            {
                $s0_2[0xaf] = $s6_1;
                $s0_2[0xb0] = $a2_4;
                
                if (*($s6_1 + 5))
                {
                    sprintf(&$s0_2[0xab], "Err [VIC_INT] : mipi fid asfifo ovf!!!\n");
                    *$s0_2 = 0xff;
                    $s0_2[2] = &fs_channel_ops;
                    $s0_2[1] = &$s0_2[0xab];
                    
                    if (private_misc_register($s0_2) < 0)
                    {
                        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                        result = 0xfffffffe;
                    }
                    else if (&$s0_2[9] < 0xfffff001)
                    {
                        memset(&$s0_2[9], 0, 0x218);
                        $s0_2[0xf] = 2;
                        $s0_2[0xd] = 0x84;
                        $s0_2[0xe] = 0x2000;
                        $s0_2[0x84] = &$s0_2[0x84];
                        $s0_2[0x85] = &$s0_2[0x84];
                        $s0_2[0x87] = &$s0_2[0x87];
                        $s0_2[0x88] = &$s0_2[0x87];
                        $s0_2[9] = 1;
                        private_spin_lock_init(&$s0_2[0x89]);
                        private_raw_mutex_init(&$s0_2[0xa], 
                            "Err [VIC_INT] : mipi ch1 hcomp err !!!\n", 0);
                        $s0_2[0x86] = 0;
                        $s0_2[0x89] = 0;
                        private_init_waitqueue_head(&$s0_2[0x8a]);
                        private_spin_lock_init(&$s0_2[0xb1]);
                        private_raw_mutex_init(&$s0_2[0xb1], 
                            "Err [VIC_INT] : mipi ch2 hcomp err !!!\n", 0);
                        private_init_completion(&$s0_2[0xb5]);
                        *((int32_t*)((char*)$s6_1 + 0x1c)) = frame_chan_event; // Fixed void pointer dereference
                        $s0_2[0xb4] = 1;
                        $s2_2 += 1;
                        continue;
                    }
                    else
                    {
                        private_misc_deregister($s0_2);
                        result = 0xffffffea;
                    }
                }
                else
                {
                    $s0_2[0xb4] = 0;
                    $s2_2 += 1;
                    continue;
                }
            }
            
            int32_t $s2_3 = $s2_2 - 1;
            
            while (true)
            {
                $s4_1 -= 0x2ec;
                
                if (!$s2_3)
                    break;
                
                tx_isp_frame_chan_deinit(*($v0 + 0xdc) + $s4_1);
                $s2_3 -= 1;
            }
            
            break;
        }
    }
    else
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments);
        result = 0xfffffff4;
    }
    
    private_kfree($v0);
    return result;
}

