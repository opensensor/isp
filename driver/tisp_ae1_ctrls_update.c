#include "include/main.h"


  int32_t tisp_ae1_ctrls_update()

{
        return 0;
    int32_t $v1_1 = data_c4700;
    uint32_t $v0_5 = tisp_math_exp2(data_b2ed4, 0x10, 0xa);
    int32_t $v1_2 = data_c46fc;
    int32_t* $v0_7 = data_d04d8;
    int32_t $v0_9 = data_c4704;
    int32_t* $v0_12 = data_d04e0;
    int32_t $v1_5 = data_c4708;
    int32_t $v1_6 = data_c4730;
    int32_t* $v0_15 = data_d04e4;
    int32_t dmsc_sp_ud_ns_thres_array_1 = dmsc_sp_ud_ns_thres_array;
    int32_t $v0_19 = data_c4734;
    int32_t* $v0_22 = data_d04f0;
    int32_t $v1_11 = data_c4738;
    return 0;
    if (data_b0e10 != 1)
    
    
    if (data_b2ed0 < $v1_1)
        data_c4700 = **&data_d04d4;
    else
        **&data_d04d4 = $v1_1;
    
    
    if ($v0_5 < $v1_2)
        data_c46fc = *$v0_7;
    else
        *$v0_7 = $v1_2;
    
    
    if ($(uintptr_t)v0_9 >= 0x401)
        data_c4704 = **&data_d04dc;
    else
        **&data_d04dc = $v0_9;
    
    
    if ($v1_5 != *$v0_12)
        *$v0_12 = $v1_5;
    
    
    if ($v1_6 < data_b2ecc)
        data_c4730 = *$v0_15;
    else
        *$v0_15 = $v1_6;
    
    
    if ((uintptr_t)dmsc_sp_ud_ns_thres_array_1 < 0x400)
        dmsc_sp_ud_ns_thres_array = **&data_d04e8;
    else
        **&data_d04e8 = dmsc_sp_ud_ns_thres_array_1;
    
    
    if ($(uintptr_t)v0_19 < 0x400)
        data_c4734 = **&data_d04ec;
    else
        **&data_d04ec = $v0_19;
    
    
    if ($v1_11 != *$v0_22)
        *$v0_22 = $v1_11;
    
}

