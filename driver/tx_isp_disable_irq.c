#include "include/main.h"


  int32_t tx_isp_disable_irq(int32_t* arg1)

{
    /* tailcall */
    return private_disable_irq(*arg1);
}

