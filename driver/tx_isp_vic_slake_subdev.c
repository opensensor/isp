#include "include/main.h"


  int32_t tx_isp_vic_slake_subdev(void* arg1)

{
    int32_t result = 0xffffffea;
        char* $s0_1 = *((char*)arg1 + 0xd4); // Fixed void pointer arithmetic
            int32_t $v1_2 = *($s0_1 + 0x128);
    
    if (arg1)
    {
        if ((uintptr_t)arg1 >= 0xfffff001)
            return 0xffffffea;
        
        result = 0xffffffea;
        
        if ($s0_1 && $(uintptr_t)s0_1 < 0xfffff001)
        {
            int32_t entry_$a2;
            
            if ($v1_2 == 4)
            {
                entry_$a2 = vic_core_s_stream(arg1, 0);
                $v1_2 = *($s0_1 + 0x128);
            }
            
            void* $s1_2;
            
            if ($v1_2 != 3)
                $s1_2 = $s0_1 + 0x130;
            else
            {
                vic_core_ops_init(arg1, 0, entry_$a2);
                $s1_2 = $s0_1 + 0x130;
            }
            
            private_mutex_lock($s1_2);
            
            if (*($s0_1 + 0x128) == 2)
                *(((int32_t*)((char*)$s0_1 + 0x128))) = 1; // Fixed void pointer dereference
            
            private_mutex_unlock($s1_2);
            return 0;
        }
    }
    
    return result;
}

