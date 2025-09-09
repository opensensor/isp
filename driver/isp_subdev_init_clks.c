#include "include/main.h"


  int32_t isp_subdev_init_clks(void* arg1, int32_t* arg2)

{
    int32_t $s5 = *(arg1 + 0xc0);
    int32_t $s1 = $s5 << 2;
    
    if ($s5)
    {
        int32_t* $v0_1 = private_kmalloc($s1, 0xd0);
        
        if (!$v0_1)
        {
            isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", "isp_subdev_init_clks");
            return 0xfffffff4;
        }
        
        memset($v0_1, 0, $s1);
        int32_t* $s6_1 = arg2;
        int32_t* $s4_1 = $v0_1;
        int32_t $s0_2 = 0;
        
        while (true)
        {
            int32_t $v0_3 = private_clk_get(*(arg1 + 4), *$s6_1);
            *$s4_1 = $v0_3;
            int32_t $s0_3;
            int32_t result;
            
            if ($v0_3 < 0xfffff001)
            {
                int32_t $a1_1 = $s6_1[1];
                int32_t result_1;
                
                if ($a1_1 != 0xffff)
                {
                    result_1 = private_clk_set_rate($v0_3, $a1_1);
                    result = result_1;
                }
                
                if ($a1_1 == 0xffff || !result_1)
                {
                    $s0_2 += 1;
                    $s6_1 = &$s6_1[2];
                    $s4_1 = &$s4_1[1];
                    
                    if ($s0_2 != $s5)
                        continue;
                    
                    *(arg1 + 0xbc) = $v0_1;
                    break;
                }
                else
                {
                    isp_printf(2, "sensor type is BT1120!\\n", *$s6_1);
                    $s0_3 = $s0_2 << 2;
                }
            }
            else
            {
                isp_printf(2, "Can not support this frame mode!!!\\n", *$s6_1);
                result = *$s4_1;
                $s0_3 = $s0_2 << 2;
            }
            
            for (void* i = $v0_1 + $s0_3; $v0_1 != i; i -= 4)
                private_clk_put(*(i - 4));
            
            private_kfree($v0_1);
            return result;
        }
    }
    else
        *(arg1 + 0xbc) = 0;
    
    return 0;
}

