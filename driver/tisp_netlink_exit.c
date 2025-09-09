#include "include/main.h"


  uint32_t tisp_netlink_exit()

{
    uint32_t nlsk_1 = nlsk;
        int32_t $a0_1 = *(nlsk_1 + 0x130);
    
    if (nlsk_1)
    {
        
        if ($a0_1)
            /* tailcall */
            return private_sock_release($a0_1);
    }
    
    return nlsk_1;
}

