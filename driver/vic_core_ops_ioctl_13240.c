#include "include/main.h"


  int32_t vic_core_ops_ioctl(void* arg1, int32_t arg2, int32_t* arg3)

{
        int32_t* $a0 = (int32_t*)((char*)arg1  + 0xe4); // Fixed void pointer arithmetic
            char* $v0_2 = (char*)(**($a0 + 0xc4)); // Fixed void pointer assignment
                int32_t $v0_3 = *($v0_2 + 4);
                    int32_t result = $v0_3($a0, *arg3);
                        return 0;
                    return result;
    if ((uintptr_t)arg2 == 0x1000000)
    {
        
        if ($a0)
        {
            
            if ($v0_2)
            {
                
                if ($v0_3)
                {
                    
                    if ((uintptr_t)result == 0xfffffdfd)
                    
                }
            }
        }
    }
    
    return 0;
}

