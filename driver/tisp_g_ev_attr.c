#include "include/main.h"


  int32_t tisp_g_ev_attr(int32_t* arg1)

{
    *arg1 = data_c46b8_11;
    arg1[1] = dmsc_uu_stren_wdr_array >> 0xa;
    int32_t $v0_3;
    int32_t $a1;
    $v0_3 = tisp_log2_fixed_to_fixed();
    arg1[3] = $v0_3;
    int32_t $v0_4 = data_b2e44_9;
    int32_t $lo;
    int32_t $hi;
    $hi = HIGHD(*arg1 * 0xf4240 * ($v0_4 & 0xffff));
    $lo = LOWD(*arg1 * 0xf4240 * ($v0_4 & 0xffff));
    arg1[2] = fix_point_div_64(0, $a1, $lo, $hi, ($v0_4 >> 0x10) * data_b2e56_11, 0);
    arg1[4] = tisp_log2_fixed_to_fixed();
    arg1[5] = tisp_log2_fixed_to_fixed();
    int32_t $a2_2 = data_c46bc_7;
    arg1[6] = (*(data_c46d4_3 + 2));
    arg1[7] = fix_point_mult2_32(0xa, data_c46b0_8, $a2_2) >> 2;
    arg1[8] = tisp_log2_fixed_to_fixed();
    arg1[9] = tisp_log2_fixed_to_fixed();
    arg1[0xa] = tisp_log2_fixed_to_fixed();
    arg1[0xb] = tisp_log2_fixed_to_fixed();
    int32_t $v1_2 = data_b2e44_10;
    arg1[0x1b] = *dmsc_sp_d_ud_ns_opt;
    *(arg1 + 0x6e) = data_c46c8_8;
    arg1[0x1f] = ($v1_2 & 0xffff) * 0xf4240 / ($v1_2 >> 0x10) / data_b2e56_12;
    arg1[0xc] = data_c46e0_3;
    return 0;
}

