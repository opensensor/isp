#include "include/main.h"


  int32_t netlink_send_msg(int32_t arg1, int16_t arg2)

{
    uint32_t $s1 = arg2;
    int32_t $ra;
    int32_t var_4 = $ra;
    int32_t $v0 = private_nlmsg_new($s1, 0x20);
    
    if ($v0)
    {
        int32_t $v0_1 = private_nlmsg_put($v0, 0, 0, 0x17, $s1, 0);
        
        if ($v0_1)
        {
            memcpy($v0_1 + 0x10, arg1, $s1);
            /* tailcall */
            return private_netlink_unicast(nlsk, $v0, 0x32, 0x40);
        }
        
        isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", "netlink_send_msg");
        kfree_skb($v0);
    }
    else
        isp_printf(2, &$LC0, "netlink_send_msg");
    
    return 0xffffffff;
}

