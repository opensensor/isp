#include "include/main.h"


  uint32_t tisp_set_sensor_analog_gain()

{
    int16_t var_28_25;
    uint32_t $v0_2 = tisp_math_exp2(data_b2ee0_1(tisp_log2_fixed_to_fixed(), &var_28_26), 0x10, 0x10);
    data_b2f04_1(var_28_27, 0);
    return $v0_2 >> 6;
}

