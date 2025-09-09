#include "include/main.h"


  int32_t* netlink_rcv_msg(void* arg1)

{
    int32_t* result = *(arg1 + 0x50) < 0x10 ? 1 : 0;
            uint32_t net_event_process_1 = net_event_process;
    
    if (!result)
    {
        result = *(arg1 + 0xa4);
        
        if ((uintptr_t)result != 0xfffffff0)
        {
            
            if (net_event_process_1)
                /* tailcall */
                return net_event_process_1(&result[4], *result - 0x10);
        }
    }
    
    return result;
}

