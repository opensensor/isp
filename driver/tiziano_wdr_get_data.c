#include "include/main.h"


  int32_t tiziano_wdr_get_data(int32_t* arg1)

{
    int32_t* $a1 = arg1;
    
    for (int32_t i = 0; i != 0x400; )
    {
        int32_t $t0_1 = *$a1;
        int32_t $a3_1 = $a1[1];
        int32_t $v1_2 = i >> 2 << 2;
        int32_t $s0_1 = $a1[2];
        int32_t $a2_1 = $a1[3];
        *($v1_2 + &wdr_hist_R0) = $t0_1 & 0x1fffff;
        *($v1_2 + &wdr_hist_G0) = ($a3_1 & 0x3ff) << 0xb | $t0_1 >> 0x15;
        *($v1_2 + &wdr_hist_B0) = $a3_1 >> 0xa & 0x1fffff;
        (&mdns_y_ass_wei_adj_value1_intp)[i >> 2] = $s0_1 & 0x1fffff;
        i += 4;
        (&mdns_c_false_edg_thres1_intp)[i >> 2] = ($a2_1 & 0x3ff) << 0xb | $s0_1 >> 0x15;
        *($v1_2 + &wdr_hist_B1) = $a2_1 >> 0xa & 0x1fffff;
        $a1 = &$a1[4];
    }
    
    int32_t $a2_4 = arg1[0x400];
    int32_t $v1_4 = arg1[0x401];
    wdr_hist_y0_num = $a2_4 & 0x1fffff;
    data_d8f00_1 = ($v1_4 & 0x3ff) << 0xb | $a2_4 >> 0x15;
    int32_t $a2_7 = arg1[0x402];
    data_d8f04_1 = $v1_4 >> 0xa & 0x1fffff;
    int32_t $v1_7 = arg1[0x403];
    wdr_hist_y1_num = $a2_7 & 0x1fffff;
    int32_t $v1_9 = arg1[0x405];
    data_d8ef8_1 = $v1_7 >> 0xa & 0x1fffff;
    data_d8ef4_1 = ($v1_7 & 0x3ff) << 0xb | $a2_7 >> 0x15;
    data_d8ee4_1 = $v1_9 & 1;
    int32_t $v1_11 = arg1[0x407];
    int32_t $a0 = arg1[0x406];
    wdr_point_y_sum = arg1[0x404];
    data_d8ee8_1 = $a0;
    data_d8eec_1 = $v1_11 & 1;
    return &wdr_point_y_sum;
}

