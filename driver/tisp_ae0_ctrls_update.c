#include "include/main.h"


  int32_t tisp_ae0_ctrls_update()

{
    int32_t $a0 = data_c46c8_2;
    
    if (data_b2ea8_2 < $a0)
        data_c46c8_3 = **&IspAeExp;
    else
        **&IspAeExp = $a0;
    
    uint32_t $v0_5 = tisp_math_exp2(data_b2e9c_2, 0x10, 0xa);
    int32_t $a0_2 = data_c46c0_4;
    
    if ($v0_5 < $a0_2)
        data_c46c0_5 = **&data_d04b8_2;
    else
        **&data_d04b8_3 = $a0_2;
    
    uint32_t $v0_10 = tisp_math_exp2(data_b2ea0_2, 0x10, 0xa);
    int32_t $v1 = data_c46c4_2;
    int32_t* $v0_12 = data_d04bc_1;
    
    if ($v0_10 < $v1)
        data_c46c4_3 = *$v0_12;
    else
        *$v0_12 = $v1;
    
    int32_t* $v0_14 = data_d04c0_1;
    int32_t $v1_1 = data_c46cc_1;
    
    if ($v1_1 != *$v0_14)
        *$v0_14 = $v1_1;
    
    int32_t dmsc_sp_d_ud_ns_opt_1 = *dmsc_sp_d_ud_ns_opt;
    int32_t* $v0_17 = data_d04c4_2;
    
    if (dmsc_sp_d_ud_ns_opt_1 < data_b2ea4_2)
        *dmsc_sp_d_ud_ns_opt = *$v0_17;
    else
        *$v0_17 = dmsc_sp_d_ud_ns_opt_1;
    
    int32_t $v0_19 = data_c471c_3;
    
    if ($v0_19 < 0x400)
        data_c471c_4 = **&data_d04c8_2;
    else
        **&data_d04c8_3 = $v0_19;
    
    int32_t $v0_22 = data_c4720_2;
    
    if ($v0_22 < 0x400)
        data_c4720_3 = **&data_d04cc_1;
    else
        **&data_d04cc_2 = $v0_22;
    
    int32_t* $v0_25 = data_d04d0_1;
    int32_t $v1_6 = *(dmsc_sp_d_ud_ns_opt + 4);
    
    if ($v1_6 != *$v0_25)
        *$v0_25 = $v1_6;
    
    return 0;
}

