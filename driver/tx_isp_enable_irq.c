#include "include/main.h"


  int32_t tx_isp_enable_irq(int32_t* arg1)

{
    /* tailcall */
    return private_enable_irq(*arg1);
}

