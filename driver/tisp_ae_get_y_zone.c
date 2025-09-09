#include "include/main.h"


  int32_t tisp_ae_get_y_zone(int32_t arg1)

{
    int32_t var_18_95 = 0;
    __private_spin_lock_irqsave(0, &var_18_96);
    memcpy(arg1, 0xd3b24, 0x384);
    private_spin_unlock_irqrestore(0, var_18_97);
    return 0;
}

