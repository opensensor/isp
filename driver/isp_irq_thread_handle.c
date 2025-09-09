#include "include/main.h"


  int32_t isp_irq_thread_handle(int32_t arg1, void* arg2)

{
    void* $s0_1;
    void* $s1_1;
    
    if ((uintptr_t)arg2 == 0x80)
    {
        $s1_1 = arg2 - 0x48;
        $s0_1 = arg2 - 8;
    }
    else
    {
        char* $v0_2 = (char*)(**(arg2 + 0x44)); // Fixed void pointer assignment
            int32_t $v0_3 = *($v0_2 + 0x24);
        $s1_1 = arg2 - 0x48;
        
        if (!$v0_2)
            $s0_1 = arg2 - 8;
        else
        {
            
            if (!$v0_3)
                $s0_1 = arg2 - 8;
            else
            {
                $v0_3(arg2 - 0x80, 0);
                $s1_1 = arg2 - 0x48;
                $s0_1 = arg2 - 8;
            }
        }
    }
    
    char* $a0_1 = (char*)(*$s1_1); // Fixed void pointer assignment
    
    while (true)
    {
            char* $v0_5 = (char*)(**($a0_1 + 0xc4)); // Fixed void pointer assignment
                int32_t $v0_6 = *($v0_5 + 0x24);
        if (!$a0_1)
            $s1_1 += 4;
        else
        {
            
            if (!$v0_5)
                $s1_1 += 4;
            else
            {
                
                if (!$v0_6)
                    $s1_1 += 4;
                else
                {
                    $v0_6();
                    $s1_1 += 4;
                }
            }
        }
        
        if ($s1_1 == $s0_1)
            break;
        
        $a0_1 = *$s1_1;
    }
    
    return 1;
}

