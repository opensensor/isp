#include "include/main.h"


  int32_t tiziano_ae_init(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t $a3;
    int32_t arg_c = $a3;
    memset(&tisp_ae_hist, 0, 0x42c);
    __builtin_memcpy(&data_d4fbc_3, 
        "\\x0d\\x00\\x00\\x00\\x40\\x00\\x00\\x00\\x90\\x00\\x00\\x00\\xc0\\x00\\x00\\x00\\x0f\\x00\\x00\\x00\\x0f\\x00\\x00\\x00", 
        0x18);
    memcpy(&tisp_ae_hist_last, &tisp_ae_hist, 0x42c);
    memset(&dmsc_sp_d_w_stren_wdr_array, 0, 0x98);
    memset(&ae_ctrls, 0, 0x10);
    tiziano_ae_params_refresh();
    tiziano_ae_init_exp_th();
    tiziano_ae_para_addr();
    **&data_d04c4_6 = arg3;
    tiziano_ae_set_hardware_param(0, data_d4678_2, 0);
    tiziano_ae_set_hardware_param(1, dmsc_alias_stren_intp, 0);
    uint32_t ta_custom_en_1 = ta_custom_en;
    
    if (ta_custom_en_1 == 1)
    {
        tisp_set_sensor_integration_time(_ae_result);
        tisp_set_sensor_analog_gain();
        int32_t $v1_1 = data_b0e10_7;
        
        if (!$v1_1)
        {
            int32_t $v0_2 = data_afcd4_1;
            system_reg_write_ae(3, 0x1030, $v0_2 << 0x10 | $v0_2);
            int32_t $v0_3 = data_afcd4_2;
            system_reg_write_ae(3, 0x1034, $v0_3 << 0x10 | $v0_3);
        }
        else if ($v1_1 == ta_custom_en_1)
        {
            int32_t $v0_4 = data_afcd4_3;
            system_reg_write_ae(3, 0x1000, $v0_4 << 0x10 | $v0_4);
            int32_t $v0_5 = data_afcd4_4;
            system_reg_write_ae(3, 0x1004, $v0_5 << 0x10 | $v0_5);
        }
        
        int32_t _AePointPos_1 = *_AePointPos;
        int32_t $v0_6 =
            fix_point_mult3_32(_AePointPos_1, _ae_result << (_AePointPos_1 & 0x1f), data_afcd0_1);
        int32_t $v1_2 = data_b0e10_8;
        dmsc_uu_stren_wdr_array = $v0_6;
        
        if ($v1_2 == 1)
        {
            tisp_set_sensor_integration_time_short(data_afcd8_1);
            tisp_set_sensor_analog_gain_short();
            int32_t $v0_7 = data_afce0_1;
            system_reg_write_ae(3, 0x100c, $v0_7 << 0x10 | $v0_7);
            int32_t $v0_8 = data_afce0_2;
            system_reg_write_ae(3, 0x1010, $v0_8 << 0x10 | $v0_8);
        }
    }
    
    system_irq_func_set(0x1b, ae0_interrupt_hist);
    system_irq_func_set(0x1a, ae0_interrupt_static);
    system_irq_func_set(0x1d, ae1_interrupt_hist);
    system_irq_func_set(0x1c, ae1_interrupt_static);
    uint32_t $a2_13 = data_b2e56_4;
    uint32_t $a3_1 = data_b2e54_4;
    int32_t $a1_5 = data_b2e44_3;
    data_b0b28_3 = $a1_5;
    data_b0b2c_3 = $a2_13;
    data_b0b30_3 = $a3_1;
    tiziano_deflicker_expt(_flicker_t, $a1_5, $a2_13, $a3_1, &_deflick_lut, &_nodes_num);
    tisp_event_set_cb(1, tisp_ae0_process);
    tisp_event_set_cb(6, tisp_ae1_process);
    private_spin_lock_init(0);
    private_spin_lock_init(0);
    ae_comp_default = data_b0c18_1;
    return 0;
}

