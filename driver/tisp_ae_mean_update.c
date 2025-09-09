#include "include/main.h"


  int32_t tisp_ae_mean_update(uint32_t* arg1, int32_t* arg2)

{
    uint32_t wmean_new_2 = wmean_new;
    uint32_t $v0 = data_b0d4c;
    uint32_t $a3 = data_b0d54;
    int32_t $a2 = 0;
        int32_t* $t4_1 = i * 0x3c + data_d4670;
        int32_t $t3_3 = 0;
            int32_t $t6_1 = $t3_3 < $a3 ? 1 : 0;
    
    for (int32_t i = 0; i < $v0; i += 1)
    {
        
        while (true)
        {
            $t3_3 += 1;
            
            if (!$t6_1)
                break;
            
            wmean_new_2 += *$t4_1;
            $t4_1 = &$t4_1[1];
            $a2 = 1;
        }
    }
    
    uint32_t wmean_new_1 = wmean_new;
    
    if ($a2)
        wmean_new_1 = wmean_new_2;
    
    uint32_t $lo_1 = ((wmean_new_1 / ($a3 * $v0)) << 0xa) / data_d04a4_1;
    *arg1 = $lo_1;
    wmean_new = $lo_1;
    int32_t result = data_d04a4_2;
    *arg2 = result;
    return result;
}

