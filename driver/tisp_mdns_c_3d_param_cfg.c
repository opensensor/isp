#include "include/main.h"


  int32_t tisp_mdns_c_3d_param_cfg()

{
    uint32_t mdns_c_edge_wei_adj_value5_intp_1 = mdns_c_edge_wei_adj_value5_intp;
    uint32_t mdns_c_edge_thr_adj_value5_intp_1 = mdns_c_edge_thr_adj_value5_intp;
    uint32_t mdns_c_luma_wei_adj_value5_intp_1 = mdns_c_luma_wei_adj_value5_intp;
    uint32_t mdns_c_luma_thr_adj_value5_intp_1 = mdns_c_luma_thr_adj_value5_intp;
    uint32_t mdns_c_dtb_wei_adj_value5_intp_1 = mdns_c_dtb_wei_adj_value5_intp;
    uint32_t mdns_c_dtb_thr_adj_value5_intp_1 = mdns_c_dtb_thr_adj_value5_intp;
    uint32_t mdns_c_ass_wei_adj_value5_intp_1 = mdns_c_ass_wei_adj_value5_intp;
    uint32_t mdns_c_ass_thr_adj_value5_intp_1 = mdns_c_ass_thr_adj_value5_intp;
    uint32_t mdns_c_sad_wei_adj_value5_intp_1 = mdns_c_sad_wei_adj_value5_intp;
    system_reg_write(0x7a64, 
        mdns_c_sad_ave_thres_intp << 3 | mdns_c_sad_ave_slope_intp << 0xc | mdns_c_sad_win_opt_intp
            | mdns_c_sad_ass_thres_intp << 0x10 | mdns_c_sad_dtb_thres_intp << 0x18);
    system_reg_write(0x7a68, mdns_c_ref_wei_fake_intp << 8 | mdns_c_ref_wei_mv_intp << 0x10);
    system_reg_write(0x7a6c, 
        mdns_c_ref_wei_f_min_intp << 8 | mdns_c_ref_wei_b_max_intp << 0x10
            | mdns_c_ref_wei_f_max_intp | mdns_c_ref_wei_b_min_intp << 0x18);
    system_reg_write(0x7a70, 
        mdns_c_ref_wei_r_min_intp << 8 | mdns_c_ref_wei_increase_intp << 0x10
            | mdns_c_ref_wei_r_max_intp);
    system_reg_write(0x7a04, 
        mdns_c_corner_thr_adj_value_intp << 8 | mdns_c_corner_wei_adj_value_intp);
    system_reg_write(0x7a08, 
        mdns_c_edge_wei_adj_seg_intp << 8 | mdns_c_edge_thr_adj_seg_intp << 0xa);
    system_reg_write(0x7a0c, 
        mdns_c_edge_wei_adj_value1_intp << 8 | mdns_c_edge_wei_adj_value2_intp << 0x10
            | mdns_c_edge_wei_adj_value0_intp | mdns_c_edge_wei_adj_value3_intp << 0x18);
    system_reg_write(0x7a10, 
        mdns_c_edge_wei_adj_value5_intp_1 << 8 | mdns_c_edge_wei_adj_value5_intp_1 << 0x10
            | mdns_c_edge_wei_adj_value4_intp | mdns_c_edge_wei_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7a14, 
        mdns_c_edge_thr_adj_value1_intp << 8 | mdns_c_edge_thr_adj_value2_intp << 0x10
            | mdns_c_edge_thr_adj_value0_intp | mdns_c_edge_thr_adj_value3_intp << 0x18);
    system_reg_write(0x7a18, 
        mdns_c_edge_thr_adj_value5_intp_1 << 8 | mdns_c_edge_thr_adj_value5_intp_1 << 0x10
            | mdns_c_edge_thr_adj_value4_intp | mdns_c_edge_thr_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7a1c, 
        mdns_c_luma_wei_adj_seg_intp << 8 | mdns_c_luma_thr_adj_seg_intp << 0xa);
    system_reg_write(0x7a20, 
        mdns_c_luma_wei_adj_value1_intp << 8 | mdns_c_luma_wei_adj_value2_intp << 0x10
            | mdns_c_luma_wei_adj_value0_intp | mdns_c_luma_wei_adj_value3_intp << 0x18);
    system_reg_write(0x7a24, 
        mdns_c_luma_wei_adj_value5_intp_1 << 8 | mdns_c_luma_wei_adj_value5_intp_1 << 0x10
            | mdns_c_luma_wei_adj_value4_intp | mdns_c_luma_wei_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7a28, 
        mdns_c_luma_thr_adj_value1_intp << 8 | mdns_c_luma_thr_adj_value2_intp << 0x10
            | mdns_c_luma_thr_adj_value0_intp | mdns_c_luma_thr_adj_value3_intp << 0x18);
    system_reg_write(0x7a2c, 
        mdns_c_luma_thr_adj_value5_intp_1 << 8 | mdns_c_luma_thr_adj_value5_intp_1 << 0x10
            | mdns_c_luma_thr_adj_value4_intp | mdns_c_luma_thr_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7a30, mdns_c_dtb_wei_adj_seg_intp << 8 | mdns_c_dtb_thr_adj_seg_intp << 0xa);
    system_reg_write(0x7a34, 
        mdns_c_dtb_wei_adj_value1_intp << 8 | mdns_c_dtb_wei_adj_value2_intp << 0x10
            | mdns_c_dtb_wei_adj_value0_intp | mdns_c_dtb_wei_adj_value3_intp << 0x18);
    system_reg_write(0x7a38, 
        mdns_c_dtb_wei_adj_value5_intp_1 << 8 | mdns_c_dtb_wei_adj_value5_intp_1 << 0x10
            | mdns_c_dtb_wei_adj_value4_intp | mdns_c_dtb_wei_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7a3c, 
        mdns_c_dtb_thr_adj_value1_intp << 8 | mdns_c_dtb_thr_adj_value2_intp << 0x10
            | mdns_c_dtb_thr_adj_value0_intp | mdns_c_dtb_thr_adj_value3_intp << 0x18);
    system_reg_write(0x7a40, 
        mdns_c_dtb_thr_adj_value5_intp_1 << 8 | mdns_c_dtb_thr_adj_value5_intp_1 << 0x10
            | mdns_c_dtb_thr_adj_value4_intp | mdns_c_dtb_thr_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7a44, mdns_c_ass_wei_adj_seg_intp << 8 | mdns_c_ass_thr_adj_seg_intp << 0xa);
    system_reg_write(0x7a48, 
        mdns_c_ass_wei_adj_value1_intp << 8 | mdns_c_ass_wei_adj_value2_intp << 0x10
            | mdns_c_ass_wei_adj_value0_intp | mdns_c_ass_wei_adj_value3_intp << 0x18);
    system_reg_write(0x7a4c, 
        mdns_c_ass_wei_adj_value5_intp_1 << 8 | mdns_c_ass_wei_adj_value5_intp_1 << 0x10
            | mdns_c_ass_wei_adj_value4_intp | mdns_c_ass_wei_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7a50, 
        mdns_c_ass_thr_adj_value1_intp << 8 | mdns_c_ass_thr_adj_value2_intp << 0x10
            | mdns_c_ass_thr_adj_value0_intp | mdns_c_ass_thr_adj_value3_intp << 0x18);
    system_reg_write(0x7a54, 
        mdns_c_ass_thr_adj_value5_intp_1 << 8 | mdns_c_ass_thr_adj_value5_intp_1 << 0x10
            | mdns_c_ass_thr_adj_value4_intp | mdns_c_ass_thr_adj_value5_intp_1 << 0x18);
    system_reg_write(0x7a58, mdns_c_sad_wei_adj_seg_intp);
    system_reg_write(0x7a5c, 
        mdns_c_sad_wei_adj_value1_intp << 8 | mdns_c_sad_wei_adj_value2_intp << 0x10
            | mdns_c_sad_wei_adj_value0_intp | mdns_c_sad_wei_adj_value3_intp << 0x18);
    system_reg_write(0x7a60, 
        mdns_c_sad_wei_adj_value5_intp_1 << 8 | mdns_c_sad_wei_adj_value5_intp_1 << 0x10
            | mdns_c_sad_wei_adj_value4_intp | mdns_c_sad_wei_adj_value5_intp_1 << 0x18);
    return 0;
}

