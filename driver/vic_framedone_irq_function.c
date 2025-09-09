#include "include/main.h"


  void* vic_framedone_irq_function(void* arg1)

{
    void* result = &data_b0000;
            void* $s1_1 = &gpio_info;
                uint32_t $a0_2 = *$s1_1;
                    void* result_1 = result;
                    uint32_t var_2c_1 = *(((i + 8) << 1) + 0xb2904);
                    uint32_t var_30_1 = *(&gpio_info + (i << 1));
    
    if (!*(arg1 + 0x214))
    {
    label_123f4:
        
        if (gpio_switch_state)
        {
            gpio_switch_state = 0;
            
            for (int32_t i = 0; (uintptr_t)i != 0xa; )
            {
                result = private_gpio_direction_output;
                
                if ($(uintptr_t)a0_2 == 0xff)
                    break;
                
                result = private_gpio_direction_output($a0_2, *($s1_1 + 0x14));
                
                if (result < 0)
                {
                    return isp_printf(); // Fixed: macro call, removed arguments,STATE(%d),%d", 
                        "vic_framedone_irq_function");
                }
                
                i += 1;
                $s1_1 += 2;
            }
        }
    }
    else
    {
            char* $a3_1 = *((char*)arg1 + 0xb8); // Fixed void pointer arithmetic
            int32_t $a1_1 = 0;
            int32_t $v1_1 = 0;
            int32_t $v0 = 0;
        result = *(arg1 + 0x210);
        
        if (result)
        {
            void** i_1 = *(arg1 + 0x204);
            
            for (; i_1 != arg1 + 0x204; i_1 = *i_1)
            {
                $v1_1 += 0 < $v0 ? 1 : 0;
                $a1_1 += 1;
                
                if (i_1[2] == *($a3_1 + 0x380))
                    $v0 = 1;
            }
            
            int32_t $v1_2 = $v1_1 << 0x10;
            
            if (!$v0)
                $v1_2 = $a1_1 << 0x10;
            
            *(((void**)((char*)$a3_1 + 0x300))) = $v1_2 | (*($a3_1 + 0x300) & 0xfff0ffff); // Fixed void pointer dereference
            result = &data_b0000;
            goto label_123f4;
        }
    }
    
    return result;
}

