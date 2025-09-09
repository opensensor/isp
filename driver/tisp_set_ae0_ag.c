#include "include/main.h"


  uint32_t tisp_set_ae0_ag(void* arg1, void* arg2, int32_t* arg3, int32_t* arg4)

{
    int32_t arg_0 = $a0;
    int32_t $s5 = *arg3;
    int32_t arg_4 = $a1;
    int32_t arg_8 = $a2;
    int32_t arg_c = $a3;
    int32_t $fp = *(arg2 + 4);
    int32_t $s7 = *(arg2 + 8);
    int32_t $a0;
    int32_t $a1;
    int32_t $a2;
    int32_t $a3;
    uint32_t ag_new_1;
    int32_t dmsc_sp_d_w_stren_wdr_array_1;
    
    if (IspAeFlag == 1)
    {
    label_6191c:
        dmsc_sp_d_w_stren_wdr_array_1 = dmsc_sp_d_w_stren_wdr_array;
    label_6192c:
        
        if (dmsc_sp_d_w_stren_wdr_array_1)
        {
            $fp = data_c46b0;
            $s7 = fix_point_mult2_32($s5, data_c46b4, data_c46bc);
        }
        
        uint32_t $v0_3 = tisp_set_sensor_analog_gain();
        ag_new = $v0_3;
        data_c46b0_1 = $v0_3;
        uint32_t $v0_4 = tisp_set_sensor_digital_gain_short();
        dg_new = $v0_4;
        data_c46b4_1 = $v0_4;
        uint32_t $v0_7 = fix_point_div_32($s5, fix_point_mult2_32($s5, $fp, $s7), 
            fix_point_mult2_32($s5, ag_new, dg_new));
        dg_new = $v0_7;
        uint32_t $v1_5 = *arg4;
        uint32_t dg_new_1 = dg_new;
        
        if ($v1_5 < $v0_7)
        {
            dg_new = $v1_5;
            dg_new_1 = dg_new;
        }
        
        IspAeFlag = 0;
        data_c46bc_1 = dg_new_1;
        ag_new_1 = ag_new;
    }
    else
    {
        if (*(arg1 + 4) != $fp)
        {
            dmsc_sp_d_w_stren_wdr_array_1 = dmsc_sp_d_w_stren_wdr_array;
            goto label_6192c;
        }
        
        if (*(arg1 + 8) != $s7)
        {
            dmsc_sp_d_w_stren_wdr_array_1 = dmsc_sp_d_w_stren_wdr_array;
            goto label_6192c;
        }
        
        if (dmsc_sp_d_w_stren_wdr_array)
            goto label_6191c;
        
        ag_new_1 = ag_new;
    }
    *(((void**)((char*)arg2 + 4))) = ag_new_1; // Fixed void pointer dereference
    uint32_t dg_new_2 = dg_new;
    *(((void**)((char*)arg2 + 8))) = dg_new_2; // Fixed void pointer dereference
    return dg_new_2;
}

