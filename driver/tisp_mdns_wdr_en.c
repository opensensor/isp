#include "include/main.h"


  int32_t tisp_mdns_wdr_en(uint32_t arg1)

{
    char* var_30 = (char*)(&data_b0000); // Fixed void pointer assignment
    void* $v0_1;
    mdns_wdr_en = arg1;
    
    if (arg1)
    {
        mdns_y_sad_ave_thres_array_now = &mdns_y_sad_ave_thres_wdr_array;
        mdns_y_sad_ass_thres_array_now = &mdns_y_sad_ass_thres_wdr_array;
        mdns_y_sta_ave_thres_array_now = &mdns_y_sta_ave_thres_wdr_array;
        mdns_y_sta_ass_thres_array_now = &mdns_y_sta_ass_thres_wdr_array;
        mdns_y_sta_motion_thres_array_now = &mdns_y_sta_motion_thres_wdr_array;
        mdns_y_ref_wei_b_max_array_now = &mdns_y_ref_wei_b_max_wdr_array;
        mdns_y_ref_wei_b_min_array_now = &mdns_y_ref_wei_b_min_wdr_array;
        mdns_y_pspa_cur_median_win_opt_array_now = &mdns_y_pspa_cur_median_win_opt_wdr_array;
        mdns_y_pspa_cur_bi_thres_array_now = &mdns_y_pspa_cur_bi_thres_wdr_array;
        mdns_y_pspa_cur_bi_wei0_array_now = &mdns_y_pspa_cur_bi_wei0_wdr_array;
        mdns_y_pspa_ref_median_win_opt_array_now = &mdns_y_pspa_ref_median_win_opt_wdr_array;
        mdns_y_pspa_ref_bi_thres_array_now = &mdns_y_pspa_ref_bi_thres_wdr_array;
        mdns_y_pspa_ref_bi_wei0_array_now = &mdns_y_pspa_ref_bi_wei0_wdr_array;
        mdns_y_piir_cur_fs_wei_array_now = &mdns_y_piir_cur_fs_wei_wdr_array;
        mdns_y_piir_ref_fs_wei_array_now = &mdns_y_piir_ref_fs_wei_wdr_array;
        mdns_y_fspa_cur_fus_wei_144_array_now = &mdns_y_fspa_cur_fus_wei_144_wdr_array;
        mdns_y_fspa_cur_fus_wei_160_array_now = &mdns_y_fspa_cur_fus_wei_160_wdr_array;
        mdns_y_fspa_cur_fus_wei_176_array_now = &mdns_y_fspa_cur_fus_wei_176_wdr_array;
        mdns_y_fspa_cur_fus_wei_192_array_now = &mdns_y_fspa_cur_fus_wei_192_wdr_array;
        mdns_y_fspa_cur_fus_wei_208_array_now = &mdns_y_fspa_cur_fus_wei_208_wdr_array;
        mdns_y_fspa_cur_fus_wei_224_array_now = &mdns_y_fspa_cur_fus_wei_224_wdr_array;
        mdns_y_fspa_cur_fus_wei_240_array_now = &mdns_y_fspa_cur_fus_wei_240_wdr_array;
        mdns_y_fspa_ref_fus_wei_144_array_now = &mdns_y_fspa_ref_fus_wei_144_wdr_array;
        mdns_y_fspa_ref_fus_wei_160_array_now = &mdns_y_fspa_ref_fus_wei_160_wdr_array;
        mdns_y_fspa_ref_fus_wei_176_array_now = &mdns_y_fspa_ref_fus_wei_176_wdr_array;
        mdns_y_fspa_ref_fus_wei_192_array_now = &mdns_y_fspa_ref_fus_wei_192_wdr_array;
        mdns_y_fspa_ref_fus_wei_208_array_now = &mdns_y_fspa_ref_fus_wei_208_wdr_array;
        mdns_y_fspa_ref_fus_wei_224_array_now = &mdns_y_fspa_ref_fus_wei_224_wdr_array;
        mdns_y_fspa_ref_fus_wei_240_array_now = &mdns_y_fspa_ref_fus_wei_240_wdr_array;
        mdns_y_fiir_fus_wei0_array_now = &mdns_y_fiir_fus_wei0_wdr_array;
        mdns_y_fiir_fus_wei1_array_now = &mdns_y_fiir_fus_wei1_wdr_array;
        mdns_y_fiir_fus_wei2_array_now = &mdns_y_fiir_fus_wei2_wdr_array;
        mdns_y_fiir_fus_wei3_array_now = &mdns_y_fiir_fus_wei3_wdr_array;
        mdns_y_fiir_fus_wei4_array_now = &mdns_y_fiir_fus_wei4_wdr_array;
        mdns_y_fiir_fus_wei5_array_now = &mdns_y_fiir_fus_wei5_wdr_array;
        mdns_y_fiir_fus_wei6_array_now = &mdns_y_fiir_fus_wei6_wdr_array;
        mdns_y_fiir_fus_wei7_array_now = &mdns_y_fiir_fus_wei7_wdr_array;
        mdns_y_fiir_fus_wei8_array_now = &mdns_y_fiir_fus_wei8_wdr_array;
        mdns_c_sad_ave_thres_array_now = &mdns_c_sad_ave_thres_wdr_array;
        mdns_c_sad_ass_thres_array_now = &mdns_c_sad_ass_thres_wdr_array;
        mdns_c_ref_wei_b_max_array_now = &mdns_c_ref_wei_b_max_wdr_array;
        mdns_c_ref_wei_b_min_array_now = &mdns_c_ref_wei_b_min_wdr_array;
        mdns_c_median_cur_ss_wei_array_now = &mdns_c_median_cur_ss_wei_wdr_array;
        mdns_c_median_cur_se_wei_array_now = &mdns_c_median_cur_se_wei_wdr_array;
        mdns_c_median_cur_ms_wei_array_now = &mdns_c_median_cur_ms_wei_wdr_array;
        mdns_c_median_cur_me_wei_array_now = &mdns_c_median_cur_me_wei_wdr_array;
        mdns_c_median_ref_ss_wei_array_now = &mdns_c_median_ref_ss_wei_wdr_array;
        mdns_c_median_ref_se_wei_array_now = &mdns_c_median_ref_se_wei_wdr_array;
        mdns_c_median_ref_ms_wei_array_now = &mdns_c_median_ref_ms_wei_wdr_array;
        mdns_c_median_ref_me_wei_array_now = &mdns_c_median_ref_me_wei_wdr_array;
        mdns_c_piir_cur_fs_wei_array_now = &mdns_c_piir_cur_fs_wei_wdr_array;
        mdns_c_piir_ref_fs_wei_array_now = &mdns_c_piir_ref_fs_wei_wdr_array;
        mdns_c_fspa_cur_fus_wei_144_array_now = &mdns_c_fspa_cur_fus_wei_144_wdr_array;
        mdns_c_fspa_cur_fus_wei_160_array_now = &mdns_c_fspa_cur_fus_wei_160_wdr_array;
        mdns_c_fspa_cur_fus_wei_176_array_now = &mdns_c_fspa_cur_fus_wei_176_wdr_array;
        mdns_c_fspa_cur_fus_wei_192_array_now = &mdns_c_fspa_cur_fus_wei_192_wdr_array;
        mdns_c_fspa_cur_fus_wei_208_array_now = &mdns_c_fspa_cur_fus_wei_208_wdr_array;
        mdns_c_fspa_cur_fus_wei_224_array_now = &mdns_c_fspa_cur_fus_wei_224_wdr_array;
        mdns_c_fspa_cur_fus_wei_240_array_now = &mdns_c_fspa_cur_fus_wei_240_wdr_array;
        mdns_c_fspa_ref_fus_wei_144_array_now = &mdns_c_fspa_ref_fus_wei_144_wdr_array;
        mdns_c_fspa_ref_fus_wei_160_array_now = &mdns_c_fspa_ref_fus_wei_160_wdr_array;
        mdns_c_fspa_ref_fus_wei_176_array_now = &mdns_c_fspa_ref_fus_wei_176_wdr_array;
        mdns_c_fspa_ref_fus_wei_192_array_now = &mdns_c_fspa_ref_fus_wei_192_wdr_array;
        mdns_c_fspa_ref_fus_wei_208_array_now = &mdns_c_fspa_ref_fus_wei_208_wdr_array;
        mdns_c_fspa_ref_fus_wei_224_array_now = &mdns_c_fspa_ref_fus_wei_224_wdr_array;
        mdns_c_fspa_ref_fus_wei_240_array_now = &mdns_c_fspa_ref_fus_wei_240_wdr_array;
        mdns_c_fiir_fus_wei0_array_now = &mdns_c_fiir_fus_wei0_wdr_array;
        mdns_c_fiir_fus_wei1_array_now = &mdns_c_fiir_fus_wei1_wdr_array;
        mdns_c_fiir_fus_wei2_array_now = &mdns_c_fiir_fus_wei2_wdr_array;
        mdns_c_fiir_fus_wei3_array_now = &mdns_c_fiir_fus_wei3_wdr_array;
        mdns_c_fiir_fus_wei4_array_now = &mdns_c_fiir_fus_wei4_wdr_array;
        mdns_c_fiir_fus_wei5_array_now = &mdns_c_fiir_fus_wei5_wdr_array;
        mdns_c_fiir_fus_wei6_array_now = &mdns_c_fiir_fus_wei6_wdr_array;
        mdns_c_fiir_fus_wei7_array_now = &mdns_c_fiir_fus_wei7_wdr_array;
        $v0_1 = &mdns_c_fiir_fus_wei8_wdr_array;
    }
    else
    {
        mdns_y_sad_ave_thres_array_now = &mdns_y_sad_ave_thres_array;
        mdns_y_sad_ass_thres_array_now = &mdns_y_sad_ass_thres_array;
        mdns_y_sta_ave_thres_array_now = &mdns_y_sta_ave_thres_array;
        mdns_y_sta_ass_thres_array_now = &mdns_y_sta_ass_thres_array;
        mdns_y_sta_motion_thres_array_now = &mdns_y_sta_motion_thres_array;
        mdns_y_ref_wei_b_max_array_now = &mdns_y_ref_wei_b_max_array;
        mdns_y_ref_wei_b_min_array_now = &mdns_y_ref_wei_b_min_array;
        mdns_y_pspa_cur_median_win_opt_array_now = &mdns_y_pspa_cur_median_win_opt_array;
        mdns_y_pspa_cur_bi_thres_array_now = &mdns_y_pspa_cur_bi_thres_array;
        mdns_y_pspa_cur_bi_wei0_array_now = &mdns_y_pspa_cur_bi_wei0_array;
        mdns_y_pspa_ref_median_win_opt_array_now = &mdns_y_pspa_ref_median_win_opt_array;
        mdns_y_pspa_ref_bi_thres_array_now = &mdns_y_pspa_ref_bi_thres_array;
        mdns_y_pspa_ref_bi_wei0_array_now = &mdns_y_pspa_ref_bi_wei0_array;
        mdns_y_piir_cur_fs_wei_array_now = &mdns_y_piir_cur_fs_wei_array;
        mdns_y_piir_ref_fs_wei_array_now = &mdns_y_piir_ref_fs_wei_array;
        mdns_y_fspa_cur_fus_wei_144_array_now = &mdns_y_fspa_cur_fus_wei_144_array;
        mdns_y_fspa_cur_fus_wei_160_array_now = &mdns_y_fspa_cur_fus_wei_160_array;
        mdns_y_fspa_cur_fus_wei_176_array_now = &mdns_y_fspa_cur_fus_wei_176_array;
        mdns_y_fspa_cur_fus_wei_192_array_now = &mdns_y_fspa_cur_fus_wei_192_array;
        mdns_y_fspa_cur_fus_wei_208_array_now = &mdns_y_fspa_cur_fus_wei_208_array;
        mdns_y_fspa_cur_fus_wei_224_array_now = &mdns_y_fspa_cur_fus_wei_224_array;
        mdns_y_fspa_cur_fus_wei_240_array_now = &mdns_y_fspa_cur_fus_wei_240_array;
        mdns_y_fspa_ref_fus_wei_144_array_now = &mdns_y_fspa_ref_fus_wei_144_array;
        mdns_y_fspa_ref_fus_wei_160_array_now = &mdns_y_fspa_ref_fus_wei_160_array;
        mdns_y_fspa_ref_fus_wei_176_array_now = &mdns_y_fspa_ref_fus_wei_176_array;
        mdns_y_fspa_ref_fus_wei_192_array_now = &mdns_y_fspa_ref_fus_wei_192_array;
        mdns_y_fspa_ref_fus_wei_208_array_now = &mdns_y_fspa_ref_fus_wei_208_array;
        mdns_y_fspa_ref_fus_wei_224_array_now = &mdns_y_fspa_ref_fus_wei_224_array;
        mdns_y_fspa_ref_fus_wei_240_array_now = &mdns_y_fspa_ref_fus_wei_240_array;
        mdns_y_fiir_fus_wei0_array_now = &mdns_y_fiir_fus_wei0_array;
        mdns_y_fiir_fus_wei1_array_now = &mdns_y_fiir_fus_wei1_array;
        mdns_y_fiir_fus_wei2_array_now = &mdns_y_fiir_fus_wei2_array;
        mdns_y_fiir_fus_wei3_array_now = &mdns_y_fiir_fus_wei3_array;
        mdns_y_fiir_fus_wei4_array_now = &mdns_y_fiir_fus_wei4_array;
        mdns_y_fiir_fus_wei5_array_now = &mdns_y_fiir_fus_wei5_array;
        mdns_y_fiir_fus_wei6_array_now = &mdns_y_fiir_fus_wei6_array;
        mdns_y_fiir_fus_wei7_array_now = &mdns_y_fiir_fus_wei7_array;
        mdns_y_fiir_fus_wei8_array_now = &mdns_y_fiir_fus_wei8_array;
        mdns_c_sad_ave_thres_array_now = &mdns_c_sad_ave_thres_array;
        mdns_c_sad_ass_thres_array_now = &mdns_c_sad_ass_thres_array;
        mdns_c_ref_wei_b_max_array_now = &mdns_c_ref_wei_b_max_array;
        mdns_c_ref_wei_b_min_array_now = &mdns_c_ref_wei_b_min_array;
        mdns_c_median_cur_ss_wei_array_now = &mdns_c_median_cur_ss_wei_array;
        mdns_c_median_cur_se_wei_array_now = &mdns_c_median_cur_se_wei_array;
        mdns_c_median_cur_ms_wei_array_now = &mdns_c_median_cur_ms_wei_array;
        mdns_c_median_cur_me_wei_array_now = &mdns_c_median_cur_me_wei_array;
        mdns_c_median_ref_ss_wei_array_now = &mdns_c_median_ref_ss_wei_array;
        mdns_c_median_ref_se_wei_array_now = &mdns_c_median_ref_se_wei_array;
        mdns_c_median_ref_ms_wei_array_now = &mdns_c_median_ref_ms_wei_array;
        mdns_c_median_ref_me_wei_array_now = &mdns_c_median_ref_me_wei_array;
        mdns_c_piir_cur_fs_wei_array_now = &mdns_c_piir_cur_fs_wei_array;
        mdns_c_piir_ref_fs_wei_array_now = &mdns_c_piir_ref_fs_wei_array;
        mdns_c_fspa_cur_fus_wei_144_array_now = &mdns_c_fspa_cur_fus_wei_144_array;
        mdns_c_fspa_cur_fus_wei_160_array_now = &mdns_c_fspa_cur_fus_wei_160_array;
        mdns_c_fspa_cur_fus_wei_176_array_now = &mdns_c_fspa_cur_fus_wei_176_array;
        mdns_c_fspa_cur_fus_wei_192_array_now = &mdns_c_fspa_cur_fus_wei_192_array;
        mdns_c_fspa_cur_fus_wei_208_array_now = &mdns_c_fspa_cur_fus_wei_208_array;
        mdns_c_fspa_cur_fus_wei_224_array_now = &mdns_c_fspa_cur_fus_wei_224_array;
        mdns_c_fspa_cur_fus_wei_240_array_now = &mdns_c_fspa_cur_fus_wei_240_array;
        mdns_c_fspa_ref_fus_wei_144_array_now = &mdns_c_fspa_ref_fus_wei_144_array;
        mdns_c_fspa_ref_fus_wei_160_array_now = &mdns_c_fspa_ref_fus_wei_160_array;
        mdns_c_fspa_ref_fus_wei_176_array_now = &mdns_c_fspa_ref_fus_wei_176_array;
        mdns_c_fspa_ref_fus_wei_192_array_now = &mdns_c_fspa_ref_fus_wei_192_array;
        mdns_c_fspa_ref_fus_wei_208_array_now = &mdns_c_fspa_ref_fus_wei_208_array;
        mdns_c_fspa_ref_fus_wei_224_array_now = &mdns_c_fspa_ref_fus_wei_224_array;
        mdns_c_fspa_ref_fus_wei_240_array_now = &mdns_c_fspa_ref_fus_wei_240_array;
        mdns_c_fiir_fus_wei0_array_now = &mdns_c_fiir_fus_wei0_array;
        mdns_c_fiir_fus_wei1_array_now = &mdns_c_fiir_fus_wei1_array;
        mdns_c_fiir_fus_wei2_array_now = &mdns_c_fiir_fus_wei2_array;
        mdns_c_fiir_fus_wei3_array_now = &mdns_c_fiir_fus_wei3_array;
        mdns_c_fiir_fus_wei4_array_now = &mdns_c_fiir_fus_wei4_array;
        mdns_c_fiir_fus_wei5_array_now = &mdns_c_fiir_fus_wei5_array;
        mdns_c_fiir_fus_wei6_array_now = &mdns_c_fiir_fus_wei6_array;
        mdns_c_fiir_fus_wei7_array_now = &mdns_c_fiir_fus_wei7_array;
        $v0_1 = &mdns_c_fiir_fus_wei8_array;
    }
    
    mdns_c_fiir_fus_wei8_array_now = $v0_1;
    /* tailcall */
    return tisp_s_mdns_ratio(data_9ab00_1);
}

