#include "include/main.h"


  int32_t tisp_netlink_init()

{
    uint32_t $v0 = private_netlink_kernel_create(init_net, 0x17, &nlcfg);
    nlsk = $v0;
    
    if ($v0)
        return 0;
    
    isp_printf(2, "Can not support this frame mode!!!\\n", "tisp_netlink_init");
    return 0xffffffff;
}

