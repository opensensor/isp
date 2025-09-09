#include "include/main.h"


  int32_t tiziano_deflicker_expt(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, uint32_t* arg5, uint32_t* arg6)

{
    _flicker_t = arg1;
    data_b0b28_1 = arg2;
    data_b0b2c_1 = arg3;
    data_b0b30_1 = arg4;
    int32_t $s3_1 = arg1 << 0x11;
    int32_t $v0 = fix_point_div_32(0x10, arg2 & 0xffff0000, arg2 << 0x10);
    uint32_t $v0_2 = fix_point_div_32(0x10, $s3_1, $v0) >> 0x10;
    int32_t $s6;
    
    if ($v0_2 >= 0x79)
    {
        $v0_2 = 0x78;
        $s6 = arg3 << 0x10;
    }
    else if (!$v0_2)
    {
        $v0_2 = 1;
        $s6 = arg3 << 0x10;
    }
    else
        $s6 = arg3 << 0x10;
    
    *arg6 = $v0_2;
    uint32_t* $fp = arg5;
    int32_t $s4_1 = 1;
    uint32_t $v1_1;
    
    while (true)
    {
        $v1_1 = *arg6;
        
        if ($v1_1 < $s4_1)
            break;
        
        fix_point_div_32(0x10, $s6, $s3_1);
        int32_t $v0_4 = fix_point_mult3_32(0x10, $s4_1 << 0x10, $v0);
        $s4_1 += 1;
        *$fp = ($v0_4 + 0x8000) >> 0x10;
        $fp = &$fp[1];
    }
    
    void* $a0_1 = &arg5[$v1_1];
    uint32_t $v0_7;
    
    while (true)
    {
        $v0_7 = *arg6;
        
        if ($v1_1 >= 0x78)
            break;
        
        $v1_1 += 1;
        *$a0_1 = arg5[$v0_7 + 0x3fffffff];
        $a0_1 += 4;
    }
    
    *arg6 = $v0_7 - 1;
    data_b0e08_5 = 1;
    return &data_b0000_18;
}

