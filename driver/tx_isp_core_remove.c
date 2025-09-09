#include "include/main.h"


  int32_t tx_isp_core_remove()

{
    void* $v0 = private_platform_get_drvdata();
    void* $s0 = *($v0 + 0xd4);
    int32_t $a0 = *($s0 + 0x1bc);
    
    if ($a0)
    {
        isp_core_tuning_deinit($a0);
        *($s0 + 0x1bc) = 0;
    }
    
    if (*($s0 + 0xe8) >= 2)
        ispcore_slake_module($s0);
    
    private_kfree(*($s0 + 0x150));
    *($s0 + 0x158) = 1;
    *($s0 + 0x150) = 0;
    tx_isp_subdev_deinit($v0);
    tisp_deinit();
    private_kfree($s0);
    return 0;
}

