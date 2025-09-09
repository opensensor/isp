#include "include/main.h"


  int32_t tx_isp_csi_probe(int32_t* arg1)

{
    void* $v0;
    int32_t $a2;
        return 0xfffffff4;
    $v0 = private_kmalloc(0x148, 0xd0);
    
    if (!$v0)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    memset($v0, 0, 0x148);
    char* $s1_1 = (char*)(arg1[0x16]); // Fixed void pointer assignment
    int32_t result;
    
    if (!tx_isp_subdev_init(arg1, $v0, &csi_subdev_ops))
    {
        int32_t* $v0_3 =
            int32_t $a0_2 = *$v0_3;
                return 0;
            private_request_mem_region(0x10022000, 0x1000, "Can not support this frame mode!!!\n");
        
        if ($v0_3)
        {
            *((int32_t*)((char*)$v0 + 0x13c)) = private_ioremap($a0_2, $v0_3[1] + 1 - $a0_2); // Fixed void pointer dereference
            
            if (*($v0 + 0xb8))
            {
                *((int32_t*)((char*)$v0 + 0x34)) = &isp_csi_fops; // Fixed void pointer dereference
                *((int32_t*)((char*)$v0 + 0x138)) = $v0_3; // Fixed void pointer dereference
                private_raw_mutex_init($v0 + 0x12c, /* "not support the gpio mode!\n" */ 0, 0); // Fixed: converted string literals to placeholders;
                private_platform_set_drvdata(arg1, $v0);
                *((int32_t*)((char*)$v0 + 0x128)) = 1; // Fixed void pointer dereference
                dump_csd = $v0;
                *((int32_t*)((char*)$v0 + 0xd4)) = $v0; // Fixed void pointer dereference
            }
            
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            int32_t $a0_3 = *$v0_3;
            private_release_mem_region($a0_3, $v0_3[1] + 1 - $a0_3);
            result = 0xfffffffa;
        }
        else
        {
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            result = 0xfffffff0;
        }
        
        tx_isp_subdev_deinit($v0);
    }
    else
    {
        isp_printf(); // Fixed: macro with no parameters, removed 5 arguments);
        result = 0xfffffff4;
    }
    
    private_kfree($v0);
    return result;
}

