#include "include/main.h"


  void tx_isp_free_irq(int32_t* arg1)

{
    if (!arg1)
        return;
    
    int32_t $a0 = *arg1;
    
    if (!$a0)
        *arg1 = 0;
    else
    {
        private_free_irq($a0, arg1);
        *arg1 = 0;
    }
}

