#include "include/main.h"


  int32_t tiziano_ae_para_addr()

{
    IspAe0WmeanParam = &IspAeStatic;
    data_d4658 = &data_d0878;
    data_d465c = &data_d0bfc;
    data_d4660 = &data_d0f80;
    data_d4664 = &data_d1304;
    data_d4668 = &data_d1688;
    data_d466c = &data_d1a0c;
    data_d4670 = 0xd37a0;
    data_d4674 = 0xd3b24;
    data_d4678 = &_ae_parameter;
    data_d467c = &_ae_zone_weight;
    data_d4680 = &_exp_parameter;
    data_d468c = &_scene_roi_weight;
    data_d4690 = &_log2_lut;
    data_d4694 = &_weight_lut;
    data_d4698 = &_AePointPos;
    data_d4684 = &_ae_stat;
    data_d4688 = &_scene_roui_weight;
    dmsc_sp_ud_std_stren_intp = &_exp_parameter;
    dmsc_deir_fusion_stren_intp = &_ae_result;
    dmsc_deir_fusion_thres_intp = &_ae_reg;
    dmsc_fc_t2_stren_intp = &_ae_wm_q;
    dmsc_fc_t1_stren_intp = &_deflick_lut;
    dmsc_fc_t1_thres_intp = &_deflicker_para;
    dmsc_fc_alias_stren_intp = &ae_ev_step;
    dmsc_sp_alias_thres_intp = &ae_stable_tol;
    dmsc_sp_ud_brig_thres_intp = &data_d1e0c;
    dmsc_sp_ud_b_stren_intp = &_nodes_num;
    dmsc_sp_d_dark_thres_intp = &ae_comp_ev_list;
    dmsc_sp_d_oe_stren_intp = &_ae_parameter;
    dmsc_fc_t3_stren_intp = &_ae_stat;
    dmsc_sp_ud_dark_thres_intp = &_AePointPos;
    dmsc_sp_d_brig_thres_intp = U"KA7-(";
    dmsc_sp_d_w_stren_intp = &ae1_comp_ev_list;
    void* $t4_1;
    
    if (data_b0e10)
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
    dmsc_nor_alias_thres_intp = &data_d220c;
    dmsc_hvaa_stren_intp = &data_d2590;
    dmsc_hvaa_thres_1_intp = &data_d2914;
    dmsc_aa_thres_1_intp = &data_d2c98;
    dmsc_hv_stren_intp = &data_d301c;
    dmsc_hv_thres_1_intp = &data_d33a0;
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

