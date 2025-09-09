#include "include/main.h"


  int32_t tisp_wdr_rx_ae0_dms(int32_t* arg1, int32_t* arg2, int32_t* arg3, void* arg4)

{
    int32_t $t1_1 = *(arg4 + 4) * *(arg4 + 0xc);
    void* $a3 = arg4 + 0x10;
    void* $v1 = &wdr_block_mean0;
    int32_t $t2 = 0;
        int32_t $v0 = $t2 < $t1_1 ? 1 : 0;
        int32_t $v0_3 = *arg1 + *arg2 + *arg3;
        int32_t $lo_1 = $v0_3 / (*$a3 * *($a3 + 0x3c));
    
    while (true)
    {
        $t2 += 1;
        
        if (!$v0)
            break;
        
        arg1 = &arg1[1];
        arg2 = &arg2[1];
        arg3 = &arg3[1];
        $a3 += 4;
        $v1 += 4;
        *($v1 - 4) = $lo_1;
    }
    
    return 0;
}

