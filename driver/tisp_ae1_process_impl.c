#include "include/main.h"


  int32_t tisp_ae1_process_impl()

{
    int32_t _AePointPos_1 = *_AePointPos;
    int32_t var_30 = 0x4000400;
    int32_t var_2c = 0x4000400;
    int32_t $v0 = 1 << (_AePointPos_1 & 0x1f);
    int32_t var_38 = $v0;
    int32_t var_34 = $v0;
    char var_c8[0x4c];
    char var_7c[0x44];
    uint32_t $v0_1 = data_b0cec;
    int32_t $a1_12 = $v0_1 + 1;
    char* $a0_6 = (char*)(&(&ev1_cache)[$v0_1]); // Fixed void pointer assignment
    int32_t $a2_8 = $a1_12;
    char* $v1_5 = (char*)(&(&ad1_cache)[$v0_1]); // Fixed void pointer assignment
    int32_t $a3_6 = 0;
        int32_t temp1_1 = $a2_8;
    
    for (int32_t i = 0; (uintptr_t)i < 0x38; i += 1)
        var_c8[i] = *(&dmsc_aa_thres_1_intp + i);
    
    Tiziano_ae1_fpga(dmsc_nor_alias_thres_intp, dmsc_hvaa_stren_intp, dmsc_hvaa_thres_1_intp, 
        dmsc_aa_stren_intp);
    
    for (int32_t i_1 = 0; (uintptr_t)i_1 < 0x40; i_1 += 1)
        var_7c[i_1] = *(i_1 + &IspAeExp);
    
    for (int32_t i_2 = 0; (uintptr_t)i_2 < 0x4c; i_2 += 1)
        var_c8[i_2] = *(&dmsc_sp_d_v2_win5_thres_intp + i_2);
    
    tisp_ae1_expt(dmsc_sp_ud_std_stren_intp, dmsc_sp_d_oe_stren_intp, dmsc_sp_d_flat_thres_intp, 
        dmsc_sp_d_flat_stren_intp);
    tisp_set_sensor_integration_time_short(data_d04a8);
    
    for (int32_t i_3 = 0; (uintptr_t)i_3 < 0x40; i_3 += 1)
        var_7c[i_3] = *(i_3 + &IspAeExp);
    
    for (int32_t i_4 = 0; (uintptr_t)i_4 < 0x4c; i_4 += 1)
        var_c8[i_4] = *(&dmsc_sp_d_v2_win5_thres_intp + i_4);
    
    tisp_set_ae1_ag(dmsc_sp_ud_std_stren_intp, dmsc_sp_d_oe_stren_intp, dmsc_sp_d_flat_thres_intp, 
        dmsc_sp_d_flat_stren_intp);
    EffectFrame = $v0_1;
    EffectCount1 = $v0_1;
    
    while (true)
    {
        $a2_8 -= 1;
        
        if (temp1_1 <= 0)
            break;
        
        *((int32_t*)((char*)$a0_6 + 4)) = *$a0_6; // Fixed void pointer dereference
        $a0_6 -= 4;
        *((int32_t*)((char*)$v1_5 + 4)) = *$v1_5; // Fixed void pointer dereference
        $v1_5 -= 4;
        $a3_6 = 1;
    }
    
    if ($a1_12 < 0)
        $a1_12 = 0;
    
    if ($a3_6)
        EffectCount1 = $v0_1 - $a1_12;
    
    ev1_cache = fix_point_mult3_32(_AePointPos_1, data_d04a8_2 << (_AePointPos_1 & 0x1f), data_d04ac_1);
    ad1_cache = fix_point_mult2_32(_AePointPos_1, data_d04ac_2, data_d04b0_1);
    uint32_t EffectFrame_1 = EffectFrame;
    EffectCount1 = EffectFrame_1;
    char* $t1_1 = (char*)(&(&ag1_cache)[EffectFrame_1]); // Fixed void pointer assignment
    char* $a3_9 = (char*)(&(&dg1_cache)[EffectFrame_1]); // Fixed void pointer assignment
    uint32_t EffectFrame_3 = EffectFrame_1;
    int32_t $t4_1 = 0;
    
    while (true)
    {
        $t1_1 -= 4;
        $a3_9 -= 4;
        
        if (EffectFrame_3 <= 0)
            break;
        
        EffectFrame_3 -= 1;
        *((int32_t*)((char*)$t1_1 + 4)) = *$t1_1; // Fixed void pointer dereference
        *((int32_t*)((char*)$a3_9 + 4)) = *$a3_9; // Fixed void pointer dereference
        $t4_1 = 1;
    }
    
    uint32_t EffectFrame_2 = EffectFrame_1;
    
    if (EffectFrame_1 >= 0)
        EffectFrame_2 = 0;
    
    if ($t4_1)
        EffectCount1 = EffectFrame_2;
    
    ag1_cache = data_d04ac_3;
    dg1_cache = data_d04b0_2;
    void* $v1_7;
    
    if (data_b0e18_1 == 1)
    {
        int32_t $t1_2 = EffectFrame_1 + 1;
        int32_t $t2_2 = 0;
            int32_t $a3_13 = $t2_2 + (EffectFrame_1 << 2);
        data_b0e18 = 0;
        EffectCount1 = EffectFrame_1;
        
        while (true)
        {
            $t2_2 -= 4;
            
            if ($t1_2 <= 0)
                break;
            
            *((int32_t*)((char*)$a3_13 + 0xafbb4)) = ev1_cache; // Fixed void pointer dereference
            *((int32_t*)((char*)$a3_13 + 0xafb8c)) = ad1_cache; // Fixed void pointer dereference
            $t1_2 -= 1;
        }
        
        EffectCount1 = EffectFrame_1;
        int32_t $a3_15 = 0;
        int32_t $t1_3 = 0;
        
        while (EffectFrame_1 > 0)
        {
            *(&(&ag1_cache)[EffectFrame_1] + $a3_15) = ag1_cache;
            *(&(&dg1_cache)[EffectFrame_1] + $a3_15) = dg1_cache;
            EffectFrame_1 -= 1;
            $a3_15 -= 4;
            $t1_3 = 1;
        }
        
        if ($t1_3)
        {
            EffectCount1 = EffectFrame_2;
            $v1_7 = &(&dg1_cache)[EffectFrame_1];
        }
        else
            $v1_7 = &(&dg1_cache)[EffectFrame_1];
    }
    else
        $v1_7 = &(&dg1_cache)[EffectFrame_1];
    
    JZ_Isp_Ae_Dg2reg(_AePointPos_1, &var_30_9, *$v1_7, &var_38_11);
    system_reg_write_ae(3, 0x100c, var_30_10);
    system_reg_write_ae(3, 0x1010, var_2c);
    return 0;
}

