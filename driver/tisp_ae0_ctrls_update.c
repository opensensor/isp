#include "include/main.h"


  int32_t tisp_ae0_ctrls_update()

{
    int32_t $a0 = data_c46c8;
    uint32_t $v0_5 = tisp_math_exp2(data_b2e9c, 0x10, 0xa);
    int32_t $a0_2 = data_c46c0;
    uint32_t $v0_10 = tisp_math_exp2(data_b2ea0, 0x10, 0xa);
    int32_t $v1 = data_c46c4;
    int32_t* $v0_12 = data_d04bc;
    int32_t* $v0_14 = data_d04c0;
    int32_t $v1_1 = data_c46cc;
    int32_t dmsc_sp_d_ud_ns_opt_1 = *dmsc_sp_d_ud_ns_opt;
    int32_t* $v0_17 = data_d04c4;
    int32_t $v0_19 = data_c471c;
    int32_t $v0_22 = data_c4720;
    int32_t* $v0_25 = data_d04d0;
    int32_t $v1_6 = *(dmsc_sp_d_ud_ns_opt + 4);
    return 0;
    
    if (data_b2ea8 < $a0)
        data_c46c8 = **&IspAeExp;
    else
        **&IspAeExp = $a0;
    
    
    if ($v0_5 < $a0_2)
        data_c46c0 = **&data_d04b8;
    else
        **&data_d04b8 = $a0_2;
    
    
    if ($v0_10 < $v1)
        data_c46c4 = *$v0_12;
    else
        *$v0_12 = $v1;
    
    
    if ($v1_1 != *$v0_14)
        *$v0_14 = $v1_1;
    
    
    if (dmsc_sp_d_ud_ns_opt_1 < data_b2ea4)
        *dmsc_sp_d_ud_ns_opt = *$v0_17;
    else
        *$v0_17 = dmsc_sp_d_ud_ns_opt_1;
    
    
    if ($(uintptr_t)v0_19 < 0x400)
        data_c471c = **&data_d04c8;
    else
        **&data_d04c8 = $v0_19;
    
    
    if ($(uintptr_t)v0_22 < 0x400)
        data_c4720 = **&data_d04cc;
    else
        **&data_d04cc = $v0_22;
    
    
    if ($v1_6 != *$v0_25)
        *$v0_25 = $v1_6;
    
}

