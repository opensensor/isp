#include "include/main.h"


  int32_t isp_csi_show(void* arg1)

{
    int32_t* $v0 = (int32_t*)((char*)arg1  + 0x3c); // Fixed void pointer arithmetic
        int32_t* $s1_1 = (int32_t*)((char*)$v0  + 0xd4); // Fixed void pointer arithmetic
            int32_t* $v0_2 = (int32_t*)((char*)$s1_1  + 0xb8); // Fixed void pointer arithmetic
            int32_t $v1_1 = *($v0_2 + 0x20);
            int32_t result = 0;
            int32_t $v0_4 = *($v0_2 + 0x24);
            void* $v0_10;
                    return result;
    
    if ($v0 && $(uintptr_t)v0 < 0xfffff001)
    {
        
        if ($s1_1 && $(uintptr_t)s1_1 < 0xfffff001)
        {
            
            if ($v1_1)
                result = seq_printf(arg1, "sensor type is BT656!\n", $v1_1);
            
            if ($v0_4)
                result += seq_printf(arg1, "sensor type is BT601!\n", $v0_4);
            
            
            if ($v1_1)
                $v0_10 = *($s1_1 + 0xb8);
            else
            {
                if (!$v0_4)
                
                $v0_10 = *($s1_1 + 0xb8);
            }
            
            return result + seq_printf(arg1, 
                "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\\n", *($v0_10 + 0x14));
        }
    }
    
    int32_t entry_a2_8;

    return 0;
}

