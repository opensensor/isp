#include "include/main.h"


  int32_t tx_isp_csi_probe(int32_t* arg1)

{
    void* $v0;
    int32_t $a2;
    $v0 = private_kmalloc(0x148, 0xd0);
    
    if (!$v0)
    {
        isp_printf(2, &$LC0, $a2);
        return 0xfffffff4;
    }
    
    memset($v0, 0, 0x148);
    void* $s1_1 = arg1[0x16];
    int32_t result;
    
    if (!tx_isp_subdev_init(arg1, $v0, &csi_subdev_ops))
    {
        int32_t* $v0_3 =
            private_request_mem_region(0x10022000, 0x1000, "Can not support this frame mode!!!\\n");
        
        if ($v0_3)
        {
            int32_t $a0_2 = *$v0_3;
            *($v0 + 0x13c) = private_ioremap($a0_2, $v0_3[1] + 1 - $a0_2);
            
            if (*($v0 + 0xb8))
            {
                *($v0 + 0x34) = &isp_csi_fops;
                *($v0 + 0x138) = $v0_3;
                private_raw_mutex_init($v0 + 0x12c, "not support the gpio mode!\\n", 0);
                private_platform_set_drvdata(arg1, $v0);
                *($v0 + 0x128) = 1;
                dump_csd = $v0;
                *($v0 + 0xd4) = $v0;
                return 0;
            }
            
            isp_printf(2, "VIC_CTRL : %08x\\n", "tx_isp_csi_probe");
            int32_t $a0_3 = *$v0_3;
            private_release_mem_region($a0_3, $v0_3[1] + 1 - $a0_3);
            result = 0xfffffffa;
        }
        else
        {
            isp_printf(2, "sensor type is BT1120!\\n", "tx_isp_csi_probe");
            result = 0xfffffff0;
        }
        
        tx_isp_subdev_deinit($v0);
    }
    else
    {
        isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", *($s1_1 + 2));
        result = 0xfffffff4;
    }
    
    private_kfree($v0);
    return result;
}

