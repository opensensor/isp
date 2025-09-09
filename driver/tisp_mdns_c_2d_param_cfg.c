#include "include/main.h"


  int32_t tisp_mdns_c_2d_param_cfg()

{
    system_reg_write(0x7a74, 
        mdns_c_median_edg_thres_intp << 8 | mdns_c_median_cur_lmt_op_en_intp << 0x10
            | mdns_c_median_smj_thres_intp | mdns_c_median_cur_lmt_wei_intp << 0x11
            | mdns_c_median_ref_lmt_op_en_intp << 0x18 | mdns_c_median_ref_lmt_wei_intp << 0x19);
    system_reg_write(0x7a78, 
        mdns_c_median_cur_se_wei_intp << 4 | mdns_c_median_cur_ms_wei_intp << 8
            | mdns_c_median_cur_ss_wei_intp | mdns_c_median_cur_me_wei_intp << 0xc);
    system_reg_write(0x7a7c, 
        mdns_c_median_ref_se_wei_intp << 4 | mdns_c_median_ref_ms_wei_intp << 8
            | mdns_c_median_ref_ss_wei_intp | mdns_c_median_ref_me_wei_intp << 0xc);
    system_reg_write(0x7a80, 
        mdns_c_bgm_cur_src_intp << 4 | mdns_c_bgm_ref_src_intp << 6 | mdns_bgm_inter_en_array
            | mdns_c_bgm_false_thres_intp << 8 | mdns_c_bgm_false_step_intp << 0x10
            | mdns_c_bgm_win_opt_intp << 0x14);
    int32_t $lo = data_9ab08_2 / 2;
    int32_t $a1_16 = 0;
    
    if ($lo & 0xf)
        $a1_16 = $lo % 0x10 / 2;
    
    int32_t $lo_2 = data_9ab04_2 / 2;
    int32_t $a0_5 = 0;
    
    if ($lo_2 & 0xf)
        $a0_5 = $lo_2 % 0x10 / 2;
    
    system_reg_write(0x7a84, $a0_5 << 0x10 | $a1_16);
    system_reg_write(0x7a88, ($lo_2 - ($a0_5 << 1)) << 0x10 | ($lo - ($a1_16 << 1)));
    system_reg_write(0x7a8c, mdns_c_fspa_cur_fus_seg_intp << 4 | mdns_c_fspa_ref_fus_seg_intp << 8);
    uint32_t mdns_c_fspa_cur_fus_wei_0_intp_1 = mdns_c_fspa_cur_fus_wei_0_intp;
    system_reg_write(0x7a90, 
        mdns_c_fspa_cur_fus_wei_0_intp_1 << 8 | mdns_c_fspa_cur_fus_wei_0_intp_1 << 0x10
            | mdns_c_fspa_cur_fus_wei_0_intp_1 | mdns_c_fspa_cur_fus_wei_0_intp_1 << 0x18);
    uint32_t mdns_c_fspa_cur_fus_wei_0_intp_2 = mdns_c_fspa_cur_fus_wei_0_intp;
    system_reg_write(0x7a94, 
        mdns_c_fspa_cur_fus_wei_0_intp_2 << 8 | mdns_c_fspa_cur_fus_wei_0_intp_2 << 0x10
            | mdns_c_fspa_cur_fus_wei_0_intp_2 | mdns_c_fspa_cur_fus_wei_0_intp_2 << 0x18);
    uint32_t mdns_c_fspa_cur_fus_wei_0_intp_3 = mdns_c_fspa_cur_fus_wei_0_intp;
    system_reg_write(0x7a98, 
        mdns_c_fspa_cur_fus_wei_0_intp_3 << 8 | mdns_c_fspa_cur_fus_wei_0_intp_3 << 0x10
            | mdns_c_fspa_cur_fus_wei_0_intp_3 | mdns_c_fspa_cur_fus_wei_144_intp << 0x18);
    system_reg_write(0x7a9c, 
        mdns_c_fspa_cur_fus_wei_176_intp << 8 | mdns_c_fspa_cur_fus_wei_192_intp << 0x10
            | mdns_c_fspa_cur_fus_wei_160_intp | mdns_c_fspa_cur_fus_wei_208_intp << 0x18);
    uint32_t mdns_c_fspa_ref_fus_wei_0_intp_1 = mdns_c_fspa_ref_fus_wei_0_intp;
    system_reg_write(0x7aa0, 
        mdns_c_fspa_ref_fus_wei_0_intp_1 << 8 | mdns_c_fspa_ref_fus_wei_0_intp_1 << 0x10
            | mdns_c_fspa_ref_fus_wei_0_intp_1 | mdns_c_fspa_ref_fus_wei_0_intp_1 << 0x18);
    uint32_t mdns_c_fspa_ref_fus_wei_0_intp_2 = mdns_c_fspa_ref_fus_wei_0_intp;
    system_reg_write(0x7aa4, 
        mdns_c_fspa_ref_fus_wei_0_intp_2 << 8 | mdns_c_fspa_ref_fus_wei_0_intp_2 << 0x10
            | mdns_c_fspa_ref_fus_wei_0_intp_2 | mdns_c_fspa_ref_fus_wei_0_intp_2 << 0x18);
    uint32_t mdns_c_fspa_ref_fus_wei_0_intp_3 = mdns_c_fspa_ref_fus_wei_0_intp;
    system_reg_write(0x7aa8, 
        mdns_c_fspa_ref_fus_wei_0_intp_3 << 8 | mdns_c_fspa_ref_fus_wei_0_intp_3 << 0x10
            | mdns_c_fspa_ref_fus_wei_0_intp_3 | mdns_c_fspa_ref_fus_wei_144_intp << 0x18);
    system_reg_write(0x7aac, 
        mdns_c_fspa_ref_fus_wei_176_intp << 8 | mdns_c_fspa_ref_fus_wei_192_intp << 0x10
            | mdns_c_fspa_ref_fus_wei_160_intp | mdns_c_fspa_ref_fus_wei_208_intp << 0x18);
    system_reg_write(0x7ab0, 
        mdns_c_piir_edge_thres1_intp << 8 | mdns_c_piir_edge_thres2_intp << 0x10
            | mdns_c_piir_edge_thres0_intp);
    system_reg_write(0x7ab4, 
        mdns_c_piir_edge_wei1_intp << 8 | mdns_c_piir_edge_wei2_intp << 0x10
            | mdns_c_piir_edge_wei0_intp | mdns_c_piir_edge_wei3_intp << 0x18);
    system_reg_write(0x7ab8, mdns_c_piir_ref_fs_wei_intp << 8 | mdns_c_piir_cur_fs_wei_intp);
    system_reg_write(0x7abc, 
        mdns_c_piir_edge_thres1_intp << 8 | mdns_c_piir_edge_thres2_intp << 0x10
            | mdns_c_piir_edge_thres0_intp);
    system_reg_write(0x7ac0, 
        mdns_c_piir_edge_wei1_intp << 8 | mdns_c_piir_edge_wei2_intp << 0x10
            | mdns_c_piir_edge_wei0_intp | mdns_c_piir_edge_wei3_intp << 0x18);
    system_reg_write(0x7ac4, mdns_c_fiir_fus_seg_intp);
    uint32_t mdns_c_fiir_fus_wei0_intp_1 = mdns_c_fiir_fus_wei0_intp;
    uint32_t $s1_4 = mdns_c_fiir_fus_wei0_intp_1 << 8 | mdns_c_fiir_fus_wei0_intp_1 << 0x10
        | mdns_c_fiir_fus_wei0_intp_1 | mdns_c_fiir_fus_wei0_intp_1 << 0x18;
    system_reg_write(0x7ac8, $s1_4);
    system_reg_write(0x7acc, $s1_4);
    system_reg_write(0x7ad0, 
        mdns_c_fiir_fus_wei2_intp << 8 | mdns_c_fiir_fus_wei3_intp << 0x10
            | mdns_c_fiir_fus_wei1_intp | mdns_c_fiir_fus_wei4_intp << 0x18);
    system_reg_write(0x7ad4, 
        mdns_c_fiir_fus_wei6_intp << 8 | mdns_c_fiir_fus_wei7_intp << 0x10
            | mdns_c_fiir_fus_wei5_intp | mdns_c_fiir_fus_wei8_intp << 0x18);
    system_reg_write(0x7ad8, 
        mdns_c_false_edg_thres0_intp << 8 | mdns_c_false_edg_thres1_intp << 0x10
            | mdns_c_false_smj_thres_intp | mdns_c_false_edg_thres2_intp << 0x18);
    uint32_t mdns_c_false_step_s0_intp_1 = mdns_c_false_step_s0_intp;
    uint32_t mdns_c_false_thres_s0_intp_1 = mdns_c_false_thres_s0_intp;
    system_reg_write(0x7adc, 
        mdns_c_false_step_s0_intp_1 << 8 | mdns_c_false_thres_s0_intp_1 << 0x10
            | mdns_c_false_thres_s0_intp_1 | mdns_c_false_step_s0_intp_1 << 0x18);
    uint32_t mdns_c_false_step_s0_intp_2 = mdns_c_false_step_s0_intp;
    uint32_t mdns_c_false_thres_s0_intp_2 = mdns_c_false_thres_s0_intp;
    system_reg_write(0x7ae0, 
        mdns_c_false_step_s0_intp_2 << 8 | mdns_c_false_thres_s0_intp_2 << 0x10
            | mdns_c_false_thres_s0_intp_2 | mdns_c_false_step_s0_intp_2 << 0x18);
    uint32_t mdns_c_false_step_m0_intp_1 = mdns_c_false_step_m0_intp;
    uint32_t mdns_c_false_thres_m0_intp_1 = mdns_c_false_thres_m0_intp;
    system_reg_write(0x7ae4, 
        mdns_c_false_step_m0_intp_1 << 8 | mdns_c_false_thres_m0_intp_1 << 0x10
            | mdns_c_false_thres_m0_intp_1 | mdns_c_false_step_m0_intp_1 << 0x18);
    uint32_t mdns_c_false_step_m0_intp_2 = mdns_c_false_step_m0_intp;
    uint32_t mdns_c_false_thres_m0_intp_2 = mdns_c_false_thres_m0_intp;
    system_reg_write(0x7ae8, 
        mdns_c_false_step_m0_intp_2 << 8 | mdns_c_false_thres_m0_intp_2 << 0x10
            | mdns_c_false_thres_m0_intp_2 | mdns_c_false_step_m0_intp_2 << 0x18);
    system_reg_write(0x7aec, 
        mdns_c_sat_lmt_thres_intp << 8 | mdns_c_sat_lmt_stren_intp << 0x10
            | mdns_c_sat_nml_stren_intp);
    return 0;
}

