#include "include/main.h"


  int32_t apical_isp_af_hist_s_attr.isra.50(int32_t arg1)

{
    uint32_t $v0_1 = var_43;
    void var_60;
    private_copy_from_user(&var_60, arg1, 0x58);
    char var_43;
    char var_44;
    
    if (!var_44)
    {
        var_44 = 1;
        $v0_1 = var_43;
    }
    
    char var_42;
    uint32_t $v0_3 = var_42_1;
    
    if ($v0_1 < 3)
    {
        var_43 = 3;
        $v0_3 = var_42;
    }
    
    if ($(uintptr_t)v0_3 < 0x10)
    {
        uint32_t $a0_1 = var_41;
            uint8_t var_9a_1 = $v0_3;
            char var_a8 = var_50;
            char var_97_1 = var_3f;
            char var_92_1 = var_3a;
            char var_96_1 = var_3e;
            char var_95_1 = var_3d;
            uint8_t var_99_1 = $a0_1;
            char var_94_1 = var_3c;
            char var_91_1 = var_39;
            char var_a7_1 = var_4f;
            int16_t var_a6_1 = var_4e;
            int16_t var_a4_1 = var_4c;
            int16_t var_a2_1 = var_4a;
            int16_t var_a0_1 = var_48;
            int16_t var_9e_1 = var_46;
            char var_9c_1 = var_44;
            char var_9b_1 = var_43;
            int16_t var_90_1 = var_38;
            char var_8e_1 = var_36;
            char var_8d_1 = var_35;
            char var_8c_1 = var_34;
            int16_t var_8a_1 = var_32;
            char var_88_1 = var_30;
            char var_86_1 = var_2e;
            char var_85_1 = var_2d;
            int16_t var_84_1 = var_2c;
            char var_82_1 = var_2a;
            char var_81_1 = var_29;
            char var_80_1 = var_28;
            int16_t var_7e_1 = var_26;
            char var_7c_1 = var_24;
            char var_7a_1 = var_22;
            char var_79_1 = var_21;
            int16_t var_78_1 = var_20;
            char var_76_1 = var_1e;
            char var_75_1 = var_1d;
            char var_74_1 = var_1c;
            int16_t var_72_1 = var_1a;
            char var_70_1 = var_18;
            char var_6e_1 = var_16;
            char var_6d_1 = var_15;
            int16_t var_6c_1 = var_14;
            char var_6a_1 = var_12;
            char var_69_1 = var_11;
            char var_68_1 = var_10;
            int16_t var_66_1 = var_e;
            char var_64_1 = var_c;
        char var_41;
        
        if ($v0_3 >= 5 && $a0_1 - (uintptr_t)5 < 0xb)
        {
            char var_50;
            char var_3f;
            char var_3a;
            char var_3e;
            char var_3d;
            char var_3c;
            char var_39;
            char var_4f;
            int16_t var_4e;
            int16_t var_4c;
            int16_t var_4a;
            int16_t var_48;
            int16_t var_46;
            int16_t var_38;
            char var_36;
            char var_35;
            char var_34;
            int16_t var_32;
            char var_30;
            char var_2e;
            char var_2d;
            int16_t var_2c;
            char var_2a;
            char var_29;
            char var_28;
            int16_t var_26;
            char var_24;
            char var_22;
            char var_21;
            int16_t var_20;
            char var_1e;
            char var_1d;
            char var_1c;
            int16_t var_1a;
            char var_18;
            char var_16;
            char var_15;
            int16_t var_14;
            char var_12;
            char var_11;
            char var_10;
            int16_t var_e;
            char var_c;
            
            for (int32_t i = 0; (uintptr_t)i < 0x48; i += 1)
            {
                char var_100[0x48];
                var_100[i] = (&var_a8)[i];
            }
            
            int32_t var_b8_1;
            tisp_s_af_attr(var_b8_2);
            return 0;
        }
    }
    
    isp_printf(); // Fixed: macro call, removed arguments\n", 
        "apical_isp_af_hist_s_attr");
    return 0xffffffff;
}

