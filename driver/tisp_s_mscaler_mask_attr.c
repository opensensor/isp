#include "include/main.h"


  int32_t tisp_s_mscaler_mask_attr(int32_t arg1)

{
    int32_t $v0 = data_ca490;
    int32_t arg_0 = arg1;
    int32_t $a1;
    int32_t arg_4 = $a1;
    int32_t $a2;
    int32_t arg_8 = $a2;
    int32_t $a3;
    int32_t arg_c = $a3;
    int32_t $a0 = data_ca490;
    
    if (!$v0)
        data_ca490 = private_kmalloc(0xac, 0xd0);
    
    
    if (!data_ca48c)
    {
        data_ca48c = private_kmalloc(0xac, 0xd0);
        $a0 = data_ca490;
    }
    
    memset($a0, 0, 0xac);
    memset(data_ca48c_1, 0, 0xac);
    memcpy(data_ca490_2, &arg_0, 0xac);
    memcpy(data_ca48c_2, &arg_0, 0xac);
    uint32_t hvflip_last_1 = hvflip_last;
    int32_t* $v0_4 = data_ca490_3;
    
    if (hvflip_last_1)
    {
        tisp_mscaler_mask_change(hvflip_last_1);
        $v0_4 = data_ca490;
    }
    
    for (int32_t i = 0; (uintptr_t)i < 0x9c; i += 1)
    {
        char var_b0[0xa0];
        var_b0[i] = *(&$v0_4[4] + i);
    }
    
    tisp_mscaler_mask_setreg(*$v0_4, $v0_4[1], $v0_4[2], $v0_4[3]);
    return 0;
}

