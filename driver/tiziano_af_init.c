#include "include/main.h"


  int32_t tiziano_af_init(int32_t arg1, int32_t arg2)

{
    af_first = 0;
    tiziano_af_params_refresh();
    data_d652c_2 = arg2;
    data_d6530_2 = arg1;
    tiziano_af_set_hardware_param();
    system_irq_func_set(0x1f, af_interrupt_static);
    memset(&af_attr, 0, 0x58);
    data_d6c6e_1 = data_b11f8_1;
    int16_t AFParam_Tilt_1 = AFParam_Tilt;
    int16_t $v1 = data_b11f4_1;
    data_d6c70_1 = data_b11fc_1;
    data_d6c74_1 = AFParam_Tilt_1;
    int16_t $a1_2 = data_b1354_2;
    data_d6c76_1 = $v1;
    char stAFParam_Zone_1 = stAFParam_Zone;
    data_d6c72_1 = $a1_2;
    data_d6c79_1 = stAFParam_Zone_1;
    char $a1_3 = data_b1380_2;
    char $v1_1 = data_b137c_4;
    data_d6c7a_1 = data_b1384_5;
    data_d6c6d_1 = 0;
    data_d6c78_1 = $a1_3;
    data_d6c7b_1 = $v1_1;
    private_spin_lock_init(0);
    return 0;
}

