#include "include/main.h"


  int32_t tx_isp_core_remove()

{
    void* $v0 = private_platform_get_drvdata();
    int32_t* $s0 = *($v0 + 0xd4);
    int32_t $a0 = $s0[0x6f];
    
    if ($a0)
    {
        isp_core_tuning_deinit($a0);
        $s0[0x6f] = 0;
    }
    
    if ($s0[0x3a] >= 2)
        ispcore_slake_module($s0);
    
    private_kfree($s0[0x54]);
    $s0[0x56] = 1;
    $s0[0x54] = 0;
    tx_isp_subdev_deinit($v0);
    tisp_deinit();
    private_kfree($s0);
    return 0;
}

