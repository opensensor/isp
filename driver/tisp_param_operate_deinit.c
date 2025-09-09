#include "include/main.h"


  int32_t tisp_param_operate_deinit()

{
    uint32_t opmsg_1 = opmsg;
    tisp_netlink_exit();
    
    if (opmsg_1)
    {
        private_kfree(opmsg_1);
        opmsg = 0;
    }
    
    tisp_code_destroy_tuning_node();
    return 0;
}

