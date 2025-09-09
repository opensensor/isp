#include "include/main.h"


  uint8_t tisp_af_get_statistics(void* arg1, int32_t arg2, int32_t arg3)

{
    void* $t2 = arg1;
    int32_t $t3 = 0;
    int32_t $t1 = 0;
    
    while (true)
    {
        int32_t* $v0_1 = $t2;
        
        if ($t1 == arg2)
            break;
        
        int32_t $t0_1 = 0;
        
        while (true)
        {
            int32_t $v1_1 = $t3 + $t0_1;
            
            if (arg3 << 2 == $t0_1)
                break;
            
            *(&af_array_fird0 + $v1_1) = *$v0_1 & 0x3fffff;
            $t0_1 += 4;
            *(&af_array_fird1 + $v1_1) = ($v0_1[1] & 0xfff) << 0xa | *$v0_1 >> 0x16;
            int32_t $s4_3 = ($v0_1[2] & 3) << 0x14;
            int32_t $a3_10 = $v0_1[1];
            $v0_1 = &$v0_1[4];
            *(&af_array_iird0 + $v1_1) = $s4_3 | $a3_10 >> 0xc;
            *(&y_sp_fl_sl_0_array + $v1_1) = (*($v0_1 - 8) & 0xfffffc) >> 2;
            *(&tisp_BCSH_u32HLSPslope + $v1_1) = (*($v0_1 - 4) & 0x7fff) << 8 | *($v0_1 - 5);
            *(&af_array_high_luma_cnt + $v1_1) = (*($v0_1 - 4) & 0x3fff1000) >> 0xf;
        }
        
        $t1 += 1;
        $t2 += arg3 << 4;
        $t3 += 0x3c;
    }
    
    uint8_t result = (*(arg1 + 0x3f) & 0xc0) | *(arg1 + 0x2c) >> 0x1e << 4 | *(arg1 + 0xc) >> 0x1e
        | *(arg1 + 0x1c) >> 0x1e << 2;
    frame_num = result;
    return result;
}

