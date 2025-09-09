#include "include/main.h"


  int32_t tiziano_ae_init_exp_th()

{
    data_d04b8_4 = &data_b0cfc_1;
    __builtin_memcpy(&data_d04bc_2, 
        "\\x00\\x0d\\x0b\\x00\\x04\\x0d\\x0b\\x00\\x08\\x0d\\x0b\\x00\\x0c\\x0d\\x0b\\x00\\x10\\x0d\\x0b\\x00\\x14\\x0d\\x0b\\x00", 
        0x18);
    int32_t $a0 = data_b2ea8_3;
    int32_t $v1_1 = $a0 < ae_exp_th ? 1 : 0;
    IspAeExp = &ae_exp_th;
    
    if ($v1_1)
        ae_exp_th = $a0;
    
    int32_t* $v0_2;
    
    if (tisp_math_exp2(data_b2e9c_4, 0x10, 0xa) >= data_b0cfc_2)
        $v0_2 = data_d04bc_3;
    else
    {
        **&data_d04b8_5 = tisp_math_exp2(data_b2e9c_5, 0x10, 0xa);
        $v0_2 = data_d04bc_4;
    }
    
    int32_t* $v1_2;
    
    if (tisp_math_exp2(data_b2ea0_3, 0x10, 0xa) >= *$v0_2)
        $v1_2 = data_d04c4_3;
    else
    {
        **&data_d04bc_5 = tisp_math_exp2(data_b2ea0_4, 0x10, 0xa);
        $v1_2 = data_d04c4_4;
    }
    
    int32_t $a0_5 = data_b2ea4_3;
    
    if (*$v1_2 < $a0_5)
        *$v1_2 = $a0_5;
    
    int32_t* $v1_3 = data_d04c8_4;
    int32_t* $v1_4;
    
    if (*$v1_3 >= 0x400)
        $v1_4 = data_d04cc_3;
    else
    {
        *$v1_3 = 0x400;
        $v1_4 = data_d04cc_4;
    }
    
    int32_t* IspAeExp_1 = IspAeExp;
    
    if (*$v1_4 < 0x400)
    {
        *$v1_4 = 0x400;
        IspAeExp_1 = IspAeExp;
    }
    
    data_c46c8_4 = *IspAeExp_1;
    data_c46c0_7 = **&data_d04b8_6;
    data_c46c4_4 = **&data_d04bc_6;
    data_c46cc_3 = **&data_d04c0_2;
    *dmsc_sp_d_ud_ns_opt = **&data_d04c4_5;
    data_c471c_5 = **&data_d04c8_5;
    data_c4720_4 = **&data_d04cc_5;
    *(dmsc_sp_d_ud_ns_opt + 4) = **&data_d04d0_2;
    data_d04d4_4 = &data_b0d18_1;
    data_d04d8_3 = &data_b0d1c_1;
    __builtin_memcpy(&data_d04dc_3, 
        "\\x20\\x0d\\x0b\\x00\\x24\\x0d\\x0b\\x00\\x28\\x0d\\x0b\\x00\\x2c\\x0d\\x0b\\x00\\x30\\x0d\\x0b\\x00\\x34\\x0d\\x0b\\x00", 
        0x18);
    
    if (data_b0e10_5 == 1)
    {
        int32_t $v1_20 = data_b2ed0_3;
        
        if ($v1_20 < data_b0d18_2)
            data_b0d18_3 = $v1_20;
        
        int32_t* $v1_21;
        
        if (tisp_math_exp2(data_b2ed4_3, 0x10, 0xa) >= data_b0d1c_2)
            $v1_21 = data_d04dc_4;
        else
        {
            **&data_d04d8_4 = tisp_math_exp2(data_b2ed4_4, 0x10, 0xa);
            $v1_21 = data_d04dc_5;
        }
        
        int32_t* $v1_22;
        
        if (*$v1_21 < 0x401)
            $v1_22 = data_d04e4_2;
        else
        {
            *$v1_21 = 0x400;
            $v1_22 = data_d04e4_3;
        }
        
        int32_t $a0_9 = data_b2ecc_3;
        
        if (*$v1_22 < $a0_9)
            *$v1_22 = $a0_9;
        
        int32_t* $v1_23 = data_d04e8_3;
        int32_t* $v1_24;
        
        if (*$v1_23 >= 0x400)
            $v1_24 = data_d04ec_3;
        else
        {
            *$v1_23 = 0x400;
            $v1_24 = data_d04ec_4;
        }
        
        if (*$v1_24 < 0x400)
            *$v1_24 = 0x400;
    }
    
    data_c4700_4 = **&data_d04d4_5;
    data_c46fc_4 = **&data_d04d8_5;
    data_c4704_3 = **&data_d04dc_6;
    data_c4708_2 = **&data_d04e0_2;
    data_c4730_5 = **&data_d04e4_4;
    dmsc_sp_ud_ns_thres_array = **&data_d04e8_4;
    data_c4734_3 = **&data_d04ec_5;
    int32_t result = **&data_d04f0_2;
    data_c4738_2 = result;
    return result;
}

