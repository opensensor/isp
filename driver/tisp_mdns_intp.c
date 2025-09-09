#include "include/main.h"


  int32_t tisp_mdns_intp(int32_t arg1)

{
    int32_t $s2 = arg1 >> 0x10;
    int32_t $s0 = arg1 & 0xffff;
    mdns_y_sad_win_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_sad_win_opt_array);
    mdns_y_sad_ave_thres_intp = tisp_simple_intp($s2, $s0, mdns_y_sad_ave_thres_array_now);
    mdns_y_sad_ave_slope_intp = tisp_simple_intp($s2, $s0, &mdns_y_sad_ave_slope_array);
    mdns_y_sad_dtb_thres_intp = tisp_simple_intp($s2, $s0, &mdns_y_sad_dtb_thres_array);
    mdns_y_sad_ass_thres_intp = tisp_simple_intp($s2, $s0, mdns_y_sad_ass_thres_array_now);
    mdns_y_sta_blk_size_intp = tisp_simple_intp($s2, $s0, &claHistG0);
    mdns_y_sta_win_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_sta_win_opt_array);
    mdns_y_sta_ave_thres_intp = tisp_simple_intp($s2, $s0, mdns_y_sta_ave_thres_array_now);
    mdns_y_sta_dtb_thres_intp = tisp_simple_intp($s2, $s0, &mdns_y_sta_dtb_thres_array);
    mdns_y_sta_ass_thres_intp = tisp_simple_intp($s2, $s0, mdns_y_sta_ass_thres_array_now);
    mdns_y_sta_motion_thres_intp = tisp_simple_intp($s2, $s0, mdns_y_sta_motion_thres_array_now);
    mdns_y_ref_wei_mv_intp = tisp_simple_intp($s2, $s0, &mdns_y_ref_wei_mv_array);
    mdns_y_ref_wei_fake_intp = tisp_simple_intp($s2, $s0, &mdns_y_ref_wei_fake_array);
    mdns_y_ref_wei_sta_fs_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_ref_wei_sta_fs_opt_array);
    mdns_y_ref_wei_psn_fs_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_ref_wei_psn_fs_opt_array);
    mdns_y_ref_wei_f_max_intp = tisp_simple_intp($s2, $s0, &mdns_y_ref_wei_f_max_array);
    mdns_y_ref_wei_f_min_intp = tisp_simple_intp($s2, $s0, &mdns_y_ref_wei_f_min_array);
    mdns_y_ref_wei_b_max_intp = tisp_simple_intp($s2, $s0, mdns_y_ref_wei_b_max_array_now);
    mdns_y_ref_wei_b_min_intp = tisp_simple_intp($s2, $s0, mdns_y_ref_wei_b_min_array_now);
    mdns_y_ref_wei_r_max_intp = tisp_simple_intp($s2, $s0, &mdns_y_ref_wei_r_max_array);
    mdns_y_ref_wei_r_min_intp = tisp_simple_intp($s2, $s0, &mdns_y_ref_wei_r_min_array);
    mdns_y_ref_wei_increase_intp = tisp_simple_intp($s2, $s0, &mdns_y_ref_wei_increase_array);
    mdns_y_corner_length_t_intp = tisp_simple_intp($s2, $s0, &mdns_y_corner_length_t_array);
    mdns_y_corner_length_b_intp = tisp_simple_intp($s2, $s0, &mdns_y_corner_length_b_array);
    mdns_y_corner_length_l_intp = tisp_simple_intp($s2, $s0, &mdns_y_corner_length_l_array);
    mdns_y_corner_length_r_intp = tisp_simple_intp($s2, $s0, &mdns_y_corner_length_r_array);
    mdns_y_edge_win_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_win_opt_array);
    mdns_y_edge_div_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_div_opt_array);
    mdns_y_edge_type_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_type_opt_array);
    mdns_y_luma_win_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_win_opt_array);
    mdns_y_dtb_div_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_div_opt_array);
    mdns_y_dtb_squ_en_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_squ_en_array);
    mdns_y_dtb_squ_div_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_squ_div_opt_array);
    mdns_y_ass_win_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_win_opt_array);
    mdns_y_ass_div_opt_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_div_opt_array);
    mdns_y_hist_sad_en_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_sad_en_array);
    mdns_y_hist_sta_en_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_sta_en_array);
    mdns_y_hist_num_thres_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_num_thres_array);
    mdns_y_hist_cmp_thres0_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_cmp_thres0_array);
    mdns_y_hist_cmp_thres1_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_cmp_thres1_array);
    mdns_y_hist_cmp_thres2_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_cmp_thres2_array);
    mdns_y_hist_cmp_thres3_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_cmp_thres3_array);
    mdns_y_hist_thres0_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_thres0_array);
    mdns_y_hist_thres1_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_thres1_array);
    mdns_y_hist_thres2_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_thres2_array);
    mdns_y_hist_thres3_intp = tisp_simple_intp($s2, $s0, &mdns_y_hist_thres3_array);
    mdns_y_edge_thr_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_thr_adj_seg_array);
    mdns_y_luma_thr_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_thr_adj_seg_array);
    mdns_y_dtb_thr_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_thr_adj_seg_array);
    mdns_y_ass_thr_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_thr_adj_seg_array);
    mdns_y_corner_thr_adj_value_intp =
        tisp_simple_intp($s2, $s0, &mdns_y_corner_thr_adj_value_array);
    mdns_y_edge_thr_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_thr_adj_value0_array);
    mdns_y_edge_thr_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_thr_adj_value1_array);
    mdns_y_edge_thr_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_thr_adj_value2_array);
    mdns_y_edge_thr_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_thr_adj_value3_array);
    mdns_y_edge_thr_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_thr_adj_value4_array);
    mdns_y_edge_thr_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_thr_adj_value5_array);
    mdns_y_luma_thr_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_thr_adj_value0_array);
    mdns_y_luma_thr_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_thr_adj_value1_array);
    mdns_y_luma_thr_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_thr_adj_value2_array);
    mdns_y_luma_thr_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_thr_adj_value3_array);
    mdns_y_luma_thr_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_thr_adj_value4_array);
    mdns_y_luma_thr_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_thr_adj_value5_array);
    mdns_y_dtb_thr_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_thr_adj_value0_array);
    mdns_y_dtb_thr_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_thr_adj_value1_array);
    mdns_y_dtb_thr_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_thr_adj_value2_array);
    mdns_y_dtb_thr_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_thr_adj_value3_array);
    mdns_y_dtb_thr_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_thr_adj_value4_array);
    mdns_y_dtb_thr_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_thr_adj_value5_array);
    mdns_y_ass_thr_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_thr_adj_value0_array);
    mdns_y_ass_thr_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_thr_adj_value1_array);
    mdns_y_ass_thr_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_thr_adj_value2_array);
    mdns_y_ass_thr_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_thr_adj_value3_array);
    mdns_y_ass_thr_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_thr_adj_value4_array);
    mdns_y_ass_thr_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_thr_adj_value5_array);
    mdns_y_edge_wei_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_wei_adj_seg_array);
    mdns_y_luma_wei_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_wei_adj_seg_array);
    mdns_y_dtb_wei_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_wei_adj_seg_array);
    mdns_y_ass_wei_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_wei_adj_seg_array);
    mdns_y_sad_wei_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_sad_wei_adj_seg_array);
    mdns_y_corner_wei_adj_value_intp =
        tisp_simple_intp($s2, $s0, &mdns_y_corner_wei_adj_value_array);
    mdns_y_edge_wei_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_wei_adj_value0_array);
    mdns_y_edge_wei_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_wei_adj_value1_array);
    mdns_y_edge_wei_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_wei_adj_value2_array);
    mdns_y_edge_wei_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_wei_adj_value3_array);
    mdns_y_edge_wei_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_wei_adj_value4_array);
    mdns_y_edge_wei_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_y_edge_wei_adj_value5_array);
    mdns_y_luma_wei_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_wei_adj_value0_array);
    mdns_y_luma_wei_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_wei_adj_value1_array);
    mdns_y_luma_wei_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_wei_adj_value2_array);
    mdns_y_luma_wei_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_wei_adj_value3_array);
    mdns_y_luma_wei_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_wei_adj_value4_array);
    mdns_y_luma_wei_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_y_luma_wei_adj_value5_array);
    mdns_y_dtb_wei_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_wei_adj_value0_array);
    mdns_y_dtb_wei_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_wei_adj_value1_array);
    mdns_y_dtb_wei_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_wei_adj_value2_array);
    mdns_y_dtb_wei_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_wei_adj_value3_array);
    mdns_y_dtb_wei_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_wei_adj_value4_array);
    mdns_y_dtb_wei_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_y_dtb_wei_adj_value5_array);
    mdns_y_ass_wei_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_wei_adj_value0_array);
    mdns_y_ass_wei_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_wei_adj_value1_array);
    mdns_y_ass_wei_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_wei_adj_value2_array);
    mdns_y_ass_wei_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_wei_adj_value3_array);
    mdns_y_ass_wei_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_wei_adj_value4_array);
    mdns_y_ass_wei_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_y_ass_wei_adj_value5_array);
    mdns_y_sad_wei_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_y_sad_wei_adj_value0_array);
    mdns_y_sad_wei_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_y_sad_wei_adj_value1_array);
    mdns_y_sad_wei_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_y_sad_wei_adj_value2_array);
    mdns_y_sad_wei_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_y_sad_wei_adj_value3_array);
    mdns_y_sad_wei_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_y_sad_wei_adj_value4_array);
    mdns_y_sad_wei_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_y_sad_wei_adj_value5_array);
    mdns_y_pspa_cur_median_win_opt_intp =
        tisp_simple_intp($s2, $s0, mdns_y_pspa_cur_median_win_opt_array_now);
    mdns_y_pspa_cur_bi_thres_intp = tisp_simple_intp($s2, $s0, mdns_y_pspa_cur_bi_thres_array_now);
    mdns_y_pspa_cur_bi_wei_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_cur_bi_wei_seg_array);
    mdns_y_pspa_cur_bi_wei0_intp = tisp_simple_intp($s2, $s0, mdns_y_pspa_cur_bi_wei0_array_now);
    mdns_y_pspa_cur_bi_wei1_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_cur_bi_wei1_array);
    mdns_y_pspa_cur_bi_wei2_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_cur_bi_wei2_array);
    mdns_y_pspa_cur_bi_wei3_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_cur_bi_wei3_array);
    mdns_y_pspa_cur_bi_wei4_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_cur_bi_wei4_array);
    mdns_y_pspa_cur_lmt_op_en_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_cur_lmt_op_en_array);
    mdns_y_pspa_cur_lmt_wei_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_cur_lmt_wei_array);
    mdns_y_pspa_ref_median_win_opt_intp =
        tisp_simple_intp($s2, $s0, mdns_y_pspa_ref_median_win_opt_array_now);
    mdns_y_pspa_ref_bi_thres_intp = tisp_simple_intp($s2, $s0, mdns_y_pspa_ref_bi_thres_array_now);
    mdns_y_pspa_ref_bi_wei_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_ref_bi_wei_seg_array);
    mdns_y_pspa_ref_bi_wei0_intp = tisp_simple_intp($s2, $s0, mdns_y_pspa_ref_bi_wei0_array_now);
    mdns_y_pspa_ref_bi_wei1_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_ref_bi_wei1_array);
    mdns_y_pspa_ref_bi_wei2_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_ref_bi_wei2_array);
    mdns_y_pspa_ref_bi_wei3_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_ref_bi_wei3_array);
    mdns_y_pspa_ref_bi_wei4_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_ref_bi_wei4_array);
    mdns_y_pspa_ref_lmt_op_en_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_ref_lmt_op_en_array);
    mdns_y_pspa_ref_lmt_wei_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_ref_lmt_wei_array);
    mdns_y_piir_edge_thres0_intp = tisp_simple_intp($s2, $s0, &mdns_y_piir_edge_thres0_array);
    mdns_y_piir_edge_thres1_intp = tisp_simple_intp($s2, $s0, &mdns_y_piir_edge_thres1_array);
    mdns_y_piir_edge_thres2_intp = tisp_simple_intp($s2, $s0, &mdns_y_piir_edge_thres2_array);
    mdns_y_piir_edge_wei0_intp = tisp_simple_intp($s2, $s0, &mdns_y_piir_edge_wei0_array);
    mdns_y_piir_edge_wei1_intp = tisp_simple_intp($s2, $s0, &mdns_y_piir_edge_wei1_array);
    mdns_y_piir_edge_wei2_intp = tisp_simple_intp($s2, $s0, &mdns_y_piir_edge_wei2_array);
    mdns_y_piir_edge_wei3_intp = tisp_simple_intp($s2, $s0, &mdns_y_piir_edge_wei3_array);
    mdns_y_piir_cur_fs_wei_intp = tisp_simple_intp($s2, $s0, mdns_y_piir_cur_fs_wei_array_now);
    mdns_y_piir_ref_fs_wei_intp = tisp_simple_intp($s2, $s0, mdns_y_piir_ref_fs_wei_array_now);
    mdns_y_pspa_fnl_fus_thres_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_fnl_fus_thres_array);
    mdns_y_pspa_fnl_fus_swei_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_fnl_fus_swei_array);
    mdns_y_pspa_fnl_fus_dwei_intp = tisp_simple_intp($s2, $s0, &mdns_y_pspa_fnl_fus_dwei_array);
    mdns_y_fspa_cur_fus_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_cur_fus_seg_array);
    mdns_y_fspa_cur_fus_wei_0_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_cur_fus_wei_0_array);
    mdns_y_fspa_cur_fus_wei_16_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_cur_fus_wei_16_array);
    mdns_y_fspa_cur_fus_wei_32_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_cur_fus_wei_32_array);
    mdns_y_fspa_cur_fus_wei_48_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_cur_fus_wei_48_array);
    mdns_y_fspa_cur_fus_wei_64_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_cur_fus_wei_64_array);
    mdns_y_fspa_cur_fus_wei_80_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_cur_fus_wei_80_array);
    mdns_y_fspa_cur_fus_wei_96_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_cur_fus_wei_96_array);
    mdns_y_fspa_cur_fus_wei_112_intp =
        tisp_simple_intp($s2, $s0, &mdns_y_fspa_cur_fus_wei_112_array);
    mdns_y_fspa_cur_fus_wei_128_intp =
        tisp_simple_intp($s2, $s0, &mdns_y_fspa_cur_fus_wei_128_array);
    mdns_y_fspa_cur_fus_wei_144_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_cur_fus_wei_144_array_now);
    mdns_y_fspa_cur_fus_wei_160_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_cur_fus_wei_160_array_now);
    mdns_y_fspa_cur_fus_wei_176_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_cur_fus_wei_176_array_now);
    mdns_y_fspa_cur_fus_wei_192_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_cur_fus_wei_192_array_now);
    mdns_y_fspa_cur_fus_wei_208_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_cur_fus_wei_208_array_now);
    mdns_y_fspa_cur_fus_wei_224_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_cur_fus_wei_224_array_now);
    mdns_y_fspa_cur_fus_wei_240_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_cur_fus_wei_240_array_now);
    mdns_y_fspa_ref_fus_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_ref_fus_seg_array);
    mdns_y_fspa_ref_fus_wei_0_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_ref_fus_wei_0_array);
    mdns_y_fspa_ref_fus_wei_16_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_ref_fus_wei_16_array);
    mdns_y_fspa_ref_fus_wei_32_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_ref_fus_wei_32_array);
    mdns_y_fspa_ref_fus_wei_48_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_ref_fus_wei_48_array);
    mdns_y_fspa_ref_fus_wei_64_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_ref_fus_wei_64_array);
    mdns_y_fspa_ref_fus_wei_80_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_ref_fus_wei_80_array);
    mdns_y_fspa_ref_fus_wei_96_intp = tisp_simple_intp($s2, $s0, &mdns_y_fspa_ref_fus_wei_96_array);
    mdns_y_fspa_ref_fus_wei_112_intp =
        tisp_simple_intp($s2, $s0, &mdns_y_fspa_ref_fus_wei_112_array);
    mdns_y_fspa_ref_fus_wei_128_intp =
        tisp_simple_intp($s2, $s0, &mdns_y_fspa_ref_fus_wei_128_array);
    mdns_y_fspa_ref_fus_wei_144_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_ref_fus_wei_144_array_now);
    mdns_y_fspa_ref_fus_wei_160_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_ref_fus_wei_160_array_now);
    mdns_y_fspa_ref_fus_wei_176_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_ref_fus_wei_176_array_now);
    mdns_y_fspa_ref_fus_wei_192_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_ref_fus_wei_192_array_now);
    mdns_y_fspa_ref_fus_wei_208_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_ref_fus_wei_208_array_now);
    mdns_y_fspa_ref_fus_wei_224_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_ref_fus_wei_224_array_now);
    mdns_y_fspa_ref_fus_wei_240_intp =
        tisp_simple_intp($s2, $s0, mdns_y_fspa_ref_fus_wei_240_array_now);
    mdns_y_fiir_edge_thres0_intp = tisp_simple_intp($s2, $s0, &mdns_y_fiir_edge_thres0_array);
    mdns_y_fiir_edge_thres1_intp = tisp_simple_intp($s2, $s0, &mdns_y_fiir_edge_thres1_array);
    mdns_y_fiir_edge_thres2_intp = tisp_simple_intp($s2, $s0, &mdns_y_fiir_edge_thres2_array);
    mdns_y_fiir_edge_wei0_intp = tisp_simple_intp($s2, $s0, &mdns_y_fiir_edge_wei0_array);
    mdns_y_fiir_edge_wei1_intp = tisp_simple_intp($s2, $s0, &mdns_y_fiir_edge_wei1_array);
    mdns_y_fiir_edge_wei2_intp = tisp_simple_intp($s2, $s0, &mdns_y_fiir_edge_wei2_array);
    mdns_y_fiir_edge_wei3_intp = tisp_simple_intp($s2, $s0, &mdns_y_fiir_edge_wei3_array);
    mdns_y_fiir_fus_seg_intp = tisp_simple_intp($s2, $s0, &mdns_y_fiir_fus_seg_array);
    mdns_y_fiir_fus_wei0_intp = tisp_simple_intp($s2, $s0, mdns_y_fiir_fus_wei0_array_now);
    mdns_y_fiir_fus_wei1_intp = tisp_simple_intp($s2, $s0, mdns_y_fiir_fus_wei1_array_now);
    mdns_y_fiir_fus_wei2_intp = tisp_simple_intp($s2, $s0, mdns_y_fiir_fus_wei2_array_now);
    mdns_y_fiir_fus_wei3_intp = tisp_simple_intp($s2, $s0, mdns_y_fiir_fus_wei3_array_now);
    mdns_y_fiir_fus_wei4_intp = tisp_simple_intp($s2, $s0, mdns_y_fiir_fus_wei4_array_now);
    mdns_y_fiir_fus_wei5_intp = tisp_simple_intp($s2, $s0, mdns_y_fiir_fus_wei5_array_now);
    mdns_y_fiir_fus_wei6_intp = tisp_simple_intp($s2, $s0, mdns_y_fiir_fus_wei6_array_now);
    mdns_y_fiir_fus_wei7_intp = tisp_simple_intp($s2, $s0, mdns_y_fiir_fus_wei7_array_now);
    mdns_y_fiir_fus_wei8_intp = tisp_simple_intp($s2, $s0, mdns_y_fiir_fus_wei8_array_now);
    mdns_y_con_thres_intp = tisp_simple_intp($s2, $s0, &mdns_y_con_thres_array);
    mdns_y_con_stren_intp = tisp_simple_intp($s2, $s0, &mdns_y_con_stren_array);
    mdns_c_sad_win_opt_intp = tisp_simple_intp($s2, $s0, &mdns_c_sad_win_opt_array);
    mdns_c_sad_ave_thres_intp = tisp_simple_intp($s2, $s0, mdns_c_sad_ave_thres_array_now);
    mdns_c_sad_ave_slope_intp = tisp_simple_intp($s2, $s0, &mdns_c_sad_ave_slope_array);
    mdns_c_sad_dtb_thres_intp = tisp_simple_intp($s2, $s0, &mdns_c_sad_dtb_thres_array);
    mdns_c_sad_ass_thres_intp = tisp_simple_intp($s2, $s0, mdns_c_sad_ass_thres_array_now);
    mdns_c_ref_wei_mv_intp = tisp_simple_intp($s2, $s0, &mdns_c_ref_wei_mv_array);
    mdns_c_ref_wei_fake_intp = tisp_simple_intp($s2, $s0, &mdns_c_ref_wei_fake_array);
    mdns_c_ref_wei_f_max_intp = tisp_simple_intp($s2, $s0, &mdns_c_ref_wei_f_max_array);
    mdns_c_ref_wei_f_min_intp = tisp_simple_intp($s2, $s0, &mdns_c_ref_wei_f_min_array);
    mdns_c_ref_wei_b_max_intp = tisp_simple_intp($s2, $s0, mdns_c_ref_wei_b_max_array_now);
    mdns_c_ref_wei_b_min_intp = tisp_simple_intp($s2, $s0, mdns_c_ref_wei_b_min_array_now);
    mdns_c_ref_wei_r_max_intp = tisp_simple_intp($s2, $s0, &mdns_c_ref_wei_r_max_array);
    mdns_c_ref_wei_r_min_intp = tisp_simple_intp($s2, $s0, &mdns_c_ref_wei_r_min_array);
    mdns_c_ref_wei_increase_intp = tisp_simple_intp($s2, $s0, &mdns_c_ref_wei_increase_array);
    mdns_c_edge_thr_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_thr_adj_seg_array);
    mdns_c_luma_thr_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_thr_adj_seg_array);
    mdns_c_dtb_thr_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_thr_adj_seg_array);
    mdns_c_ass_thr_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_thr_adj_seg_array);
    mdns_c_corner_thr_adj_value_intp =
        tisp_simple_intp($s2, $s0, &mdns_c_corner_thr_adj_value_array);
    mdns_c_edge_thr_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_thr_adj_value0_array);
    mdns_c_edge_thr_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_thr_adj_value1_array);
    mdns_c_edge_thr_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_thr_adj_value2_array);
    mdns_c_edge_thr_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_thr_adj_value3_array);
    mdns_c_edge_thr_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_thr_adj_value4_array);
    mdns_c_edge_thr_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_thr_adj_value5_array);
    mdns_c_luma_thr_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_thr_adj_value0_array);
    mdns_c_luma_thr_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_thr_adj_value1_array);
    mdns_c_luma_thr_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_thr_adj_value2_array);
    mdns_c_luma_thr_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_thr_adj_value3_array);
    mdns_c_luma_thr_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_thr_adj_value4_array);
    mdns_c_luma_thr_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_thr_adj_value5_array);
    mdns_c_dtb_thr_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_thr_adj_value0_array);
    mdns_c_dtb_thr_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_thr_adj_value1_array);
    mdns_c_dtb_thr_adj_value2_intp = tisp_simple_intp($s2, $s0, &tmpMapB);
    mdns_c_dtb_thr_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_thr_adj_value3_array);
    mdns_c_dtb_thr_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_thr_adj_value4_array);
    mdns_c_dtb_thr_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_thr_adj_value5_array);
    mdns_c_ass_thr_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_thr_adj_value0_array);
    mdns_c_ass_thr_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_thr_adj_value1_array);
    mdns_c_ass_thr_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_thr_adj_value2_array);
    mdns_c_ass_thr_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_thr_adj_value3_array);
    mdns_c_ass_thr_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_thr_adj_value4_array);
    mdns_c_ass_thr_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_thr_adj_value5_array);
    mdns_c_edge_wei_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_wei_adj_seg_array);
    mdns_c_luma_wei_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_wei_adj_seg_array);
    mdns_c_dtb_wei_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_wei_adj_seg_array);
    mdns_c_ass_wei_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_wei_adj_seg_array);
    mdns_c_sad_wei_adj_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_sad_wei_adj_seg_array);
    mdns_c_corner_wei_adj_value_intp =
        tisp_simple_intp($s2, $s0, &mdns_c_corner_wei_adj_value_array);
    mdns_c_edge_wei_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_wei_adj_value0_array);
    mdns_c_edge_wei_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_wei_adj_value1_array);
    mdns_c_edge_wei_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_wei_adj_value2_array);
    mdns_c_edge_wei_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_wei_adj_value3_array);
    mdns_c_edge_wei_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_wei_adj_value4_array);
    mdns_c_edge_wei_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_c_edge_wei_adj_value5_array);
    mdns_c_luma_wei_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_wei_adj_value0_array);
    mdns_c_luma_wei_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_wei_adj_value1_array);
    mdns_c_luma_wei_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_wei_adj_value2_array);
    mdns_c_luma_wei_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_wei_adj_value3_array);
    mdns_c_luma_wei_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_wei_adj_value4_array);
    mdns_c_luma_wei_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_c_luma_wei_adj_value5_array);
    mdns_c_dtb_wei_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_wei_adj_value0_array);
    mdns_c_dtb_wei_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_wei_adj_value1_array);
    mdns_c_dtb_wei_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_wei_adj_value2_array);
    mdns_c_dtb_wei_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_wei_adj_value3_array);
    mdns_c_dtb_wei_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_wei_adj_value4_array);
    mdns_c_dtb_wei_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_c_dtb_wei_adj_value5_array);
    mdns_c_ass_wei_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_wei_adj_value0_array);
    mdns_c_ass_wei_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_wei_adj_value1_array);
    mdns_c_ass_wei_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_wei_adj_value2_array);
    mdns_c_ass_wei_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_wei_adj_value3_array);
    mdns_c_ass_wei_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_wei_adj_value4_array);
    mdns_c_ass_wei_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_c_ass_wei_adj_value5_array);
    mdns_c_sad_wei_adj_value0_intp = tisp_simple_intp($s2, $s0, &mdns_c_sad_wei_adj_value0_array);
    mdns_c_sad_wei_adj_value1_intp = tisp_simple_intp($s2, $s0, &mdns_c_sad_wei_adj_value1_array);
    mdns_c_sad_wei_adj_value2_intp = tisp_simple_intp($s2, $s0, &mdns_c_sad_wei_adj_value2_array);
    mdns_c_sad_wei_adj_value3_intp = tisp_simple_intp($s2, $s0, &mdns_c_sad_wei_adj_value3_array);
    mdns_c_sad_wei_adj_value4_intp = tisp_simple_intp($s2, $s0, &mdns_c_sad_wei_adj_value4_array);
    mdns_c_sad_wei_adj_value5_intp = tisp_simple_intp($s2, $s0, &mdns_c_sad_wei_adj_value5_array);
    mdns_c_median_smj_thres_intp = tisp_simple_intp($s2, $s0, &mdns_c_median_smj_thres_array);
    mdns_c_median_edg_thres_intp = tisp_simple_intp($s2, $s0, &mdns_c_median_edg_thres_array);
    mdns_c_median_cur_lmt_op_en_intp =
        tisp_simple_intp($s2, $s0, &mdns_c_median_cur_lmt_op_en_array);
    mdns_c_median_cur_lmt_wei_intp = tisp_simple_intp($s2, $s0, &mdns_c_median_cur_lmt_wei_array);
    mdns_c_median_cur_ss_wei_intp = tisp_simple_intp($s2, $s0, mdns_c_median_cur_ss_wei_array_now);
    mdns_c_median_cur_se_wei_intp = tisp_simple_intp($s2, $s0, mdns_c_median_cur_se_wei_array_now);
    mdns_c_median_cur_ms_wei_intp = tisp_simple_intp($s2, $s0, mdns_c_median_cur_ms_wei_array_now);
    mdns_c_median_cur_me_wei_intp = tisp_simple_intp($s2, $s0, mdns_c_median_cur_me_wei_array_now);
    mdns_c_median_ref_lmt_op_en_intp =
        tisp_simple_intp($s2, $s0, &mdns_c_median_ref_lmt_op_en_array);
    mdns_c_median_ref_lmt_wei_intp = tisp_simple_intp($s2, $s0, &mdns_c_median_ref_lmt_wei_array);
    mdns_c_median_ref_ss_wei_intp = tisp_simple_intp($s2, $s0, mdns_c_median_ref_ss_wei_array_now);
    mdns_c_median_ref_se_wei_intp = tisp_simple_intp($s2, $s0, mdns_c_median_ref_se_wei_array_now);
    mdns_c_median_ref_ms_wei_intp = tisp_simple_intp($s2, $s0, mdns_c_median_ref_ms_wei_array_now);
    mdns_c_median_ref_me_wei_intp = tisp_simple_intp($s2, $s0, mdns_c_median_ref_me_wei_array_now);
    mdns_c_bgm_win_opt_intp = tisp_simple_intp($s2, $s0, &mdns_c_bgm_win_opt_array);
    mdns_c_bgm_cur_src_intp = tisp_simple_intp($s2, $s0, &mdns_c_bgm_cur_src_array);
    mdns_c_bgm_ref_src_intp = tisp_simple_intp($s2, $s0, &mdns_c_bgm_ref_src_array);
    mdns_c_bgm_false_thres_intp = tisp_simple_intp($s2, $s0, &mdns_c_bgm_false_thres_array);
    mdns_c_bgm_false_step_intp = tisp_simple_intp($s2, $s0, &mdns_c_bgm_false_step_array);
    mdns_c_piir_edge_thres0_intp = tisp_simple_intp($s2, $s0, &mdns_c_piir_edge_thres0_array);
    mdns_c_piir_edge_thres1_intp = tisp_simple_intp($s2, $s0, &mdns_c_piir_edge_thres1_array);
    mdns_c_piir_edge_thres2_intp = tisp_simple_intp($s2, $s0, &mdns_c_piir_edge_thres2_array);
    mdns_c_piir_edge_wei0_intp = tisp_simple_intp($s2, $s0, &mdns_c_piir_edge_wei0_array);
    mdns_c_piir_edge_wei1_intp = tisp_simple_intp($s2, $s0, &mdns_c_piir_edge_wei1_array);
    mdns_c_piir_edge_wei2_intp = tisp_simple_intp($s2, $s0, &mdns_c_piir_edge_wei2_array);
    mdns_c_piir_edge_wei3_intp = tisp_simple_intp($s2, $s0, &mdns_c_piir_edge_wei3_array);
    mdns_c_piir_cur_fs_wei_intp = tisp_simple_intp($s2, $s0, mdns_c_piir_cur_fs_wei_array_now);
    mdns_c_piir_ref_fs_wei_intp = tisp_simple_intp($s2, $s0, mdns_c_piir_ref_fs_wei_array_now);
    mdns_c_fspa_cur_fus_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_cur_fus_seg_array);
    mdns_c_fspa_cur_fus_wei_0_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_cur_fus_wei_0_array);
    mdns_c_fspa_cur_fus_wei_16_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_cur_fus_wei_16_array);
    mdns_c_fspa_cur_fus_wei_32_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_cur_fus_wei_32_array);
    mdns_c_fspa_cur_fus_wei_48_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_cur_fus_wei_48_array);
    mdns_c_fspa_cur_fus_wei_64_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_cur_fus_wei_64_array);
    mdns_c_fspa_cur_fus_wei_80_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_cur_fus_wei_80_array);
    mdns_c_fspa_cur_fus_wei_96_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_cur_fus_wei_96_array);
    mdns_c_fspa_cur_fus_wei_112_intp =
        tisp_simple_intp($s2, $s0, &mdns_c_fspa_cur_fus_wei_112_array);
    mdns_c_fspa_cur_fus_wei_128_intp =
        tisp_simple_intp($s2, $s0, &mdns_c_fspa_cur_fus_wei_128_array);
    mdns_c_fspa_cur_fus_wei_144_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_cur_fus_wei_144_array_now);
    mdns_c_fspa_cur_fus_wei_160_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_cur_fus_wei_160_array_now);
    mdns_c_fspa_cur_fus_wei_176_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_cur_fus_wei_176_array_now);
    mdns_c_fspa_cur_fus_wei_192_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_cur_fus_wei_192_array_now);
    mdns_c_fspa_cur_fus_wei_208_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_cur_fus_wei_208_array_now);
    mdns_c_fspa_cur_fus_wei_224_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_cur_fus_wei_224_array_now);
    mdns_c_fspa_cur_fus_wei_240_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_cur_fus_wei_240_array_now);
    mdns_c_fspa_ref_fus_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_ref_fus_seg_array);
    mdns_c_fspa_ref_fus_wei_0_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_ref_fus_wei_0_array);
    mdns_c_fspa_ref_fus_wei_16_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_ref_fus_wei_16_array);
    mdns_c_fspa_ref_fus_wei_32_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_ref_fus_wei_32_array);
    mdns_c_fspa_ref_fus_wei_48_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_ref_fus_wei_48_array);
    mdns_c_fspa_ref_fus_wei_64_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_ref_fus_wei_64_array);
    mdns_c_fspa_ref_fus_wei_80_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_ref_fus_wei_80_array);
    mdns_c_fspa_ref_fus_wei_96_intp = tisp_simple_intp($s2, $s0, &mdns_c_fspa_ref_fus_wei_96_array);
    mdns_c_fspa_ref_fus_wei_112_intp =
        tisp_simple_intp($s2, $s0, &mdns_c_fspa_ref_fus_wei_112_array);
    mdns_c_fspa_ref_fus_wei_128_intp =
        tisp_simple_intp($s2, $s0, &mdns_c_fspa_ref_fus_wei_128_array);
    mdns_c_fspa_ref_fus_wei_144_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_ref_fus_wei_144_array_now);
    mdns_c_fspa_ref_fus_wei_160_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_ref_fus_wei_160_array_now);
    mdns_c_fspa_ref_fus_wei_176_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_ref_fus_wei_176_array_now);
    mdns_c_fspa_ref_fus_wei_192_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_ref_fus_wei_192_array_now);
    mdns_c_fspa_ref_fus_wei_208_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_ref_fus_wei_208_array_now);
    mdns_c_fspa_ref_fus_wei_224_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_ref_fus_wei_224_array_now);
    mdns_c_fspa_ref_fus_wei_240_intp =
        tisp_simple_intp($s2, $s0, mdns_c_fspa_ref_fus_wei_240_array_now);
    mdns_c_fiir_edge_thres0_intp = tisp_simple_intp($s2, $s0, &mdns_c_fiir_edge_thres0_array);
    mdns_c_fiir_edge_thres1_intp = tisp_simple_intp($s2, $s0, &mdns_c_fiir_edge_thres1_array);
    mdns_c_fiir_edge_thres2_intp = tisp_simple_intp($s2, $s0, &mdns_c_fiir_edge_thres2_array);
    mdns_c_fiir_edge_wei0_intp = tisp_simple_intp($s2, $s0, &mdns_c_fiir_edge_wei0_array);
    mdns_c_fiir_edge_wei1_intp = tisp_simple_intp($s2, $s0, &mdns_c_fiir_edge_wei1_array);
    mdns_c_fiir_edge_wei2_intp = tisp_simple_intp($s2, $s0, &mdns_c_fiir_edge_wei2_array);
    mdns_c_fiir_edge_wei3_intp = tisp_simple_intp($s2, $s0, &mdns_c_fiir_edge_wei3_array);
    mdns_c_fiir_fus_seg_intp = tisp_simple_intp($s2, $s0, &mdns_c_fiir_fus_seg_array);
    mdns_c_fiir_fus_wei0_intp = tisp_simple_intp($s2, $s0, mdns_c_fiir_fus_wei0_array_now);
    mdns_c_fiir_fus_wei1_intp = tisp_simple_intp($s2, $s0, mdns_c_fiir_fus_wei1_array_now);
    mdns_c_fiir_fus_wei2_intp = tisp_simple_intp($s2, $s0, mdns_c_fiir_fus_wei2_array_now);
    mdns_c_fiir_fus_wei3_intp = tisp_simple_intp($s2, $s0, mdns_c_fiir_fus_wei3_array_now);
    mdns_c_fiir_fus_wei4_intp = tisp_simple_intp($s2, $s0, mdns_c_fiir_fus_wei4_array_now);
    mdns_c_fiir_fus_wei5_intp = tisp_simple_intp($s2, $s0, mdns_c_fiir_fus_wei5_array_now);
    mdns_c_fiir_fus_wei6_intp = tisp_simple_intp($s2, $s0, mdns_c_fiir_fus_wei6_array_now);
    mdns_c_fiir_fus_wei7_intp = tisp_simple_intp($s2, $s0, mdns_c_fiir_fus_wei7_array_now);
    mdns_c_fiir_fus_wei8_intp = tisp_simple_intp($s2, $s0, mdns_c_fiir_fus_wei8_array_now);
    mdns_c_false_smj_thres_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_smj_thres_array);
    mdns_c_false_edg_thres0_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_edg_thres0_array);
    mdns_c_false_edg_thres1_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_edg_thres1_array);
    mdns_c_false_edg_thres2_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_edg_thres2_array);
    mdns_c_false_thres_s0_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_thres_s0_array);
    mdns_c_false_thres_s1_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_thres_s1_array);
    mdns_c_false_thres_s2_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_thres_s2_array);
    mdns_c_false_thres_s3_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_thres_s3_array);
    mdns_c_false_step_s0_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_step_s0_array);
    mdns_c_false_step_s1_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_step_s1_array);
    mdns_c_false_step_s2_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_step_s2_array);
    mdns_c_false_step_s3_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_step_s3_array);
    mdns_c_false_thres_m0_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_thres_m0_array);
    mdns_c_false_thres_m1_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_thres_m1_array);
    mdns_c_false_thres_m2_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_thres_m2_array);
    mdns_c_false_thres_m3_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_thres_m3_array);
    mdns_c_false_step_m0_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_step_m0_array);
    mdns_c_false_step_m1_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_step_m1_array);
    mdns_c_false_step_m2_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_step_m2_array);
    mdns_c_false_step_m3_intp = tisp_simple_intp($s2, $s0, &mdns_c_false_step_m3_array);
    mdns_c_sat_lmt_thres_intp = tisp_simple_intp($s2, $s0, &mdns_c_sat_lmt_thres_array);
    mdns_c_sat_lmt_stren_intp = tisp_simple_intp($s2, $s0, &mdns_c_sat_lmt_stren_array);
    mdns_c_sat_nml_stren_intp = tisp_simple_intp($s2, $s0, &mdns_c_sat_nml_stren_array);
    return 0;
}

