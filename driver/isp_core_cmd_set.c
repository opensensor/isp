#include "include/main.h"


  int32_t isp_core_cmd_set(int32_t arg1, int32_t arg2, int32_t arg3)

{
    char* $v0 = private_kmalloc(arg3 + 1, 0xd0);
    
    if (!$v0)
        return 0xfffffff4;
    
    if (private_copy_from_user($v0, arg2, arg3))
    {
        private_kfree($v0);
        return 0xfffffff2;
    }
    
    char* $v1_1 = $v0;
    char const* const $v0_3 = "flags = 0x%08x, jzflags = %p,0x%08x";
    int32_t $a0_3 = 6;
    uint32_t $at_1;
    uint32_t $a1_1;
    
    while (true)
    {
        $a1_1 = *$v1_1;
        $at_1 = *$v0_3;
        
        if ($a0_3)
        {
            $a0_3 -= 1;
            $v1_1 = &$v1_1[1];
            
            if ($at_1 != $a1_1)
                break;
            
            $v0_3 = &$v0_3[1];
            
            if ($a1_1)
                continue;
        }
        
        $a1_1 = $at_1;
        break;
    }
    
    uint32_t $a1_2 = $a1_1 - $at_1;
    
    if ($a1_2)
    {
        char* $v1_2 = $v0;
        char const* const $v0_4 = "Can not support this frame mode!!!\\n";
        int32_t $a0_4 = 0x10;
        uint32_t $at_2;
        uint32_t $a1_3;
        
        while (true)
        {
            $a1_3 = *$v1_2;
            $at_2 = *$v0_4;
            
            if ($a0_4)
            {
                $a0_4 -= 1;
                $v1_2 = &$v1_2[1];
                
                if ($at_2 != $a1_3)
                    break;
                
                $v0_4 = &$v0_4[1];
                
                if ($a1_3)
                    continue;
            }
            
            $a1_3 = $at_2;
            break;
        }
        
        uint32_t $a1_4 = $a1_3 - $at_2;
        
        if ($a1_4)
        {
            char* $v1_3 = $v0;
            char const* const $v0_6 = "sensor type is BT1120!\\n";
            int32_t $a0_6 = 0x11;
            uint32_t $at_3;
            uint32_t $a1_5;
            
            while (true)
            {
                $a1_5 = *$v1_3;
                $at_3 = *$v0_6;
                
                if ($a0_6)
                {
                    $a0_6 -= 1;
                    $v1_3 = &$v1_3[1];
                    
                    if ($at_3 != $a1_5)
                        break;
                    
                    $v0_6 = &$v0_6[1];
                    
                    if ($a1_5)
                        continue;
                }
                
                $a1_5 = $at_3;
                break;
            }
            
            uint32_t $a1_6 = $a1_5 - $at_3;
            
            if ($a1_6)
            {
                char* $v1_4 = $v0;
                char* $v0_8 = "VIC_CTRL : %08x\\n";
                int32_t $a0_8 = 0x12;
                uint32_t $at_4;
                uint32_t $a1_7;
                
                while (true)
                {
                    $a1_7 = *$v1_4;
                    $at_4 = *$v0_8;
                    
                    if ($a0_8)
                    {
                        $a0_8 -= 1;
                        $v1_4 = &$v1_4[1];
                        
                        if ($at_4 != $a1_7)
                            break;
                        
                        $v0_8 = &$v0_8[1];
                        
                        if ($a1_7)
                            continue;
                    }
                    
                    $a1_7 = $at_4;
                    break;
                }
                
                uint32_t $a1_8 = $a1_7 - $at_4;
                
                if (!$a1_8)
                    isp_ch0_pre_dequeue_valid_lines = simple_strtoull(&$v0[0x13], $a1_8, 0);
            }
            else
                isp_ch0_pre_dequeue_interrupt_process = simple_strtoull(&$v0[0x12], $a1_6, 0);
        }
        else
            isp_ch0_pre_dequeue_time = simple_strtoull(&$v0[0x11], $a1_4, 0);
    }
    else
    {
        isp_core_debug_type = 1;
        data_ca554_1 = 1;
        private_msleep(0xc8, $a1_2);
    }
    
    private_kfree($v0);
    return arg3;
}

