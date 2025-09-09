#include "include/main.h"


  int32_t tisp_set_sensor_integration_time_short(int32_t arg1)

{
    void var_38_37;
    int16_t var_26_3;
    
    if (!data_c470c_1)
    {
        int32_t $v0_5 = data_b2ef0_1(arg1, &var_38_38);
        data_d04a8_1 = $v0_5;
        
        if (arg1 != $v0_5)
        {
            int32_t _AePointPos_1 = *_AePointPos;
            int32_t $v0_6 =
                fix_point_mult2_32(_AePointPos_1, data_d04ac_1, arg1 << (_AePointPos_1 & 0x1f));
            int32_t _AePointPos_2 = *_AePointPos;
            data_d04ac_2 = fix_point_div_32(_AePointPos_2, $v0_6, $v0_5 << (_AePointPos_2 & 0x1f));
        }
        
        data_b2ef8_1(var_26_4, 0);
        data_c46f8_1 = data_d04a8_2;
    }
    else
    {
        int32_t $v0_2 = data_b2ef0_2(data_c46f8_2, &var_38_39);
        data_d04a8_3 = $v0_2;
        data_c46f8_3 = $v0_2;
        data_b2ef8_2(var_26_5, 0);
    }
    return 0;
}

