#include "include/main.h"


  int32_t tx_isp_vic_remove(int32_t arg1)

{
    void* $v0 = private_platform_get_drvdata();
    void* $s0 = $v0;
    int32_t $s1 = 0;
    
    if ($v0)
    {
        if ($v0 < 0xfffff001)
            $s1 = *($s0 + 0xd4);
        else
            $s0 = nullptr;
    }
    
    private_platform_set_drvdata(arg1, 0);
    tx_isp_subdev_deinit($s0);
    private_kfree($s1);
    return 0;
}

