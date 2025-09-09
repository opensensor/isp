#include "include/main.h"


  int32_t isp_irq_handle(int32_t arg1, void* arg2)

{
    int32_t result;
    
    if (arg2 != 0x80)
    {
        void* $v0_2 = **(arg2 + 0x44);
        result = 1;
        
        if ($v0_2)
        {
            int32_t $v0_3 = *($v0_2 + 0x20);
            
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
    void* $a0_1 = *$s2;
    
    while (true)
    {
        if (!$a0_1)
            $s2 = &$s2[1];
        else
        {
            void* $v0_6 = **($a0_1 + 0xc4);
            
            if (!$v0_6)
                $s2 = &$s2[1];
            else
            {
                int32_t $v0_7 = *($v0_6 + 0x20);
                
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

