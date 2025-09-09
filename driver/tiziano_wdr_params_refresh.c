#include "include/main.h"


  int32_t tiziano_wdr_params_refresh()

{
    void* $v0 = &var_40;
    int32_t* $v1 = &param_wdr_tool_control_array;
        int32_t i_1 = i;
    memcpy(&param_wdr_para_array, 0xa7424, 0x28);
    memcpy(&param_wdr_gam_y_array, 0xa76cc, 0x84);
    memcpy(&param_wdr_w_point_weight_x_array, 0xa7750, 0x10);
    memcpy(&param_wdr_w_point_weight_y_array, 0xa7760, 0x10);
    memcpy(&param_wdr_w_point_weight_pow_array, 0xa7770, 0xc);
    memcpy(U"#',17=DKS[clu~", U"#',17=DKS[clu~", 0x84);
    memcpy(&param_wdr_detail_th_w_array, 0xa7800, 0x1c);
    memcpy(&param_wdr_contrast_t_y_mux_array, 0xa781c, 0x14);
    memcpy(&param_wdr_ct_cl_para_array, 0xa7830, 0x10);
    memcpy(&param_wdr_stat_para_array, 0xa78bc, 0x1c);
    memcpy(&param_wdr_degost_para_array, 0xa78d8, 0x34);
    memcpy(&param_wdr_darkLable_array, 0xa790c, 0x14);
    memcpy(&param_wdr_darkLableN_array, 0xa7920, 0x10);
    memcpy(&param_wdr_darkWeight_array, 0xa7930, 0x14);
    memcpy(&param_wdr_thrLable_array, 0xa7944, 0x6c);
    memcpy(&param_computerModle_software_in_array, 0xa79b0, 0x10);
    memcpy(&param_deviationPara_software_in_array, 0xa79c0, 0x14);
    memcpy(&param_ratioPara_software_in_array, 0xa79d4, 0x1c);
    memcpy(&param_x_thr_software_in_array, 0xa79f0, 0x10);
    memcpy(&param_y_thr_software_in_array, 0xa7a00, 0x10);
    memcpy(&param_thrPara_software_in_array, 0xa7a10, 0x50);
    memcpy(&param_xy_pix_low_software_in_array, 0xa7a60, 0x58);
    memcpy(&param_motionThrPara_software_in_array, 0xa7ab8, 0x44);
    memcpy(&param_d_thr_normal_software_in_array, 0xa7afc, 0x68);
    memcpy(&param_d_thr_normal1_software_in_array, 0xa7b64, 0x68);
    memcpy(&param_d_thr_normal2_software_in_array, 0xa7bcc, 0x68);
    memcpy(&param_d_thr_normal_min_software_in_array, 0xa7c34, 0x68);
    memcpy(&param_multiValueLow_software_in_array, &data_a7c9c, 0x68);
    memcpy(&param_multiValueHigh_software_in_array, U"$$@@", 0x68);
    memcpy(&param_d_thr_2_software_in_array, 0xa7d6c, 0x68);
    memcpy(&param_wdr_detial_para_software_in_array, &data_a7dd4, 0x20);
    memcpy(U"JRZx", 0xa7df4, 0x6c);
    memcpy(&param_wdr_dbg_out_array, 0xa7e60, 8);
    memcpy(&wdr_ev_list, 0xa7e68, 0x24);
    memcpy(&wdr_weight_b_in_list, 0xa7e8c, 0x24);
    memcpy(&wdr_weight_p_in_list, 0xa7eb0, 0x24);
    memcpy(&wdr_ev_list_deghost, 0xa7ed4, 0x24);
    memcpy(&wdr_weight_in_list_deghost, 0xa7ef8, 0x24);
    memcpy(&wdr_detail_w_in0_list, 0xa7f1c, 0x24);
    memcpy(&wdr_detail_w_in1_list, 0xa7f40, 0x24);
    memcpy(&wdr_detail_w_in4_list[0x12], U"         @@@@@@@@@@@@@@@@@@", 0x24);
    memcpy(&wdr_detail_w_in4_list[9], &data_a7f64[9], 0x24);
    memcpy(U"@@@@@@@@@@@@@@@@@@         ", &data_a7f64[0x12], 0x24);
    memcpy(&mdns_y_fspa_ref_fus_wei_224_wdr_array, &data_a7f64[0x1b], 0x40);
    void var_40;
    memcpy(&var_40, 0x98010, 0x38);
    
    for (int32_t i = 0; (uintptr_t)i != 0xe; )
    {
        i += 1;
        
        if (i_1 != 2)
            *$v1 = *$v0;
        
        $v0 += 4;
        $v1 = &$v1[1];
    }
    
    memcpy(&mdns_c_luma_wei_adj_value0_array, 0xa744c, 0x80);
    memcpy(&param_wdr_weightLUT02_array, 0xa74cc, 0x80);
    memcpy(&param_wdr_weightLUT12_array, 0xa754c, 0x80);
    memcpy(&param_wdr_weightLUT22_array, 0xa75cc, 0x80);
    memcpy(&param_wdr_weightLUT21_array, 0xa764c, 0x80);
    memcpy(&param_centre5x5_w_distance_array, 0xa7840, 0x7c);
    tiziano_wdr_gamma_refresh();
    return 0;
}

