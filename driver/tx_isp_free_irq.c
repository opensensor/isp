#include "include/main.h"


  void tx_isp_free_irq(int32_t* arg1)

{
    int32_t $a0 = *arg1;
    if (!(uintptr_t)arg1)
        return;
    
    
    if (!$a0)
        *arg1 = 0;
    else
    {
        private_free_irq($a0, arg1);
        *arg1 = 0;
    }
}

