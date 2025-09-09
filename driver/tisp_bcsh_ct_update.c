#include "include/main.h"


  int32_t tisp_bcsh_ct_update(uint32_t arg1)

{
    int32_t BCSH_real_1 = BCSH_real;
    int32_t $a0;
        int32_t $v1_1 = data_c5480;
    data_9a610 = arg1;
    cm_ct.31987 = arg1;
    
    if (BCSH_real_1 != 1)
    {
        $a0 = arg1 - $v1_1;
        
        if ($v1_1 >= arg1)
            $a0 = $v1_1 - arg1;
    }
    
    if (BCSH_real_1 == 1 || data_c5484_1 < $a0)
    {
        tiziano_bcsh_update();
        uint32_t cm_ct.31987_1 = cm_ct.31987;
        BCSH_real = 0;
        data_c5480 = cm_ct.31987_1;
    }
    
    return 0;
}

