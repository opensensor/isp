#include "include/main.h"


  int32_t frame_channel_vidioc_get_fmt(void* arg1, int32_t arg2)

{
    int32_t result = tx_isp_send_event_to_remote(*(arg1 + 0x2bc), 0x3000001, &var_80);
    if (!arg1)
        return 0xffffffea;
    
    if ((uintptr_t)arg1 >= 0xfffff001)
        return 0xffffffea;
    
    int32_t var_80;
    
    if (result && (uintptr_t)result != 0xfffffdfd)
    {
        isp_printf(); // Fixed: macro call, removed arguments);
        return result;
    }
    
    int32_t var_64_1 = 8;
    int32_t var_70_1_1 = 4;
    var_80_4 = 1;
    int32_t $v0_1;
    int32_t $a2_3;
    $v0_1 = private_copy_to_user(arg2, &var_80_5, 0x70);
    
    if (!$v0_1)
    {
        memcpy(arg1 + 0x23c, &var_80, 0x70);
        return 0;
    }
    
    isp_printf(); // Fixed: macro call, removed arguments;
    return 0xfffffff4;
}

