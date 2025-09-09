#include "include/main.h"


  int32_t apical_isp_mask_s_attr.isra.29(int32_t arg1)

{
    int32_t var_b8;
    private_copy_from_user(&var_b8_1, arg1, 0xac);
    
    for (int32_t i = 0; i < 0x9c; i += 1)
    {
        char var_158[0xa0];
        char var_a8[0xa4];
        var_158_1[i] = var_a8_1[i];
    }
    
    tisp_s_mscaler_mask_attr(var_b8_2);
    return 0;
}

