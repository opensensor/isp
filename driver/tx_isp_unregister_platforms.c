#include "include/main.h"


  int32_t (*)() tx_isp_unregister_platforms(void* arg1)

{
    void* $s0 = arg1;
    void* $v0 = *($s0 + 4);
    int32_t (* result)();
    
    while (true)
    {
        int32_t $a0;
        
        if (!$v0)
            $a0 = *$s0;
        else
        {
            (*($v0 + 4))(*$s0);
            $a0 = *$s0;
        }
        
        result = private_platform_device_unregister;
        
        if ($a0)
            result = private_platform_device_unregister($a0);
        
        $s0 += 8;
        
        if ($s0 == arg1 + 0x80)
            break;
        
        $v0 = *($s0 + 4);
    }
    
    return result;
}

