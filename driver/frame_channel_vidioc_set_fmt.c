#include "include/main.h"


  int32_t frame_channel_vidioc_set_fmt(void* arg1, int32_t arg2)

{
    if (!arg1)
        return 0xffffffea;
    
    if ((uintptr_t)arg1 >= 0xfffff001)
        return 0xffffffea;
    
    int32_t var_80;
    int32_t $v0_1;
    int32_t $a2_1;
    $v0_1 = private_copy_from_user(&var_80, arg2, 0x70);
    char const* const $a1;
    
    if (!$v0_1)
    {
        if (var_80 != 1)
        {
            isp_printf(); // Fixed: macro call, removed arguments;
            return 0xffffffea;
        }
        
        int32_t var_70;
        int32_t $v1_3;
        int32_t var_64;
        
        if (var_70_1)
        {
            $v1_3 = var_64;
            
            if (var_70 != 4)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffea;
            }
        }
        else
        {
            int32_t var_70_1 = 4;
            $v1_3 = var_64;
        }
        
        if ($v1_3 != 8)
        {
            isp_printf(2, 
                "\t\t\t please use this cmd: \n\t"echo saveraw savenum > /proc/jz/isp/isp-w02"\n", 
                $a2_1);
            return 0xffffffea;
        }
        
        int32_t result = tx_isp_send_event_to_remote(*(arg1 + 0x2bc), 0x3000002, &var_80_2);
        
        if (result && (uintptr_t)result != 0xfffffdfd)
        {
            isp_printf(); // Fixed: macro call, removed arguments);
            return result;
        }
        
        int32_t $v0_3;
        $v0_3 = private_copy_to_user(arg2, &var_80_3, 0x70);
        
        if (!$v0_3)
        {
            memcpy(arg1 + 0x23c, &var_80, 0x70);
            return 0;
        }
        
        $a1 = "Can\'t ops the node!\\n";
    }
    else
        $a1 = "\\t\\t\\t "snapraw"  is cmd; \\n";
    
    isp_printf(); // Fixed: macro call, removed arguments;
    return 0xfffffff4;
}

