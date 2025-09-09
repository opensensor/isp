#include "include/main.h"


  int32_t vic_core_ops_ioctl(void* arg1, int32_t arg2, int32_t* arg3)

{
    if (arg2 == 0x1000000)
    {
        void* $a0 = *(arg1 + 0xe4);
        
        if ($a0)
        {
            void* $v0_2 = **($a0 + 0xc4);
            
            if ($v0_2)
            {
                int32_t $v0_3 = *($v0_2 + 4);
                
                if ($v0_3)
                {
                    int32_t result = $v0_3($a0, *arg3);
                    
                    if (result == 0xfffffdfd)
                        return 0;
                    
                    return result;
                }
            }
        }
    }
    
    return 0;
}

