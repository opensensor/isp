#include "include/main.h"


  int32_t isp_irq_handle(int32_t arg1, void* arg2)

{
    int32_t result;
        char* $v0_2 = (char*)(**(arg2 + 0x44)); // Fixed void pointer assignment
            int32_t $v0_3 = *($v0_2 + 0x20);
    
    if ((uintptr_t)arg2 != 0x80)
    {
        result = 1;
        
        if ($v0_2)
        {
            
            if (!$v0_3)
                result = 1;
            else
            {
                result = 1;
                
                if ($v0_3(arg2 - 0x80, 0, 0) == 2)
                    result = 2;
            }
        }
    }
    else
        result = 1;
    
    int32_t* $s2 = arg2 - 0x48;
    char* $a0_1 = (char*)(*$s2); // Fixed void pointer assignment
    
    while (true)
    {
            char* $v0_6 = (char*)(**($a0_1 + 0xc4)); // Fixed void pointer assignment
                int32_t $v0_7 = *($v0_6 + 0x20);
        if (!$a0_1)
            $s2 = &$s2[1];
        else
        {
            
            if (!$v0_6)
                $s2 = &$s2[1];
            else
            {
                
                if ($v0_7 && $v0_7() == 2)
                    result = 2;
                
                $s2 = &$s2[1];
            }
        }
        
        if ($s2 == arg2 - 8)
            break;
        
        $a0_1 = *$s2;
    }
    
    return result;
}

