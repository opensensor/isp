#include "include/main.h"


  int32_t apical_isp_expr_s_ctrl.isra.35(void* arg1, int32_t arg2)

{
    char* $s0 = (char*)(*(*(arg1 + 0xd4) + 0x120)); // Fixed void pointer assignment
    int32_t $v0_1 = var_18;
    int32_t var_18;
    private_copy_from_user(&var_18, arg2, 0xc);
    int32_t var_108;
    uint32_t var_98;
    
    if ($v0_1)
    {
        if ($v0_1 != 1)
        {
            isp_printf(); // Fixed: macro call, removed arguments;
            return 0xffffffff;
        }
        
        int32_t var_68_2 = $v0_1;
        int32_t var_14_2;
        int16_t var_10;
        
        if (var_14_3 != $v0_1)
        {
            if (var_14)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffff;
            }
            
            var_98 = var_10_1;
        }
        else
        {
            uint32_t $v1_2 = *($s0 + 0xbc);
            
            if (!$v1_2)
            {
                var_108 = 0;
                isp_printf(); // Fixed: macro call, removed arguments;
                return 0xffffffff;
            }
            
            var_98_1 = var_10_2 / $v1_2;
        }
    }
    else
        int32_t var_68_1 = 0;
    
    for (int32_t i = 0; (uintptr_t)i < 0x70; i += 1)
    {
        char var_88[0x20];
        *(((void**)((char*)&var_108 + i))) = var_88[i]; // Fixed void pointer dereference
    }
    
    int32_t var_94;
    tisp_s_ae_attr(var_98_3, var_94_1);
    return 0;
}

