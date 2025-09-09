#include "include/main.h"


  uint32_t tisp_ae_g_scene_luma(uint32_t* arg1)

{
    uint32_t $v1_3 = ((data_b0de0_1 * tisp_ae_hist) >> 2) + ((data_d4fa4_1 * data_b0de4_1) >> 2);
    int32_t $lo = data_b2e1c_4 * sensor_info / 4;
    int32_t _AePointPos_1 = *_AePointPos;
    int32_t $a1_5 = $lo - $v1_3;
    
    if ($v1_3 >= $lo)
        $a1_5 = 1;
    
    int32_t $v0_5 = fix_point_div_32(_AePointPos_1, $a1_5 << (_AePointPos_1 & 0x1f), 
        $lo << (_AePointPos_1 & 0x1f));
    fix_point_mult2_32(_AePointPos_1, *(((data_b0cec_2 + 1) << 2) + &ev0_cache), $v0_5);
    uint8_t var_20_198;
    uint8_t $t1;
    int32_t $t2_1;
    $t1 = tisp_ae_g_luma(&var_20_199);
    
    if (!var_20_200)
        var_20_201 = $t1;
    
    void* $s2 = &scene_luma_old;
    
    for (void* i = &scene_luma_old; i != &data_afb38_1; )
    {
        int32_t $v0_8 = *(i + 4);
        i += 4;
        *(i - 4) = $v0_8;
    }
    
    data_afb38_2 = fix_point_div_32(_AePointPos_1, $t2_1, var_20_202 << (_AePointPos_1 & 0x1f)) >> 2;
    int32_t i_1 = 0;
    uint32_t $v0_11 = 0;
    
    do
    {
        i_1 += 1;
        int32_t $a2_4 = i_1 * *$s2;
        $s2 += 4;
        $v0_11 += $a2_4;
    } while (i_1 != 8);
    
    scene_luma_wmean = $v0_11;
    uint32_t result = $v0_11 / 0x24;
    scene_luma_weight = 0x24;
    *arg1 = result;
    return result;
}

