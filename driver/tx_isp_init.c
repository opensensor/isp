#include "include/main.h"


  int32_t tx_isp_init()

{
    int32_t $v0;
    int32_t $a2;
    int32_t $s0;
        int32_t $v0_1;
            int32_t $v0_3 = private_platform_driver_register(&tx_isp_driver);
                return 0;
            return $v0_3;
    $v0 = private_driver_get_interface();
    char const* const $a1_1;
    
    if (!$v0)
    {
        $v0_1 = private_platform_device_register(&tx_isp_platform_device);
        $s0 = $v0_1;
        
        if (!$v0_1)
        {
            
            if (!$v0_3)
            
            private_platform_device_unregister(&tx_isp_platform_device);
        }
        
        $a1_1 = "not support the gpio mode!\\n";
    }
    else
    {
        $s0 = $v0;
        $a1_1 = "VIC_CTRL : %08x\n";
    }
    
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    return $s0;
}

