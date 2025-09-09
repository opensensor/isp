#include "include/main.h"


  int32_t isp_irq_thread_handle(int32_t arg1, void* arg2)

{
    void* $s0_1;
    void* $s1_1;
    
    if (arg2 == 0x80)
    {
        $s1_1 = arg2 - 0x48;
        $s0_1 = arg2 - 8;
    }
    else
    {
        void* $v0_2 = **(arg2 + 0x44);
        $s1_1 = arg2 - 0x48;
        
        if (!$v0_2)
            $s0_1 = arg2 - 8;
        else
        {
            int32_t $v0_3 = *($v0_2 + 0x24);
            
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
    
    void* $a0_1 = *$s1_1;
    
    while (true)
    {
        if (!$a0_1)
            $s1_1 += 4;
        else
        {
            void* $v0_5 = **($a0_1 + 0xc4);
            
            if (!$v0_5)
                $s1_1 += 4;
            else
            {
                int32_t $v0_6 = *($v0_5 + 0x24);
                
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

