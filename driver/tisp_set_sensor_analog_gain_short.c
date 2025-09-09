#include "include/main.h"


  uint32_t tisp_set_sensor_analog_gain_short()

{
    void var_28;
    uint32_t $v0_2 = tisp_math_exp2(data_b2ee4(tisp_log2_fixed_to_fixed(), &var_28), 0x10, 0x10);
    int16_t var_1a;
    data_b2f08(var_1a, 0);
    return $v0_2 >> 6;
}

