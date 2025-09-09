#include "include/main.h"


  uint32_t tisp_ae_g_scene_luma(uint32_t* arg1)

{
    uint32_t $v1_3 = ((data_b0de0 * tisp_ae_hist) >> 2) + ((data_d4fa4 * data_b0de4) >> 2);
    int32_t $lo = data_b2e1c * sensor_info / 4;
    int32_t _AePointPos_1 = *_AePointPos;
    int32_t $a1_5 = $lo - $v1_3;
    int32_t $v0_5 = fix_point_div_32(_AePointPos_1, $a1_5 << (_AePointPos_1 & 0x1f), 
    void* $s2 = &scene_luma_old;
        int32_t $v0_8 = *(i + 4);
    
    if ($v1_3 >= $lo)
        $a1_5 = 1;
    
        $lo << (_AePointPos_1 & 0x1f));
    fix_point_mult2_32(_AePointPos_1, *(((data_b0cec + 1) << 2) + &ev0_cache), $v0_5);
    uint8_t var_20;
    uint8_t $t1;
    int32_t $t2_1;
    $t1 = tisp_ae_g_luma(&var_20);
    
    if (!var_20)
        var_20 = $t1;
    
    
    for (void* i = &scene_luma_old; i != &data_afb38; )
    {
        i += 4;
        *(i - 4) = $v0_8;
    }
    
    data_afb38_1 = fix_point_div_32(_AePointPos_1, $t2_1, var_20_14 << (_AePointPos_1 & 0x1f)) >> 2;
    int32_t i_1 = 0;
    uint32_t $v0_11 = 0;
    
    do
    {
        int32_t $a2_4 = i_1 * *$s2;
    uint32_t result = $v0_11 / 0x24;
        i_1 += 1;
        $s2 += 4;
        $v0_11 += $a2_4;
    } while (i_1 != 8);
    
    scene_luma_wmean = $v0_11;
    scene_luma_weight = 0x24;
    *arg1 = result;
    return result;
}

