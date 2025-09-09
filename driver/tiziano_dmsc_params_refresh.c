#include "include/main.h"


  int32_t tiziano_dmsc_params_refresh()

{
    uint32_t $a0 = data_9a44c;
    return 0;
    memcpy(&dmsc_uu_np_array, 0x9dc64, 0x40);
    memcpy(&dmsc_r_deir_array, 0x9dca4, 0x20);
    memcpy(&dmsc_g_deir_array, 0x9dcc4, 0x20);
    memcpy(&dmsc_b_deir_array, 0x9dce4, 0x20);
    memcpy(&dmsc_sp_d_sigma_3_np_array, 0x9dd04, 0x40);
    memcpy(&dmsc_sp_d_w_wei_np_array, U"          ", 0x58);
    memcpy(&dmsc_sp_d_b_wei_np_array, U"          ", 0x58);
    memcpy(&dmsc_sp_ud_w_wei_np_array, U"    ", 0x58);
    memcpy(&dmsc_sp_ud_b_wei_np_array, U"    ", 0x58);
    memcpy(&dmsc_out_opt, 0x9dea4, 4);
    memcpy(&dmsc_hv_thres_1_array, U"ddddddddddd", 0x24);
    memcpy(&dmsc_hv_stren_array, &data_9dea8[9], 0x24);
    memcpy(&dmsc_aa_thres_1_array, U"ddddddddddd", 0x24);
    memcpy(&dmsc_aa_stren_array, &data_9def0[9], 0x24);
    memcpy(&dmsc_hvaa_thres_1_array, 0x9df38, 0x24);
    memcpy(&dmsc_hvaa_stren_array, &data_9df5c, 0x24);
    memcpy(&dmsc_dir_par_array, 0x9df80, 0x24);
    memcpy(&dmsc_uu_thres_array, 0x9dfa4, 0x24);
    memcpy(&dmsc_uu_stren_array, 0x9dfc8, 0x24);
    memcpy(&dmsc_uu_par_array, &data_9dfec, 0x10);
    memcpy(&dmsc_alias_stren_array, 0x9dffc, 0x24);
    memcpy(&dmsc_alias_thres_1_array, 0x9e020, 0x24);
    memcpy(&dmsc_alias_thres_2_array, 0x9e044, 0x24);
    memcpy(&dmsc_alias_dir_thres_array, U"ddddddddd8\n", 0x24);
    memcpy(&dmsc_alias_par_array, &data_9e068[9], 0x10);
    memcpy(&dmsc_nor_alias_thres_array, 0x9e09c, 0x24);
    memcpy(&dmsc_nor_par_array, 0x9e0c0, 0x10);
    memcpy(&dmsc_sp_d_w_stren_array, U"ZZZF2-(((ZZZF2-(((", 0x24);
    memcpy(&dmsc_sp_d_b_stren_array, &data_9e0d0[9], 0x24);
    memcpy(&dmsc_sp_d_brig_thres_array, 0x9e118, 0x24);
    memcpy(&dmsc_sp_d_dark_thres_array, 0x9e13c, 0x24);
    memcpy(&dmsc_sp_d_v2_win5_thres_array, 0x9e160, 0x24);
    memcpy(&dmsc_sp_d_flat_stren_array, 0x9e184, 0x24);
    memcpy(&dmsc_sp_d_flat_thres_array, 0x9e1a8, 0x24);
    memcpy(&dmsc_sp_d_oe_stren_array, U">>>>>>>>>", 0x24);
    memcpy(&dmsc_sp_d_par_array, &data_9e1f0, 0x2c);
    memcpy(&dmsc_sp_ud_w_stren_array, U"PPP<(((", 0x24);
    memcpy(&dmsc_sp_ud_b_stren_array, U"PPP<(((", 0x24);
    memcpy(&dmsc_sp_ud_brig_thres_array, 0x9e264, 0x24);
    memcpy(&dmsc_sp_ud_dark_thres_array, 0x9e288, 0x24);
    memcpy(&dmsc_sp_ud_std_stren_array, 0x9e2ac, 0x24);
    memcpy(&dmsc_sp_ud_std_thres_array, 0x9e2d0, 0x24);
    memcpy(&dmsc_sp_ud_flat_thres_array, 0x9e2f4, 0x24);
    memcpy(&dmsc_sp_ud_flat_stren_array, U"\n\n\n\n\n\n\n\n\n$$$$$$$$$", 0x24);
    memcpy(&dmsc_sp_ud_oe_stren_array, &data_9e318[9], 0x24);
    memcpy(&dmsc_sp_ud_par_array, &data_9e360, 0x34);
    memcpy(&dmsc_sp_ud_v1_v2_par_array, 0x9e394, 0x28);
    memcpy(&dmsc_sp_alias_thres_array, 0x9e3bc, 0x24);
    memcpy(&dmsc_sp_alias_par_array, 0x9e3e0, 8);
    memcpy(&dmsc_rgb_dir_thres_array, U"ddddddddd", 0x24);
    memcpy(&dmsc_rgb_alias_stren_array, 0x9e40c, 0x24);
    memcpy(&dmsc_rgb_alias_par_array, 0x9e430, 8);
    memcpy(&dmsc_fc_alias_stren_array, 0x9e438, 0x24);
    memcpy(&dmsc_fc_t1_thres_array, U"222222222", 0x24);
    memcpy(&dmsc_fc_t1_stren_array, &data_9e45c[9], 0x24);
    memcpy(&dmsc_fc_t2_stren_array, 0x9e4a4, 0x24);
    memcpy(&dmsc_fc_t3_stren_array, 0x9e4c8, 0x24);
    memcpy(&dmsc_fc_lum_stren_array, 0x9e4ec, 0x24);
    memcpy(&dmsc_fc_lum_thres_array, 0x9e510, 0x24);
    memcpy(&dmsc_fc_par_array, 0x9e534, 0x28);
    memcpy(&dmsc_deir_oe_en, 0x9e55c, 8);
    memcpy(&dmsc_deir_rgb_ir_oe_slope, 0x9e564, 0x14);
    memcpy(&dmsc_deir_fusion_thres_array, 0x9e578, 0x24);
    memcpy(&dmsc_deir_fusion_stren_array, 0x9e59c, 0x24);
    memcpy(&dmsc_sp_d_ns_thres_array, &data_9e5c0, 0x24);
    memcpy(&dmsc_sp_ud_ns_thres_array, 0x9e5e4, 0x24);
    memcpy(&dmsc_sp_d_ud_ns_opt, 0x9e608, 8);
    memcpy(&dmsc_uu_thres_wdr_array, 0x9e610, 0x24);
    memcpy(&dmsc_uu_stren_wdr_array, U"dZPFF<", 0x24);
    memcpy(&dmsc_sp_d_w_stren_wdr_array, U"ZZZZPPFFFddddPPFF2PPPPPP", 0x24);
    memcpy(&dmsc_sp_d_b_stren_wdr_array, &data_9e658[9], 0x24);
    memcpy(&dmsc_sp_ud_w_stren_wdr_array, &data_9e658[0x12], 0x24);
    memcpy(&dmsc_sp_ud_b_stren_wdr_array, 0x9e6c4, 0x24);
    memcpy(&dmsc_awb_gain, 0x9e6e8, 0xc);
    
    if ($(uintptr_t)a0 != 0x80)
        tisp_dmsc_sharpness_set($a0);
    
}

