#include "include/main.h"


  int32_t tisp_s_mscaler_hvflip_mask(char arg1)

{
    int32_t result = data_ca490_13;
    uint32_t $s1 = arg1;
    
    if (result)
    {
        tisp_mscaler_mask_change($s1 ^ hvflip_last);
        int32_t* $v0 = data_ca490_14;
        
        for (int32_t i = 0; i < 0x9c; i += 1)
        {
            char var_b0_74[0xa0];
            var_b0_75[i] = *(&$v0[4] + i);
        }
        
        result = tisp_mscaler_mask_setreg(*$v0, $v0[1], $v0[2], $v0[3]);
    }
    
    hvflip_last = $s1;
    return result;
}

