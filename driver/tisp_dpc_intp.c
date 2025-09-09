#include "include/main.h"


  int32_t tisp_dpc_intp(int32_t arg1)

{
    int32_t $s2 = arg1 >> 0x10;
    int32_t $s0 = arg1 & 0xffff;
    dpc_d_m1_fthres_intp = tisp_simple_intp($s2, $s0, dpc_d_m1_fthres_array_now);
    dpc_d_m1_dthres_intp = tisp_simple_intp($s2, $s0, dpc_d_m1_dthres_array_now);
    dpc_d_m2_level_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_level_array);
    dpc_d_m2_hthres_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_hthres_array);
    dpc_d_m2_lthres_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_lthres_array);
    dpc_d_m2_p0_d1_thres_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_p0_d1_thres_array);
    dpc_d_m2_p1_d1_thres_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_p1_d1_thres_array);
    dpc_d_m2_p2_d1_thres_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_p2_d1_thres_array);
    dpc_d_m2_p3_d1_thres_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_p3_d1_thres_array);
    dpc_d_m2_p0_d2_thres_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_p0_d2_thres_array);
    dpc_d_m2_p1_d2_thres_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_p1_d2_thres_array);
    dpc_d_m2_p2_d2_thres_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_p2_d2_thres_array);
    dpc_d_m2_p3_d2_thres_intp = tisp_simple_intp($s2, $s0, &dpc_d_m2_p3_d2_thres_array);
    dpc_d_m3_fthres_intp = tisp_simple_intp($s2, $s0, dpc_d_m3_fthres_array_now);
    dpc_d_m3_dthres_intp = tisp_simple_intp($s2, $s0, dpc_d_m3_dthres_array_now);
    ctr_stren_intp = tisp_simple_intp($s2, $s0, &ctr_stren_array);
    ctr_md_thres_intp = tisp_simple_intp($s2, $s0, &ctr_md_thres_array);
    ctr_el_thres_intp = tisp_simple_intp($s2, $s0, &ctr_el_thres_array);
    ctr_eh_thres_intp = tisp_simple_intp($s2, $s0, &ctr_eh_thres_array);
    return 0;
}

