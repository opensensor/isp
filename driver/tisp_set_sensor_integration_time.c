#include "include/main.h"


  int32_t tisp_set_sensor_integration_time(int32_t arg1)

{
    void var_38_34;
    int16_t var_28_22;
    
    if (!dmsc_sp_d_w_stren_wdr_array)
    {
        int32_t $v0_5 = data_b2eec_1(arg1, &var_38_35);
        _ae_reg = $v0_5;
        
        if (arg1 != $v0_5)
        {
            int32_t _AePointPos_1 = *_AePointPos;
            int32_t $v0_6 =
                fix_point_mult2_32(_AePointPos_1, data_d04a0_1, arg1 << (_AePointPos_1 & 0x1f));
            int32_t _AePointPos_2 = *_AePointPos;
            data_d04a0_2 = fix_point_div_32(_AePointPos_2, $v0_6, $v0_5 << (_AePointPos_2 & 0x1f));
        }
        
        data_b2ef4_1(var_28_23, 0);
        data_c46b8_1 = _ae_reg;
    }
    else
    {
        int32_t $v0_2 = data_b2eec_2(data_c46b8_2, &var_38_36);
        _ae_reg = $v0_2;
        data_c46b8_3 = $v0_2;
        data_b2ef4_2(var_28_24, 0);
    }
    return 0;
}

