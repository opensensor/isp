#include "include/main.h"


  int32_t tisp_mdns_y_3d_param_cfg()

{
    int32_t mdns_y_sta_blk_size_intp_1 = mdns_y_sta_blk_size_intp;
    
    if (mdns_y_sta_blk_size_intp_1 & 3)
        mdns_y_sta_blk_size_intp_1 = ((mdns_y_sta_blk_size_intp_1 >> 2) + 1) << 2;
    
    if (mdns_y_sta_blk_size_intp_1 < 0x10)
        mdns_y_sta_blk_size_intp_1 = 0x10;
    
    int32_t $s5 = data_9ab08_1;
    int32_t $hi = $s5 % mdns_y_sta_blk_size_intp_1;
    int32_t $s4 = data_9ab04_1;
    int32_t $fp = mdns_y_sta_blk_size_intp_1 >> 1;
    int32_t $s2 = $s4 - $fp;
    int32_t $s3 = $s5 - $fp;
    int32_t $hi_1 = $s4 % mdns_y_sta_blk_size_intp_1;
    int32_t $t6 = 0 < $hi ? 1 : 0;
    int32_t $hi_2 = $s2 % mdns_y_sta_blk_size_intp_1;
    int32_t $t5 = 0 < $hi_1 ? 1 : 0;
    int32_t $hi_3 = $s3 % mdns_y_sta_blk_size_intp_1;
    int32_t $lo_2 = $s5 / mdns_y_sta_blk_size_intp_1;
    int32_t $a1 = 0 < $hi_3 ? 1 : 0;
    int32_t $v1_3;
    
    $v1_3 = !$hi ? $lo_2 : $lo_2 + 1;
    
    int32_t $t2 = $s2 / mdns_y_sta_blk_size_intp_1;
    
    if ($hi_2)
        $t2 += 1;
    
    int32_t $s7 = $s3 / mdns_y_sta_blk_size_intp_1;
    
    if ($hi_3)
        $s7 += 1;
    
    int32_t $t1 = $s4 / mdns_y_sta_blk_size_intp_1;
    
    if ($hi_1)
        $t1 += 1;
    
    int32_t $s6 = $s3 / mdns_y_sta_blk_size_intp_1;
    
    if ($hi_3)
        $s6 += 1;
    
    int32_t $a3_1 = $s2 / mdns_y_sta_blk_size_intp_1;
    
    if ($hi_2)
        $a3_1 += 1;
    
    int32_t $t5_1 = $t5 << 1;
    system_reg_write(0x7870, $t5_1 | $t6);
    int32_t $t4_2 = (0 < $hi_2 ? 1 : 0) << 1;
    system_reg_write(0x7888, $t4_2 | $t6);
    system_reg_write(0x78a0, $t5_1 | $a1);
    system_reg_write(0x78b8, $t4_2 | $a1);
    int32_t $s1_2 = mdns_y_sta_blk_size_intp_1 << 8 | mdns_y_sta_blk_size_intp_1;
    system_reg_write(0x7874, 
        ($s5 / mdns_y_sta_blk_size_intp_1 + $t6) << 0x18 | $s1_2
            | ($s4 / mdns_y_sta_blk_size_intp_1 + $t5) << 0x10);
    system_reg_write(0x788c, $v1_3 << 0x18 | $s1_2 | $t2 << 0x10);
    system_reg_write(0x78a4, $s7 << 0x18 | $s1_2 | $t1 << 0x10);
    int32_t $s1_4 = $fp << 0x10;
    system_reg_write(0x78bc, $s6 << 0x18 | $s1_2 | $a3_1 << 0x10);
    system_reg_write(0x7880, $s1_4);
    system_reg_write(0x7898, $fp);
    int32_t $s2_1 = $s2 << 0x10;
    system_reg_write(0x78b0, $fp | $s1_4);
    system_reg_write(0x7884, $s5 | $s2_1);
    system_reg_write(0x789c, $s4 << 0x10 | $s3);
    system_reg_write(0x78b4, $s3 | $s2_1);
    uint32_t $s1_8 =
        mdns_sta_max_num_array << 4 | mdns_y_sta_motion_thres_intp << 8 | mdns_sta_inter_en_array;
    system_reg_write(0x7878, $s1_8);
    system_reg_write(0x7890, $s1_8);
    system_reg_write(0x78a8, $s1_8);
    system_reg_write(0x78c0, $s1_8);
    uint32_t $s1_15 = mdns_y_sta_ave_thres_intp << 4 | mdns_y_sta_ass_thres_intp << 0x10
        | mdns_y_sta_win_opt_intp | mdns_y_sta_dtb_thres_intp << 0x18;
    system_reg_write(0x787c, $s1_15);
    system_reg_write(0x7894, $s1_15);
    system_reg_write(0x78ac, $s1_15);
    system_reg_write(0x78c4, $s1_15);
    system_reg_write(0x7944, 
        mdns_y_sad_ave_thres_intp << 3 | mdns_y_sad_ave_slope_intp << 0xc | mdns_y_sad_win_opt_intp
            | mdns_y_sad_ass_thres_intp << 0x10 | mdns_y_sad_dtb_thres_intp << 0x18);
    int32_t $s2_8 =
        data_cb42c_1 << 8 | data_cb430_1 << 0x10 | mdns_y_ref_wei_sta_array | data_cb434_1 << 0x18;
    system_reg_write(0x7948, $s2_8);
    system_reg_write(0x7958, $s2_8);
    system_reg_write(0x7968, $s2_8);
    system_reg_write(0x7978, $s2_8);
    int32_t $s2_15 = data_cb43c_1 << 8 | data_cb440_1 << 0x10 | data_cb438_1 | data_cb444_1 << 0x18;
    system_reg_write(0x794c, $s2_15);
    system_reg_write(0x795c, $s2_15);
    system_reg_write(0x796c, $s2_15);
    system_reg_write(0x797c, $s2_15);
    int32_t $s2_22 = data_cb44c_1 << 8 | data_cb450_1 << 0x10 | data_cb448_1 | data_cb454_1 << 0x18;
    system_reg_write(0x7950, $s2_22);
    system_reg_write(0x7960, $s2_22);
    system_reg_write(0x7970, $s2_22);
    system_reg_write(0x7980, $s2_22);
    int32_t $s1_18 = data_cb45c_1 << 8 | data_cb460_1 << 0x10 | data_cb458_1 | data_cb464_1 << 0x18;
    system_reg_write(0x7954, $s1_18);
    system_reg_write(0x7964, $s1_18);
    system_reg_write(0x7974, $s1_18);
    system_reg_write(0x7984, $s1_18);
    system_reg_write(0x7988, 
        data_cb3ec_1 << 8 | data_cb3f0_1 << 0x10 | mdns_y_ref_wei_psn_array | data_cb3f4_1 << 0x18);
    system_reg_write(0x798c, 
        data_cb3fc_1 << 8 | data_cb400_1 << 0x10 | data_cb3f8_1 | data_cb404_1 << 0x18);
    system_reg_write(0x7990, 
        data_cb40c_1 << 8 | data_cb410_1 << 0x10 | data_cb408_1 | data_cb414_1 << 0x18);
    system_reg_write(0x7994, 
        data_cb41c_1 << 8 | data_cb420_1 << 0x10 | data_cb418_1 | data_cb424_1 << 0x18);
    system_reg_write(0x7998, 
        mdns_y_ref_wei_sta_fs_opt_intp << 4 | mdns_y_ref_wei_fake_intp << 8
            | mdns_y_ref_wei_psn_fs_opt_intp | mdns_y_ref_wei_mv_intp << 0x10);
    system_reg_write(0x799c, 
        mdns_y_ref_wei_f_min_intp << 8 | mdns_y_ref_wei_b_max_intp << 0x10
            | mdns_y_ref_wei_f_max_intp | mdns_y_ref_wei_b_min_intp << 0x18);
    system_reg_write(0x79a0, 
        mdns_y_ref_wei_r_min_intp << 8 | mdns_y_ref_wei_increase_intp << 0x10
            | mdns_y_ref_wei_r_max_intp);
    uint32_t mdns_y_hist_sta_en_intp_1 = mdns_y_hist_sta_en_intp;
    system_reg_write(0x78c8, 
        mdns_y_hist_sta_en_intp_1 << 1 | mdns_y_hist_sta_en_intp_1 << 2 | mdns_y_hist_sad_en_intp
            | mdns_y_hist_sta_en_intp_1 << 3 | mdns_y_hist_sta_en_intp_1 << 4);
    system_reg_write(0x78d4, mdns_y_hist_thres1_intp << 0x10 | mdns_y_hist_thres0_intp);
    system_reg_write(0x78d8, mdns_y_hist_thres3_intp << 0x10 | mdns_y_hist_thres2_intp);
    system_reg_write(0x78cc, mdns_y_hist_cmp_thres1_intp << 0x10 | mdns_y_hist_cmp_thres0_intp);
    system_reg_write(0x78d0, mdns_y_hist_cmp_thres3_intp << 0x10 | mdns_y_hist_cmp_thres2_intp);
    system_reg_write(0x78e0, 
        mdns_y_corner_length_b_intp << 8 | mdns_y_corner_length_l_intp << 0x10
            | mdns_y_corner_length_t_intp | mdns_y_corner_length_r_intp << 0x18);
    system_reg_write(0x78e4, 
        mdns_y_corner_thr_adj_value_intp << 8 | mdns_y_corner_wei_adj_value_intp);
    system_reg_write(0x78e8, 
        mdns_y_edge_div_opt_intp << 2 | mdns_y_edge_type_opt_intp << 4 | mdns_y_edge_win_opt_intp
            | mdns_y_edge_wei_adj_seg_intp << 8 | mdns_y_edge_thr_adj_seg_intp << 0xa);
    system_reg_write(0x78ec, 
        mdns_y_edge_wei_adj_value1_intp << 8 | mdns_y_edge_wei_adj_value2_intp << 0x10
            | mdns_y_edge_wei_adj_value0_intp | mdns_y_edge_wei_adj_value3_intp << 0x18);
    uint32_t mdns_y_edge_wei_adj_value5_intp_1 = mdns_y_edge_wei_adj_value5_intp;
    system_reg_write(0x78f0, 
        mdns_y_edge_wei_adj_value5_intp_1 << 8 | mdns_y_edge_wei_adj_value5_intp_1 << 0x10
            | mdns_y_edge_wei_adj_value4_intp | mdns_y_edge_wei_adj_value5_intp_1 << 0x18);
    system_reg_write(0x78f4, 
        mdns_y_edge_thr_adj_value1_intp << 8 | mdns_y_edge_thr_adj_value2_intp << 0x10
            | mdns_y_edge_thr_adj_value0_intp | mdns_y_edge_thr_adj_value3_intp << 0x18);
    uint32_t mdns_y_edge_thr_adj_value5_intp_1 = mdns_y_edge_thr_adj_value5_intp;
    system_reg_write(0x78f8, 
        mdns_y_edge_thr_adj_value5_intp_1 << 8 | mdns_y_edge_thr_adj_value5_intp_1 << 0x10
            | mdns_y_edge_thr_adj_value4_intp | mdns_y_edge_thr_adj_value5_intp_1 << 0x18);
    system_reg_write(0x78fc, 
        mdns_y_luma_wei_adj_seg_intp << 8 | mdns_y_luma_thr_adj_seg_intp << 0xa
            | mdns_y_luma_win_opt_intp);
    system_reg_write(0x7900, 
        mdns_y_luma_wei_adj_value1_intp << 8 | mdns_y_luma_wei_adj_value2_intp << 0x10
            | mdns_y_luma_wei_adj_value0_intp | mdns_y_luma_wei_adj_value3_intp << 0x18);
    uint32_t mdns_y_luma_wei_adj_value5_intp_1 = mdns_y_luma_wei_adj_value5_intp;
    system_reg_write(0x7904, 
        mdns_y_luma_wei_adj_value5_intp_1 << 8 | mdns_y_luma_wei_adj_value5_intp_1 << 0x10
            | mdns_y_luma_wei_adj_value4_intp | mdns_y_luma_wei_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7908, 
        mdns_y_luma_thr_adj_value1_intp << 8 | mdns_y_luma_thr_adj_value2_intp << 0x10
            | mdns_y_luma_thr_adj_value0_intp | mdns_y_luma_thr_adj_value3_intp << 0x18);
    uint32_t mdns_y_luma_thr_adj_value5_intp_1 = mdns_y_luma_thr_adj_value5_intp;
    system_reg_write(0x790c, 
        mdns_y_luma_thr_adj_value5_intp_1 << 8 | mdns_y_luma_thr_adj_value5_intp_1 << 0x10
            | mdns_y_luma_thr_adj_value4_intp | mdns_y_luma_thr_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7910, 
        mdns_y_dtb_div_opt_intp << 2 | mdns_y_dtb_squ_en_intp << 4 | mdns_y_dtb_squ_div_opt_intp
            | mdns_y_dtb_wei_adj_seg_intp << 8 | mdns_y_dtb_thr_adj_seg_intp << 0xa);
    system_reg_write(0x7914, 
        mdns_y_dtb_wei_adj_value1_intp << 8 | mdns_y_dtb_wei_adj_value2_intp << 0x10
            | mdns_y_dtb_wei_adj_value0_intp | mdns_y_dtb_wei_adj_value3_intp << 0x18);
    uint32_t mdns_y_dtb_wei_adj_value5_intp_1 = mdns_y_dtb_wei_adj_value5_intp;
    system_reg_write(0x7918, 
        mdns_y_dtb_wei_adj_value5_intp_1 << 8 | mdns_y_dtb_wei_adj_value5_intp_1 << 0x10
            | mdns_y_dtb_wei_adj_value4_intp | mdns_y_dtb_wei_adj_value5_intp_1 << 0x18);
    system_reg_write(0x791c, 
        mdns_y_dtb_thr_adj_value1_intp << 8 | mdns_y_dtb_thr_adj_value2_intp << 0x10
            | mdns_y_dtb_thr_adj_value0_intp | mdns_y_dtb_thr_adj_value3_intp << 0x18);
    uint32_t mdns_y_dtb_thr_adj_value5_intp_1 = mdns_y_dtb_thr_adj_value5_intp;
    system_reg_write(0x7920, 
        mdns_y_dtb_thr_adj_value5_intp_1 << 8 | mdns_y_dtb_thr_adj_value5_intp_1 << 0x10
            | mdns_y_dtb_thr_adj_value4_intp | mdns_y_dtb_thr_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7924, 
        mdns_y_ass_win_opt_intp << 4 | mdns_y_ass_wei_adj_seg_intp << 8 | mdns_y_ass_div_opt_intp
            | mdns_y_ass_thr_adj_seg_intp << 0xa);
    system_reg_write(0x7928, 
        mdns_y_ass_wei_adj_value1_intp << 8 | mdns_y_ass_wei_adj_value2_intp << 0x10
            | mdns_y_ass_wei_adj_value0_intp | mdns_y_ass_wei_adj_value3_intp << 0x18);
    uint32_t mdns_y_ass_wei_adj_value5_intp_1 = mdns_y_ass_wei_adj_value5_intp;
    system_reg_write(0x792c, 
        mdns_y_ass_wei_adj_value5_intp_1 << 8 | mdns_y_ass_wei_adj_value5_intp_1 << 0x10
            | mdns_y_ass_wei_adj_value4_intp | mdns_y_ass_wei_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7930, 
        mdns_y_ass_thr_adj_value1_intp << 8 | mdns_y_ass_thr_adj_value2_intp << 0x10
            | mdns_y_ass_thr_adj_value0_intp | mdns_y_ass_thr_adj_value3_intp << 0x18);
    uint32_t mdns_y_ass_thr_adj_value5_intp_1 = mdns_y_ass_thr_adj_value5_intp;
    system_reg_write(0x7934, 
        mdns_y_ass_thr_adj_value5_intp_1 << 8 | mdns_y_ass_thr_adj_value5_intp_1 << 0x10
            | mdns_y_ass_thr_adj_value4_intp | mdns_y_ass_thr_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7938, mdns_y_sad_wei_adj_seg_intp);
    system_reg_write(0x793c, 
        mdns_y_sad_wei_adj_value1_intp << 8 | mdns_y_sad_wei_adj_value2_intp << 0x10
            | mdns_y_sad_wei_adj_value0_intp | mdns_y_sad_wei_adj_value3_intp << 0x18);
    uint32_t mdns_y_sad_wei_adj_value5_intp_1 = mdns_y_sad_wei_adj_value5_intp;
    system_reg_write(0x7940, 
        mdns_y_sad_wei_adj_value5_intp_1 << 8 | mdns_y_sad_wei_adj_value5_intp_1 << 0x10
            | mdns_y_sad_wei_adj_value4_intp | mdns_y_sad_wei_adj_value5_intp_1 << 0x18);
    return 0;
}

