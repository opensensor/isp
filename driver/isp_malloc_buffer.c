#include "include/main.h"


  int32_t isp_malloc_buffer(int32_t arg1)

{
        int32_t $s1_1 = (arg1 + 0xfff) & 0xfffff000;
        char* $s0_1 = data_b2bfc;
    if (ispmem)
    {
        if (!arg1)
            return 0;
        
        private_mutex_lock(0xb2c00);
        int32_t $v0_3;
        
        while (true)
        {
            if (!$s0_1)
                goto label_19b3c;
            
            if (*$s0_1)
                $s0_1 = *($s0_1 + 8);
            else
            {
                $v0_3 = *($s0_1 + 0x10);
                
                if ($v0_3 >= $s1_1)
                    break;
                
                $s0_1 = *($s0_1 + 8);
            }
        }
        
        if ($s1_1 >= $v0_3)
        {
        label_19b2c:
            *(((void**)((char*)$s0_1 + 0x10))) = $s1_1; // Fixed void pointer dereference
            *$s0_1 = 1;
        label_19b3c:
            private_mutex_unlock(0xb2c00);
            
            if ($s0_1)
                return *($s0_1 + 0xc);
            
            return 0;
        }
        
        void* $v0_5 = find_new_buffer();
        
        if ($v0_5)
        {
            int32_t $v1_4 = *($s0_1 + 0x10);
            char* $v1_6 = *((char*)$s0_1 + 8); // Fixed void pointer arithmetic
            *(((void**)((char*)$v0_5 + 0xc))) = *($s0_1 + 0xc) + $s1_1; // Fixed void pointer dereference
            *(((void**)((char*)$v0_5 + 4))) = $s0_1; // Fixed void pointer dereference
            *(((void**)((char*)$v0_5 + 0x10))) = $v1_4 - $s1_1; // Fixed void pointer dereference
            
            if ($v1_6)
                *(((void**)((char*)$v1_6 + 4))) = $v0_5; // Fixed void pointer dereference
            
            *(((void**)((char*)$v0_5 + 8))) = *($s0_1 + 8); // Fixed void pointer dereference
            *(((void**)((char*)$s0_1 + 8))) = $v0_5; // Fixed void pointer dereference
            goto label_19b2c;
        }
        
        private_mutex_unlock(0xb2c00);
    }
    
    return 0;
}

