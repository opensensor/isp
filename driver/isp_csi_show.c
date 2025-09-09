#include "include/main.h"


  int32_t isp_csi_show(void* arg1)

{
    void* $v0 = *(arg1 + 0x3c);
    
    if ($v0 && $v0 < 0xfffff001)
    {
        void* $s1_1 = *($v0 + 0xd4);
        
        if ($s1_1 && $s1_1 < 0xfffff001)
        {
            void* $v0_2 = *($s1_1 + 0xb8);
            int32_t $v1_1 = *($v0_2 + 0x20);
            int32_t result = 0;
            int32_t $v0_4 = *($v0_2 + 0x24);
            
            if ($v1_1)
                result = seq_printf(arg1, "sensor type is BT656!\\n", $v1_1);
            
            if ($v0_4)
                result += seq_printf(arg1, "sensor type is BT601!\\n", $v0_4);
            
            void* $v0_10;
            
            if ($v1_1)
                $v0_10 = *($s1_1 + 0xb8);
            else
            {
                if (!$v0_4)
                    return result;
                
                $v0_10 = *($s1_1 + 0xb8);
            }
            
            return result + seq_printf(arg1, 
                "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\\n", *($v0_10 + 0x14));
        }
    }
    
    int32_t entry_$a2;
    isp_printf(2, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n", entry_$a2);
    return 0;
}

