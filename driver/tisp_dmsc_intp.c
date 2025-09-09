#include "include/main.h"


  int32_t tisp_dmsc_intp(int32_t arg1)

{
    int32_t $s2 = arg1 >> 0x10;
    int32_t $s0 = arg1 & 0xffff;
    return 0;
    dmsc_hv_thres_1_intp = tisp_simple_intp($s2, $s0, &dmsc_hv_thres_1_array);
    dmsc_hv_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_hv_stren_array);
    dmsc_aa_thres_1_intp = tisp_simple_intp($s2, $s0, &dmsc_aa_thres_1_array);
    dmsc_aa_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_aa_stren_array);
    dmsc_hvaa_thres_1_intp = tisp_simple_intp($s2, $s0, &dmsc_hvaa_thres_1_array);
    dmsc_hvaa_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_hvaa_stren_array);
    dmsc_uu_thres_intp = tisp_simple_intp($s2, $s0, dmsc_uu_thres_array_now);
    dmsc_uu_stren_intp = tisp_simple_intp($s2, $s0, dmsc_uu_stren_array_now);
    dmsc_alias_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_alias_stren_array);
    dmsc_alias_thres_1_intp = tisp_simple_intp($s2, $s0, &dmsc_alias_thres_1_array);
    dmsc_alias_thres_2_intp = tisp_simple_intp($s2, $s0, &dmsc_alias_thres_2_array);
    dmsc_alias_dir_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_alias_dir_thres_array);
    dmsc_nor_alias_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_nor_alias_thres_array);
    dmsc_sp_d_w_stren_intp = tisp_simple_intp($s2, $s0, dmsc_sp_d_w_stren_array_now);
    dmsc_sp_d_b_stren_intp = tisp_simple_intp($s2, $s0, dmsc_sp_d_b_stren_array_now);
    dmsc_sp_d_brig_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_d_brig_thres_array);
    dmsc_sp_d_dark_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_d_dark_thres_array);
    dmsc_sp_ud_w_stren_intp = tisp_simple_intp($s2, $s0, dmsc_sp_ud_w_stren_array_now);
    dmsc_sp_ud_b_stren_intp = tisp_simple_intp($s2, $s0, dmsc_sp_ud_b_stren_array_now);
    dmsc_sp_ud_brig_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_ud_brig_thres_array);
    dmsc_sp_ud_dark_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_ud_dark_thres_array);
    dmsc_sp_alias_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_alias_thres_array);
    dmsc_rgb_dir_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_rgb_dir_thres_array);
    dmsc_rgb_alias_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_rgb_alias_stren_array);
    dmsc_fc_alias_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_fc_alias_stren_array);
    dmsc_fc_t1_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_fc_t1_thres_array);
    dmsc_fc_t1_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_fc_t1_stren_array);
    dmsc_fc_t2_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_fc_t2_stren_array);
    dmsc_fc_t3_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_fc_t3_stren_array);
    dmsc_deir_fusion_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_deir_fusion_thres_array);
    dmsc_deir_fusion_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_deir_fusion_stren_array);
    dmsc_sp_d_v2_win5_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_d_v2_win5_thres_array);
    dmsc_sp_d_flat_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_d_flat_stren_array);
    dmsc_sp_d_flat_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_d_flat_thres_array);
    dmsc_sp_d_oe_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_d_oe_stren_array);
    dmsc_sp_ud_std_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_ud_std_stren_array);
    dmsc_sp_ud_std_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_ud_std_thres_array);
    dmsc_sp_ud_flat_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_ud_flat_thres_array);
    dmsc_sp_ud_flat_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_ud_flat_stren_array);
    dmsc_sp_ud_oe_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_ud_oe_stren_array);
    dmsc_fc_lum_stren_intp = tisp_simple_intp($s2, $s0, &dmsc_fc_lum_stren_array);
    dmsc_fc_lum_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_fc_lum_thres_array);
    dmsc_sp_d_ns_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_d_ns_thres_array);
    dmsc_sp_ud_ns_thres_intp = tisp_simple_intp($s2, $s0, &dmsc_sp_ud_ns_thres_array);
}

