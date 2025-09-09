#include "include/main.h"


  int32_t tiziano_af_init(int32_t arg1, int32_t arg2)

{
    int16_t AFParam_Tilt_1 = AFParam_Tilt;
    int16_t $v1 = data_b11f4;
    int16_t $a1_2 = data_b1354;
    char stAFParam_Zone_1 = stAFParam_Zone;
    char $a1_3 = data_b1380;
    char $v1_1 = data_b137c;
    af_first = 0;
    tiziano_af_params_refresh();
    data_d652c = arg2;
    data_d6530 = arg1;
    tiziano_af_set_hardware_param();
    system_irq_func_set(0x1f, af_interrupt_static);
    memset(&af_attr, 0, 0x58);
    data_d6c6e = data_b11f8;
    data_d6c70 = data_b11fc;
    data_d6c74 = AFParam_Tilt_1;
    data_d6c76 = $v1;
    data_d6c72 = $a1_2;
    data_d6c79 = stAFParam_Zone_1;
    data_d6c7a = data_b1384;
    data_d6c6d = 0;
    data_d6c78 = $a1_3;
    data_d6c7b = $v1_1;
    private_spin_lock_init(0);
    return 0;
}

