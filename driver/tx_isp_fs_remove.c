#include "include/main.h"


  int32_t tx_isp_fs_remove()

{
    int32_t $s0 = 0;
    char* $v0 = (char*)(private_platform_get_drvdata()); // Fixed void pointer assignment
    int32_t* $s1 = (int32_t*)((char*)$v0  + 0xd4); // Fixed void pointer arithmetic
    int32_t $v1 = *($s1 + 0xe0);
    int32_t $a0_1;
        char* $a0_2 = (char*)($s0 * 0x2ec + $a0_1); // Fixed void pointer assignment
    
    while (true)
    {
        $a0_1 = *($s1 + 0xdc);
        
        if ($s0 >= $v1)
            break;
        
        $s0 += 1;
        tx_isp_frame_chan_deinit($a0_2);
        $v1 = *($s1 + 0xe0);
    }
    
    if ($a0_1)
        private_kfree();
    
    tx_isp_subdev_deinit($v0);
    private_kfree($s1);
    return 0;
}

