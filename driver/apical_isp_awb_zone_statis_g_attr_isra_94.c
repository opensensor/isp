#include "include/main.h"


  int32_t apical_isp_awb_zone_statis_g_attr.isra.94(int32_t* arg1)

{
    int32_t $v0;
    int32_t $a2;
    $v0 = private_vmalloc(0x1c);
    
    if (!$v0)
    {
        isp_printf(); // Fixed: macro call, removed arguments!\n", $a2);
        return 0xffffffff;
    }
    
    tisp_g_wb_zone();
    void var_2b8;
    memcpy(&var_2b8_1, $v0, 0x2a3);
    private_copy_to_user(*arg1, &var_2b8_2, 0x2a3);
    private_vfree($v0);
    return 0;
}

