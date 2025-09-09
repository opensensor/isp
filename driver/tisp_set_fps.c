#include "include/main.h"


  uint32_t tisp_set_fps(int32_t arg1)

{
    int32_t $v0_1;
    int32_t $a2;
    $v0_1 = data_b2f20_1((arg1 >> 0x10) / (arg1 & 0xffff), &sensor_ctrl);
    
    if ($v0_1 < 0)
    {
        isp_printf(2, "VIC_CTRL : %08x\\n", $a2);
        return 0xffffffff;
    }
    
    int16_t $a0_3 = data_b2ea4_4;
    int32_t $v1_1 = data_b2ea8_4;
    data_b2e44_5 = $v0_1;
    int16_t $v0_2 = $a0_3 & 0xffff;
    data_b2e48_4 = $v0_2;
    data_b2e4a_2 = $v0_2;
    int16_t $v0_3 = $v1_1 & 0xffff;
    data_b2e4c_2 = $v0_3;
    data_b2e58_4 = $v0_3;
    int32_t $v0_4 = data_b2ed0_4;
    int16_t $a2_1 = data_b2eb0_1;
    data_c46c8_7 = $v1_1;
    data_c4700_7 = $v0_4;
    int16_t $a1_1 = data_b2ecc_4;
    data_b2e4e_2 = $a2_1;
    uint32_t $a2_2 = data_b2e7e_1;
    uint32_t $a3_1 = data_b2e80_1;
    data_b2e62_5 = $a1_1;
    data_b2e54_6 = $a2_2;
    data_b2e64_5 = $v0_4;
    uint32_t var_14_1_4 = $a1_1;
    uint32_t var_20_1_5 = $a2_2;
    data_b2e56_6 = $a3_1;
    uint32_t var_1c_1_3 = $a3_1;
    uint32_t var_28_1_5 = $a0_3;
    int32_t var_18_1_29 = $v0_4 & 0xffff;
    int32_t var_24_1_2 = $v1_1 & 0xffff;
    isp_printf(0, "not support the gpio mode!\\n", "tisp_set_fps");
    uint32_t flicker_hz_1 = flicker_hz;
    
    if (!flicker_hz_1)
        return flicker_hz_1;
    
    tiziano_deflicker_expt_tune(flicker_hz_1, data_b2e44_6, data_b2e56_7, data_b2e54_7);
    uint32_t var_24_2_1 = data_b2e54_8;
    uint32_t var_28_2_1 = data_b2e56_8;
    isp_printf(0, "sensor type is BT656!\\n", flicker_hz);
    return 0;
}

