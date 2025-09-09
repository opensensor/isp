#include "include/main.h"


  int32_t tx_isp_vic_activate_subdev(void* arg1)

{
    int32_t result = 0xffffffea;
    
    if (arg1)
    {
        if (arg1 >= 0xfffff001)
            return 0xffffffea;
        
        void* $s0_1 = *(arg1 + 0xd4);
        result = 0xffffffea;
        
        if ($s0_1 && $s0_1 < 0xfffff001)
        {
            private_mutex_lock($s0_1 + 0x130);
            
            if (*($s0_1 + 0x128) == 1)
                *($s0_1 + 0x128) = 2;
            
            private_mutex_unlock($s0_1 + 0x130);
            return 0;
        }
    }
    
    return result;
}

