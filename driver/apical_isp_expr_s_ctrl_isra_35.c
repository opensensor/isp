#include "include/main.h"


  int32_t apical_isp_expr_s_ctrl.isra.35(void* arg1, int32_t arg2)

{
    void* $s0 = *(*(arg1 + 0xd4) + 0x120);
    int32_t var_18_26;
    private_copy_from_user(&var_18_27, arg2, 0xc);
    int32_t $v0_1 = var_18_28;
    int32_t var_108;
    uint32_t var_98;
    
    if ($v0_1)
    {
        if ($v0_1 != 1)
        {
            isp_printf(1, "VIC_CTRL : %08x\\n", "apical_isp_expr_s_ctrl");
            return 0xffffffff;
        }
        
        int32_t var_68_2 = $v0_1;
        int32_t var_14_2;
        int16_t var_10;
        
        if (var_14_3 != $v0_1)
        {
            if (var_14_4)
            {
                isp_printf(1, "sensor type is BT1120!\\n", "apical_isp_expr_s_ctrl");
                return 0xffffffff;
            }
            
            var_98_3 = var_10_2;
        }
        else
        {
            uint32_t $v1_2 = *($s0 + 0xbc);
            
            if (!$v1_2)
            {
                var_108_1 = 0;
                isp_printf(1, "Can not support this frame mode!!!\\n", "apical_isp_expr_s_ctrl");
                return 0xffffffff;
            }
            
            var_98_4 = var_10_3 / $v1_2;
        }
    }
    else
        int32_t var_68_1 = 0;
    
    for (int32_t i = 0; i < 0x70; i += 1)
    {
        char var_88[0x20];
        *(&var_108_2 + i) = var_88_1[i];
    }
    
    int32_t var_94;
    tisp_s_ae_attr(var_98_5, var_94_1);
    return 0;
}

