#include "include/main.h"


  int32_t tisp_bcsh_ev_update(int32_t arg1)

{
    int32_t BCSH_real_1 = BCSH_real;
    data_9a614_3 = arg1;
    uint32_t $a0 = arg1 >> 0xa;
    cm_ev.31983 = $a0;
    int32_t $a0_1;
    
    if (BCSH_real_1 != 1)
    {
        int32_t $v1_1 = data_c5478_1;
        $a0_1 = $v1_1 - $a0;
        
        if ($v1_1 < $a0)
            $a0_1 = $a0 - $v1_1;
    }
    
    if (BCSH_real_1 == 1 || data_c547c_1 < $a0_1)
    {
        tiziano_bcsh_update();
        uint32_t cm_ev.31983_1 = cm_ev.31983;
        BCSH_real = 0;
        data_c5478_2 = cm_ev.31983_1;
    }
    
    return 0;
}

