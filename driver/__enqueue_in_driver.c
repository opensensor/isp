#include "include/main.h"


  int32_t __enqueue_in_driver(void* arg1)

{
    void* $s1 = *(arg1 + 0x44);
    *(arg1 + 0x48) = 3;
    *(arg1 + 0x4c) = 3;
    int32_t result = tx_isp_send_event_to_remote(*($s1 + 0x298), 0x3000005, arg1 + 0x68);
    
    if (result && result != 0xfffffdfd)
        isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", *($s1 + 0x29c));
    
    return result;
}

