#include "include/main.h"


  int32_t tx_isp_vin_remove(int32_t arg1)

{
    char* $v0 = (char*)(private_platform_get_drvdata()); // Fixed void pointer assignment
    void* const $s0 = $v0;
    void* const $s1 = nullptr;
    
    if ($v0)
    {
        if ($(uintptr_t)v0 >= 0xfffff001)
            $s0 = nullptr;
        
        $s1 = $s0;
    }
    
    private_platform_set_drvdata(arg1, 0);
    tx_isp_subdev_deinit($s1);
    private_kfree($s0);
    return 0;
}

