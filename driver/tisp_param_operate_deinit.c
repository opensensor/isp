#include "include/main.h"


  int32_t tisp_param_operate_deinit()

{
    tisp_netlink_exit();
    uint32_t opmsg_1 = opmsg;
    
    if (opmsg_1)
    {
        private_kfree(opmsg_1);
        opmsg = 0;
    }
    
    tisp_code_destroy_tuning_node();
    return 0;
}

