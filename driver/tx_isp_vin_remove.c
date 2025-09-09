#include "include/main.h"


  int32_t tx_isp_vin_remove(int32_t arg1)

{
    void* $v0 = private_platform_get_drvdata();
    void* const $s0 = $v0;
    void* const $s1 = nullptr;
    
    if ($v0)
    {
        if ($v0 >= 0xfffff001)
            $s0 = nullptr;
        
        $s1 = $s0;
    }
    
    private_platform_set_drvdata(arg1, 0);
    tx_isp_subdev_deinit($s1);
    private_kfree($s0);
    return 0;
}

