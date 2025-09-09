#include "include/main.h"


  int32_t (*)() tisp_ae_algo_handle(void* arg1)

{
    int32_t $v1 = data_b2ee0_2;
    int32_t $a2 = *(arg1 + 0x10);
    int32_t $v1_1 = data_b2ee4_2;
    int32_t $s3 = *(arg1 + 0x14);
    int32_t $v1_2 = data_b2ef0_3;
    int32_t $fp = *(arg1 + 0x24);
    int32_t $v1_3 = data_b2ef8_3;
    int32_t $s0 = data_b2ef4_3;
    int32_t $v1_4 = data_b2f08_2;
    int32_t $s6 = data_b2f04_2;
    int32_t $v1_5 = *(arg1 + 0x1c);
    int32_t $v1_6 = *(arg1 + 0x20);
    int32_t $v1_7 = *(arg1 + 0x2c);
    void var_14e;
    int32_t $v0_1 = data_b2eec_3($a2, &var_14e_1);
    int32_t $s7 = $v0_1 << 0xa;
    
    if ($a2 != $v0_1)
        fix_point_div_32(0xa, fix_point_mult2_32(0xa, $s3, $a2 << 0xa), $s7);
    
    void var_158_5;
    int16_t var_13e;
    $s0(var_13e_1, &var_158_6);
    int32_t (* result_1)() = tisp_log2_fixed_to_fixed;
    data_c46b8_9 = $v0_1;
    uint32_t (* var_30_1_14)(int32_t arg1, char arg2, char arg3) = tisp_math_exp2;
    int16_t var_68_19;
    uint32_t $s4_1 = tisp_math_exp2($v1(tisp_log2_fixed_to_fixed(), &var_68_20), 0x10, 0x10) >> 6;
    $s6(var_68_21, &var_68_22);
    int32_t $a2_4 = *(arg1 + 0x18);
    int32_t $a1_6 = *(arg1 + 0x14);
    data_c46b4_5 = 0x400;
    int32_t $v0_10 = fix_point_mult2_32(0xa, $v1_5, 
        fix_point_div_32(0xa, fix_point_mult2_32(0xa, $a1_6, $a2_4), 
            fix_point_mult2_32(0xa, $s4_1, 0x400)));
    void* var_48_1_8 = &data_d0000_4;
    int32_t $v0_12 = $v0_10 << 0x10 | $v0_10;
    int32_t $a1_10;
    int32_t $a2_8;
    
    if (ae_wdr_en)
    {
        system_reg_write_ae(3, 0x1000, $v0_12);
        $a1_10 = 0x1004;
        $a2_8 = $v0_12;
    }
    else
    {
        system_reg_write_ae(3, 0x1030, $v0_12);
        $a1_10 = 0x1034;
        $a2_8 = $v0_12;
    }
    
    system_reg_write_ae(3, $a1_10, $a2_8);
    int32_t (* var_2c_1_7)(int32_t arg1, int32_t arg2, int32_t arg3) = fix_point_mult3_32;
    data_c46bc_6 = $v0_10;
    uint32_t $v0_16 = fix_point_mult2_32(0xa, fix_point_mult3_32(0xa, $s7, $s4_1), $v0_10);
    uint32_t $v0_17 = var_2c_1_8(0xa, $s4_1, 0x400);
    
    if (*(var_48_1_9 + 0x4770) == 1)
    {
        int32_t $v0_20 = $v1_2($v1_6, &var_14e_2);
        
        if ($v1_6 != $v0_20)
            fix_point_div_32(0xa, fix_point_mult2_32(0xa, $fp, $v1_6 << 0xa), $v0_20 << 0xa);
        
        int16_t var_13c;
        $v1_3(var_13c_1, &var_158_7);
        data_c46f8_9 = $v0_20;
        uint32_t $fp_1 = var_30_1_15($v1_1(result_1(), &var_68_23), 0x10, 0x10) >> 6;
        int16_t var_5a;
        $v1_4(var_5a_1, &var_68_24);
        data_c4710_5 = 0x400;
        int32_t $v0_36 = fix_point_mult2_32(0xa, $v1_7, 
            fix_point_div_32(0xa, fix_point_mult2_32(0xa, *(arg1 + 0x24), *(arg1 + 0x28)), 
                fix_point_mult2_32(0xa, $fp_1, 0x400)));
        int32_t $s5_2 = $v0_36 << 0x10 | $v0_36;
        system_reg_write_ae(3, 0x100c, $s5_2);
        system_reg_write_ae(3, 0x1010, $s5_2);
        dmsc_awb_gain = $v0_36;
        var_2c_1_9(0xa, $fp_1, 0x400);
    }
    
    if ($v0_16 != ta_custom_ev)
    {
        int32_t var_90_1_4 = 7;
        int32_t var_84_1_5 = 0;
        void var_98_132;
        tisp_event_push(&var_98_133);
        ta_custom_ev = $v0_16;
        dmsc_uu_stren_wdr_array = $v0_16;
    }
    
    if ($v0_17 != ta_custom_tgain)
    {
        int32_t $v0_44 = result_1();
        int32_t var_c0_1_17 = 4;
        int32_t var_b8_1_10 = $v0_44;
        int32_t var_b4_1_1 = 0;
        void var_c8_17;
        tisp_event_push(&var_c8_18);
        data_c46d4_2 = $v0_44;
        ta_custom_tgain = $v0_17;
    }
    
    int32_t (* result)() = result_1;
    
    if ($s4_1 != ta_custom_again)
    {
        int32_t $v0_46 = result();
        int32_t var_f0_1_1 = 5;
        int32_t var_e8_1_1 = $v0_46;
        int32_t var_e4_1_1 = 0;
        void var_f8_25;
        result = tisp_event_push(&var_f8_26);
        data_c46b0_6 = $s4_1;
        data_c46d8_2 = $v0_46;
        ta_custom_again = $s4_1;
    }
    
    return result;
}

