#include "include/main.h"


  int32_t tisp_bcsh_ev_update(int32_t arg1)

{
    int32_t BCSH_real_1 = BCSH_real;
    uint32_t $a0 = arg1 >> 0xa;
        int32_t $v1_1 = data_c5478;
    data_9a614 = arg1;
    cm_ev.31983 = $a0;
    int32_t $a0_1;
    
    if (BCSH_real_1 != 1)
    {
        $a0_1 = $v1_1 - $a0;
        
        if ($v1_1 < $a0)
            $a0_1 = $a0 - $v1_1;
    }
    
    if (BCSH_real_1 == 1 || data_c547c_1 < $a0_1)
    {
        uint32_t cm_ev.31983_1 = cm_ev.31983;
        tiziano_bcsh_update();
        BCSH_real = 0;
        data_c5478 = cm_ev.31983_1;
    }
    
    return 0;
}

