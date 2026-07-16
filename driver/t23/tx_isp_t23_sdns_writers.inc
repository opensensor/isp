/* Generated from the checked-in T31 SDNS source reference. */
#define REGTRACE_T23_SDNS_WORD(base, index) \
    (*(const uint32_t *)((uintptr_t)(base) + (uintptr_t)(index) * 4U))

static noinline void regtrace_t23_sdns_grad_thres_opt_cfg(void) {
    system_reg_write(0x8800, (REGTRACE_T23_SDNS_WORD(sdns_aa_mv_det_opt, 1) << 4) | REGTRACE_T23_SDNS_WORD(sdns_aa_mv_det_opt, 0));
    system_reg_write(0x8804, (sdns_grad_zy_thres_intp << 16) | sdns_grad_zx_thres_intp);
    system_reg_write(0x8808, (sdns_std_thr2_intp << 16) | (sdns_std_thr1_intp << 4) | REGTRACE_T23_SDNS_WORD(sdns_aa_mv_det_opt, 2));
}

static noinline void regtrace_t23_sdns_h_mv_wei_opt_cfg(void) {
    system_reg_write(0x880c, REGTRACE_T23_SDNS_WORD(sdns_h_mv_wei_now, 0) | (REGTRACE_T23_SDNS_WORD(sdns_h_mv_wei_now, 1) << 4) |
                     (REGTRACE_T23_SDNS_WORD(sdns_h_mv_wei_now, 2) << 8) | (REGTRACE_T23_SDNS_WORD(sdns_h_mv_wei_now, 3) << 12));
}

static noinline void regtrace_t23_sdns_mv_seg_number_num_thres_cfg(void) {
    system_reg_write(0x8810, REGTRACE_T23_SDNS_WORD(sdns_aa_mv_det_opt, 3) | (REGTRACE_T23_SDNS_WORD(sdns_aa_mv_det_opt, 4) << 8));
    system_reg_write(0x8814, sdns_mv_num_thr_5x5_intp | (sdns_mv_num_thr_7x7_intp << 8) |
                     (sdns_mv_num_thr_9x9_intp << 16) | (sdns_mv_num_thr_11x11_intp << 24));
}

static noinline void regtrace_t23_sdns_g_det_val_div_cfg(void) {
    system_reg_write(0x8818, REGTRACE_T23_SDNS_WORD(sdns_aa_mv_det_opt, 5) | (REGTRACE_T23_SDNS_WORD(sdns_aa_mv_det_opt, 6) << 24));
}

static noinline void regtrace_t23_sdns_r_s_mv_cfg(void) {
    int i;
    for (i = 0; i < 7; i++)
        system_reg_write(0x881c + i * 4, REGTRACE_T23_SDNS_WORD(sdns_r_s, i * 2) | (REGTRACE_T23_SDNS_WORD(sdns_r_s, i * 2 + 1) << 16));
    system_reg_write(0x8838, REGTRACE_T23_SDNS_WORD(sdns_r_s, 14));
    for (i = 0; i < 7; i++)
        system_reg_write(0x883c + i * 4, REGTRACE_T23_SDNS_WORD(sdns_r_mv, i * 2) | (REGTRACE_T23_SDNS_WORD(sdns_r_mv, i * 2 + 1) << 16));
    system_reg_write(0x8858, REGTRACE_T23_SDNS_WORD(sdns_r_mv, 14));
}

static noinline void regtrace_t23_sdns_h_s_cfg(void) {
    system_reg_write(0x885c, sdns_h_s_1_intp | (sdns_h_s_2_intp << 8) |
                     (sdns_h_s_3_intp << 16) | (sdns_h_s_4_intp << 24));
    system_reg_write(0x8860, sdns_h_s_5_intp | (sdns_h_s_6_intp << 8) |
                     (sdns_h_s_7_intp << 16) | (sdns_h_s_8_intp << 24));
    system_reg_write(0x8864, sdns_h_s_9_intp | (sdns_h_s_10_intp << 8) |
                     (sdns_h_s_11_intp << 16) | (sdns_h_s_12_intp << 24));
    system_reg_write(0x8868, sdns_h_s_13_intp | (sdns_h_s_14_intp << 8) |
                     (sdns_h_s_15_intp << 16) | (sdns_h_s_16_intp << 24));
}

static noinline void regtrace_t23_sdns_h_mv_cfg(void) {
    system_reg_write(0x886c, sdns_h_mv_1_intp | (sdns_h_mv_2_intp << 8) |
                     (sdns_h_mv_3_intp << 16) | (sdns_h_mv_4_intp << 24));
    system_reg_write(0x8870, sdns_h_mv_5_intp | (sdns_h_mv_6_intp << 8) |
                     (sdns_h_mv_7_intp << 16) | (sdns_h_mv_8_intp << 24));
    system_reg_write(0x8874, sdns_h_mv_9_intp | (sdns_h_mv_10_intp << 8) |
                     (sdns_h_mv_11_intp << 16) | (sdns_h_mv_12_intp << 24));
    system_reg_write(0x8878, sdns_h_mv_13_intp | (sdns_h_mv_14_intp << 8) |
                     (sdns_h_mv_15_intp << 16) | (sdns_h_mv_16_intp << 24));
}

static noinline void regtrace_t23_sdns_dark_light_tt_opt_cfg(void) {
    system_reg_write(0x887c, sdns_dark_thres_intp | (sdns_light_thres_intp << 8) |
                     (sdns_h_val_max << 16) | (sdns_sharpen_tt_opt_intp << 24));
}

static noinline void regtrace_t23_sdns_d_s1_thres_cfg(void) {
    int i;
    for (i = 0; i < 7; i++)
        system_reg_write(0x8880 + i * 4, REGTRACE_T23_SDNS_WORD(sdns_d_s1_thr, i * 2) | (REGTRACE_T23_SDNS_WORD(sdns_d_s1_thr, i * 2 + 1) << 16));
    system_reg_write(0x889c, REGTRACE_T23_SDNS_WORD(sdns_d_s1_thr, 14));
}

static noinline void regtrace_t23_sdns_w_thres_cfg(void) {
    int i;
    for (i = 0; i < 8; i++)
        system_reg_write(0x88a0 + i * 4, REGTRACE_T23_SDNS_WORD(sdns_w_thr, i * 2) | (REGTRACE_T23_SDNS_WORD(sdns_w_thr, i * 2 + 1) << 8));
}

static noinline void regtrace_t23_sdns_hls_en_ave_filter_cfg(void) {
    system_reg_write(0x88c0, REGTRACE_T23_SDNS_WORD(sdns_ave_fliter_now, 0) | (REGTRACE_T23_SDNS_WORD(sdns_ave_fliter_now, 1) << 1) |
                     (sdns_ave_thres_intp << 4) | (REGTRACE_T23_SDNS_WORD(sdns_ave_fliter_now, 2) << 16));
}

static noinline void regtrace_t23_sdns_gaussian_y_cfg(void) {
    system_reg_write(0x88c4, 0x2999a);
    system_reg_write(0x88c8, 0x1999a);
    system_reg_write(0x88cc, 0x999a);
}

static noinline void regtrace_t23_sdns_gaussian_x_cfg(void) {
    static const uint32_t lut[32] = {
        0x000f0007, 0x0033001e, 0x003b001c, 0x00ce0079,
        0x0084003e, 0x01d00111, 0x00eb006e, 0x033801e5,
        0x016f00ad, 0x050802f7, 0x021000f8, 0x073e0445,
        0x02ce0152, 0x09db05d0, 0x03aa01b9, 0x0ce00797,
        0x04a3022e, 0x104c099b, 0x05ba02b1, 0x141e0bdb,
        0x04960342, 0x18580e59, 0x083f03e1, 0x1cf91113,
        0x09ae048d, 0x22001409, 0x0b3b0547, 0x276f173e,
        0x0ce3047f, 0x2d441aad, 0x0ea906e5, 0x33801e5b
    };
    int i;
    for (i = 0; i < 32; i++)
        system_reg_write(0x88d0 + i * 4, lut[i]);
}

static noinline void regtrace_t23_sdns_gaussian_k_cfg(void) {
    static const uint32_t lut[64] = {
        0x3333, 0x2000, 0x1111, 0x055d, 0x0ccd, 0x0842, 0x0421, 0x0153,
        0x05c8, 0x03a8, 0x01d1, 0x0097, 0x0342, 0x020c, 0x0106, 0x0055,
        0x0212, 0x0152, 0x00a7, 0x0037, 0x0172, 0x00ea, 0x0074, 0x0026,
        0x010f, 0x00ac, 0x0055, 0x001c, 0x00d0, 0x0084, 0x0041, 0x0015,
        0x00a4, 0x0068, 0x0034, 0x0011, 0x0085, 0x0054, 0x002a, 0x000e,
        0x006e, 0x0046, 0x0023, 0x000b, 0x005c, 0x003b, 0x001d, 0x0009,
        0x004f, 0x0032, 0x0019, 0x0008, 0x0044, 0x002b, 0x0015, 0x0007,
        0x003b, 0x0025, 0x0013, 0x0006, 0x0034, 0x0021, 0x0010, 0x0005
    };
    int i;
    for (i = 0; i < 64; i++)
        system_reg_write(0x8950 + i * 4, lut[i]);
}

static noinline void regtrace_t23_sdns_h_line_cfg(void) {
    system_reg_write(0x8a50, 0x04030201);
    system_reg_write(0x8a54, 0x08070605);
    system_reg_write(0x8a58, 0x0c0b0a09);
    system_reg_write(0x8a5c, 0x100f0e0d);
}

static noinline void regtrace_t23_sdns_sp_std_en_seg_opt_cfg(void) {
    system_reg_write(0x8a60, (REGTRACE_T23_SDNS_WORD(sdns_sharpen_g_std, 1) << 4) | REGTRACE_T23_SDNS_WORD(sdns_sharpen_g_std, 0));
}

static noinline void regtrace_t23_sdns_sp_uu_cfg(void) {
    system_reg_write(0x8a64, REGTRACE_T23_SDNS_WORD(sdns_sp_uu_par, 0) | (sdns_sp_uu_thres_intp << 8));
    system_reg_write(0x8a68, sdns_sp_uu_stren_intp | (REGTRACE_T23_SDNS_WORD(sdns_sp_uu_par, 1) << 16) | (REGTRACE_T23_SDNS_WORD(sdns_sp_uu_par, 2) << 24));
    system_reg_write(0x8a6c, sdns_sp_mv_uu_thres_intp | (sdns_sp_mv_uu_stren_intp << 8));
    system_reg_write(0x8a70, REGTRACE_T23_SDNS_WORD(sdns_sp_mv_wei_uu_value, 0) | (REGTRACE_T23_SDNS_WORD(sdns_sp_mv_wei_uu_value, 1) << 4) |
                     (REGTRACE_T23_SDNS_WORD(sdns_sp_mv_wei_uu_value, 2) << 8) | (REGTRACE_T23_SDNS_WORD(sdns_sp_mv_wei_uu_value, 3) << 12));
}

static noinline void regtrace_t23_sdns_sp_v2_d_w_b_ll_hl_flat_cfg(void) {
    system_reg_write(0x8a74, REGTRACE_T23_SDNS_WORD(sdns_sp_d_v2_sigma_win5_slope, 0) | (sdns_sp_d_v2_win5_thres_intp << 8) |
                     (REGTRACE_T23_SDNS_WORD(sdns_sp_d_v2_sigma_win5_slope, 1) << 16) | (REGTRACE_T23_SDNS_WORD(sdns_sp_d_wbhl_flat, 0) << 24));
    system_reg_write(0x8a78, sdns_sp_d_w_sp_stren_0_intp | (sdns_sp_d_w_sp_stren_1_intp << 16));
    system_reg_write(0x8a7c, sdns_sp_d_w_sp_stren_2_intp | (sdns_sp_d_w_sp_stren_3_intp << 16));
    system_reg_write(0x8a80, REGTRACE_T23_SDNS_WORD(sdns_sp_d_wbhl_flat, 1) | (REGTRACE_T23_SDNS_WORD(sdns_sp_d_wbhl_flat, 2) << 16));
    system_reg_write(0x8a84, sdns_sp_d_b_sp_stren_0_intp | (sdns_sp_d_b_sp_stren_1_intp << 16));
    system_reg_write(0x8a88, sdns_sp_d_b_sp_stren_2_intp | (sdns_sp_d_b_sp_stren_3_intp << 16));
    system_reg_write(0x8a8c, REGTRACE_T23_SDNS_WORD(sdns_sp_d_wbhl_flat, 3) | (REGTRACE_T23_SDNS_WORD(sdns_sp_d_wbhl_flat, 4) << 16));
    system_reg_write(0x8a90, REGTRACE_T23_SDNS_WORD(sdns_sp_d_wbhl_flat, 5) | (REGTRACE_T23_SDNS_WORD(sdns_sp_d_wbhl_flat, 6) << 16));
    system_reg_write(0x8a94, REGTRACE_T23_SDNS_WORD(sdns_sp_d_wbhl_flat, 7) | (sdns_sp_d_flat_thres_intp << 16));
    system_reg_write(0x8a98, REGTRACE_T23_SDNS_WORD(sdns_sp_d_wbhl_flat, 8) | (sdns_sp_d_flat_stren_intp << 16));
}

static noinline void regtrace_t23_sdns_sp_ud_v2_v1_coef_w_wei_opt_cfg(void) {
    system_reg_write(0x8a9c, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_v2_1_coef, 0) | (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_v2_1_coef, 1) << 4) |
                     (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_v2_1_coef, 2) << 8) | (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_v2_1_coef, 3) << 12) |
                     (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_v2_1_coef, 4) << 16));
    system_reg_write(0x8aa0, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_v2_1_coef, 5) | (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_v2_1_coef, 6) << 8) |
                     (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_v2_1_coef, 7) << 12) | (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 0) << 16));
}

static noinline void regtrace_t23_sdns_sp_ud_w_stren_cfg(void) {
    system_reg_write(0x8aa4, sdns_sp_ud_w_sp_stren_0_intp | (sdns_sp_ud_w_sp_stren_1_intp << 16));
    system_reg_write(0x8aa8, sdns_sp_ud_w_sp_stren_2_intp | (sdns_sp_ud_w_sp_stren_3_intp << 16));
}

static noinline void regtrace_t23_sdns_sp_ud_w_limit_b_wei_opt_cfg(void) {
    system_reg_write(0x8aac, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 1) | (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 2) << 16));
}

static noinline void regtrace_t23_sdns_sp_ud_b_stren_cfg(void) {
    system_reg_write(0x8ab0, sdns_sp_ud_b_sp_stren_0_intp | (sdns_sp_ud_b_sp_stren_1_intp << 16));
    system_reg_write(0x8ab4, sdns_sp_ud_b_sp_stren_2_intp | (sdns_sp_ud_b_sp_stren_3_intp << 16));
}

static noinline void regtrace_t23_sdns_sp_ud_b_limit_srd_ll_hl_flat_cfg(void) {
    system_reg_write(0x8ab8, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 3) | (sdns_sp_ud_std_thres_intp << 16) |
                     (sdns_sp_ud_std_stren_intp << 24));
    system_reg_write(0x8abc, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 4) | (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 5) << 8) |
                     (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 6) << 16));
    system_reg_write(0x8ac0, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 7) | (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 8) << 8) |
                     (sdns_sp_ud_flat_thres_intp << 20));
    system_reg_write(0x8ac4, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 9) | (sdns_sp_ud_flat_stren_intp << 16));
}

static noinline void regtrace_t23_sdns_sp_ud_stren_shift_opt_cfg(void) {
    system_reg_write(0x8ac8, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_wbhl_flat, 10));
}

static noinline void regtrace_t23_sdns_sp_uu_np_array_cfg(void) {
    int i;
    for (i = 0; i < 4; i++)
        system_reg_write(0x8acc + i * 4, REGTRACE_T23_SDNS_WORD(sdns_sp_uu_np_array, i * 4) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_uu_np_array, i * 4 + 1) << 8) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_uu_np_array, i * 4 + 2) << 16) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_uu_np_array, i * 4 + 3) << 24));
}

static noinline void regtrace_t23_sdns_sp_d_w_wei_np_array_cfg(void) {
    int i;
    for (i = 0; i < 5; i++)
        system_reg_write(0x8adc + i * 4, REGTRACE_T23_SDNS_WORD(sdns_sp_d_w_wei_np_array, i * 4) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_d_w_wei_np_array, i * 4 + 1) << 8) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_d_w_wei_np_array, i * 4 + 2) << 16) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_d_w_wei_np_array, i * 4 + 3) << 24));
    system_reg_write(0x8af0, REGTRACE_T23_SDNS_WORD(sdns_sp_d_w_wei_np_array, 20) | (REGTRACE_T23_SDNS_WORD(sdns_sp_d_w_wei_np_array, 21) << 8));
}

static noinline void regtrace_t23_sdns_sp_d_b_wei_np_array_cfg(void) {
    int i;
    for (i = 0; i < 5; i++)
        system_reg_write(0x8af4 + i * 4, REGTRACE_T23_SDNS_WORD(sdns_sp_d_b_wei_np_array, i * 4) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_d_b_wei_np_array, i * 4 + 1) << 8) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_d_b_wei_np_array, i * 4 + 2) << 16) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_d_b_wei_np_array, i * 4 + 3) << 24));
    system_reg_write(0x8b08, REGTRACE_T23_SDNS_WORD(sdns_sp_d_b_wei_np_array, 20) | (REGTRACE_T23_SDNS_WORD(sdns_sp_d_b_wei_np_array, 21) << 8));
}

static noinline void regtrace_t23_sdns_sp_ud_w_wei_np_array_cfg(void) {
    int i;
    for (i = 0; i < 5; i++)
        system_reg_write(0x8b0c + i * 4, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_w_wei_np_array, i * 4) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_w_wei_np_array, i * 4 + 1) << 8) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_w_wei_np_array, i * 4 + 2) << 16) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_w_wei_np_array, i * 4 + 3) << 24));
    system_reg_write(0x8b20, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_w_wei_np_array, 20) | (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_w_wei_np_array, 21) << 8));
}

static noinline void regtrace_t23_sdns_sp_ud_b_wei_np_array_cfg(void) {
    int i;
    for (i = 0; i < 5; i++)
        system_reg_write(0x8b24 + i * 4, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_b_wei_np_array, i * 4) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_b_wei_np_array, i * 4 + 1) << 8) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_b_wei_np_array, i * 4 + 2) << 16) |
                         (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_b_wei_np_array, i * 4 + 3) << 24));
    system_reg_write(0x8b38, REGTRACE_T23_SDNS_WORD(sdns_sp_ud_b_wei_np_array, 20) | (REGTRACE_T23_SDNS_WORD(sdns_sp_ud_b_wei_np_array, 21) << 8));
}

#undef REGTRACE_T23_SDNS_WORD
