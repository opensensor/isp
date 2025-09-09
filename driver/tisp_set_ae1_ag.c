#include "include/main.h"


  uint32_t tisp_set_ae1_ag(void* arg1, void* arg2, int32_t* arg3, int32_t* arg4)

{
    int32_t $s5 = *arg3;
    int32_t $a2;
    int32_t arg_8 = $a2;
    int32_t $a0;
    int32_t arg_0 = $a0;
    int32_t $a1;
    int32_t arg_4 = $a1;
    int32_t $a3;
    int32_t arg_c = $a3;
    int32_t dmsc_uu_thres_wdr_array_1 = *(arg2 + 0x10);
    int32_t $a2_1 = *(arg2 + 0x14);
    uint32_t ag1_new_1;
    int32_t $v0_2;
    
    if (data_b0df4_1 == 1)
    {
    label_61ac0:
        $v0_2 = data_c470c_2;
    label_61ac8:
        
        if ($v0_2)
        {
            dmsc_uu_thres_wdr_array_1 = dmsc_uu_thres_wdr_array;
            $a2_1 = fix_point_mult2_32($s5, data_c4710_1, dmsc_awb_gain);
        }
        
        uint32_t $v0_5 = tisp_set_sensor_analog_gain_short();
        ag1_new = $v0_5;
        dmsc_uu_thres_wdr_array = $v0_5;
        dg1_new = 0x400;
        data_c4710_2 = 0x400;
        uint32_t $v0_8 = fix_point_div_32($s5, 
            fix_point_mult2_32($s5, dmsc_uu_thres_wdr_array_1, $a2_1), 
            fix_point_mult2_32($s5, ag1_new, dg1_new));
        uint32_t $v1_2 = *arg4;
        dg1_new = $v0_8;
        uint32_t dg1_new_1 = dg1_new;
        
        if ($v1_2 < $v0_8)
        {
            dg1_new = $v1_2;
            dg1_new_1 = dg1_new;
        }
        
        data_b0df4_2 = 0;
        dmsc_awb_gain = dg1_new_1;
        ag1_new_1 = ag1_new;
    }
    else
    {
        if (*(arg1 + 0x10) != dmsc_uu_thres_wdr_array_1)
        {
            $v0_2 = data_c470c_3;
            goto label_61ac8;
        }
        
        $v0_2 = data_c470c_4;
        
        if (*(arg1 + 0x14) != $a2_1)
            goto label_61ac8;
        
        ag1_new_1 = ag1_new;
        
        if ($v0_2)
            goto label_61ac0;
    }
    *(arg2 + 0x10) = ag1_new_1;
    uint32_t dg1_new_2 = dg1_new;
    *(arg2 + 0x14) = dg1_new_2;
    return dg1_new_2;
}

