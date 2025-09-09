#include "include/main.h"


  int32_t tx_isp_enable_irq(void* arg1)

{
    int32_t var_18_121 = 0;
    __private_spin_lock_irqsave(arg1 + 0x80, &var_18_122);
    int32_t $v0 = *(arg1 + 0x84);
    int32_t $a1_1 = var_18_123;
    
    if ($v0)
    {
        $v0(arg1 + 0x80, $a1_1);
        $a1_1 = var_18_124;
    }
    
    return private_spin_unlock_irqrestore(arg1 + 0x80, $a1_1);
}

