#include "include/main.h"


  int32_t vic_core_s_stream(void* arg1, int32_t arg2)

{
    int32_t $v0 = 0xffffffea;
    
    if (arg1)
    {
        if (arg1 >= 0xfffff001)
            return 0xffffffea;
        
        void* $s1_1 = *(arg1 + 0xd4);
        $v0 = 0xffffffea;
        
        if ($s1_1 && $s1_1 < 0xfffff001)
        {
            int32_t $v1_3 = *($s1_1 + 0x128);
            
            if (!arg2)
            {
                $v0 = 0;
                
                if ($v1_3 == 4)
                    *($s1_1 + 0x128) = 3;
            }
            else
            {
                $v0 = 0;
                
                if ($v1_3 != 4)
                {
                    tx_vic_disable_irq();
                    int32_t $v0_1 = tx_isp_vic_start($s1_1);
                    *($s1_1 + 0x128) = 4;
                    tx_vic_enable_irq();
                    return $v0_1;
                }
            }
        }
    }
    
    return $v0;
}

