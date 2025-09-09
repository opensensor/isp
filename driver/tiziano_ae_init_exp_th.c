#include "include/main.h"


  int32_t tiziano_ae_init_exp_th()

{
    int32_t $a0 = data_b2ea8;
    int32_t $v1_1 = $a0 < ae_exp_th ? 1 : 0;
    int32_t* $v0_2;
    data_d04b8 = &data_b0cfc;
    __builtin_memcpy(&data_d04bc, 
        "\x00\x0d\x0b\x00\x04\x0d\x0b\x00\x08\x0d\x0b\x00\x0c\x0d\x0b\x00\x10\x0d\x0b\x00\x14\x0d\x0b\x00", 
        0x18);
    IspAeExp = &ae_exp_th;
    
    if ($v1_1)
        ae_exp_th = $a0;
    
    
    if (tisp_math_exp2(data_b2e9c, 0x10, 0xa) >= data_b0cfc)
        $v0_2 = data_d04bc;
    else
    {
        **&data_d04b8 = tisp_math_exp2(data_b2e9c, 0x10, 0xa);
        $v0_2 = data_d04bc;
    }
    
    int32_t* $v1_2;
    
    if (tisp_math_exp2(data_b2ea0_1, 0x10, 0xa) >= *$v0_2)
        $v1_2 = data_d04c4_1;
    else
    {
        **&data_d04bc = tisp_math_exp2(data_b2ea0, 0x10, 0xa);
        $v1_2 = data_d04c4;
    }
    
    int32_t $a0_5 = data_b2ea4_1;
    
    if (*$v1_2 < $a0_5)
        *$v1_2 = $a0_5;
    
    int32_t* $v1_3 = data_d04c8_1;
    int32_t* $v1_4;
    
    if (*$(uintptr_t)v1_3 >= 0x400)
        $v1_4 = data_d04cc_1;
    else
    {
        *$v1_3 = 0x400;
        $v1_4 = data_d04cc;
    }
    
    int32_t* IspAeExp_1 = IspAeExp;
    
    if (*$(uintptr_t)v1_4 < 0x400)
    {
        *$v1_4 = 0x400;
        IspAeExp_1 = IspAeExp;
    }
    
    data_c46c8_1 = *IspAeExp_1;
    data_c46c0_2 = **&data_d04b8_1;
    data_c46c4_1 = **&data_d04bc_1;
    data_c46cc_1 = **&data_d04c0_1;
    *dmsc_sp_d_ud_ns_opt = **&data_d04c4_2;
    data_c471c_1 = **&data_d04c8_2;
    data_c4720_1 = **&data_d04cc_2;
    *((int32_t*)((char*)dmsc_sp_d_ud_ns_opt + 4)) = **&data_d04d0_1; // Fixed void pointer dereference
    data_d04d4_1 = &data_b0d18_1;
    data_d04d8_2 = &data_b0d1c_1;
    __builtin_memcpy(&data_d04dc_1, 
        "\\x20\\x0d\\x0b\\x00\\x24\\x0d\\x0b\\x00\\x28\\x0d\\x0b\\x00\\x2c\\x0d\\x0b\\x00\\x30\\x0d\\x0b\\x00\\x34\\x0d\\x0b\\x00", 
        0x18);
    
    if (data_b0e10_2 == 1)
    {
        int32_t $v1_20 = data_b2ed0;
        int32_t* $v1_21;
        
        if ($v1_20 < data_b0d18)
            data_b0d18 = $v1_20;
        
        
        if (tisp_math_exp2(data_b2ed4, 0x10, 0xa) >= data_b0d1c)
            $v1_21 = data_d04dc;
        else
        {
            **&data_d04d8 = tisp_math_exp2(data_b2ed4, 0x10, 0xa);
            $v1_21 = data_d04dc;
        }
        
        int32_t* $v1_22;
        
        if (*$(uintptr_t)v1_21 < 0x401)
            $v1_22 = data_d04e4_1;
        else
        {
            *$v1_21 = 0x400;
            $v1_22 = data_d04e4;
        }
        
        int32_t $a0_9 = data_b2ecc_1;
        
        if (*$v1_22 < $a0_9)
            *$v1_22 = $a0_9;
        
        int32_t* $v1_23 = data_d04e8_1;
        int32_t* $v1_24;
        
        if (*$(uintptr_t)v1_23 >= 0x400)
            $v1_24 = data_d04ec_1;
        else
        {
            *$v1_23 = 0x400;
            $v1_24 = data_d04ec;
        }
        
        if (*$(uintptr_t)v1_24 < 0x400)
            *$v1_24 = 0x400;
    }
    
    data_c4700_1 = **&data_d04d4_2;
    data_c46fc_1 = **&data_d04d8_3;
    data_c4704_1 = **&data_d04dc_2;
    data_c4708_1 = **&data_d04e0_1;
    data_c4730_1 = **&data_d04e4_2;
    dmsc_sp_ud_ns_thres_array = **&data_d04e8_2;
    data_c4734_1 = **&data_d04ec_2;
    int32_t result = **&data_d04f0_1;
    data_c4738_1 = result;
    return result;
}

