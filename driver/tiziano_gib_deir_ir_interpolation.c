#include "include/main.h"


  uint32_t tiziano_gib_deir_ir_interpolation(int32_t arg1)

{
    int32_t gib_ir_point_1 = gib_ir_point;
    int32_t $ra;
    int32_t var_4 = $ra;
    int32_t $t7 = data_aa218;
    int32_t $t5 = data_aa210;
    int32_t $t8 = data_aa214;
        uint32_t $v1_3;
    
    if ($t7 >= arg1)
    {
        
        if ($t8 < arg1)
            $v1_3 = 1;
        else if ($t5 < arg1)
            $v1_3 = 2;
        else if (gib_ir_point_1 >= arg1)
            $v1_3 = 4;
        else
            $v1_3 = 3;
        
        deir_flag.31810 = $v1_3;
    }
    else
        deir_flag.31810 = 0;
    
    uint32_t deir_flag.31810_1 = deir_flag.31810;
    
    if (deir_flag.31810_1)
    {
            goto label_31f18;
        if (deir_flag.31810_1 != 2 && deir_flag.31810_1 != 4)
        {
            deir_flag_last.31811 = deir_flag.31810_1;
        }
        
        if (deir_flag_last.31811 != deir_flag.31810_1)
        {
            goto label_31f18;
            deir_flag_last.31811 = deir_flag.31810_1;
        }
    }
    else if (deir_flag_last.31811)
    {
            void* $a0;
            void* $a1_3;
            void* $a2;
        deir_flag_last.31811 = deir_flag.31810_1;
    label_31f18:
        
        if (deir_flag.31810_1 < 5)
        {
            int32_t (* $t9)(void* arg1, void* arg2, void* arg3) = tiziano_gib_deir_reg;
            
            switch (deir_flag.31810_1)
            {
                case 0:
                {
                    $a2 = &tiziano_gib_deir_b_h;
                    $a1_3 = &tiziano_gib_deir_g_h;
                    $a0 = &tiziano_gib_deir_r_h;
                    break;
                }
                case 1:
                {
                    int32_t $t3_1;
                    int32_t $t4_1;
                    int32_t $t7_1;
                    int32_t $t8_1;
                    int32_t $t3_2;
                    int32_t $t4_2;
                    int32_t $t7_2;
                    int32_t $t8_2;
                    void* $t3_3;
                    void* $t5_1;
                    void* $t6;
                    $t3_1 = tiziano_gib_deir_interpolate(&gib_deir_r.31816, arg1, $t7, $t8, 
                        &tiziano_gib_deir_r_h, &tiziano_gib_deir_r_m);
                    $t3_2 = $t4_1(&gib_deir_g.31817, $t3_1, $t7_1, $t8_1, &tiziano_gib_deir_g_h, 
                        &tiziano_gib_deir_g_m);
                    $t3_3 = $t4_2(&gib_deir_b.31818, $t3_2, $t7_2, $t8_2, &tiziano_gib_deir_b_h, 
                        &tiziano_gib_deir_b_m);
                    $a2 = $t3_3 - 0x3cc8;
                    $a1_3 = $t6 - 0x3c44;
                    $a0 = $t5_1 - 0x3bc0;
                    break;
                }
                case 2:
                {
                    $a2 = &tiziano_gib_deir_b_m;
                    $a1_3 = &tiziano_gib_deir_g_m;
                    $a0 = &tiziano_gib_deir_r_m;
                    break;
                }
                case 3:
                {
                    int32_t $t3_4;
                    int32_t $t4_3;
                    int32_t $t5_2;
                    int32_t $t6_1;
                    int32_t $t3_5;
                    int32_t $t4_4;
                    int32_t $t5_3;
                    int32_t $t6_2;
                    void* $t3_6;
                    void* $t7_3;
                    void* $t8_3;
                    $t3_4 = tiziano_gib_deir_interpolate(&gib_deir_r.31816, arg1, $t5, 
                        gib_ir_point_1, &tiziano_gib_deir_r_m, &tiziano_gib_deir_r_l);
                    $t3_5 = $t4_3(&gib_deir_g.31817, $t3_4, $t5_2, $t6_1, &tiziano_gib_deir_g_m, 
                        &tiziano_gib_deir_g_l);
                    $t3_6 = $t4_4(&gib_deir_b.31818, $t3_5, $t5_3, $t6_2, &tiziano_gib_deir_b_m, 
                        &tiziano_gib_deir_b_l);
                    $a2 = $t3_6 - 0x3cc8;
                    $a1_3 = $t8_3 - 0x3c44;
                    $a0 = $t7_3 - 0x3bc0;
                    break;
                }
                case 4:
                {
                    $a2 = &tiziano_gib_deir_b_l;
                    $a1_3 = &tiziano_gib_deir_g_l;
                    $a0 = &tiziano_gib_deir_r_l;
                    break;
                }
            }
            
            /* tailcall */
            return $t9($a0, $a1_3, $a2);
        }
    }
    
    return deir_flag.31810_1;
}

