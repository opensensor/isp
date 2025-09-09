#include "include/main.h"


  int32_t tx_isp_core_remove()

{
    void* $v0 = private_platform_get_drvdata();
    char* $s0 = *((char*)$v0 + 0xd4); // Fixed void pointer arithmetic
    int32_t $a0 = *($s0 + 0x1bc);
    
    if ($a0)
    {
        isp_core_tuning_deinit($a0);
        *(((int32_t*)((char*)$s0 + 0x1bc))) = 0; // Fixed void pointer dereference
    }
    
    if (*($s0 + 0xe8) >= 2)
        ispcore_slake_module($s0);
    
    private_kfree(*($s0 + 0x150));
    *(((int32_t*)((char*)$s0 + 0x158))) = 1; // Fixed void pointer dereference
    *(((int32_t*)((char*)$s0 + 0x150))) = 0; // Fixed void pointer dereference
    tx_isp_subdev_deinit($v0);
    tisp_deinit();
    private_kfree($s0);
    return 0;
}

