#include "include/main.h"


  int32_t tx_isp_probe(int32_t* arg1)

{
    void* $v0;
    int32_t $a2;
    $v0 = private_kmalloc(0x120, 0xd0);
    
    if (!$v0)
    {
        isp_printf(2, &$LC33, $a2);
        return 0xfffffff4;
    }
    
    int32_t $a2_1 = memset($v0, 0, 0x120);
    void* $s2_1 = arg1[0x16];
    int32_t result;
    
    if (!$s2_1)
        result = 0xffffffea;
    else if (*($s2_1 + 4) >= 0x11)
    {
        isp_printf(2, "saveraw", $a2_1);
        result = 0xffffffea;
    }
    else
    {
        int32_t* $s6_1 = $v0 + 0x84;
        int32_t $fp_1 = 0;
        uint32_t $v0_5;
        
        while (true)
        {
            $v0_5 = *($s2_1 + 4);
            
            if ($fp_1 >= $v0_5)
                break;
            
            int32_t* $a1_1 = *(*($s2_1 + 8) + ($fp_1 << 2));
            int32_t $a0_2 = 0;
            *$s6_1 = $a1_1;
            
            while (true)
            {
                int32_t* $s5_1 = *(&isp_drivers + $a0_2);
                char* $v0_9 = *$a1_1;
                char* $v1_3 = $s5_1[5];
                uint32_t $t0_1 = *$v0_9;
                uint32_t $at_1;
                
                while (true)
                {
                    $at_1 = *$v1_3;
                    $v0_9 = &$v0_9[1];
                    $v1_3 = &$v1_3[1];
                    
                    if ($at_1 != $t0_1)
                        break;
                    
                    uint32_t temp1_2 = $t0_1;
                    $t0_1 = *$v0_9;
                    
                    if (!temp1_2)
                    {
                        $t0_1 = $at_1;
                        break;
                    }
                }
                
                $a0_2 += 4;
                
                if (!($t0_1 - $at_1))
                {
                    int32_t result_1 = private_platform_device_register($a1_1);
                    result = result_1;
                    
                    if (result_1)
                    {
                        isp_printf(2, 
                            "width is %d, height is %d, imagesize is %d\\n, save num is %d, buf size is %d", 
                            $fp_1);
                        *$s6_1 = 0;
                        goto label_201f8;
                    }
                    
                    int32_t $v0_4 = *$s5_1;
                    
                    if (!$v0_4)
                        $s6_1[1] = $s5_1;
                    else
                    {
                        $v0_4($a1_1, $a1_1);
                        $s6_1[1] = $s5_1;
                    }
                    
                    $fp_1 += 1;
                    goto label_200e0;
                }
                
                if ($a0_2 == 0x14)
                {
                    $fp_1 += 1;
                label_200e0:
                    $s6_1 = &$s6_1[2];
                    break;
                }
            }
        }
        
        *($v0 + 0x80) = $v0_5;
        private_spin_lock_init($v0 + 0x114);
        
        if (!tx_isp_module_init(arg1, $v0))
        {
            *($v0 + 0xc) = 0xff;
            *($v0 + 0x30) = &tx_isp_fops;
            *($v0 + 0x14) = &tx_isp_fops;
            *($v0 + 0x10) = &$LC37;
            int32_t result_2;
            int32_t $a2_4;
            result_2 = private_misc_register($v0 + 0xc);
            result = result_2;
            
            if (result_2 >= 0)
            {
                int32_t $v0_11;
                int32_t $a2_5;
                $v0_11 = private_jz_proc_mkdir("\\t cmd:\\n");
                *($v0 + 0x11c) = $v0_11;
                
                if ($v0_11)
                {
                    private_platform_set_drvdata(arg1, $v0);
                    globe_ispdev = $v0;
                    int32_t result_3 = tx_isp_create_graph_and_nodes($v0);
                    result = result_3;
                    
                    if (!result_3)
                    {
                        isp_mem_init();
                        *($v0 + 0x104) =
                            "\\t\\t\\t use cmd " snapraw" you should set ispmem first!!!!!\\n";
                        isp_printf(1, "\\t\\t\\t "snapraw"  is cmd; \\n", 
                            "\\t\\t\\t use cmd " snapraw" you should set ispmem first!!!!!\\n");
                        return 0;
                    }
                    
                    private_proc_remove(*($v0 + 0x11c));
                }
                else
                    isp_printf(2, "\\t\\t snapraw\\n", $a2_5);
                
                private_misc_deregister($v0 + 0xc);
            }
            else
            {
                isp_printf(2, "help:\\n", $a2_4);
                result = 0xfffffffe;
            }
            
            tx_isp_module_deinit($v0);
        }
        else
        {
            isp_printf(2, "help", *($s2_1 + 2));
            result = 0xfffffff4;
        }
        
    label_201f8:
        tx_isp_unregister_platforms($v0 + 0x84);
    }
    
    private_kfree($v0);
    return result;
}

