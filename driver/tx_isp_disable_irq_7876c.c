#include "include/main.h"


  int32_t tx_isp_disable_irq(void* arg1)

{
    int32_t var_18 = 0;
    int32_t $a1_1 = var_18;
    __private_spin_lock_irqsave(arg1 + 0x80, &var_18);
    
    if (*(arg1 + 0x84))
    {
        (*(arg1 + 0x88))(arg1 + 0x80, $a1_1);
        $a1_1 = var_18;
    }
    
    return private_spin_unlock_irqrestore(arg1 + 0x80, $a1_1);
}

