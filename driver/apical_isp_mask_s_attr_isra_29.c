#include "include/main.h"


  int32_t apical_isp_mask_s_attr.isra.29(int32_t arg1)

{
    int32_t var_b8;
        char var_158[0xa0];
        char var_a8[0xa4];
    private_copy_from_user(&var_b8, arg1, 0xac);
    
    for (int32_t i = 0; (uintptr_t)i < 0x9c; i += 1)
    {
        var_158[i] = var_a8[i];
    }
    
    tisp_s_mscaler_mask_attr(var_b8);
    return 0;
}

