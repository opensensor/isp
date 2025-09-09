#include "include/main.h"


  int32_t frame_channel_vidioc_set_fmt(void* arg1, int32_t arg2)

{
    if (!arg1)
        return 0xffffffea;
    
    if (arg1 >= 0xfffff001)
        return 0xffffffea;
    
    int32_t var_80_5;
    int32_t $v0_1;
    int32_t $a2_1;
    $v0_1 = private_copy_from_user(&var_80_6, arg2, 0x70);
    char const* const $a1;
    
    if (!$v0_1)
    {
        if (var_80_7 != 1)
        {
            isp_printf(2, "\\t\\t\\t "savenum" is the num of you save raw picture.\\n ", $a2_1);
            return 0xffffffea;
        }
        
        int32_t var_70_5;
        int32_t $v1_3;
        int32_t var_64_3;
        
        if (var_70_6)
        {
            $v1_3 = var_64_4;
            
            if (var_70_7 != 4)
            {
                isp_printf(2, "\\t\\t saveraw\\n", $a2_1);
                return 0xffffffea;
            }
        }
        else
        {
            int32_t var_70_1_1 = 4;
            $v1_3 = var_64_5;
        }
        
        if ($v1_3 != 8)
        {
            isp_printf(2, 
                "\\t\\t\\t please use this cmd: \\n\\t"echo saveraw savenum > /proc/jz/isp/isp-w02"\\n", 
                $a2_1);
            return 0xffffffea;
        }
        
        int32_t result = tx_isp_send_event_to_remote(*(arg1 + 0x2bc), 0x3000002, &var_80_8);
        
        if (result && result != 0xfffffdfd)
        {
            isp_printf(2, "\\t\\t\\t "saveraw"  is cmd; \\n", *(arg1 + 0x2c0));
            return result;
        }
        
        int32_t $v0_3;
        $v0_3 = private_copy_to_user(arg2, &var_80_9, 0x70);
        
        if (!$v0_3)
        {
            memcpy(arg1 + 0x23c, &var_80_10, 0x70);
            return 0;
        }
        
        $a1 = "Can\'t ops the node!\\n";
    }
    else
        $a1 = "\\t\\t\\t "snapraw"  is cmd; \\n";
    
    isp_printf(2, $a1, $a2_1);
    return 0xfffffff4;
}

