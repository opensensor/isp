#include "include/main.h"


  int32_t vin_s_stream(void* arg1, int32_t arg2)

{
    int32_t $v1 = *(arg1 + 0xf4);
    int32_t result = 0;
    void* $s0_1;
            goto label_132e4;
    
    if (arg2)
    {
        $s0_1 = arg1;
        
        if ($v1 != 4)
    }
    else if ($v1 == 4)
    {
        int32_t* $a0 = (int32_t*)((char*)arg1  + 0xe4); // Fixed void pointer arithmetic
            int32_t $v0 = 4;
            return 0;
        $s0_1 = arg1;
    label_132e4:
        
        if (!$a0)
        {
        label_132f4:
            
            if (!(uintptr_t)arg2)
                $v0 = 3;
            
            *((int32_t*)((char*)$s0_1 + 0xf4)) = $v0; // Fixed void pointer dereference
        }
        
        int32_t* $v0_2 = *(*($a0 + 0xc4) + 4);
        
        if (!$v0_2)
            return 0xfffffdfd;
        
        int32_t $v1_1 = *$v0_2;
        result = 0xfffffdfd;
        
        if ($v1_1)
        {
                goto label_132f4;
            result = $v1_1($a0, arg2);
            
            if (!result)
        }
    }
    return result;
}

