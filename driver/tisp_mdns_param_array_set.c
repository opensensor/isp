#include "include/main.h"


  int32_t tisp_mdns_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_10_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0x180 >= 0x1d7)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    int32_t $v0_1;
    
    switch (arg1)
    {
        case 0x180:
        {
            memcpy(&mdns_y_filter_en_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x181:
        {
            memcpy(&mdns_y_sf_cur_en_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x182:
        {
            memcpy(&mdns_y_sf_ref_en_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x183:
        {
            memcpy(&mdns_y_debug_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x184:
        {
            memcpy(&mdns_uv_filter_en_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x185:
        {
            memcpy(&mdns_uv_sf_cur_en_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x186:
        {
            memcpy(&mdns_uv_sf_ref_en_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x187:
        {
            memcpy(&mdns_uv_debug_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x188:
        {
            memcpy(&mdns_ass_enable_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x189:
        {
            memcpy(&mdns_sta_inter_en_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x18a:
        {
            memcpy(&mdns_sta_group_num_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x18b:
        {
            memcpy(&mdns_sta_max_num_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x18c:
        {
            memcpy(&mdns_bgm_enable_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x18d:
        {
            memcpy(&mdns_bgm_inter_en_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x18e:
        {
            memcpy(&mdns_psn_enable_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x18f:
        {
            memcpy(&mdns_psn_max_num_array, arg2, 4);
            $v0_1 = 4;
            break;
        }
        case 0x190:
        {
            memcpy(&mdns_ref_wei_byps_array, arg2, 4);
            tisp_mdns_all_reg_refresh(data_9a9d0);
            tisp_mdns_top_func_refresh();
            tisp_mdns_reg_trigger();
            $v0_1 = 4;
            break;
        }
        case 0x191:
        {
            memcpy(&mdns_y_sad_win_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x192:
        {
            memcpy(&mdns_y_sad_ave_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x193:
        {
            memcpy(&mdns_y_sad_ave_slope_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x194:
        {
            memcpy(&mdns_y_sad_dtb_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x195:
        {
            memcpy(&mdns_y_sad_ass_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x196:
        {
            memcpy(&claHistG0, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x197:
        {
            memcpy(&mdns_y_sta_win_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x198:
        {
            memcpy(&mdns_y_sta_ave_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x199:
        {
            memcpy(&mdns_y_sta_dtb_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x19a:
        {
            memcpy(&mdns_y_sta_ass_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x19b:
        {
            memcpy(&mdns_y_sta_motion_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x19c:
        {
            memcpy(&mdns_y_ref_wei_sta_array, arg2, 0x40);
            $v0_1 = 0x40;
            break;
        }
        case 0x19d:
        {
            memcpy(&mdns_y_ref_wei_psn_array, arg2, 0x40);
            $v0_1 = 0x40;
            break;
        }
        case 0x19e:
        {
            memcpy(&mdns_y_ref_wei_mv_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x19f:
        {
            memcpy(&mdns_y_ref_wei_fake_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1a0:
        {
            memcpy(&mdns_y_ref_wei_sta_fs_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1a1:
        {
            memcpy(&mdns_y_ref_wei_psn_fs_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1a2:
        {
            memcpy(&mdns_y_ref_wei_f_max_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1a3:
        {
            memcpy(&mdns_y_ref_wei_f_min_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1a4:
        {
            memcpy(&mdns_y_ref_wei_b_max_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1a5:
        {
            memcpy(&mdns_y_ref_wei_b_min_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1a6:
        {
            memcpy(&mdns_y_ref_wei_r_max_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1a7:
        {
            memcpy(&mdns_y_ref_wei_r_min_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1a8:
        {
            memcpy(&mdns_y_ref_wei_increase_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1a9:
        {
            memcpy(&mdns_y_corner_length_t_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1aa:
        {
            memcpy(&mdns_y_corner_length_b_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ab:
        {
            memcpy(&mdns_y_corner_length_l_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ac:
        {
            memcpy(&mdns_y_corner_length_r_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ad:
        {
            memcpy(&mdns_y_edge_win_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ae:
        {
            memcpy(&mdns_y_edge_div_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1af:
        {
            memcpy(&mdns_y_edge_type_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1b0:
        {
            memcpy(&mdns_y_luma_win_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1b1:
        {
            memcpy(&mdns_y_dtb_div_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1b2:
        {
            memcpy(&mdns_y_dtb_squ_en_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1b3:
        {
            memcpy(&mdns_y_dtb_squ_div_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1b4:
        {
            memcpy(&mdns_y_ass_win_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1b5:
        {
            memcpy(&mdns_y_ass_div_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1b6:
        {
            memcpy(&mdns_y_hist_sad_en_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1b7:
        {
            memcpy(&mdns_y_hist_sta_en_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1b8:
        {
            memcpy(&mdns_y_hist_num_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1b9:
        {
            memcpy(&mdns_y_hist_cmp_thres0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ba:
        {
            memcpy(&mdns_y_hist_cmp_thres1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1bb:
        {
            memcpy(&mdns_y_hist_cmp_thres2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1bc:
        {
            memcpy(&mdns_y_hist_cmp_thres3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1bd:
        {
            memcpy(&mdns_y_hist_thres0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1be:
        {
            memcpy(&mdns_y_hist_thres1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1bf:
        {
            memcpy(&mdns_y_hist_thres2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1c0:
        {
            memcpy(&mdns_y_hist_thres3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1c1:
        {
            memcpy(&mdns_y_edge_thr_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1c2:
        {
            memcpy(&mdns_y_luma_thr_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1c3:
        {
            memcpy(&mdns_y_dtb_thr_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1c4:
        {
            memcpy(&mdns_y_ass_thr_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1c5:
        {
            memcpy(&mdns_y_corner_thr_adj_value_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1c6:
        {
            memcpy(&mdns_y_edge_thr_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1c7:
        {
            memcpy(&mdns_y_edge_thr_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1c8:
        {
            memcpy(&mdns_y_edge_thr_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1c9:
        {
            memcpy(&mdns_y_edge_thr_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ca:
        {
            memcpy(&mdns_y_edge_thr_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1cb:
        {
            memcpy(&mdns_y_edge_thr_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1cc:
        {
            memcpy(&mdns_y_luma_thr_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1cd:
        {
            memcpy(&mdns_y_luma_thr_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ce:
        {
            memcpy(&mdns_y_luma_thr_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1cf:
        {
            memcpy(&mdns_y_luma_thr_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1d0:
        {
            memcpy(&mdns_y_luma_thr_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1d1:
        {
            memcpy(&mdns_y_luma_thr_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1d2:
        {
            memcpy(&mdns_y_dtb_thr_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1d3:
        {
            memcpy(&mdns_y_dtb_thr_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1d4:
        {
            memcpy(&mdns_y_dtb_thr_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1d5:
        {
            memcpy(&mdns_y_dtb_thr_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1d6:
        {
            memcpy(&mdns_y_dtb_thr_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1d7:
        {
            memcpy(&mdns_y_dtb_thr_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1d8:
        {
            memcpy(&mdns_y_ass_thr_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1d9:
        {
            memcpy(&mdns_y_ass_thr_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1da:
        {
            memcpy(&mdns_y_ass_thr_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1db:
        {
            memcpy(&mdns_y_ass_thr_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1dc:
        {
            memcpy(&mdns_y_ass_thr_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1dd:
        {
            memcpy(&mdns_y_ass_thr_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1de:
        {
            memcpy(&mdns_y_edge_wei_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1df:
        {
            memcpy(&mdns_y_luma_wei_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1e0:
        {
            memcpy(&mdns_y_dtb_wei_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1e1:
        {
            memcpy(&mdns_y_ass_wei_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1e2:
        {
            memcpy(&mdns_y_sad_wei_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1e3:
        {
            memcpy(&mdns_y_corner_wei_adj_value_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1e4:
        {
            memcpy(&mdns_y_edge_wei_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1e5:
        {
            memcpy(&mdns_y_edge_wei_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1e6:
        {
            memcpy(&mdns_y_edge_wei_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1e7:
        {
            memcpy(&mdns_y_edge_wei_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1e8:
        {
            memcpy(&mdns_y_edge_wei_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1e9:
        {
            memcpy(&mdns_y_edge_wei_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ea:
        {
            memcpy(&mdns_y_luma_wei_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1eb:
        {
            memcpy(&mdns_y_luma_wei_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ec:
        {
            memcpy(&mdns_y_luma_wei_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ed:
        {
            memcpy(&mdns_y_luma_wei_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ee:
        {
            memcpy(&mdns_y_luma_wei_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ef:
        {
            memcpy(&mdns_y_luma_wei_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1f0:
        {
            memcpy(&mdns_y_dtb_wei_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1f1:
        {
            memcpy(&mdns_y_dtb_wei_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1f2:
        {
            memcpy(&mdns_y_dtb_wei_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1f3:
        {
            memcpy(&mdns_y_dtb_wei_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1f4:
        {
            memcpy(&mdns_y_dtb_wei_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1f5:
        {
            memcpy(&mdns_y_dtb_wei_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1f6:
        {
            memcpy(&mdns_y_ass_wei_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1f7:
        {
            memcpy(&mdns_y_ass_wei_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1f8:
        {
            memcpy(&mdns_y_ass_wei_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1f9:
        {
            memcpy(&mdns_y_ass_wei_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1fa:
        {
            memcpy(&mdns_y_ass_wei_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1fb:
        {
            memcpy(&mdns_y_ass_wei_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1fc:
        {
            memcpy(&mdns_y_sad_wei_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1fd:
        {
            memcpy(&mdns_y_sad_wei_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1fe:
        {
            memcpy(&mdns_y_sad_wei_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x1ff:
        {
            memcpy(&mdns_y_sad_wei_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x200:
        {
            memcpy(&mdns_y_sad_wei_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x201:
        {
            memcpy(&mdns_y_sad_wei_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x202:
        {
            memcpy(&mdns_y_sad_ave_thres_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x203:
        {
            memcpy(&mdns_y_sad_ass_thres_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x204:
        {
            memcpy(&mdns_y_sta_ave_thres_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x205:
        {
            memcpy(&mdns_y_sta_ass_thres_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x206:
        {
            memcpy(&mdns_y_sta_motion_thres_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x207:
        {
            memcpy(&mdns_y_ref_wei_b_max_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x208:
        {
            memcpy(&mdns_y_ref_wei_b_min_wdr_array, arg2, 0x24);
            tisp_mdns_all_reg_refresh(data_9a9d0);
            tisp_mdns_reg_trigger();
            $v0_1 = 0x24;
            break;
        }
        case 0x209:
        {
            memcpy(&mdns_y_pspa_cur_median_win_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x20a:
        {
            memcpy(&mdns_y_pspa_cur_bi_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x20b:
        {
            memcpy(&mdns_y_pspa_cur_bi_wei_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x20c:
        {
            memcpy(&mdns_y_pspa_cur_bi_wei0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x20d:
        {
            memcpy(&mdns_y_pspa_cur_bi_wei1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x20e:
        {
            memcpy(&mdns_y_pspa_cur_bi_wei2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x20f:
        {
            memcpy(&mdns_y_pspa_cur_bi_wei3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x210:
        {
            memcpy(&mdns_y_pspa_cur_bi_wei4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x211:
        {
            memcpy(&mdns_y_pspa_cur_lmt_op_en_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x212:
        {
            memcpy(&mdns_y_pspa_cur_lmt_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x213:
        {
            memcpy(&mdns_y_pspa_ref_median_win_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x214:
        {
            memcpy(&mdns_y_pspa_ref_bi_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x215:
        {
            memcpy(&mdns_y_pspa_ref_bi_wei_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x216:
        {
            memcpy(&mdns_y_pspa_ref_bi_wei0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x217:
        {
            memcpy(&mdns_y_pspa_ref_bi_wei1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x218:
        {
            memcpy(&mdns_y_pspa_ref_bi_wei2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x219:
        {
            memcpy(&mdns_y_pspa_ref_bi_wei3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x21a:
        {
            memcpy(&mdns_y_pspa_ref_bi_wei4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x21b:
        {
            memcpy(&mdns_y_pspa_ref_lmt_op_en_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x21c:
        {
            memcpy(&mdns_y_pspa_ref_lmt_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x21d:
        {
            memcpy(&mdns_y_piir_edge_thres0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x21e:
        {
            memcpy(&mdns_y_piir_edge_thres1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x21f:
        {
            memcpy(&mdns_y_piir_edge_thres2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x220:
        {
            memcpy(&mdns_y_piir_edge_wei0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x221:
        {
            memcpy(&mdns_y_piir_edge_wei1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x222:
        {
            memcpy(&mdns_y_piir_edge_wei2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x223:
        {
            memcpy(&mdns_y_piir_edge_wei3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x224:
        {
            memcpy(&mdns_y_piir_cur_fs_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x225:
        {
            memcpy(&mdns_y_piir_ref_fs_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x226:
        {
            memcpy(&mdns_y_pspa_fnl_fus_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x227:
        {
            memcpy(&mdns_y_pspa_fnl_fus_swei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x228:
        {
            memcpy(&mdns_y_pspa_fnl_fus_dwei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x229:
        {
            memcpy(&mdns_y_fspa_cur_fus_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x22a:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x22b:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_16_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x22c:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_32_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x22d:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_48_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x22e:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_64_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x22f:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_80_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x230:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_96_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x231:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_112_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x232:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_128_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x233:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_144_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x234:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_160_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x235:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_176_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x236:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_192_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x237:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_208_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x238:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_224_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x239:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_240_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x23a:
        {
            memcpy(&mdns_y_fspa_ref_fus_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x23b:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x23c:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_16_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x23d:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_32_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x23e:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_48_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x23f:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_64_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x240:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_80_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x241:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_96_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x242:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_112_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x243:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_128_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x244:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_144_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x245:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_160_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x246:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_176_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x247:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_192_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x248:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_208_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x249:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_224_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x24a:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_240_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x24b:
        {
            memcpy(&mdns_y_fiir_edge_thres0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x24c:
        {
            memcpy(&mdns_y_fiir_edge_thres1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x24d:
        {
            memcpy(&mdns_y_fiir_edge_thres2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x24e:
        {
            memcpy(&mdns_y_fiir_edge_wei0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x24f:
        {
            memcpy(&mdns_y_fiir_edge_wei1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x250:
        {
            memcpy(&mdns_y_fiir_edge_wei2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x251:
        {
            memcpy(&mdns_y_fiir_edge_wei3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x252:
        {
            memcpy(&mdns_y_fiir_fus_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x253:
        {
            memcpy(&mdns_y_fiir_fus_wei0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x254:
        {
            memcpy(&mdns_y_fiir_fus_wei1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x255:
        {
            memcpy(&mdns_y_fiir_fus_wei2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x256:
        {
            memcpy(&mdns_y_fiir_fus_wei3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x257:
        {
            memcpy(&mdns_y_fiir_fus_wei4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x258:
        {
            memcpy(&mdns_y_fiir_fus_wei5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x259:
        {
            memcpy(&mdns_y_fiir_fus_wei6_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x25a:
        {
            memcpy(&mdns_y_fiir_fus_wei7_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x25b:
        {
            memcpy(&mdns_y_fiir_fus_wei8_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x25c:
        {
            memcpy(&mdns_y_con_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x25d:
        {
            memcpy(&mdns_y_con_stren_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x25e:
        {
            memcpy(&mdns_y_pspa_cur_median_win_opt_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x25f:
        {
            memcpy(&mdns_y_pspa_cur_bi_thres_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x260:
        {
            memcpy(&mdns_y_pspa_cur_bi_wei0_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x261:
        {
            memcpy(&mdns_y_pspa_ref_median_win_opt_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x262:
        {
            memcpy(&mdns_y_pspa_ref_bi_thres_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x263:
        {
            memcpy(&mdns_y_pspa_ref_bi_wei0_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x264:
        {
            memcpy(&mdns_y_piir_cur_fs_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x265:
        {
            memcpy(&mdns_y_piir_ref_fs_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x266:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_144_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x267:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_160_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x268:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_176_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x269:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_192_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x26a:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_208_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x26b:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_224_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x26c:
        {
            memcpy(&mdns_y_fspa_cur_fus_wei_240_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x26d:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_144_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x26e:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_160_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x26f:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_176_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x270:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_192_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x271:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_208_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x272:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_224_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x273:
        {
            memcpy(&mdns_y_fspa_ref_fus_wei_240_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x274:
        {
            memcpy(&mdns_y_fiir_fus_wei0_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x275:
        {
            memcpy(&mdns_y_fiir_fus_wei1_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x276:
        {
            memcpy(&mdns_y_fiir_fus_wei2_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x277:
        {
            memcpy(&mdns_y_fiir_fus_wei3_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x278:
        {
            memcpy(&mdns_y_fiir_fus_wei4_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x279:
        {
            memcpy(&mdns_y_fiir_fus_wei5_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x27a:
        {
            memcpy(&mdns_y_fiir_fus_wei6_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x27b:
        {
            memcpy(&mdns_y_fiir_fus_wei7_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x27c:
        {
            memcpy(&mdns_y_fiir_fus_wei8_wdr_array, arg2, 0x24);
            tisp_mdns_all_reg_refresh(data_9a9d0);
            tisp_mdns_reg_trigger();
            $v0_1 = 0x24;
            break;
        }
        case 0x27d:
        {
            memcpy(&mdns_c_sad_win_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x27e:
        {
            memcpy(&mdns_c_sad_ave_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x27f:
        {
            memcpy(&mdns_c_sad_ave_slope_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x280:
        {
            memcpy(&mdns_c_sad_dtb_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x281:
        {
            memcpy(&mdns_c_sad_ass_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x282:
        {
            memcpy(&mdns_c_ref_wei_mv_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x283:
        {
            memcpy(&mdns_c_ref_wei_fake_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x284:
        {
            memcpy(&mdns_c_ref_wei_f_max_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x285:
        {
            memcpy(&mdns_c_ref_wei_f_min_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x286:
        {
            memcpy(&mdns_c_ref_wei_b_max_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x287:
        {
            memcpy(&mdns_c_ref_wei_b_min_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x288:
        {
            memcpy(&mdns_c_ref_wei_r_max_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x289:
        {
            memcpy(&mdns_c_ref_wei_r_min_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x28a:
        {
            memcpy(&mdns_c_ref_wei_increase_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x28b:
        {
            memcpy(&mdns_c_edge_thr_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x28c:
        {
            memcpy(&mdns_c_luma_thr_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x28d:
        {
            memcpy(&mdns_c_dtb_thr_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x28e:
        {
            memcpy(&mdns_c_ass_thr_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x28f:
        {
            memcpy(&mdns_c_corner_thr_adj_value_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x290:
        {
            memcpy(&mdns_c_edge_thr_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x291:
        {
            memcpy(&mdns_c_edge_thr_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x292:
        {
            memcpy(&mdns_c_edge_thr_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x293:
        {
            memcpy(&mdns_c_edge_thr_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x294:
        {
            memcpy(&mdns_c_edge_thr_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x295:
        {
            memcpy(&mdns_c_edge_thr_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x296:
        {
            memcpy(&mdns_c_luma_thr_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x297:
        {
            memcpy(&mdns_c_luma_thr_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x298:
        {
            memcpy(&mdns_c_luma_thr_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x299:
        {
            memcpy(&mdns_c_luma_thr_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x29a:
        {
            memcpy(&mdns_c_luma_thr_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x29b:
        {
            memcpy(&mdns_c_luma_thr_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x29c:
        {
            memcpy(&mdns_c_dtb_thr_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x29d:
        {
            memcpy(&mdns_c_dtb_thr_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x29e:
        {
            memcpy(&tmpMapB, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x29f:
        {
            memcpy(&mdns_c_dtb_thr_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2a0:
        {
            memcpy(&mdns_c_dtb_thr_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2a1:
        {
            memcpy(&mdns_c_dtb_thr_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2a2:
        {
            memcpy(&mdns_c_ass_thr_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2a3:
        {
            memcpy(&mdns_c_ass_thr_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2a4:
        {
            memcpy(&mdns_c_ass_thr_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2a5:
        {
            memcpy(&mdns_c_ass_thr_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2a6:
        {
            memcpy(&mdns_c_ass_thr_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2a7:
        {
            memcpy(&mdns_c_ass_thr_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2a8:
        {
            memcpy(&mdns_c_edge_wei_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2a9:
        {
            memcpy(&mdns_c_luma_wei_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2aa:
        {
            memcpy(&mdns_c_dtb_wei_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ab:
        {
            memcpy(&mdns_c_ass_wei_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ac:
        {
            memcpy(&mdns_c_sad_wei_adj_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ad:
        {
            memcpy(&mdns_c_corner_wei_adj_value_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ae:
        {
            memcpy(&mdns_c_edge_wei_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2af:
        {
            memcpy(&mdns_c_edge_wei_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2b0:
        {
            memcpy(&mdns_c_edge_wei_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2b1:
        {
            memcpy(&mdns_c_edge_wei_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2b2:
        {
            memcpy(&mdns_c_edge_wei_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2b3:
        {
            memcpy(&mdns_c_edge_wei_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2b4:
        {
            memcpy(&mdns_c_luma_wei_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2b5:
        {
            memcpy(&mdns_c_luma_wei_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2b6:
        {
            memcpy(&mdns_c_luma_wei_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2b7:
        {
            memcpy(&mdns_c_luma_wei_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2b8:
        {
            memcpy(&mdns_c_luma_wei_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2b9:
        {
            memcpy(&mdns_c_luma_wei_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ba:
        {
            memcpy(&mdns_c_dtb_wei_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2bb:
        {
            memcpy(&mdns_c_dtb_wei_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2bc:
        {
            memcpy(&mdns_c_dtb_wei_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2bd:
        {
            memcpy(&mdns_c_dtb_wei_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2be:
        {
            memcpy(&mdns_c_dtb_wei_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2bf:
        {
            memcpy(&mdns_c_dtb_wei_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2c0:
        {
            memcpy(&mdns_c_ass_wei_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2c1:
        {
            memcpy(&mdns_c_ass_wei_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2c2:
        {
            memcpy(&mdns_c_ass_wei_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2c3:
        {
            memcpy(&mdns_c_ass_wei_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2c4:
        {
            memcpy(&mdns_c_ass_wei_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2c5:
        {
            memcpy(&mdns_c_ass_wei_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2c6:
        {
            memcpy(&mdns_c_sad_wei_adj_value0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2c7:
        {
            memcpy(&mdns_c_sad_wei_adj_value1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2c8:
        {
            memcpy(&mdns_c_sad_wei_adj_value2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2c9:
        {
            memcpy(&mdns_c_sad_wei_adj_value3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ca:
        {
            memcpy(&mdns_c_sad_wei_adj_value4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2cb:
        {
            memcpy(&mdns_c_sad_wei_adj_value5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2cc:
        {
            memcpy(&mdns_c_sad_ave_thres_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2cd:
        {
            memcpy(&mdns_c_sad_ass_thres_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ce:
        {
            memcpy(&mdns_c_ref_wei_b_max_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2cf:
        {
            memcpy(&mdns_c_ref_wei_b_min_wdr_array, arg2, 0x24);
            tisp_mdns_all_reg_refresh(data_9a9d0);
            tisp_mdns_reg_trigger();
            $v0_1 = 0x24;
            break;
        }
        case 0x2d0:
        {
            memcpy(&mdns_c_median_smj_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2d1:
        {
            memcpy(&mdns_c_median_edg_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2d2:
        {
            memcpy(&mdns_c_median_cur_lmt_op_en_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2d3:
        {
            memcpy(&mdns_c_median_cur_lmt_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2d4:
        {
            memcpy(&mdns_c_median_cur_ss_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2d5:
        {
            memcpy(&mdns_c_median_cur_se_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2d6:
        {
            memcpy(&mdns_c_median_cur_ms_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2d7:
        {
            memcpy(&mdns_c_median_cur_me_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2d8:
        {
            memcpy(&mdns_c_median_ref_lmt_op_en_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2d9:
        {
            memcpy(&mdns_c_median_ref_lmt_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2da:
        {
            memcpy(&mdns_c_median_ref_ss_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2db:
        {
            memcpy(&mdns_c_median_ref_se_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2dc:
        {
            memcpy(&mdns_c_median_ref_ms_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2dd:
        {
            memcpy(&mdns_c_median_ref_me_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2de:
        {
            memcpy(&mdns_c_bgm_win_opt_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2df:
        {
            memcpy(&mdns_c_bgm_cur_src_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2e0:
        {
            memcpy(&mdns_c_bgm_ref_src_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2e1:
        {
            memcpy(&mdns_c_bgm_false_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2e2:
        {
            memcpy(&mdns_c_bgm_false_step_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2e3:
        {
            memcpy(&mdns_c_piir_edge_thres0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2e4:
        {
            memcpy(&mdns_c_piir_edge_thres1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2e5:
        {
            memcpy(&mdns_c_piir_edge_thres2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2e6:
        {
            memcpy(&mdns_c_piir_edge_wei0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2e7:
        {
            memcpy(&mdns_c_piir_edge_wei1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2e8:
        {
            memcpy(&mdns_c_piir_edge_wei2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2e9:
        {
            memcpy(&mdns_c_piir_edge_wei3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ea:
        {
            memcpy(&mdns_c_piir_cur_fs_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2eb:
        {
            memcpy(&mdns_c_piir_ref_fs_wei_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ec:
        {
            memcpy(&mdns_c_fspa_cur_fus_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ed:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ee:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_16_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ef:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_32_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2f0:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_48_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2f1:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_64_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2f2:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_80_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2f3:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_96_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2f4:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_112_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2f5:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_128_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2f6:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_144_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2f7:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_160_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2f8:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_176_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2f9:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_192_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2fa:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_208_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2fb:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_224_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2fc:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_240_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2fd:
        {
            memcpy(&mdns_c_fspa_ref_fus_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2fe:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x2ff:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_16_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x300:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_32_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x301:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_48_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x302:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_64_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x303:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_80_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x304:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_96_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x305:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_112_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x306:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_128_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x307:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_144_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x308:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_160_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x309:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_176_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x30a:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_192_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x30b:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_208_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x30c:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_224_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x30d:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_240_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x30e:
        {
            memcpy(&mdns_c_fiir_edge_thres0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x30f:
        {
            memcpy(&mdns_c_fiir_edge_thres1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x310:
        {
            memcpy(&mdns_c_fiir_edge_thres2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x311:
        {
            memcpy(&mdns_c_fiir_edge_wei0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x312:
        {
            memcpy(&mdns_c_fiir_edge_wei1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x313:
        {
            memcpy(&mdns_c_fiir_edge_wei2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x314:
        {
            memcpy(&mdns_c_fiir_edge_wei3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x315:
        {
            memcpy(&mdns_c_fiir_fus_seg_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x316:
        {
            memcpy(&mdns_c_fiir_fus_wei0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x317:
        {
            memcpy(&mdns_c_fiir_fus_wei1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x318:
        {
            memcpy(&mdns_c_fiir_fus_wei2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x319:
        {
            memcpy(&mdns_c_fiir_fus_wei3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x31a:
        {
            memcpy(&mdns_c_fiir_fus_wei4_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x31b:
        {
            memcpy(&mdns_c_fiir_fus_wei5_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x31c:
        {
            memcpy(&mdns_c_fiir_fus_wei6_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x31d:
        {
            memcpy(&mdns_c_fiir_fus_wei7_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x31e:
        {
            memcpy(&mdns_c_fiir_fus_wei8_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x31f:
        {
            memcpy(&mdns_c_false_smj_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x320:
        {
            memcpy(&mdns_c_false_edg_thres0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x321:
        {
            memcpy(&mdns_c_false_edg_thres1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x322:
        {
            memcpy(&mdns_c_false_edg_thres2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x323:
        {
            memcpy(&mdns_c_false_thres_s0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x324:
        {
            memcpy(&mdns_c_false_thres_s1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x325:
        {
            memcpy(&mdns_c_false_thres_s2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x326:
        {
            memcpy(&mdns_c_false_thres_s3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x327:
        {
            memcpy(&mdns_c_false_step_s0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x328:
        {
            memcpy(&mdns_c_false_step_s1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x329:
        {
            memcpy(&mdns_c_false_step_s2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x32a:
        {
            memcpy(&mdns_c_false_step_s3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x32b:
        {
            memcpy(&mdns_c_false_thres_m0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x32c:
        {
            memcpy(&mdns_c_false_thres_m1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x32d:
        {
            memcpy(&mdns_c_false_thres_m2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x32e:
        {
            memcpy(&mdns_c_false_thres_m3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x32f:
        {
            memcpy(&mdns_c_false_step_m0_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x330:
        {
            memcpy(&mdns_c_false_step_m1_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x331:
        {
            memcpy(&mdns_c_false_step_m2_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x332:
        {
            memcpy(&mdns_c_false_step_m3_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x333:
        {
            memcpy(&mdns_c_sat_lmt_thres_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x334:
        {
            memcpy(&mdns_c_sat_lmt_stren_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x335:
        {
            memcpy(&mdns_c_median_cur_ss_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x336:
        {
            memcpy(&mdns_c_median_cur_se_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x337:
        {
            memcpy(&mdns_c_median_cur_ms_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x338:
        {
            memcpy(&mdns_c_median_cur_me_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x339:
        {
            memcpy(&mdns_c_median_ref_ss_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x33a:
        {
            memcpy(&mdns_c_median_ref_se_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x33b:
        {
            memcpy(&mdns_c_median_ref_ms_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x33c:
        {
            memcpy(&mdns_c_median_ref_me_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x33d:
        {
            memcpy(&mdns_c_piir_cur_fs_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x33e:
        {
            memcpy(&mdns_c_piir_ref_fs_wei_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x33f:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_144_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x340:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_160_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x341:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_176_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x342:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_192_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x343:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_208_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x344:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_224_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x345:
        {
            memcpy(&mdns_c_fspa_cur_fus_wei_240_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x346:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_144_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x347:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_160_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x348:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_176_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x349:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_192_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x34a:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_208_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x34b:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_224_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x34c:
        {
            memcpy(&mdns_c_fspa_ref_fus_wei_240_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x34d:
        {
            memcpy(&mdns_c_fiir_fus_wei0_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x34e:
        {
            memcpy(&mdns_c_fiir_fus_wei1_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x34f:
        {
            memcpy(&mdns_c_fiir_fus_wei2_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x350:
        {
            memcpy(&mdns_c_fiir_fus_wei3_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x351:
        {
            memcpy(&mdns_c_fiir_fus_wei4_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x352:
        {
            memcpy(&mdns_c_fiir_fus_wei5_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x353:
        {
            memcpy(&mdns_c_fiir_fus_wei6_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x354:
        {
            memcpy(&mdns_c_fiir_fus_wei7_wdr_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
        case 0x355:
        {
            memcpy(&mdns_c_fiir_fus_wei8_wdr_array, arg2, 0x24);
            tisp_mdns_all_reg_refresh(data_9a9d0);
            tisp_mdns_reg_trigger();
            $v0_1 = 0x24;
            break;
        }
        case 0x356:
        {
            memcpy(&mdns_c_sat_nml_stren_array, arg2, 0x24);
            $v0_1 = 0x24;
            break;
        }
    }
    
    *arg3 = $v0_1;
    return 0;
}

