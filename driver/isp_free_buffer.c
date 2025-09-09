#include "include/main.h"


  int32_t isp_free_buffer(int32_t arg1)

{
    char* $v1 = data_b2bfc;
    private_mutex_lock(0xb2c00);
    char* i;
    
    while (true)
    {
        if (!$v1)
        {
            i = data_b2bfc;
            break;
        }
        
        if (!*$v1)
            $v1 = *($v1 + 8);
        else
        {
            if (*($v1 + 0xc) == arg1)
            {
                *$v1 = 0;
                i = data_b2bfc;
                break;
            }
            
            $v1 = *($v1 + 8);
        }
    }
    
    while (i)
    {
        char* i_1 = *(i + 8);
            int32_t $a0_2 = *(i + 0x10);
                char* $v0_7 = *((char*)i_1 + 8); // Fixed void pointer arithmetic
        
        if (*i)
            i = i_1;
        else if (!i_1)
            i = i_1;
        else if (*i_1)
            i = i_1;
        else
        {
            
            if ($a0_2 + *(i + 0xc) != *(i_1 + 0xc))
                i = i_1;
            else
            {
                *(((void**)((char*)i + 0x10))) = *(i_1 + 0x10) + $a0_2; // Fixed void pointer dereference
                *(((void**)((char*)i + 8))) = *(i_1 + 8); // Fixed void pointer dereference
                
                if ($v0_7)
                    *(((void**)((char*)$v0_7 + 4))) = i; // Fixed void pointer dereference
                
                memset(i_1, 0, 0x14);
                i_1[1] = 0;
            }
        }
    }
    
    /* tailcall */
    return private_mutex_unlock(0xb2c00);
}

