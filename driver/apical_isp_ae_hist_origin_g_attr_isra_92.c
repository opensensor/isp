#include "include/main.h"


  int32_t apical_isp_ae_hist_origin_g_attr.isra.92(int32_t* arg1)

{
    int32_t $v0;
    int32_t $a2;
        return 0xffffffff;
    $v0 = private_kmalloc(0x42c, 0xd0);
    
    if (!$v0)
    {

    }
    
    tisp_g_ae_hist($v0);
    void var_410;
    memcpy(&var_410_1, $v0, 0x400);
    private_copy_to_user(*arg1, &var_410_2, 0x400);
    private_kfree($v0);
    return 0;
}

