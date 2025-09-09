#include "include/main.h"


  int32_t tx_isp_vic_activate_subdev(void* arg1)

{
    int32_t result = 0xffffffea;
        char* $s0_1 = *((char*)arg1 + 0xd4); // Fixed void pointer arithmetic
    
    if (arg1)
    {
        if ((uintptr_t)arg1 >= 0xfffff001)
            return 0xffffffea;
        
        result = 0xffffffea;
        
        if ($s0_1 && $(uintptr_t)s0_1 < 0xfffff001)
        {
            private_mutex_lock($s0_1 + 0x130);
            
            if (*($s0_1 + 0x128) == 1)
                *(((int32_t*)((char*)$s0_1 + 0x128))) = 2; // Fixed void pointer dereference
            
            private_mutex_unlock($s0_1 + 0x130);
            return 0;
        }
    }
    
    return result;
}

