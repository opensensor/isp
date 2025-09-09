#include "include/main.h"


  int32_t tx_isp_csi_remove(int32_t arg1)

{
    void* $v0 = private_platform_get_drvdata();
    void* $s1 = $v0;
    void* const $s0 = nullptr;
    
    if ($v0)
    {
        int32_t $s0_1 = $v0 < 0xfffff001 ? 1 : 0;
        
        if (!$s0_1)
            $s1 = nullptr;
        
        void* const $v0_1 = $s1;
        
        if (!$s0_1)
            $v0_1 = nullptr;
        
        $s0 = $v0_1;
    }
    
    void* $v1 = *($s0 + 0xb8);
    int32_t* $s2 = *($s0 + 0x138);
    *($v1 + 0x10) &= 0xfffffffe;
    void* $v1_1 = *($s0 + 0xb8);
    *($v1_1 + 0x10) |= 1;
    private_platform_set_drvdata(arg1, 0);
    private_iounmap(*($s0 + 0x13c));
    int32_t $a0_2 = *$s2;
    private_release_mem_region($a0_2, $s2[1] + 1 - $a0_2);
    tx_isp_subdev_deinit($s1);
    private_kfree($s0);
    return 0;
}

