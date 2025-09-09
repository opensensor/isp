#include "include/main.h"


  int32_t tisp_mdns_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 - 0x180 >= 0x1d7)
    {
        int32_t var_18_1_15 = arg1;
        isp_printf(2, &$LC0, "tisp_mdns_param_array_get");
        return 0xffffffff;
    }
    
    uint32_t* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x180:
        {
            $a1_1 = &mdns_y_filter_en_array;
            $s1_1 = 4;
            break;
        }
        case 0x181:
        {
            $a1_1 = &mdns_y_sf_cur_en_array;
            $s1_1 = 4;
            break;
        }
        case 0x182:
        {
            $a1_1 = &mdns_y_sf_ref_en_array;
            $s1_1 = 4;
            break;
        }
        case 0x183:
        {
            $a1_1 = &mdns_y_debug_array;
            $s1_1 = 4;
            break;
        }
        case 0x184:
        {
            $a1_1 = &mdns_uv_filter_en_array;
            $s1_1 = 4;
            break;
        }
        case 0x185:
        {
            $a1_1 = &mdns_uv_sf_cur_en_array;
            $s1_1 = 4;
            break;
        }
        case 0x186:
        {
            $a1_1 = &mdns_uv_sf_ref_en_array;
            $s1_1 = 4;
            break;
        }
        case 0x187:
        {
            $a1_1 = &mdns_uv_debug_array;
            $s1_1 = 4;
            break;
        }
        case 0x188:
        {
            $a1_1 = &mdns_ass_enable_array;
            $s1_1 = 4;
            break;
        }
        case 0x189:
        {
            $a1_1 = &mdns_sta_inter_en_array;
            $s1_1 = 4;
            break;
        }
        case 0x18a:
        {
            $a1_1 = &mdns_sta_group_num_array;
            $s1_1 = 4;
            break;
        }
        case 0x18b:
        {
            $a1_1 = &mdns_sta_max_num_array;
            $s1_1 = 4;
            break;
        }
        case 0x18c:
        {
            $a1_1 = &mdns_bgm_enable_array;
            $s1_1 = 4;
            break;
        }
        case 0x18d:
        {
            $a1_1 = &mdns_bgm_inter_en_array;
            $s1_1 = 4;
            break;
        }
        case 0x18e:
        {
            $a1_1 = &mdns_psn_enable_array;
            $s1_1 = 4;
            break;
        }
        case 0x18f:
        {
            $a1_1 = &mdns_psn_max_num_array;
            $s1_1 = 4;
            break;
        }
        case 0x190:
        {
            $a1_1 = &mdns_ref_wei_byps_array;
            $s1_1 = 4;
            break;
        }
        case 0x191:
        {
            $a1_1 = &mdns_y_sad_win_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x192:
        {
            $a1_1 = &mdns_y_sad_ave_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x193:
        {
            $a1_1 = &mdns_y_sad_ave_slope_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x194:
        {
            $a1_1 = &mdns_y_sad_dtb_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x195:
        {
            $a1_1 = &mdns_y_sad_ass_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x196:
        {
            $a1_1 = &claHistG0;
            $s1_1 = 0x24;
            break;
        }
        case 0x197:
        {
            $a1_1 = &mdns_y_sta_win_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x198:
        {
            $a1_1 = &mdns_y_sta_ave_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x199:
        {
            $a1_1 = &mdns_y_sta_dtb_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x19a:
        {
            $a1_1 = &mdns_y_sta_ass_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x19b:
        {
            $a1_1 = &mdns_y_sta_motion_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x19c:
        {
            $a1_1 = &mdns_y_ref_wei_sta_array;
            $s1_1 = 0x40;
            break;
        }
        case 0x19d:
        {
            $a1_1 = &mdns_y_ref_wei_psn_array;
            $s1_1 = 0x40;
            break;
        }
        case 0x19e:
        {
            $a1_1 = &mdns_y_ref_wei_mv_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x19f:
        {
            $a1_1 = &mdns_y_ref_wei_fake_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1a0:
        {
            $a1_1 = &mdns_y_ref_wei_sta_fs_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1a1:
        {
            $a1_1 = &mdns_y_ref_wei_psn_fs_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1a2:
        {
            $a1_1 = &mdns_y_ref_wei_f_max_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1a3:
        {
            $a1_1 = &mdns_y_ref_wei_f_min_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1a4:
        {
            $a1_1 = &mdns_y_ref_wei_b_max_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1a5:
        {
            $a1_1 = &mdns_y_ref_wei_b_min_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1a6:
        {
            $a1_1 = &mdns_y_ref_wei_r_max_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1a7:
        {
            $a1_1 = &mdns_y_ref_wei_r_min_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1a8:
        {
            $a1_1 = &mdns_y_ref_wei_increase_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1a9:
        {
            $a1_1 = &mdns_y_corner_length_t_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1aa:
        {
            $a1_1 = &mdns_y_corner_length_b_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ab:
        {
            $a1_1 = &mdns_y_corner_length_l_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ac:
        {
            $a1_1 = &mdns_y_corner_length_r_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ad:
        {
            $a1_1 = &mdns_y_edge_win_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ae:
        {
            $a1_1 = &mdns_y_edge_div_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1af:
        {
            $a1_1 = &mdns_y_edge_type_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1b0:
        {
            $a1_1 = &mdns_y_luma_win_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1b1:
        {
            $a1_1 = &mdns_y_dtb_div_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1b2:
        {
            $a1_1 = &mdns_y_dtb_squ_en_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1b3:
        {
            $a1_1 = &mdns_y_dtb_squ_div_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1b4:
        {
            $a1_1 = &mdns_y_ass_win_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1b5:
        {
            $a1_1 = &mdns_y_ass_div_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1b6:
        {
            $a1_1 = &mdns_y_hist_sad_en_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1b7:
        {
            $a1_1 = &mdns_y_hist_sta_en_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1b8:
        {
            $a1_1 = &mdns_y_hist_num_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1b9:
        {
            $a1_1 = &mdns_y_hist_cmp_thres0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ba:
        {
            $a1_1 = &mdns_y_hist_cmp_thres1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1bb:
        {
            $a1_1 = &mdns_y_hist_cmp_thres2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1bc:
        {
            $a1_1 = &mdns_y_hist_cmp_thres3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1bd:
        {
            $a1_1 = &mdns_y_hist_thres0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1be:
        {
            $a1_1 = &mdns_y_hist_thres1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1bf:
        {
            $a1_1 = &mdns_y_hist_thres2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1c0:
        {
            $a1_1 = &mdns_y_hist_thres3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1c1:
        {
            $a1_1 = &mdns_y_edge_thr_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1c2:
        {
            $a1_1 = &mdns_y_luma_thr_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1c3:
        {
            $a1_1 = &mdns_y_dtb_thr_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1c4:
        {
            $a1_1 = &mdns_y_ass_thr_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1c5:
        {
            $a1_1 = &mdns_y_corner_thr_adj_value_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1c6:
        {
            $a1_1 = &mdns_y_edge_thr_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1c7:
        {
            $a1_1 = &mdns_y_edge_thr_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1c8:
        {
            $a1_1 = &mdns_y_edge_thr_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1c9:
        {
            $a1_1 = &mdns_y_edge_thr_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ca:
        {
            $a1_1 = &mdns_y_edge_thr_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1cb:
        {
            $a1_1 = &mdns_y_edge_thr_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1cc:
        {
            $a1_1 = &mdns_y_luma_thr_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1cd:
        {
            $a1_1 = &mdns_y_luma_thr_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ce:
        {
            $a1_1 = &mdns_y_luma_thr_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1cf:
        {
            $a1_1 = &mdns_y_luma_thr_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1d0:
        {
            $a1_1 = &mdns_y_luma_thr_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1d1:
        {
            $a1_1 = &mdns_y_luma_thr_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1d2:
        {
            $a1_1 = &mdns_y_dtb_thr_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1d3:
        {
            $a1_1 = &mdns_y_dtb_thr_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1d4:
        {
            $a1_1 = &mdns_y_dtb_thr_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1d5:
        {
            $a1_1 = &mdns_y_dtb_thr_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1d6:
        {
            $a1_1 = &mdns_y_dtb_thr_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1d7:
        {
            $a1_1 = &mdns_y_dtb_thr_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1d8:
        {
            $a1_1 = &mdns_y_ass_thr_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1d9:
        {
            $a1_1 = &mdns_y_ass_thr_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1da:
        {
            $a1_1 = &mdns_y_ass_thr_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1db:
        {
            $a1_1 = &mdns_y_ass_thr_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1dc:
        {
            $a1_1 = &mdns_y_ass_thr_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1dd:
        {
            $a1_1 = &mdns_y_ass_thr_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1de:
        {
            $a1_1 = &mdns_y_edge_wei_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1df:
        {
            $a1_1 = &mdns_y_luma_wei_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1e0:
        {
            $a1_1 = &mdns_y_dtb_wei_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1e1:
        {
            $a1_1 = &mdns_y_ass_wei_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1e2:
        {
            $a1_1 = &mdns_y_sad_wei_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1e3:
        {
            $a1_1 = &mdns_y_corner_wei_adj_value_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1e4:
        {
            $a1_1 = &mdns_y_edge_wei_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1e5:
        {
            $a1_1 = &mdns_y_edge_wei_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1e6:
        {
            $a1_1 = &mdns_y_edge_wei_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1e7:
        {
            $a1_1 = &mdns_y_edge_wei_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1e8:
        {
            $a1_1 = &mdns_y_edge_wei_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1e9:
        {
            $a1_1 = &mdns_y_edge_wei_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ea:
        {
            $a1_1 = &mdns_y_luma_wei_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1eb:
        {
            $a1_1 = &mdns_y_luma_wei_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ec:
        {
            $a1_1 = &mdns_y_luma_wei_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ed:
        {
            $a1_1 = &mdns_y_luma_wei_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ee:
        {
            $a1_1 = &mdns_y_luma_wei_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ef:
        {
            $a1_1 = &mdns_y_luma_wei_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1f0:
        {
            $a1_1 = &mdns_y_dtb_wei_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1f1:
        {
            $a1_1 = &mdns_y_dtb_wei_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1f2:
        {
            $a1_1 = &mdns_y_dtb_wei_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1f3:
        {
            $a1_1 = &mdns_y_dtb_wei_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1f4:
        {
            $a1_1 = &mdns_y_dtb_wei_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1f5:
        {
            $a1_1 = &mdns_y_dtb_wei_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1f6:
        {
            $a1_1 = &mdns_y_ass_wei_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1f7:
        {
            $a1_1 = &mdns_y_ass_wei_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1f8:
        {
            $a1_1 = &mdns_y_ass_wei_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1f9:
        {
            $a1_1 = &mdns_y_ass_wei_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1fa:
        {
            $a1_1 = &mdns_y_ass_wei_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1fb:
        {
            $a1_1 = &mdns_y_ass_wei_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1fc:
        {
            $a1_1 = &mdns_y_sad_wei_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1fd:
        {
            $a1_1 = &mdns_y_sad_wei_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1fe:
        {
            $a1_1 = &mdns_y_sad_wei_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x1ff:
        {
            $a1_1 = &mdns_y_sad_wei_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x200:
        {
            $a1_1 = &mdns_y_sad_wei_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x201:
        {
            $a1_1 = &mdns_y_sad_wei_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x202:
        {
            $a1_1 = &mdns_y_sad_ave_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x203:
        {
            $a1_1 = &mdns_y_sad_ass_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x204:
        {
            $a1_1 = &mdns_y_sta_ave_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x205:
        {
            $a1_1 = &mdns_y_sta_ass_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x206:
        {
            $a1_1 = &mdns_y_sta_motion_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x207:
        {
            $a1_1 = &mdns_y_ref_wei_b_max_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x208:
        {
            $a1_1 = &mdns_y_ref_wei_b_min_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x209:
        {
            $a1_1 = &mdns_y_pspa_cur_median_win_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x20a:
        {
            $a1_1 = &mdns_y_pspa_cur_bi_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x20b:
        {
            $a1_1 = &mdns_y_pspa_cur_bi_wei_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x20c:
        {
            $a1_1 = &mdns_y_pspa_cur_bi_wei0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x20d:
        {
            $a1_1 = &mdns_y_pspa_cur_bi_wei1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x20e:
        {
            $a1_1 = &mdns_y_pspa_cur_bi_wei2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x20f:
        {
            $a1_1 = &mdns_y_pspa_cur_bi_wei3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x210:
        {
            $a1_1 = &mdns_y_pspa_cur_bi_wei4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x211:
        {
            $a1_1 = &mdns_y_pspa_cur_lmt_op_en_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x212:
        {
            $a1_1 = &mdns_y_pspa_cur_lmt_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x213:
        {
            $a1_1 = &mdns_y_pspa_ref_median_win_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x214:
        {
            $a1_1 = &mdns_y_pspa_ref_bi_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x215:
        {
            $a1_1 = &mdns_y_pspa_ref_bi_wei_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x216:
        {
            $a1_1 = &mdns_y_pspa_ref_bi_wei0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x217:
        {
            $a1_1 = &mdns_y_pspa_ref_bi_wei1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x218:
        {
            $a1_1 = &mdns_y_pspa_ref_bi_wei2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x219:
        {
            $a1_1 = &mdns_y_pspa_ref_bi_wei3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x21a:
        {
            $a1_1 = &mdns_y_pspa_ref_bi_wei4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x21b:
        {
            $a1_1 = &mdns_y_pspa_ref_lmt_op_en_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x21c:
        {
            $a1_1 = &mdns_y_pspa_ref_lmt_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x21d:
        {
            $a1_1 = &mdns_y_piir_edge_thres0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x21e:
        {
            $a1_1 = &mdns_y_piir_edge_thres1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x21f:
        {
            $a1_1 = &mdns_y_piir_edge_thres2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x220:
        {
            $a1_1 = &mdns_y_piir_edge_wei0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x221:
        {
            $a1_1 = &mdns_y_piir_edge_wei1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x222:
        {
            $a1_1 = &mdns_y_piir_edge_wei2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x223:
        {
            $a1_1 = &mdns_y_piir_edge_wei3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x224:
        {
            $a1_1 = &mdns_y_piir_cur_fs_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x225:
        {
            $a1_1 = &mdns_y_piir_ref_fs_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x226:
        {
            $a1_1 = &mdns_y_pspa_fnl_fus_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x227:
        {
            $a1_1 = &mdns_y_pspa_fnl_fus_swei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x228:
        {
            $a1_1 = &mdns_y_pspa_fnl_fus_dwei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x229:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x22a:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x22b:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_16_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x22c:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_32_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x22d:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_48_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x22e:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_64_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x22f:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_80_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x230:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_96_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x231:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_112_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x232:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_128_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x233:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_144_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x234:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_160_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x235:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_176_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x236:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_192_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x237:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_208_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x238:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_224_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x239:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_240_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x23a:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x23b:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x23c:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_16_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x23d:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_32_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x23e:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_48_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x23f:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_64_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x240:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_80_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x241:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_96_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x242:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_112_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x243:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_128_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x244:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_144_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x245:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_160_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x246:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_176_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x247:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_192_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x248:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_208_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x249:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_224_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x24a:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_240_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x24b:
        {
            $a1_1 = &mdns_y_fiir_edge_thres0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x24c:
        {
            $a1_1 = &mdns_y_fiir_edge_thres1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x24d:
        {
            $a1_1 = &mdns_y_fiir_edge_thres2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x24e:
        {
            $a1_1 = &mdns_y_fiir_edge_wei0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x24f:
        {
            $a1_1 = &mdns_y_fiir_edge_wei1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x250:
        {
            $a1_1 = &mdns_y_fiir_edge_wei2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x251:
        {
            $a1_1 = &mdns_y_fiir_edge_wei3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x252:
        {
            $a1_1 = &mdns_y_fiir_fus_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x253:
        {
            $a1_1 = &mdns_y_fiir_fus_wei0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x254:
        {
            $a1_1 = &mdns_y_fiir_fus_wei1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x255:
        {
            $a1_1 = &mdns_y_fiir_fus_wei2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x256:
        {
            $a1_1 = &mdns_y_fiir_fus_wei3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x257:
        {
            $a1_1 = &mdns_y_fiir_fus_wei4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x258:
        {
            $a1_1 = &mdns_y_fiir_fus_wei5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x259:
        {
            $a1_1 = &mdns_y_fiir_fus_wei6_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x25a:
        {
            $a1_1 = &mdns_y_fiir_fus_wei7_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x25b:
        {
            $a1_1 = &mdns_y_fiir_fus_wei8_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x25c:
        {
            $a1_1 = &mdns_y_con_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x25d:
        {
            $a1_1 = &mdns_y_con_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x25e:
        {
            $a1_1 = &mdns_y_pspa_cur_median_win_opt_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x25f:
        {
            $a1_1 = &mdns_y_pspa_cur_bi_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x260:
        {
            $a1_1 = &mdns_y_pspa_cur_bi_wei0_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x261:
        {
            $a1_1 = &mdns_y_pspa_ref_median_win_opt_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x262:
        {
            $a1_1 = &mdns_y_pspa_ref_bi_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x263:
        {
            $a1_1 = &mdns_y_pspa_ref_bi_wei0_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x264:
        {
            $a1_1 = &mdns_y_piir_cur_fs_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x265:
        {
            $a1_1 = &mdns_y_piir_ref_fs_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x266:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_144_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x267:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_160_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x268:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_176_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x269:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_192_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x26a:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_208_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x26b:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_224_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x26c:
        {
            $a1_1 = &mdns_y_fspa_cur_fus_wei_240_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x26d:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_144_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x26e:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_160_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x26f:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_176_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x270:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_192_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x271:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_208_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x272:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_224_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x273:
        {
            $a1_1 = &mdns_y_fspa_ref_fus_wei_240_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x274:
        {
            $a1_1 = &mdns_y_fiir_fus_wei0_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x275:
        {
            $a1_1 = &mdns_y_fiir_fus_wei1_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x276:
        {
            $a1_1 = &mdns_y_fiir_fus_wei2_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x277:
        {
            $a1_1 = &mdns_y_fiir_fus_wei3_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x278:
        {
            $a1_1 = &mdns_y_fiir_fus_wei4_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x279:
        {
            $a1_1 = &mdns_y_fiir_fus_wei5_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x27a:
        {
            $a1_1 = &mdns_y_fiir_fus_wei6_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x27b:
        {
            $a1_1 = &mdns_y_fiir_fus_wei7_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x27c:
        {
            $a1_1 = &mdns_y_fiir_fus_wei8_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x27d:
        {
            $a1_1 = &mdns_c_sad_win_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x27e:
        {
            $a1_1 = &mdns_c_sad_ave_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x27f:
        {
            $a1_1 = &mdns_c_sad_ave_slope_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x280:
        {
            $a1_1 = &mdns_c_sad_dtb_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x281:
        {
            $a1_1 = &mdns_c_sad_ass_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x282:
        {
            $a1_1 = &mdns_c_ref_wei_mv_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x283:
        {
            $a1_1 = &mdns_c_ref_wei_fake_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x284:
        {
            $a1_1 = &mdns_c_ref_wei_f_max_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x285:
        {
            $a1_1 = &mdns_c_ref_wei_f_min_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x286:
        {
            $a1_1 = &mdns_c_ref_wei_b_max_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x287:
        {
            $a1_1 = &mdns_c_ref_wei_b_min_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x288:
        {
            $a1_1 = &mdns_c_ref_wei_r_max_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x289:
        {
            $a1_1 = &mdns_c_ref_wei_r_min_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x28a:
        {
            $a1_1 = &mdns_c_ref_wei_increase_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x28b:
        {
            $a1_1 = &mdns_c_edge_thr_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x28c:
        {
            $a1_1 = &mdns_c_luma_thr_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x28d:
        {
            $a1_1 = &mdns_c_dtb_thr_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x28e:
        {
            $a1_1 = &mdns_c_ass_thr_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x28f:
        {
            $a1_1 = &mdns_c_corner_thr_adj_value_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x290:
        {
            $a1_1 = &mdns_c_edge_thr_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x291:
        {
            $a1_1 = &mdns_c_edge_thr_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x292:
        {
            $a1_1 = &mdns_c_edge_thr_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x293:
        {
            $a1_1 = &mdns_c_edge_thr_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x294:
        {
            $a1_1 = &mdns_c_edge_thr_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x295:
        {
            $a1_1 = &mdns_c_edge_thr_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x296:
        {
            $a1_1 = &mdns_c_luma_thr_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x297:
        {
            $a1_1 = &mdns_c_luma_thr_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x298:
        {
            $a1_1 = &mdns_c_luma_thr_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x299:
        {
            $a1_1 = &mdns_c_luma_thr_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x29a:
        {
            $a1_1 = &mdns_c_luma_thr_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x29b:
        {
            $a1_1 = &mdns_c_luma_thr_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x29c:
        {
            $a1_1 = &mdns_c_dtb_thr_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x29d:
        {
            $a1_1 = &mdns_c_dtb_thr_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x29e:
        {
            $a1_1 = &tmpMapB;
            $s1_1 = 0x24;
            break;
        }
        case 0x29f:
        {
            $a1_1 = &mdns_c_dtb_thr_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2a0:
        {
            $a1_1 = &mdns_c_dtb_thr_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2a1:
        {
            $a1_1 = &mdns_c_dtb_thr_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2a2:
        {
            $a1_1 = &mdns_c_ass_thr_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2a3:
        {
            $a1_1 = &mdns_c_ass_thr_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2a4:
        {
            $a1_1 = &mdns_c_ass_thr_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2a5:
        {
            $a1_1 = &mdns_c_ass_thr_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2a6:
        {
            $a1_1 = &mdns_c_ass_thr_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2a7:
        {
            $a1_1 = &mdns_c_ass_thr_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2a8:
        {
            $a1_1 = &mdns_c_edge_wei_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2a9:
        {
            $a1_1 = &mdns_c_luma_wei_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2aa:
        {
            $a1_1 = &mdns_c_dtb_wei_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ab:
        {
            $a1_1 = &mdns_c_ass_wei_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ac:
        {
            $a1_1 = &mdns_c_sad_wei_adj_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ad:
        {
            $a1_1 = &mdns_c_corner_wei_adj_value_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ae:
        {
            $a1_1 = &mdns_c_edge_wei_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2af:
        {
            $a1_1 = &mdns_c_edge_wei_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2b0:
        {
            $a1_1 = &mdns_c_edge_wei_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2b1:
        {
            $a1_1 = &mdns_c_edge_wei_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2b2:
        {
            $a1_1 = &mdns_c_edge_wei_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2b3:
        {
            $a1_1 = &mdns_c_edge_wei_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2b4:
        {
            $a1_1 = &mdns_c_luma_wei_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2b5:
        {
            $a1_1 = &mdns_c_luma_wei_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2b6:
        {
            $a1_1 = &mdns_c_luma_wei_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2b7:
        {
            $a1_1 = &mdns_c_luma_wei_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2b8:
        {
            $a1_1 = &mdns_c_luma_wei_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2b9:
        {
            $a1_1 = &mdns_c_luma_wei_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ba:
        {
            $a1_1 = &mdns_c_dtb_wei_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2bb:
        {
            $a1_1 = &mdns_c_dtb_wei_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2bc:
        {
            $a1_1 = &mdns_c_dtb_wei_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2bd:
        {
            $a1_1 = &mdns_c_dtb_wei_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2be:
        {
            $a1_1 = &mdns_c_dtb_wei_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2bf:
        {
            $a1_1 = &mdns_c_dtb_wei_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2c0:
        {
            $a1_1 = &mdns_c_ass_wei_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2c1:
        {
            $a1_1 = &mdns_c_ass_wei_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2c2:
        {
            $a1_1 = &mdns_c_ass_wei_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2c3:
        {
            $a1_1 = &mdns_c_ass_wei_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2c4:
        {
            $a1_1 = &mdns_c_ass_wei_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2c5:
        {
            $a1_1 = &mdns_c_ass_wei_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2c6:
        {
            $a1_1 = &mdns_c_sad_wei_adj_value0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2c7:
        {
            $a1_1 = &mdns_c_sad_wei_adj_value1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2c8:
        {
            $a1_1 = &mdns_c_sad_wei_adj_value2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2c9:
        {
            $a1_1 = &mdns_c_sad_wei_adj_value3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ca:
        {
            $a1_1 = &mdns_c_sad_wei_adj_value4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2cb:
        {
            $a1_1 = &mdns_c_sad_wei_adj_value5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2cc:
        {
            $a1_1 = &mdns_c_sad_ave_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2cd:
        {
            $a1_1 = &mdns_c_sad_ass_thres_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ce:
        {
            $a1_1 = &mdns_c_ref_wei_b_max_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2cf:
        {
            $a1_1 = &mdns_c_ref_wei_b_min_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2d0:
        {
            $a1_1 = &mdns_c_median_smj_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2d1:
        {
            $a1_1 = &mdns_c_median_edg_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2d2:
        {
            $a1_1 = &mdns_c_median_cur_lmt_op_en_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2d3:
        {
            $a1_1 = &mdns_c_median_cur_lmt_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2d4:
        {
            $a1_1 = &mdns_c_median_cur_ss_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2d5:
        {
            $a1_1 = &mdns_c_median_cur_se_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2d6:
        {
            $a1_1 = &mdns_c_median_cur_ms_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2d7:
        {
            $a1_1 = &mdns_c_median_cur_me_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2d8:
        {
            $a1_1 = &mdns_c_median_ref_lmt_op_en_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2d9:
        {
            $a1_1 = &mdns_c_median_ref_lmt_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2da:
        {
            $a1_1 = &mdns_c_median_ref_ss_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2db:
        {
            $a1_1 = &mdns_c_median_ref_se_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2dc:
        {
            $a1_1 = &mdns_c_median_ref_ms_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2dd:
        {
            $a1_1 = &mdns_c_median_ref_me_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2de:
        {
            $a1_1 = &mdns_c_bgm_win_opt_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2df:
        {
            $a1_1 = &mdns_c_bgm_cur_src_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2e0:
        {
            $a1_1 = &mdns_c_bgm_ref_src_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2e1:
        {
            $a1_1 = &mdns_c_bgm_false_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2e2:
        {
            $a1_1 = &mdns_c_bgm_false_step_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2e3:
        {
            $a1_1 = &mdns_c_piir_edge_thres0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2e4:
        {
            $a1_1 = &mdns_c_piir_edge_thres1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2e5:
        {
            $a1_1 = &mdns_c_piir_edge_thres2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2e6:
        {
            $a1_1 = &mdns_c_piir_edge_wei0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2e7:
        {
            $a1_1 = &mdns_c_piir_edge_wei1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2e8:
        {
            $a1_1 = &mdns_c_piir_edge_wei2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2e9:
        {
            $a1_1 = &mdns_c_piir_edge_wei3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ea:
        {
            $a1_1 = &mdns_c_piir_cur_fs_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2eb:
        {
            $a1_1 = &mdns_c_piir_ref_fs_wei_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ec:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ed:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ee:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_16_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ef:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_32_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2f0:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_48_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2f1:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_64_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2f2:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_80_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2f3:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_96_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2f4:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_112_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2f5:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_128_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2f6:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_144_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2f7:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_160_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2f8:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_176_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2f9:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_192_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2fa:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_208_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2fb:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_224_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2fc:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_240_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2fd:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2fe:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x2ff:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_16_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x300:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_32_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x301:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_48_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x302:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_64_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x303:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_80_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x304:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_96_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x305:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_112_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x306:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_128_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x307:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_144_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x308:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_160_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x309:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_176_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x30a:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_192_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x30b:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_208_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x30c:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_224_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x30d:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_240_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x30e:
        {
            $a1_1 = &mdns_c_fiir_edge_thres0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x30f:
        {
            $a1_1 = &mdns_c_fiir_edge_thres1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x310:
        {
            $a1_1 = &mdns_c_fiir_edge_thres2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x311:
        {
            $a1_1 = &mdns_c_fiir_edge_wei0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x312:
        {
            $a1_1 = &mdns_c_fiir_edge_wei1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x313:
        {
            $a1_1 = &mdns_c_fiir_edge_wei2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x314:
        {
            $a1_1 = &mdns_c_fiir_edge_wei3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x315:
        {
            $a1_1 = &mdns_c_fiir_fus_seg_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x316:
        {
            $a1_1 = &mdns_c_fiir_fus_wei0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x317:
        {
            $a1_1 = &mdns_c_fiir_fus_wei1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x318:
        {
            $a1_1 = &mdns_c_fiir_fus_wei2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x319:
        {
            $a1_1 = &mdns_c_fiir_fus_wei3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x31a:
        {
            $a1_1 = &mdns_c_fiir_fus_wei4_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x31b:
        {
            $a1_1 = &mdns_c_fiir_fus_wei5_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x31c:
        {
            $a1_1 = &mdns_c_fiir_fus_wei6_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x31d:
        {
            $a1_1 = &mdns_c_fiir_fus_wei7_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x31e:
        {
            $a1_1 = &mdns_c_fiir_fus_wei8_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x31f:
        {
            $a1_1 = &mdns_c_false_smj_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x320:
        {
            $a1_1 = &mdns_c_false_edg_thres0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x321:
        {
            $a1_1 = &mdns_c_false_edg_thres1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x322:
        {
            $a1_1 = &mdns_c_false_edg_thres2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x323:
        {
            $a1_1 = &mdns_c_false_thres_s0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x324:
        {
            $a1_1 = &mdns_c_false_thres_s1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x325:
        {
            $a1_1 = &mdns_c_false_thres_s2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x326:
        {
            $a1_1 = &mdns_c_false_thres_s3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x327:
        {
            $a1_1 = &mdns_c_false_step_s0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x328:
        {
            $a1_1 = &mdns_c_false_step_s1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x329:
        {
            $a1_1 = &mdns_c_false_step_s2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x32a:
        {
            $a1_1 = &mdns_c_false_step_s3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x32b:
        {
            $a1_1 = &mdns_c_false_thres_m0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x32c:
        {
            $a1_1 = &mdns_c_false_thres_m1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x32d:
        {
            $a1_1 = &mdns_c_false_thres_m2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x32e:
        {
            $a1_1 = &mdns_c_false_thres_m3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x32f:
        {
            $a1_1 = &mdns_c_false_step_m0_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x330:
        {
            $a1_1 = &mdns_c_false_step_m1_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x331:
        {
            $a1_1 = &mdns_c_false_step_m2_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x332:
        {
            $a1_1 = &mdns_c_false_step_m3_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x333:
        {
            $a1_1 = &mdns_c_sat_lmt_thres_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x334:
        {
            $a1_1 = &mdns_c_sat_lmt_stren_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x335:
        {
            $a1_1 = &mdns_c_median_cur_ss_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x336:
        {
            $a1_1 = &mdns_c_median_cur_se_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x337:
        {
            $a1_1 = &mdns_c_median_cur_ms_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x338:
        {
            $a1_1 = &mdns_c_median_cur_me_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x339:
        {
            $a1_1 = &mdns_c_median_ref_ss_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x33a:
        {
            $a1_1 = &mdns_c_median_ref_se_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x33b:
        {
            $a1_1 = &mdns_c_median_ref_ms_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x33c:
        {
            $a1_1 = &mdns_c_median_ref_me_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x33d:
        {
            $a1_1 = &mdns_c_piir_cur_fs_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x33e:
        {
            $a1_1 = &mdns_c_piir_ref_fs_wei_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x33f:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_144_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x340:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_160_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x341:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_176_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x342:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_192_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x343:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_208_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x344:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_224_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x345:
        {
            $a1_1 = &mdns_c_fspa_cur_fus_wei_240_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x346:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_144_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x347:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_160_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x348:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_176_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x349:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_192_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x34a:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_208_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x34b:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_224_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x34c:
        {
            $a1_1 = &mdns_c_fspa_ref_fus_wei_240_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x34d:
        {
            $a1_1 = &mdns_c_fiir_fus_wei0_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x34e:
        {
            $a1_1 = &mdns_c_fiir_fus_wei1_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x34f:
        {
            $a1_1 = &mdns_c_fiir_fus_wei2_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x350:
        {
            $a1_1 = &mdns_c_fiir_fus_wei3_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x351:
        {
            $a1_1 = &mdns_c_fiir_fus_wei4_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x352:
        {
            $a1_1 = &mdns_c_fiir_fus_wei5_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x353:
        {
            $a1_1 = &mdns_c_fiir_fus_wei6_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x354:
        {
            $a1_1 = &mdns_c_fiir_fus_wei7_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x355:
        {
            $a1_1 = &mdns_c_fiir_fus_wei8_wdr_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x356:
        {
            $a1_1 = &mdns_c_sat_nml_stren_array;
            $s1_1 = 0x24;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

