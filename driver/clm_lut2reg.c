#include "include/main.h"


  int32_t clm_lut2reg(int16_t* arg1, int32_t arg2, void* arg3, int32_t arg4)

{
    int32_t result;
        int32_t* $t6_1 = arg3 + i;
        int32_t $t5_1 = arg4 + i;
        int16_t* $t7_1 = &arg1[0x23];
        int32_t $t3_1 = arg2;
            int32_t $v1_3 = arg1[3] << 0x1b | (*arg1 & 0x1ff) | (arg1[2] & 0x1ff) << 0x12;
            uint32_t $v0_4 = arg1[1];
            int32_t $v1_8 = *($t3_1 - 1) << 0x1c | (*($t3_1 - 5) & 0x7f)
    
    for (int32_t i = 0; (uintptr_t)i != 0x690; )
    {
        
        do
        {
            arg1 = &arg1[5];
            *$t6_1 = $v1_3 | ($v0_4 & 0x1ff) << 9;
            $t6_1 = &$t6_1[2];
            $t3_1 += 5;
            *($t6_1 - 4) = (*(arg1 - 2) & 0x1ff) << 4 | (*(arg1 - 4) & 0x1e0) >> 5;
                | (*($t3_1 - 2) & 0x7f) << 0x15 | (*($t3_1 - 3) & 0x7f) << 0xe;
            $t5_1 += 8;
            *($t5_1 - 8) = $v1_8 | (*($t3_1 - 4) & 0x7f) << 7;
            result = (*($t3_1 - 1) & 0x70) >> 4;
            *($t5_1 - 4) = result;
        } while ($t7_1 != arg1);
        
        i += 0x38;
        arg2 += 0x23;
        arg1 = $t7_1;
    }
    
    return result;
}

