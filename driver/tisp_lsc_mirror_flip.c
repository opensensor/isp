#include "include/main.h"


  int32_t tisp_lsc_mirror_flip(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)

{
    int32_t lsc_mesh_size_1 = *lsc_mesh_size;
    int32_t $s1 = *(lsc_mesh_size + 4);
    int32_t $s1_3 = arg2 / $s1 + 1 + (0 < arg2 % $s1 ? 1 : 0);
    int32_t $s0_2 = arg1 / lsc_mesh_size_1 + 1 + (0 < arg1 % lsc_mesh_size_1 ? 1 : 0);
    int32_t $s2_1 = ($s0_2 & 1) + $s0_2;
    
    if (arg3 == 1)
    {
        tisp_lsc_upside_down_lut(&lsc_a_lut, $s1_3, $s2_1);
        tisp_lsc_upside_down_lut(&lsc_t_lut, $s1_3, $s2_1);
        tisp_lsc_upside_down_lut(&lsc_d_lut, $s1_3, $s2_1);
    }
    
    if (arg4 == 1)
    {
        int32_t $t1_1 = 0;
        int32_t $t2_1 = 0;
            int32_t $t0_1 = $s0_2 - 1;
            int32_t i = 0;
                int32_t $t5_3 = (i + $t1_1) / 2 * 3;
                int32_t $t6_3 = ($t0_1 + $t1_1) / 2 * 3;
                int32_t $t7_1;
                int32_t $t8_2;
                int32_t $t9_2;
                int32_t $t7_2;
                int32_t $t8_3;
                int32_t $t9_3;
                int32_t $t5_4;
                int32_t $t6_4;
                int32_t $t7_3;
                int32_t $t8_4;
                int32_t $t9_4;
                int32_t $t7_4;
                int32_t $t8_5;
                int32_t $t9_5;
                int32_t $t7_5;
                int32_t $t8_6;
                int32_t $t9_6;
                int32_t $t5_5;
                int32_t $t6_5;
                int32_t $t7_6;
                int32_t $t8_7;
                int32_t $t9_7;
                int32_t $t5_6;
                int32_t $t7_7;
                int32_t $t8_8;
                int32_t $t9_8;
                int32_t $t5_7;
                int32_t $t7_8;
                int32_t $t8_9;
                int32_t $t9_9;
        
        while (true)
        {
            
            if ($t2_1 >= $s1_3)
                break;
            
            
            while (i < $s0_2 >> 1)
            {
                $t7_1 = tisp_lsc_lut_mirror_exchange(&lsc_a_lut, $t5_3, $t6_3, i & 1, $t0_1 & 1);
                $t7_2 = $t7_1(&lsc_a_lut, $t5_3 + 2, $t6_3 + 2, $t8_2, $t9_2);
                $t5_4 = $t7_2(&lsc_a_lut, $t5_3 + 1, $t6_3 + 1, $t8_3, $t9_3);
                $t7_4 = $t7_3(&lsc_t_lut, $t5_4, $t6_4, $t8_4, $t9_4);
                $t7_5 = $t7_4(&lsc_t_lut, $t5_3 + 2, $t6_3 + 2, $t8_5, $t9_5);
                $t5_5 = $t7_5(&lsc_t_lut, $t5_3 + 1, $t6_3 + 1, $t8_6, $t9_6);
                $t5_6 = $t7_6(&lsc_d_lut, $t5_5, $t6_5, $t8_7, $t9_7);
                $t5_7 = $t7_7($t5_6 - 0x1a78, $t5_3 + 2, $t6_3 + 2, $t8_8, $t9_8);
                $t7_8($t5_7 - 0x1a78, $t5_3 + 1, $t6_3 + 1, $t8_9, $t9_9);
                i += 1;
                $t0_1 -= 1;
            }
            
            $t2_1 += 1;
            $t1_1 += $s2_1;
        }
    }
    
    lsc_api_flag = 1;
    data_9a400_5 = 1;
    tisp_lsc_write_lut_datas();
    lsc_api_flag = 0;
    return 0;
}

