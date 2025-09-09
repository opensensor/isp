#include "include/main.h"


  int32_t tisp_ccm_ct_update(int32_t arg1)

{
    int32_t $a1 = data_c52f4;
    int32_t $v0 = arg1 - $a1;
    data_9a450 = arg1;
    
    if ($a1 >= arg1)
        $v0 = $a1 - arg1;
    
    if (data_c52f8 < $v0)
    {
        jz_isp_ccm();
        data_c52f4 = arg1;
    }
    
    return 0;
}

