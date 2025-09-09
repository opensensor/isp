#include "include/main.h"


  int32_t apical_isp_af_hist_s_attr.isra.50(int32_t arg1)

{
    void var_60;
    private_copy_from_user(&var_60_1, arg1, 0x58);
    char var_43;
    uint32_t $v0_1 = var_43_1;
    char var_44;
    
    if (!var_44_2)
    {
        var_44_3 = 1;
        $v0_1 = var_43_2;
    }
    
    char var_42;
    uint32_t $v0_3 = var_42_1;
    
    if ($v0_1 < 3)
    {
        var_43_3 = 3;
        $v0_3 = var_42_2;
    }
    
    if ($v0_3 < 0x10)
    {
        char var_41;
        uint32_t $a0_1 = var_41_1;
        
        if ($v0_3 >= 5 && $a0_1 - 5 < 0xb)
        {
            uint8_t var_9a_1 = $v0_3;
            char var_50_3;
            char var_a8_2 = var_50_4;
            char var_3f;
            char var_97_1 = var_3f_1;
            char var_3a;
            char var_92_1 = var_3a_1;
            char var_3e;
            char var_96_1 = var_3e_1;
            char var_3d;
            char var_95_1 = var_3d_1;
            uint8_t var_99_1 = $a0_1;
            char var_3c;
            char var_94_1_1 = var_3c_3;
            char var_39;
            char var_91_1 = var_39_1;
            char var_4f;
            char var_a7_1 = var_4f_1;
            int16_t var_4e;
            int16_t var_a6_1 = var_4e_1;
            int16_t var_4c;
            int16_t var_a4_1 = var_4c_1;
            int16_t var_4a;
            int16_t var_a2_1 = var_4a_1;
            int16_t var_48_10;
            int16_t var_a0_1_1 = var_48_11;
            int16_t var_46;
            int16_t var_9e_1 = var_46_1;
            char var_9c_1_1 = var_44_4;
            char var_9b_1 = var_43_4;
            int16_t var_38_9;
            int16_t var_90_1_1 = var_38_10;
            char var_36;
            char var_8e_1 = var_36_1;
            char var_35;
            char var_8d_1 = var_35_1;
            char var_34;
            char var_8c_1 = var_34_2;
            int16_t var_32;
            int16_t var_8a_1 = var_32_1;
            char var_30_4;
            char var_88_1_1 = var_30_5;
            char var_2e;
            char var_86_1 = var_2e_1;
            char var_2d;
            char var_85_1 = var_2d_1;
            int16_t var_2c;
            int16_t var_84_1 = var_2c_2;
            char var_2a;
            char var_82_1 = var_2a_2;
            char var_29;
            char var_81_1 = var_29_1;
            char var_28;
            char var_80_1 = var_28_2;
            int16_t var_26;
            int16_t var_7e_1 = var_26_1;
            char var_24;
            char var_7c_1 = var_24_2;
            char var_22;
            char var_7a_1 = var_22_1;
            char var_21;
            char var_79_1 = var_21_1;
            int16_t var_20_6;
            int16_t var_78_1 = var_20_7;
            char var_1e;
            char var_76_1 = var_1e_1;
            char var_1d;
            char var_75_1 = var_1d_1;
            char var_1c;
            char var_74_1 = var_1c_2;
            int16_t var_1a;
            int16_t var_72_1 = var_1a_1;
            char var_18_29;
            char var_70_1 = var_18_30;
            char var_16;
            char var_6e_1 = var_16_1;
            char var_15;
            char var_6d_1 = var_15_1;
            int16_t var_14_5;
            int16_t var_6c_1 = var_14_6;
            char var_12;
            char var_6a_1 = var_12_1;
            char var_11;
            char var_69_1 = var_11_1;
            char var_10_4;
            char var_68_1_1 = var_10_5;
            int16_t var_e;
            int16_t var_66_1 = var_e_1;
            char var_c;
            char var_64_1 = var_c_1;
            
            for (int32_t i = 0; i < 0x48; i += 1)
            {
                char var_100[0x48];
                var_100_1[i] = (&var_a8_3)[i];
            }
            
            int32_t var_b8_3;
            tisp_s_af_attr(var_b8_4);
            return 0;
        }
    }
    
    isp_printf(1, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n", 
        "apical_isp_af_hist_s_attr");
    return 0xffffffff;
}

