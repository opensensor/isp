#include "include/main.h"


  uint32_t vic_mdma_enable(void* arg1, int32_t arg2, int32_t arg3, uint32_t arg4, int32_t arg5, char arg6)

{
    int32_t $a1 = *(arg1 + 0xdc);
    uint32_t $t1 = arg6;
    int32_t $v1_1 = *(arg1 + 0xe0);
    int32_t $v1_2 = $a1 * $v1_1;
    int32_t $a1_1 = $v1_2 << 1;
    int32_t $t0_7 = $v1_2 + arg5;
        int32_t $v0_5 = $v1_2 + $t0_7;
        int32_t $v0_6 = $v1_2 + $v0_5;
    
    if ($t1 != 7)
        $a1 <<= 1;
    
    vic_mdma_ch0_set_buff_index = 4;
    vic_mdma_ch1_set_buff_index = 4;
    vic_mdma_ch0_sub_get_num = arg4;
    
    if (arg3)
        vic_mdma_ch1_sub_get_num = arg4;
    
    *(*(arg1 + 0xb8) + 0x308) = 1;
    *(*(arg1 + 0xb8) + 0x304) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0);
    *(*(arg1 + 0xb8) + 0x310) = $a1;
    *(*(arg1 + 0xb8) + 0x314) = $a1;
    *(*(arg1 + 0xb8) + 0x318) = arg5;
    int32_t $v1_6;
    
    if (!arg3)
    {
        *(*(arg1 + 0xb8) + 0x31c) = $t0_7;
        *(*(arg1 + 0xb8) + 0x320) = $v0_5;
        *(*(arg1 + 0xb8) + 0x324) = $v0_6;
        $v1_6 = $v1_2 + $v0_6;
    }
    else
    {
        int32_t $v1_3 = arg5 + $a1_1;
        int32_t $v1_4 = $a1_1 + $v1_3;
        int32_t $v1_5 = $a1_1 + $v1_4;
        *(*(arg1 + 0xb8) + 0x31c) = $v1_3;
        *(*(arg1 + 0xb8) + 0x320) = $v1_4;
        *(*(arg1 + 0xb8) + 0x324) = $v1_5;
        $v1_6 = $a1_1 + $v1_5;
    }
    
    int32_t $t2_3 = $t0_7 + $a1_1;
    *(*(arg1 + 0xb8) + 0x328) = $v1_6;
    int32_t $a2_2 = $t2_3 + $a1_1;
    *(*(arg1 + 0xb8) + 0x340) = $t0_7;
    int32_t $v1_7 = $a2_2 + $a1_1;
    *(*(arg1 + 0xb8) + 0x344) = $t2_3;
    *(*(arg1 + 0xb8) + 0x348) = $a2_2;
    *(*(arg1 + 0xb8) + 0x34c) = $v1_7;
    int32_t $v0_12 = $v1_7 + $a1_1;
    *(*(arg1 + 0xb8) + 0x350) = $v0_12;
    int32_t $v0_13;
    
    if ($t1 != 7)
        $v0_13 = arg4 < 8 ? 1 : 0;
    else
    {
        *(*(arg1 + 0xb8) + 0x32c) = $t0_7;
        *(*(arg1 + 0xb8) + 0x330) = $t2_3;
        *(*(arg1 + 0xb8) + 0x334) = $a2_2;
        *(*(arg1 + 0xb8) + 0x338) = $v1_7;
        *(*(arg1 + 0xb8) + 0x33c) = $v0_12;
        $v0_13 = arg4 < 8 ? 1 : 0;
    }
    
    uint32_t result;
    int32_t $a3;
    
    if ($v0_13)
    {
        result = arg4 << 0x10;
        $a3 = result | 0x80000020 | $t1;
    }
    else
    {
        result = 0x80080020;
        $a3 = $t1 | 0x80080020;
    }
    
    *(*(arg1 + 0xb8) + 0x300) = $a3;
    return result;
}

