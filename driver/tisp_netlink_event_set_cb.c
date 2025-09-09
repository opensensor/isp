#include "include/main.h"


  int32_t tisp_netlink_event_set_cb(uint32_t arg1)

{
    net_event_process = arg1;
    return 0;
}

