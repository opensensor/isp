#include "include/main.h"


  int32_t tisp_set_sensor_integration_time(int32_t arg1)

{
        int32_t $v0_5 = data_b2eec(arg1, &var_38);
            int32_t _AePointPos_1 = *_AePointPos;
            int32_t $v0_6 =
            int32_t _AePointPos_2 = *_AePointPos;
    void var_38;
    int16_t var_28;
    
    if (!dmsc_sp_d_w_stren_wdr_array)
    {
        _ae_reg = $v0_5;
        
        if (arg1 != $v0_5)
        {
                fix_point_mult2_32(_AePointPos_1, data_d04a0, arg1 << (_AePointPos_1 & 0x1f));
            data_d04a0 = fix_point_div_32(_AePointPos_2, $v0_6, $v0_5 << (_AePointPos_2 & 0x1f));
        }
        
        data_b2ef4_1(var_28_2, 0);
        data_c46b8_1 = _ae_reg;
    }
    else
    {
        int32_t $v0_2 = data_b2eec(data_c46b8, &var_38);
        _ae_reg = $v0_2;
        data_c46b8 = $v0_2;
        data_b2ef4(var_28, 0);
    }
    return 0;
}

