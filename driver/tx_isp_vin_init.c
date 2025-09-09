#include "include/main.h"


  int32_t tx_isp_vin_init(void* arg1, int32_t arg2)

{
    int32_t* $a0 = (int32_t*)((char*)arg1  + 0xe4); // Fixed void pointer arithmetic
    int32_t result;
    
    if (!$a0)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        result = 0xffffffff;
    }
    else
    {
        char* $v0_1 = (char*)(**($a0 + 0xc4)); // Fixed void pointer assignment
            int32_t $v0_2 = *($v0_1 + 4);
        
        if (!$v0_1)
            result = 0;
        else
        {
            
            if (!$v0_2)
                result = 0;
            else
            {
                result = $v0_2();
                
                if ((uintptr_t)result == 0xfffffdfd)
                    result = 0;
            }
        }
    }
    
    int32_t $v1 = 3;
    
    if (!(uintptr_t)arg2)
        $v1 = 2;
    
    *((int32_t*)((char*)arg1 + 0xf4)) = $v1; // Fixed void pointer dereference
    return result;
}

