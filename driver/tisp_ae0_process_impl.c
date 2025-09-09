#include "include/main.h"


  int32_t tisp_ae0_process_impl()

{
    int32_t _AePointPos_1 = *_AePointPos;
    int32_t var_38_44 = 0x4000400;
    int32_t var_34_26 = 0x4000400;
    int32_t $v0 = 1 << (_AePointPos_1 & 0x1f);
    int32_t var_40_47 = $v0;
    int32_t var_3c_15 = $v0;
    
    for (int32_t i = 0; i < 0x40; i += 1)
    {
        char var_b4_18[0x44];
        var_b4_19[i] = *(i + &IspAeExp);
    }
    
    for (int32_t i_1 = 0; i_1 < 0x5c; i_1 += 1)
    {
        char var_110_3[0x14];
        var_110_4[i_1] = *(i_1 + &dmsc_sp_ud_std_stren_intp);
    }
    
    char var_148_5[0x38];
    
    for (int32_t i_2 = 0; i_2 < 0x38; i_2 += 1)
        var_148_6[i_2] = *(&data_d4664_2 + i_2);
    
    Tiziano_ae0_fpga(IspAe0WmeanParam, data_d4658_2, data_d465c_2, data_d4660_2);
    
    if (ta_custom_en)
        return 0;
    
    tisp_set_sensor_integration_time(_ae_reg);
    
    for (int32_t i_3 = 0; i_3 < 0x40; i_3 += 1)
    {
        char var_fc_3[0x48];
        var_fc_4[i_3] = *(i_3 + &IspAeExp);
    }
    
    for (int32_t i_4 = 0; i_4 < 0x4c; i_4 += 1)
        var_148_7[i_4] = *(&dmsc_sp_d_v2_win5_thres_intp + i_4);
    
    tisp_set_ae0_ag(dmsc_sp_ud_std_stren_intp, dmsc_sp_d_oe_stren_intp, dmsc_sp_d_flat_thres_intp, 
        dmsc_sp_d_flat_stren_intp);
    uint32_t $v0_2 = data_b0cec_3;
    int32_t $v1 = $v0_2 << 2;
    int32_t $a1_13 = $v0_2 + 1;
    void* $a0_3 = &ev0_cache + $v1;
    EffectFrame = $v0_2;
    EffectCount0 = $v0_2;
    int32_t $a2_3 = $a1_13;
    void* $v1_1 = &ad0_cache + $v1;
    int32_t $a3_13 = 0;
    
    while (true)
    {
        int32_t temp1_1 = $a2_3;
        $a2_3 -= 1;
        
        if (temp1_1 <= 0)
            break;
        
        *($a0_3 + 4) = *$a0_3;
        $a0_3 -= 4;
        *($v1_1 + 4) = *$v1_1;
        $v1_1 -= 4;
        $a3_13 = 1;
    }
    
    if ($a1_13 < 0)
        $a1_13 = 0;
    
    if ($a3_13)
        EffectCount0 = $v0_2 - $a1_13;
    
    void* var_2c_1_4 = &data_b0000_20;
    void* var_30_1_11 = &data_b0000_21;
    int32_t $v0_4 =
        fix_point_mult3_32(_AePointPos_1, _ae_reg << (_AePointPos_1 & 0x1f), data_d04a0_3);
    int32_t $a2_5 = data_d04a4_3;
    *(var_30_1_12 - 0x3bc) = $v0_4;
    *(var_2c_1_5 - 0x3e4) = fix_point_mult2_32(_AePointPos_1, data_d04a0_4, $a2_5);
    uint32_t EffectFrame_1 = EffectFrame;
    EffectCount0 = EffectFrame_1;
    void* $a0_6 = &(&ag0_cache)[EffectFrame_1];
    void* $v1_3 = &(&dg0_cache)[EffectFrame_1];
    uint32_t EffectFrame_4 = EffectFrame_1;
    int32_t $t7_1 = 0;
    
    while (true)
    {
        $a0_6 -= 4;
        $v1_3 -= 4;
        
        if (EffectFrame_4 <= 0)
            break;
        
        EffectFrame_4 -= 1;
        *($a0_6 + 4) = *$a0_6;
        $t7_1 = 1;
        *($v1_3 + 4) = *$v1_3;
    }
    
    uint32_t EffectFrame_3 = EffectFrame_1;
    
    if (EffectFrame_1 >= 0)
        EffectFrame_3 = 0;
    
    if ($t7_1)
        EffectCount0 = EffectFrame_3;
    
    ag0_cache = data_d04a0_5;
    int32_t $a0_7 = data_b0e14_3;
    dg0_cache = data_d04a4_4;
    
    if ($a0_7 == 1)
    {
        data_b0e14_4 = 0;
        EffectCount0 = EffectFrame_1;
        int32_t $a0_9 = EffectFrame_1 + 1;
        int32_t $a1_19 = 0;
        
        while (true)
        {
            $a1_19 -= 4;
            
            if ($a0_9 <= 0)
                break;
            
            int32_t $v1_6 = $a1_19 + (EffectFrame_1 << 2);
            *($v1_6 + 0xafc4c) = *(var_30_1_13 - 0x3bc);
            *($v1_6 + 0xafc24) = *(var_2c_1_6 - 0x3e4);
            $a0_9 -= 1;
        }
        
        EffectCount0 = EffectFrame_1;
        int32_t $v1_8 = 0;
        int32_t $a0_10 = 0;
        
        while (EffectFrame_1 > 0)
        {
            *(&(&ag0_cache)[EffectFrame_1] + $v1_8) = ag0_cache;
            *(&(&dg0_cache)[EffectFrame_1] + $v1_8) = dg0_cache;
            EffectFrame_1 -= 1;
            $v1_8 -= 4;
            $a0_10 = 1;
        }
        
        if ($a0_10)
            EffectCount0 = EffectFrame_3;
    }
    
    int32_t* var_30_2_3 = &dg0_cache;
    memcpy(&_ae_result, &_ae_reg, 0x18);
    JZ_Isp_Ae_Dg2reg(_AePointPos_1, &var_38_45, var_30_2_4[EffectFrame], &var_40_48);
    int32_t $v0_9 = data_b0e10_9;
    
    if (!$v0_9)
    {
        system_reg_write_ae(3, 0x1030, var_38_46);
        system_reg_write_ae(3, 0x1034, var_34_27);
    }
    else if ($v0_9 == 1)
    {
        system_reg_write_ae(3, 0x1000, var_38_47);
        system_reg_write_ae(3, 0x1004, var_34_28);
    }
    
    tisp_ae_g_scene_luma(0xb0dec);
    int32_t var_68_1_4 = 7;
    int32_t $v0_13 = *(&ev0_cache + ((EffectFrame + 1) << 2));
    int32_t var_5c_1_3 = 0;
    void var_70_19;
    tisp_event_push(&var_70_20);
    dmsc_uu_stren_wdr_array = $v0_13;
    int32_t $a0_15 = *(&ad0_cache + ((EffectFrame + 1) << 2)) << 6;
    total_gain_new = $a0_15;
    uint32_t EffectFrame_2;
    
    if ($a0_15 != total_gain_old || dmsc_sp_d_w_stren_wdr_array)
    {
        total_gain_old = $a0_15;
        int32_t $v0_19 = tisp_log2_fixed_to_fixed();
        int32_t var_68_2_1 = 4;
        int32_t var_60_2_1 = $v0_19;
        int32_t var_5c_2_1 = 0;
        tisp_event_push(&var_70_21);
        data_c46d4_1 = $v0_19;
        EffectFrame_2 = EffectFrame;
    }
    else
        EffectFrame_2 = EffectFrame;
    
    int32_t $a0_18 = (&ag0_cache)[EffectFrame_2] << 6;
    again_new = $a0_18;
    
    if ($a0_18 != again_old)
        again_old = $a0_18;
    else
    {
        if (!dmsc_sp_d_w_stren_wdr_array)
            return 0;
        
        again_old = $a0_18;
    }
    
    int32_t $v0_21 = tisp_log2_fixed_to_fixed();
    int32_t var_68_3_1 = 5;
    int32_t var_60_3_1 = $v0_21;
    int32_t var_5c_3_1 = 0;
    tisp_event_push(&var_70_22);
    data_c46d8_1 = $v0_21;
    return 0;
}

