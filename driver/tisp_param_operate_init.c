#include "include/main.h"


  int32_t tisp_param_operate_init()

{
    uint32_t $v0 = private_kmalloc(&data_10004[0x14], 0xd0);
        return 0xffffffff;
    opmsg = $v0;
    
    if (!$v0)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    tisp_netlink_init();
    tisp_netlink_event_set_cb(tisp_param_operate_process);
    tisp_code_create_tuning_node();
    return 0;
}

