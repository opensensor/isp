#include "include/main.h"


  uint32_t tisp_set_fps(int32_t arg1)

{
    int32_t $v0_1;
    int32_t $a2;
        return 0xffffffff;
    $v0_1 = data_b2f20((arg1 >> 0x10) / (arg1 & 0xffff), &sensor_ctrl);
    
    if ($v0_1 < 0)
    {

    }
    
    int16_t $a0_3 = data_b2ea4_2;
    int32_t $v1_1 = data_b2ea8_1;
    data_b2e44_2 = $v0_1;
    int16_t $v0_2 = $a0_3 & 0xffff;
    data_b2e48_1 = $v0_2;
    data_b2e4a_1 = $v0_2;
    int16_t $v0_3 = $v1_1 & 0xffff;
    data_b2e4c_1 = $v0_3;
    data_b2e58_1 = $v0_3;
    int32_t $v0_4 = data_b2ed0_1;
    int16_t $a2_1 = data_b2eb0_1;
    data_c46c8_2 = $v1_1;
    data_c4700_2 = $v0_4;
    int16_t $a1_1 = data_b2ecc_2;
    data_b2e4e_1 = $a2_1;
    uint32_t $a2_2 = data_b2e7e_1;
    uint32_t $a3_1 = data_b2e80_1;
    data_b2e62_1 = $a1_1;
    data_b2e54_2 = $a2_2;
    data_b2e64_1 = $v0_4;
    uint32_t var_14_1_3 = $a1_1;
    uint32_t var_20_1_1 = $a2_2;
    data_b2e56_2 = $a3_1;
    uint32_t var_1c_1 = $a3_1;
    uint32_t var_28_1_1 = $a0_3;
    int32_t var_18_1_1 = $v0_4 & 0xffff;
    int32_t var_24_1 = $v1_1 & 0xffff;

    uint32_t flicker_hz_1 = flicker_hz;
    
    if (!flicker_hz_1)
        return flicker_hz_1;
    
    tiziano_deflicker_expt_tune(flicker_hz_1, data_b2e44_3, data_b2e56_3, data_b2e54_3);
    uint32_t var_24_2 = data_b2e54_4;
    uint32_t var_28_2_1 = data_b2e56_4;

    return 0;
}

