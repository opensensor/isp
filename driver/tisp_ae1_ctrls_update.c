#include "include/main.h"


  int32_t tisp_ae1_ctrls_update()

{
    if (data_b0e10_3 != 1)
        return 0;
    
    int32_t $v1_1 = data_c4700_2;
    
    if (data_b2ed0_2 < $v1_1)
        data_c4700_3 = **&data_d04d4_2;
    else
        **&data_d04d4_3 = $v1_1;
    
    uint32_t $v0_5 = tisp_math_exp2(data_b2ed4_2, 0x10, 0xa);
    int32_t $v1_2 = data_c46fc_2;
    int32_t* $v0_7 = data_d04d8_2;
    
    if ($v0_5 < $v1_2)
        data_c46fc_3 = *$v0_7;
    else
        *$v0_7 = $v1_2;
    
    int32_t $v0_9 = data_c4704_1;
    
    if ($v0_9 >= 0x401)
        data_c4704_2 = **&data_d04dc_1;
    else
        **&data_d04dc_2 = $v0_9;
    
    int32_t* $v0_12 = data_d04e0_1;
    int32_t $v1_5 = data_c4708_1;
    
    if ($v1_5 != *$v0_12)
        *$v0_12 = $v1_5;
    
    int32_t $v1_6 = data_c4730_3;
    int32_t* $v0_15 = data_d04e4_1;
    
    if ($v1_6 < data_b2ecc_2)
        data_c4730_4 = *$v0_15;
    else
        *$v0_15 = $v1_6;
    
    int32_t dmsc_sp_ud_ns_thres_array_1 = dmsc_sp_ud_ns_thres_array;
    
    if (dmsc_sp_ud_ns_thres_array_1 < 0x400)
        dmsc_sp_ud_ns_thres_array = **&data_d04e8_1;
    else
        **&data_d04e8_2 = dmsc_sp_ud_ns_thres_array_1;
    
    int32_t $v0_19 = data_c4734_1;
    
    if ($v0_19 < 0x400)
        data_c4734_2 = **&data_d04ec_1;
    else
        **&data_d04ec_2 = $v0_19;
    
    int32_t* $v0_22 = data_d04f0_1;
    int32_t $v1_11 = data_c4738_1;
    
    if ($v1_11 != *$v0_22)
        *$v0_22 = $v1_11;
    
    return 0;
}

