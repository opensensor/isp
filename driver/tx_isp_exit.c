#include "include/main.h"


  int32_t tx_isp_exit()

{
    private_platform_driver_unregister(&tx_isp_driver);
    /* tailcall */
    return private_platform_device_unregister(&tx_isp_platform_device);
}

