#include "include/main.h"


  int32_t tisp_ccm_ev_update(int32_t arg1)

{
    data_9a454_2 = arg1;
    uint32_t $s0 = arg1 >> 0xa;
    int32_t $a0 = data_c52ec_2;
    int32_t $v0 = $s0 - $a0;
    
    if ($a0 >= $s0)
        $v0 = $a0 - $s0;
    
    if (data_c52f0_2 < $v0)
    {
        jz_isp_ccm();
        data_c52ec_3 = $s0;
    }
    
    return 0;
}

