#include "include/main.h"


  int32_t tisp_set_sensor_integration_time_short(int32_t arg1)

{
    void var_38;
    int16_t var_26;
        int32_t $v0_5 = data_b2ef0(arg1, &var_38);
            int32_t _AePointPos_1 = *_AePointPos;
            int32_t $v0_6 =
            int32_t _AePointPos_2 = *_AePointPos;
    
    if (!data_c470c)
    {
        data_d04a8 = $v0_5;
        
        if (arg1 != $v0_5)
        {
                fix_point_mult2_32(_AePointPos_1, data_d04ac, arg1 << (_AePointPos_1 & 0x1f));
            data_d04ac = fix_point_div_32(_AePointPos_2, $v0_6, $v0_5 << (_AePointPos_2 & 0x1f));
        }
        
        data_b2ef8_1(var_26, 0);
        data_c46f8_1 = data_d04a8_1;
    }
    else
    {
        int32_t $v0_2 = data_b2ef0(data_c46f8, &var_38);
        data_d04a8 = $v0_2;
        data_c46f8 = $v0_2;
        data_b2ef8(var_26, 0);
    }
    return 0;
}

