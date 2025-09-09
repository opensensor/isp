#include "include/main.h"


  int32_t tisp_mdns_y_2d_param_cfg()

{
    uint32_t mdns_y_pspa_cur_bi_wei0_intp_1 = mdns_y_pspa_cur_bi_wei0_intp;
    uint32_t mdns_y_pspa_ref_bi_wei0_intp_1 = mdns_y_pspa_ref_bi_wei0_intp;
    uint32_t mdns_y_fspa_cur_fus_wei_0_intp_1 = mdns_y_fspa_cur_fus_wei_0_intp;
    uint32_t mdns_y_fspa_cur_fus_wei_0_intp_2 = mdns_y_fspa_cur_fus_wei_0_intp;
    uint32_t mdns_y_fspa_cur_fus_wei_0_intp_3 = mdns_y_fspa_cur_fus_wei_0_intp;
    uint32_t mdns_y_fspa_ref_fus_wei_0_intp_1 = mdns_y_fspa_ref_fus_wei_0_intp;
    uint32_t mdns_y_fspa_ref_fus_wei_0_intp_2 = mdns_y_fspa_ref_fus_wei_0_intp;
    uint32_t mdns_y_fspa_ref_fus_wei_0_intp_3 = mdns_y_fspa_ref_fus_wei_0_intp;
    uint32_t mdns_y_fiir_fus_wei0_intp_1 = mdns_y_fiir_fus_wei0_intp;
    uint32_t $s1_2 = mdns_y_fiir_fus_wei0_intp_1 << 8 | mdns_y_fiir_fus_wei0_intp_1 << 0x10
    system_reg_write(0x79a4, 
        mdns_y_pspa_cur_bi_wei_seg_intp << 8 | mdns_y_pspa_cur_lmt_op_en_intp << 0xa
            | mdns_y_pspa_cur_bi_thres_intp);
    system_reg_write(0x79a8, 
        mdns_y_pspa_cur_bi_wei0_intp_1 << 4 | mdns_y_pspa_cur_bi_wei0_intp_1 << 8
            | mdns_y_pspa_cur_bi_wei0_intp_1 | mdns_y_pspa_cur_bi_wei0_intp_1 << 0xc
            | mdns_y_pspa_cur_bi_wei0_intp_1 << 0x10 | mdns_y_pspa_cur_bi_wei0_intp_1 << 0x14);
    system_reg_write(0x79ac, 
        mdns_y_pspa_ref_bi_wei_seg_intp << 8 | mdns_y_pspa_ref_lmt_op_en_intp << 0xa
            | mdns_y_pspa_ref_bi_thres_intp);
    system_reg_write(0x79b0, 
        mdns_y_pspa_ref_bi_wei0_intp_1 << 4 | mdns_y_pspa_ref_bi_wei0_intp_1 << 8
            | mdns_y_pspa_ref_bi_wei0_intp_1 | mdns_y_pspa_ref_bi_wei0_intp_1 << 0xc
            | mdns_y_pspa_ref_bi_wei0_intp_1 << 0x10 | mdns_y_pspa_ref_bi_wei0_intp_1 << 0x14);
    system_reg_write(0x79b4, 
        mdns_y_pspa_ref_median_win_opt_intp << 1 | mdns_y_fspa_cur_fus_seg_intp << 4
            | mdns_y_pspa_cur_median_win_opt_intp | mdns_y_fspa_ref_fus_seg_intp << 8
            | mdns_y_pspa_fnl_fus_thres_intp << 0x10 | mdns_y_pspa_fnl_fus_dwei_intp << 0x18
            | mdns_y_pspa_fnl_fus_swei_intp << 0x1c);
    system_reg_write(0x79b8, 
        mdns_y_fspa_cur_fus_wei_0_intp_1 << 8 | mdns_y_fspa_cur_fus_wei_0_intp_1 << 0x10
            | mdns_y_fspa_cur_fus_wei_0_intp_1 | mdns_y_fspa_cur_fus_wei_0_intp_1 << 0x18);
    system_reg_write(0x79bc, 
        mdns_y_fspa_cur_fus_wei_0_intp_2 << 8 | mdns_y_fspa_cur_fus_wei_0_intp_2 << 0x10
            | mdns_y_fspa_cur_fus_wei_0_intp_2 | mdns_y_fspa_cur_fus_wei_0_intp_2 << 0x18);
    system_reg_write(0x79c0, 
        mdns_y_fspa_cur_fus_wei_0_intp_3 << 8 | mdns_y_fspa_cur_fus_wei_0_intp_3 << 0x10
            | mdns_y_fspa_cur_fus_wei_0_intp_3 | mdns_y_fspa_cur_fus_wei_144_intp << 0x18);
    system_reg_write(0x79c4, 
        mdns_y_fspa_cur_fus_wei_176_intp << 8 | mdns_y_fspa_cur_fus_wei_192_intp << 0x10
            | mdns_y_fspa_cur_fus_wei_160_intp | mdns_y_fspa_cur_fus_wei_208_intp << 0x18);
    system_reg_write(0x79c8, 
        mdns_y_fspa_ref_fus_wei_0_intp_1 << 8 | mdns_y_fspa_ref_fus_wei_0_intp_1 << 0x10
            | mdns_y_fspa_ref_fus_wei_0_intp_1 | mdns_y_fspa_ref_fus_wei_0_intp_1 << 0x18);
    system_reg_write(0x79cc, 
        mdns_y_fspa_ref_fus_wei_0_intp_2 << 8 | mdns_y_fspa_ref_fus_wei_0_intp_2 << 0x10
            | mdns_y_fspa_ref_fus_wei_0_intp_2 | mdns_y_fspa_ref_fus_wei_0_intp_2 << 0x18);
    system_reg_write(0x79d0, 
        mdns_y_fspa_ref_fus_wei_0_intp_3 << 8 | mdns_y_fspa_ref_fus_wei_0_intp_3 << 0x10
            | mdns_y_fspa_ref_fus_wei_0_intp_3 | mdns_y_fspa_ref_fus_wei_144_intp << 0x18);
    system_reg_write(0x79d4, 
        mdns_y_fspa_ref_fus_wei_176_intp << 8 | mdns_y_fspa_ref_fus_wei_192_intp << 0x10
            | mdns_y_fspa_ref_fus_wei_160_intp | mdns_y_fspa_ref_fus_wei_208_intp << 0x18);
    system_reg_write(0x79d8, 
        mdns_y_piir_edge_thres1_intp << 8 | mdns_y_piir_edge_thres2_intp << 0x10
            | mdns_y_piir_edge_thres0_intp);
    system_reg_write(0x79dc, 
        mdns_y_piir_edge_wei1_intp << 8 | mdns_y_piir_edge_wei2_intp << 0x10
            | mdns_y_piir_edge_wei0_intp | mdns_y_piir_edge_wei3_intp << 0x18);
    system_reg_write(0x79e0, mdns_y_piir_ref_fs_wei_intp << 8 | mdns_y_piir_cur_fs_wei_intp);
    system_reg_write(0x79e4, 
        mdns_y_piir_edge_thres1_intp << 8 | mdns_y_piir_edge_thres2_intp << 0x10
            | mdns_y_piir_edge_thres0_intp);
    system_reg_write(0x79e8, 
        mdns_y_piir_edge_wei1_intp << 8 | mdns_y_piir_edge_wei2_intp << 0x10
            | mdns_y_piir_edge_wei0_intp | mdns_y_piir_edge_wei3_intp << 0x18);
    system_reg_write(0x79ec, mdns_y_fiir_fus_seg_intp);
        | mdns_y_fiir_fus_wei0_intp_1 | mdns_y_fiir_fus_wei0_intp_1 << 0x18;
    system_reg_write(0x79f0, $s1_2);
    system_reg_write(0x79f4, $s1_2);
    system_reg_write(0x79f8, 
        mdns_y_fiir_fus_wei2_intp << 8 | mdns_y_fiir_fus_wei3_intp << 0x10
            | mdns_y_fiir_fus_wei1_intp | mdns_y_fiir_fus_wei4_intp << 0x18);
    system_reg_write(0x79fc, 
        mdns_y_fiir_fus_wei6_intp << 8 | mdns_y_fiir_fus_wei7_intp << 0x10
            | mdns_y_fiir_fus_wei5_intp | mdns_y_fiir_fus_wei8_intp << 0x18);
    system_reg_write(0x7a00, mdns_y_con_stren_intp << 8);
    return 0;
}

