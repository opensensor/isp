#include "include/main.h"


  uint32_t tisp_set_sensor_digital_gain_short()

{
    void var_28_30;
    uint32_t $v0_2 = tisp_math_exp2(data_b2ee8_1(tisp_log2_fixed_to_fixed(), &var_28_31), 0x10, 0x10);
    int16_t var_26_6;
    data_b2f0c_1(var_26_7, 0);
    return $v0_2 >> 6;
}

