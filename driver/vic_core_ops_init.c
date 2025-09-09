#include "include/main.h"


  int32_t vic_core_ops_init(void* arg1, int32_t arg2, int32_t arg3)

{
    if (!arg1 || arg1 >= 0xfffff001)
    {
        isp_printf(2, "The parameter is invalid!\\n", arg3);
        return 0xffffffea;
    }
    
    void* $s1_1 = *(arg1 + 0xd4);
    int32_t $v0_2 = *($s1_1 + 0x128);
    int32_t result;
    
    if (!arg2)
    {
        result = 0;
        
        if ($v0_2 != 2)
        {
            tx_vic_disable_irq();
            *($s1_1 + 0x128) = 2;
        }
    }
    else
    {
        result = 0;
        
        if ($v0_2 != 3)
        {
            tx_vic_enable_irq();
            *($s1_1 + 0x128) = 3;
        }
    }
    
    return result;
}

