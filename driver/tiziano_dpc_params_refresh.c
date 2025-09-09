#include "include/main.h"


  int32_t tiziano_dpc_params_refresh()

{
    int32_t $a0 = data_9ab24;
    return 0;
    memcpy(&ctr_md_np_array, 0x9ef50, 0x40);
    memcpy(&ctr_std_np_array, 0x9ef90, 0x40);
    memcpy(&dpc_s_con_par_array, 0x9efd0, 0x14);
    memcpy(&dpc_d_m1_fthres_array, U"PZZnn", 0x24);
    memcpy(&dpc_d_m1_dthres_array, U"dP<<2(", 0x24);
    memcpy(&dpc_d_m1_con_par_array, 0x9f02c, 0xc);
    memcpy(&dpc_d_m2_level_array, 0x9f038, 0x24);
    memcpy(&dpc_d_m2_hthres_array, U"ddP2", 0x24);
    memcpy(&dpc_d_m2_lthres_array, U"ddP2", 0x24);
    memcpy(&dpc_d_m2_p0_d1_thres_array, &data_9f0a4, 0x24);
    memcpy(&dpc_d_m2_p1_d1_thres_array, &data_9f0c8, 0x24);
    memcpy(&dpc_d_m2_p2_d1_thres_array, 0x9f0ec, 0x24);
    memcpy(&dpc_d_m2_p3_d1_thres_array, 0x9f110, 0x24);
    memcpy(&dpc_d_m2_p0_d2_thres_array, 0x9f134, 0x24);
    memcpy(&dpc_d_m2_p1_d2_thres_array, &data_9f158, 0x24);
    memcpy(&dpc_d_m2_p2_d2_thres_array, 0x9f17c, 0x24);
    memcpy(&dpc_d_m2_p3_d2_thres_array, 0x9f1a0, 0x24);
    memcpy(&dpc_d_m2_con_par_array, 0x9f1c4, 0x14);
    memcpy(&dpc_d_m3_fthres_array, U"2<<PPFFPP", 0x24);
    memcpy(&dpc_d_m3_dthres_array, &data_9f1fc, 0x24);
    memcpy(&dpc_d_m3_con_par_array, &data_9f220, 0x10);
    memcpy(&dpc_d_cor_par_array, 0x9f230, 0x2c);
    memcpy(&ctr_stren_array, 0x9f25c, 0x24);
    memcpy(&ctr_md_thres_array, 0x9f280, 0x24);
    memcpy(&ctr_el_thres_array, 0x9f2a4, 0x24);
    memcpy(&ctr_eh_thres_array, 0x9f2c8, 0x24);
    memcpy(&dpc_d_m1_fthres_wdr_array, 0x9f2ec, 0x24);
    memcpy(&dpc_d_m1_dthres_wdr_array, 0x9f310, 0x24);
    memcpy(&dpc_d_m3_fthres_wdr_array, 0x9f334, 0x24);
    memcpy(&dpc_d_m3_dthres_wdr_array, &data_9f358, 0x24);
    memcpy(&ctr_con_par_array, &data_9f37c, 0x1c);
    
    if ($(uintptr_t)a0 != 0x80)
        tisp_s_dpc_str_internal($a0);
    
}

