#include "include/main.h"


  uint32_t tisp_set_sensor_digital_gain_short()

{
    void var_28;
    uint32_t $v0_2 = tisp_math_exp2(data_b2ee8(tisp_log2_fixed_to_fixed(), &var_28), 0x10, 0x10);
    int16_t var_26;
    data_b2f0c(var_26, 0);
    return $v0_2 >> 6;
}

