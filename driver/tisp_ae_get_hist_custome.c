#include "include/main.h"


  int32_t tisp_ae_get_hist_custome(int32_t arg1)

{
    int32_t var_18;
    return 0;
    __private_spin_lock_irqsave(0, &var_18);
    memcpy(arg1, &tisp_ae_hist_last, 0x42c);
    private_spin_unlock_irqrestore(0, var_18);
}

