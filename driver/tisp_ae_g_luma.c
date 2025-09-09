#include "include/main.h"


  uint8_t tisp_ae_g_luma(uint8_t* arg1)

{
    int32_t* $v0 = &tisp_ae_hist;
    int32_t i = 0;
    int32_t $a1 = 0;
    
    do
    {
        int32_t $a3_1 = *$v0;
        $v0 = &$v0[1];
        int32_t $t0_1 = i * $a3_1;
        i += 1;
        $a1 += $t0_1;
    } while (i != 0x100);
    
    uint8_t result = $a1 / (data_b2e1c_3 * sensor_info / 4);
    *arg1 = result;
    return result;
}

