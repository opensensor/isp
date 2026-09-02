/* T23 defog payload within the selected IQ profile. */
#define REGTRACE_T23_DEFOG_TUNING_OFFSET 0x13728U
#define REGTRACE_T23_DEFOG_TUNING_SIZE   0x854U
#define REGTRACE_T23_DEFOG_TUNING_FIELDS(X) \
    X(param_defog_weightlut20, 0x000U, 0x80U) \
    X(param_defog_weightlut02, 0x080U, 0x80U) \
    X(param_defog_weightlut12, 0x100U, 0x80U) \
    X(param_defog_weightlut22, 0x180U, 0x80U) \
    X(param_defog_weightlut21, 0x200U, 0x80U) \
    X(param_defog_col_ct_array, 0x280U, 0x38U) \
    X(param_defog_cent3_w_dis_array, 0x2b8U, 0x60U) \
    X(param_defog_cent5_w_dis_array, 0x318U, 0x7cU) \
    X(defog_ev_list, 0x394U, 0x24U) \
    X(defog_trsy0_list, 0x3b8U, 0x24U) \
    X(defog_trsy1_list, 0x3dcU, 0x24U) \
    X(defog_trsy2_list, 0x400U, 0x24U) \
    X(defog_trsy3_list, 0x424U, 0x24U) \
    X(defog_trsy4_list, 0x448U, 0x24U) \
    X(defog_rgbra_list, 0x46cU, 0x24U) \
    X(param_defog_main_para_array, 0x490U, 0x2cU) \
    X(param_defog_color_control_array, 0x4bcU, 0x38U) \
    X(param_defog_lc_s_array, 0x4f4U, 0x28U) \
    X(param_defog_lc_v_array, 0x51cU, 0x28U) \
    X(param_defog_cc_s_array, 0x544U, 0x20U) \
    X(param_defog_cc_v_array, 0x564U, 0x24U) \
    X(param_defog_dark_l1_array, 0x588U, 0x28U) \
    X(param_defog_dark_l2_array, 0x5b0U, 0x28U) \
    X(param_defog_block_t_x_array, 0x5d8U, 0x14U) \
    X(param_defog_block_t_y_array, 0x5ecU, 0x14U) \
    X(defog_t_par_list1, 0x600U, 0x2cU) \
    X(defog_t_par_list2, 0x62cU, 0x74U) \
    X(defog_manual_ctrl, 0x6a0U, 0x1cU) \
    X(defog_ev_list_wdr, 0x6bcU, 0x24U) \
    X(defog_trsy0_list_wdr, 0x6e0U, 0x24U) \
    X(defog_trsy1_list_wdr, 0x704U, 0x24U) \
    X(defog_trsy2_list_wdr, 0x728U, 0x24U) \
    X(defog_trsy3_list_wdr, 0x74cU, 0x24U) \
    X(defog_trsy4_list_wdr, 0x770U, 0x24U) \
    X(param_defog_main_para_wdr_array, 0x794U, 0x2cU) \
    X(param_defog_block_t_x_wdr_array, 0x7c0U, 0x14U) \
    X(param_defog_fpga_para_wdr_array, 0x7d4U, 0x40U) \
    X(param_defog_fpga_para_array, 0x814U, 0x40U)
