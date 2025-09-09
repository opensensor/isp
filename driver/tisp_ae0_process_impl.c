#include "include/main.h"


  int32_t tisp_ae0_process_impl()

{
    int32_t _AePointPos_1 = *_AePointPos;
    int32_t var_38 = 0x4000400;
    int32_t var_34 = 0x4000400;
    int32_t $v0 = 1 << (_AePointPos_1 & 0x1f);
    int32_t var_40 = $v0;
    int32_t var_3c = $v0;
        char var_b4[0x44];
    
    for (int32_t i = 0; (uintptr_t)i < 0x40; i += 1)
    {
        var_b4[i] = *(i + &IspAeExp);
    }
    
    for (int32_t i_1 = 0; (uintptr_t)i_1 < 0x5c; i_1 += 1)
    {
        char var_110[0x14];
        var_110[i_1] = *(i_1 + &dmsc_sp_ud_std_stren_intp);
    }
    
    char var_148_1[0x38];
    
    for (int32_t i_2 = 0; (uintptr_t)i_2 < 0x38; i_2 += 1)
        var_148_2[i_2] = *(&data_d4664_1 + i_2);
    
    Tiziano_ae0_fpga(IspAe0WmeanParam, data_d4658_1, data_d465c_1, data_d4660_1);
    
    if (ta_custom_en)
        return 0;
    
    tisp_set_sensor_integration_time(_ae_reg);
    
    for (int32_t i_3 = 0; (uintptr_t)i_3 < 0x40; i_3 += 1)
    {
        char var_fc[0x48];
        var_fc[i_3] = *(i_3 + &IspAeExp);
    }
    
    for (int32_t i_4 = 0; (uintptr_t)i_4 < 0x4c; i_4 += 1)
        var_148_3[i_4] = *(&dmsc_sp_d_v2_win5_thres_intp + i_4);
    
    tisp_set_ae0_ag(dmsc_sp_ud_std_stren_intp, dmsc_sp_d_oe_stren_intp, dmsc_sp_d_flat_thres_intp, 
        dmsc_sp_d_flat_stren_intp);
    uint32_t $v0_2 = data_b0cec_1;
    int32_t $v1 = $v0_2 << 2;
    int32_t $a1_13 = $v0_2 + 1;
    char* $a0_3 = (char*)(&ev0_cache + $v1); // Fixed void pointer assignment
    EffectFrame = $v0_2;
    EffectCount0 = $v0_2;
    int32_t $a2_3 = $a1_13;
    char* $v1_1 = (char*)(&ad0_cache + $v1); // Fixed void pointer assignment
    int32_t $a3_13 = 0;
    
    while (true)
    {
        int32_t temp1_1 = $a2_3;
        $a2_3 -= 1;
        
        if (temp1_1 <= 0)
            break;
        
        *((int32_t*)((char*)$a0_3 + 4)) = *$a0_3; // Fixed void pointer dereference
        $a0_3 -= 4;
        *((int32_t*)((char*)$v1_1 + 4)) = *$v1_1; // Fixed void pointer dereference
        $v1_1 -= 4;
        $a3_13 = 1;
    }
    
    if ($a1_13 < 0)
        $a1_13 = 0;
    
    if ($a3_13)
        EffectCount0 = $v0_2 - $a1_13;
    
    char* var_2c_1 = (char*)(&data_b0000_8); // Fixed void pointer assignment
    char* var_30_1_1 = (char*)(&data_b0000_9); // Fixed void pointer assignment
    int32_t $v0_4 =
        fix_point_mult3_32(_AePointPos_1, _ae_reg << (_AePointPos_1 & 0x1f), data_d04a0_1);
    int32_t $a2_5 = data_d04a4_3;
    *(var_30_1_2 - 0x3bc) = $v0_4;
    *(var_2c_1_1 - 0x3e4) = fix_point_mult2_32(_AePointPos_1, data_d04a0_2, $a2_5);
    uint32_t EffectFrame_1 = EffectFrame;
    EffectCount0 = EffectFrame_1;
    char* $a0_6 = (char*)(&(&ag0_cache)[EffectFrame_1]); // Fixed void pointer assignment
    char* $v1_3 = (char*)(&(&dg0_cache)[EffectFrame_1]); // Fixed void pointer assignment
    uint32_t EffectFrame_4 = EffectFrame_1;
    int32_t $t7_1 = 0;
    
    while (true)
    {
        $a0_6 -= 4;
        $v1_3 -= 4;
        
        if (EffectFrame_4 <= 0)
            break;
        
        EffectFrame_4 -= 1;
        *((int32_t*)((char*)$a0_6 + 4)) = *$a0_6; // Fixed void pointer dereference
        $t7_1 = 1;
        *((int32_t*)((char*)$v1_3 + 4)) = *$v1_3; // Fixed void pointer dereference
    }
    
    uint32_t EffectFrame_3 = EffectFrame_1;
    
    if (EffectFrame_1 >= 0)
        EffectFrame_3 = 0;
    
    if ($t7_1)
        EffectCount0 = EffectFrame_3;
    
    ag0_cache = data_d04a0_3;
    int32_t $a0_7 = data_b0e14_1;
    dg0_cache = data_d04a4_4;
    
    if ($a0_7 == 1)
    {
        int32_t $a0_9 = EffectFrame_1 + 1;
        int32_t $a1_19 = 0;
            int32_t $v1_6 = $a1_19 + (EffectFrame_1 << 2);
        data_b0e14 = 0;
        EffectCount0 = EffectFrame_1;
        
        while (true)
        {
            $a1_19 -= 4;
            
            if ($a0_9 <= 0)
                break;
            
            *((int32_t*)((char*)$v1_6 + 0xafc4c)) = *(var_30_1 - 0x3bc); // Fixed void pointer dereference
            *((int32_t*)((char*)$v1_6 + 0xafc24)) = *(var_2c_1 - 0x3e4); // Fixed void pointer dereference
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
    
    int32_t* var_30_2_1 = &dg0_cache;
    memcpy(&_ae_result, &_ae_reg, 0x18);
    JZ_Isp_Ae_Dg2reg(_AePointPos_1, &var_38_12, var_30_2_2[EffectFrame], &var_40_28);
    int32_t $v0_9 = data_b0e10_4;
    
    if (!$v0_9)
    {
        system_reg_write_ae(3, 0x1030, var_38);
        system_reg_write_ae(3, 0x1034, var_34);
    }
    else if ($v0_9 == 1)
    {
        system_reg_write_ae(3, 0x1000, var_38);
        system_reg_write_ae(3, 0x1004, var_34);
    }
    
    tisp_ae_g_scene_luma(0xb0dec);
    int32_t var_68_1_2 = 7;
    int32_t $v0_13 = *(&ev0_cache + ((EffectFrame + 1) << 2));
    int32_t var_5c_1_1 = 0;
    void var_70_11;
    tisp_event_push(&var_70_12);
    dmsc_uu_stren_wdr_array = $v0_13;
    int32_t $a0_15 = *(&ad0_cache + ((EffectFrame + 1) << 2)) << 6;
    total_gain_new = $a0_15;
    uint32_t EffectFrame_2;
    
    if ($a0_15 != total_gain_old || dmsc_sp_d_w_stren_wdr_array)
    {
        int32_t $v0_19 = tisp_log2_fixed_to_fixed();
        int32_t var_68_2 = 4;
        int32_t var_60_2 = $v0_19;
        int32_t var_5c_2 = 0;
        total_gain_old = $a0_15;
        tisp_event_push(&var_70);
        data_c46d4 = $v0_19;
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
            return 0;
        if (!dmsc_sp_d_w_stren_wdr_array)
        
        again_old = $a0_18;
    }
    
    int32_t $v0_21 = tisp_log2_fixed_to_fixed();
    int32_t var_68_3_1 = 5;
    int32_t var_60_3_1 = $v0_21;
    int32_t var_5c_3_1 = 0;
    tisp_event_push(&var_70_13);
    data_c46d8_1 = $v0_21;
    return 0;
}

