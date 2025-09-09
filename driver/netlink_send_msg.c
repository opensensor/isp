#include "include/main.h"


  int32_t netlink_send_msg(int32_t arg1, int16_t arg2)

{
    uint32_t $s1 = arg2;
    int32_t var_4 = $ra;
    int32_t $v0 = private_nlmsg_new($s1, 0x20);
        int32_t $v0_1 = private_nlmsg_put($v0, 0, 0, 0x17, $s1, 0);
    int32_t $ra;
    
    if ($v0)
    {
        
        if ($v0_1)
        {
            memcpy($v0_1 + 0x10, arg1, $s1);
            /* tailcall */
            return private_netlink_unicast(nlsk, $v0, 0x32, 0x40);
        }
        
        isp_printf(); // Fixed: macro call, removed arguments;
        kfree_skb($v0);
    }
    else
        isp_printf(); // Fixed: macro call, removed arguments;
    
    return 0xffffffff;
}

