#include "include/main.h"


  void* tiziano_defog_get_data(void* arg1)

{
    int32_t* $a0 = arg1 + 8;
    void* result = &defog_block_hist_info;
    
    for (int32_t i = 0; i != 0x2d0; )
    {
        int32_t $v1_1 = *$a0;
        int32_t $a3_1 = $a0[1];
        int32_t $t1_1 = $a0[2];
        int32_t $t0_1 = $a0[3];
        *(&defog_sum_block_b + i) = $v1_1 & 0xffffff;
        *(&defog_sum_block_g + i) = ($a3_1 & 0xffff) << 8 | $v1_1 >> 0x18;
        *(&defog_sum_block_r + i) = ($t0_1 >> 0x10 & 0xfff) << 0xc | ($a3_1 >> 0x10 & 0xfff);
        i += 4;
        *result = $t1_1 & 0xffff;
        *(result + 4) = $t1_1 >> 0x10;
        *(result + 8) = $t0_1 & 0xffff;
        $a0 = &$a0[4];
        result += 0xc;
    }
    
    return result;
}

