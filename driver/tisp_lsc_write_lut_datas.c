#include "include/main.h"


  int32_t tisp_lsc_write_lut_datas()

{
    lsc_count += 1;
    
    if (!lsc_api_flag)
    {
        lsc_ct_update_flag = tisp_lsc_judge_ct_update_flag();
        lsc_gain_update_flag = tisp_lsc_judge_gain_update_flag();
    }
    
    if (lsc_ct_update_flag == 1 || data_9a400_1 == 1 || lsc_api_flag == 1)
    {
        int32_t $v0_5 = data_9a408;
            int32_t $a1_2 = data_9a410;
            uint32_t $lo_1 = ((data_9a40c - $a1_2) << 0xc) / (data_9a414 - $a1_2);
            int32_t $a3_2 = data_9a428 << 2;
                int32_t $v0_10 = *(&lsc_a_lut + i);
                int32_t $v1_5 = *(&lsc_t_lut + i);
                int32_t $t4_1 = $v0_10 >> 0xc;
                int32_t $v0_11 = $v0_10 & 0xfff;
                int32_t $a0_5 = (((($v1_5 >> 0xc) - $t4_1) * $lo_1) >> 0xc) + $t4_1;
                int32_t $v0_12 = (((($v1_5 & 0xfff) - $v0_11) * $lo_1) >> 0xc) + $v0_11;
        
        if (!$v0_5)
            memcpy(&lsc_final_lut, &lsc_a_lut, 0x1ffc);
        else if ($v0_5 == 1)
        {
            
            for (int32_t i = 0; $a3_2 != i; i += 4)
            {
                
                if ($a0_5 < 0)
                    $a0_5 = 0;
                
                if ($v0_12 < 0)
                    $v0_12 = 0;
                
                if ($(uintptr_t)a0_5 >= 0x1000)
                    $a0_5 = 0xfff;
                
                if ($(uintptr_t)v0_12 >= 0x1000)
                    $v0_12 = 0xfff;
                
                *(&lsc_final_lut + i) = $a0_5 << 0xc | $v0_12;
            }
        }
        else if ($v0_5 == 2)
            memcpy(&lsc_final_lut, &lsc_t_lut, 0x1ffc);
        else if ($v0_5 != 3)
            memcpy(&lsc_final_lut, &lsc_d_lut, 0x1ffc);
        else
        {
            int32_t $a1_5 = data_9a418;
            uint32_t $lo_2 = ((data_9a40c - $a1_5) << 0xc) / (data_9a41c - $a1_5);
            int32_t $a3_4 = data_9a428 << 2;
                int32_t $v0_18 = *(&lsc_t_lut + i_1);
                int32_t $v1_16 = *(&lsc_d_lut + i_1);
                int32_t $t4_3 = $v0_18 >> 0xc;
                int32_t $v0_19 = $v0_18 & 0xfff;
                int32_t $a0_11 = (((($v1_16 >> 0xc) - $t4_3) * $lo_2) >> 0xc) + $t4_3;
                int32_t $v0_20 = (((($v1_16 & 0xfff) - $v0_19) * $lo_2) >> 0xc) + $v0_19;
            
            for (int32_t i_1 = 0; i_1 != $a3_4; i_1 += 4)
            {
                
                if ($a0_11 < 0)
                    $a0_11 = 0;
                
                if ($v0_20 < 0)
                    $v0_20 = 0;
                
                if ($(uintptr_t)a0_11 >= 0x1000)
                    $a0_11 = 0xfff;
                
                if ($(uintptr_t)v0_20 >= 0x1000)
                    $v0_20 = 0xfff;
                
                *(&lsc_final_lut + i_1) = $a0_11 << 0xc | $v0_20;
            }
        }
    }
    
    uint32_t lsc_mesh_scale_1 = lsc_mesh_scale;
    int32_t $s0 = 0x800;
    
    if (lsc_mesh_scale_1)
    {
        $s0 = 0x400;
        
        if (lsc_mesh_scale_1 != 1)
        {
            $s0 = 0x100;
            
            if (lsc_mesh_scale_1 == 2)
                $s0 = 0x200;
        }
    }
    
    uint32_t lsc_api_flag_1;
    
    if (lsc_gain_update_flag != 1 && lsc_ct_update_flag != 1 && data_9a400_2 != 1)
        lsc_api_flag_1 = lsc_api_flag;
    
    if (lsc_gain_update_flag == 1 || lsc_ct_update_flag == 1 || data_9a400_3 == 1
        || lsc_api_flag == 1)
    {
        char* $s6_1 = (char*)(&lsc_final_lut); // Fixed void pointer assignment
        int32_t $fp_1 = 0;
            int32_t $a2_1 = *$s6_1;
            int32_t $a0_13 = *($s6_1 + 4);
            int32_t $v1_30 = *($s6_1 + 8);
            uint32_t lsc_curr_str_1 = lsc_curr_str;
            int32_t $t3_5 = $s0 + (((($a2_1 & 0xfff) - $s0) * lsc_curr_str_1) >> 0xc);
            int32_t $t1_5 = $s0 + (((($a2_1 >> 0xc) - $s0) * lsc_curr_str_1) >> 0xc);
            int32_t $v0_28 = $s0 + (((($v1_30 >> 0xc) - $s0) * lsc_curr_str_1) >> 0xc);
            int32_t $t2_5 = $s0 + (((($a0_13 & 0xfff) - $s0) * lsc_curr_str_1) >> 0xc);
            int32_t $a2_6 = $s0 + (((($v1_30 & 0xfff) - $s0) * lsc_curr_str_1) >> 0xc);
            int32_t $s4_1 = 0xfff;
            int32_t $a0_18 = $s0 + (((($a0_13 >> 0xc) - $s0) * lsc_curr_str_1) >> 0xc);
            int32_t $s3_1 = $fp_1 << 4;
            int32_t $t6_2 = $a0_18;
            int32_t $t2_6 = $t2_5;
            int32_t $t5_2 = $v0_28;
        
        while ($fp_1 * 3 < data_9a428)
        {
            
            if ($t3_5 < 0)
                $t3_5 = 0;
            
            
            if ($v0_28 < 0)
                $v0_28 = 0;
            
            if ($t1_5 < 0)
                $t1_5 = 0;
            
            
            if ($t2_5 < 0)
                $t2_5 = 0;
            
            
            if ($a2_6 < 0)
                $a2_6 = 0;
            
            if ($(uintptr_t)t1_5 >= 0x1000)
                $t1_5 = 0xfff;
            
            if ($(uintptr_t)t3_5 >= 0x1000)
                $t3_5 = 0xfff;
            
            if ($a0_18 < 0)
                $a0_18 = 0;
            
            system_reg_write($s3_1 + 0x28000, $t1_5 << 0xc | $t3_5);
            
            if ($(uintptr_t)t6_2 >= 0x1000)
                $t6_2 = 0xfff;
            
            if ($(uintptr_t)t2_6 >= 0x1000)
                $t2_6 = 0xfff;
            
            system_reg_write($s3_1 + 0x28004, $t6_2 << 0xc | $t2_6);
            
            if ($(uintptr_t)t5_2 >= 0x1000)
                $t5_2 = 0xfff;
            
            if ($(uintptr_t)a2_6 < 0x1000)
                $s4_1 = $a2_6;
            
            system_reg_write($s3_1 + 0x28008, $t5_2 << 0xc | $s4_1);
            $fp_1 += 1;
            $s6_1 += 0xc;
        }
        
        system_reg_write(0x2800c, 0);
        lsc_api_flag_1 = lsc_api_flag;
    }
    
    if (!lsc_api_flag_1)
    {
        lsc_ct_update_flag = 0;
        lsc_gain_update_flag = 0;
        data_9a400 = 0;
    }
    
    return 0;
}

