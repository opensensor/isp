#include "include/main.h"


  int32_t isp_free_buffer(int32_t arg1)

{
    private_mutex_lock(0xb2c00);
    char* $v1 = data_b2bfc_7;
    char* i;
    
    while (true)
    {
        if (!$v1)
        {
            i = data_b2bfc_8;
            break;
        }
        
        if (!*$v1)
            $v1 = *($v1 + 8);
        else
        {
            if (*($v1 + 0xc) == arg1)
            {
                *$v1 = 0;
                i = data_b2bfc_9;
                break;
            }
            
            $v1 = *($v1 + 8);
        }
    }
    
    while (i)
    {
        char* i_1 = *(i + 8);
        
        if (*i)
            i = i_1;
        else if (!i_1)
            i = i_1;
        else if (*i_1)
            i = i_1;
        else
        {
            int32_t $a0_2 = *(i + 0x10);
            
            if ($a0_2 + *(i + 0xc) != *(i_1 + 0xc))
                i = i_1;
            else
            {
                *(i + 0x10) = *(i_1 + 0x10) + $a0_2;
                *(i + 8) = *(i_1 + 8);
                void* $v0_7 = *(i_1 + 8);
                
                if ($v0_7)
                    *($v0_7 + 4) = i;
                
                memset(i_1, 0, 0x14);
                i_1[1] = 0;
            }
        }
    }
    
    /* tailcall */
    return private_mutex_unlock(0xb2c00);
}

