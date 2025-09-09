#include "include/main.h"


  int32_t isp_subdev_init_clks(void* arg1, int32_t* arg2)

{
    int32_t $s5 = *(arg1 + 0xc0);
    int32_t $s1 = $s5 << 2;
        int32_t* $v0_1 = private_kmalloc($s1, 0xd0);
            return 0xfffffff4;
    
    if ($s5)
    {
        
        if (!$v0_1)
        {
            isp_printf(); // Fixed: macro with no parameters, removed 5 arguments;
        }
        
        memset($v0_1, 0, $s1);
        int32_t* $s6_1 = arg2;
        int32_t* $s4_1 = $v0_1;
        int32_t $s0_2 = 0;
        
        while (true)
        {
            int32_t $v0_3 = private_clk_get(*(arg1 + 4), *$s6_1);
            int32_t $s0_3;
            int32_t result;
                int32_t $a1_1 = $s6_1[1];
                int32_t result_1;
            *$s4_1 = $v0_3;
            
            if ($(uintptr_t)v0_3 < 0xfffff001)
            {
                
                if ($(uintptr_t)a1_1 != 0xffff)
                {
                    result_1 = private_clk_set_rate($v0_3, $a1_1);
                    result = result_1;
                }
                
                if ($(uintptr_t)a1_1 == 0xffff || !(uintptr_t)result_1)
                {
                    $s0_2 += 1;
                    $s6_1 = &$s6_1[2];
                    $s4_1 = &$s4_1[1];
                    
                    if ($s0_2 != $s5)
                        continue;
                    
                    *((int32_t*)((char*)arg1 + 0xbc)) = $v0_1; // Fixed void pointer dereference
                    break;
                }
                else
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    $s0_3 = $s0_2 << 2;
                }
            }
            else
            {
                isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                result = *$s4_1;
                $s0_3 = $s0_2 << 2;
            }
            
            for (char* i = (char*)($v0_1 + $s0_3); // Fixed void pointer assignment $v0_1 != i; i -= 4)
                private_clk_put(*(i - 4));
            
            private_kfree($v0_1);
            return result;
        }
    }
    else
        *((int32_t*)((char*)arg1 + 0xbc)) = 0; // Fixed void pointer dereference
    
    return 0;
}

