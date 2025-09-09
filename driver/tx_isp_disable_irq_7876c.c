#include "include/main.h"


  int32_t tx_isp_disable_irq(void* arg1)

{
    int32_t var_18_125 = 0;
    __private_spin_lock_irqsave(arg1 + 0x80, &var_18_126);
    int32_t $a1_1 = var_18_127;
    
    if (*(arg1 + 0x84))
    {
        (*(arg1 + 0x88))(arg1 + 0x80, $a1_1);
        $a1_1 = var_18_128;
    }
    
    return private_spin_unlock_irqrestore(arg1 + 0x80, $a1_1);
}

