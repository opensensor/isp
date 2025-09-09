#include "include/main.h"


  int32_t tiziano_ae_para_addr()

{
    IspAe0WmeanParam = &IspAeStatic;
    data_d4658_1 = &data_d0878_1;
    data_d465c_1 = &data_d0bfc_1;
    data_d4660_1 = &data_d0f80_1;
    data_d4664_1 = &data_d1304_1;
    data_d4668_1 = &data_d1688_1;
    data_d466c_1 = &data_d1a0c_6;
    data_d4670_2 = 0xd37a0;
    data_d4674_1 = 0xd3b24;
    data_d4678_1 = &_ae_parameter;
    data_d467c_1 = &_ae_zone_weight;
    data_d4680_1 = &_exp_parameter;
    data_d468c_1 = &_scene_roi_weight;
    data_d4690_1 = &_log2_lut;
    data_d4694_1 = &_weight_lut;
    data_d4698_1 = &_AePointPos;
    data_d4684_1 = &_ae_stat;
    data_d4688_1 = &_scene_roui_weight;
    dmsc_sp_ud_std_stren_intp = &_exp_parameter;
    dmsc_deir_fusion_stren_intp = &_ae_result;
    dmsc_deir_fusion_thres_intp = &_ae_reg;
    dmsc_fc_t2_stren_intp = &_ae_wm_q;
    dmsc_fc_t1_stren_intp = &_deflick_lut;
    dmsc_fc_t1_thres_intp = &_deflicker_para;
    dmsc_fc_alias_stren_intp = &ae_ev_step;
    dmsc_sp_alias_thres_intp = &ae_stable_tol;
    dmsc_sp_ud_brig_thres_intp = &data_d1e0c_1;
    dmsc_sp_ud_b_stren_intp = &_nodes_num;
    dmsc_sp_d_dark_thres_intp = &ae_comp_ev_list;
    dmsc_sp_d_oe_stren_intp = &_ae_parameter;
    dmsc_fc_t3_stren_intp = &_ae_stat;
    dmsc_sp_ud_dark_thres_intp = &_AePointPos;
    dmsc_sp_d_brig_thres_intp = U"KA7-(";
    dmsc_sp_d_w_stren_intp = &ae1_comp_ev_list;
    void* $t4_1;
    
    if (data_b0e10_4)
    {
        dmsc_sp_d_flat_thres_intp = &ae0_ev_list_wdr;
        dmsc_sp_d_flat_stren_intp = &_lum_list_wdr;
        dmsc_sp_d_v2_win5_thres_intp = U"KA7-(";
        dmsc_rgb_alias_stren_intp = &_scene_para_wdr;
        dmsc_rgb_dir_thres_intp = &ae_scene_mode_th_wdr;
        dmsc_sp_ud_w_stren_intp = &ae_comp_param_wdr;
        $t4_1 = &ae_extra_at_list_wdr;
    }
    else
    {
        dmsc_sp_d_flat_thres_intp = &ae0_ev_list;
        dmsc_sp_d_flat_stren_intp = &_lum_list;
        dmsc_sp_d_v2_win5_thres_intp = U"KA7-(";
        dmsc_rgb_alias_stren_intp = &_scene_para;
        dmsc_rgb_dir_thres_intp = &ae_scene_mode_th;
        dmsc_sp_ud_w_stren_intp = &ae_comp_param;
        $t4_1 = &ae_extra_at_list;
    }
    
    dmsc_sp_d_b_stren_intp = $t4_1;
    dmsc_nor_alias_thres_intp = &data_d220c_2;
    dmsc_hvaa_stren_intp = &data_d2590_1;
    dmsc_hvaa_thres_1_intp = &data_d2914_1;
    dmsc_aa_thres_1_intp = &data_d2c98_1;
    dmsc_hv_stren_intp = &data_d301c_1;
    dmsc_hv_thres_1_intp = &data_d33a0_1;
    dmsc_alias_thres_2_intp = 0xd3ea8;
    dmsc_alias_thres_1_intp = 0xd422c;
    dmsc_alias_stren_intp = &_ae_parameter;
    dmsc_alias_dir_thres_intp = &_ae_zone_weight;
    dmsc_uu_stren_intp = &_exp_parameter;
    dmsc_uu_thres_intp = &_ae_stat;
    dmsc_sp_ud_b_stren_wdr_array = &_scene_roui_weight;
    data_c4644_1 = &_scene_roi_weight;
    data_c4648_1 = &_log2_lut;
    data_c464c_1 = &_weight_lut;
    data_c4650_1 = &_AePointPos;
    return &dmsc_nor_alias_thres_intp;
}

