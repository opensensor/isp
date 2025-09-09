#include "include/main.h"


  int32_t tiziano_adr_params_refresh()

{
    memcpy(&param_adr_para_array, U"\\n", 0x20);
    memcpy(&param_adr_ctc_kneepoint_array, 0xa5dec, 0x44);
    memcpy(&param_adr_min_kneepoint_array, 0xa5e30, 0x5c);
    memcpy(&param_adr_map_kneepoint_array, 0xa5e8c, 0x5c);
    memcpy(&param_adr_coc_kneepoint_y1_array, 0xa5ee8, 0x30);
    memcpy(&param_adr_coc_kneepoint_y2_array, 0xa5f18, 0x30);
    memcpy(&param_adr_coc_kneepoint_y3_array, 0xa5f48, 0x30);
    memcpy(&param_adr_coc_kneepoint_y4_array, 0xa5f78, 0x30);
    memcpy(&param_adr_coc_kneepoint_y5_array, 0xa5fa8, 0x30);
    memcpy(&param_adr_coc_adjust_array, 0xa5fd8, 0x38);
    memcpy(&param_adr_stat_block_hist_diff_array, 0xa608c, 0x10);
    memcpy(&adr_tm_base_lut, 0xa609c, 0x24);
    memcpy(&param_adr_gam_x_array, 0xa60c0, 0x102);
    memcpy(&param_adr_gam_y_array, 0xa61c2, 0x102);
    memcpy(&adr_ctc_map2cut_y, 0xa62c4, 0x24);
    memcpy(&adr_light_end, 0xa62e8, 0x74);
    memcpy(&adr_block_light, 0xa635c, 0x3c);
    memcpy(&adr_map_mode, 0xa6398, 0x2c);
    memcpy(&histSub_4096_diff, 0xa63c4, 0x20);
    memcpy(&adr_ev_list, 0xa641c, 0x24);
    memcpy(&adr_ligb_list, 0xa6440, 0x24);
    memcpy(&adr_mapb1_list, 0xa6464, 0x24);
    memcpy(&adr_mapb2_list, 0xa6488, 0x24);
    memcpy(&adr_mapb3_list, 0xa64ac, 0x24);
    memcpy(&adr_mapb4_list, 0xa64d0, 0x24);
    memcpy(&adr_blp2_list, 0xa66f0, 0x24);
    memcpy(&adr_ev_list_wdr, 0xa65f4, 0x24);
    memcpy(&adr_ligb_list_wdr, 0xa6618, 0x24);
    memcpy(&adr_mapb1_list_wdr, &data_a663c_1, 0x24);
    memcpy(&adr_mapb2_list_wdr, 0xa6660, 0x24);
    memcpy(&adr_mapb3_list_wdr, 0xa6684, 0x24);
    memcpy(&adr_mapb4_list_wdr, 0xa66a8, 0x24);
    memcpy(&adr_blp2_list_wdr, 0xa66cc, 0x24);
    memcpy(&adr_ctc_map2cut_y_wdr, 0xa64f4, 0x24);
    memcpy(&adr_light_end_wdr, 0xa6518, 0x74);
    memcpy(&adr_block_light_wdr, 0xa658c, 0x3c);
    memcpy(&adr_map_mode_wdr, 0xa65c8, 0x2c);
    void var_40_32;
    memcpy(&var_40_33, 0xa63e4, 0x38);
    void* $v0 = &var_40_34;
    int32_t* $a1 = &param_adr_tool_control_array;
    
    for (int32_t i = 0; i != 0xe; )
    {
        int32_t i_1 = i;
        i += 1;
        
        if (i_1 != 1)
            *$a1 = *$v0;
        
        $v0 += 4;
        $a1 = &$a1[1];
    }
    
    int32_t param_adr_tool_control_array_1 = param_adr_tool_control_array;
    void* $a1_1;
    
    if (param_adr_tool_control_array_1 != 1)
    {
        if (param_adr_tool_control_array_1)
        {
            isp_printf(2, "VIC_CTRL : %08x\\n", 0xe);
            return 0xffffffff;
        }
        
        memcpy(&param_adr_weight_20_lut_array, &param_adr_weight_20_lut_array_tmp, 0x80);
        memcpy(&param_adr_weight_02_lut_array, &param_adr_weight_02_lut_array_tmp, 0x80);
        memcpy(&param_adr_weight_12_lut_array, &param_adr_weight_12_lut_array_tmp, 0x80);
        memcpy(&param_adr_weight_22_lut_array, &param_adr_weight_22_lut_array_tmp, 0x80);
        memcpy(&param_adr_weight_21_lut_array, &param_adr_weight_21_lut_array_tmp, 0x80);
        $a1_1 = &param_adr_centre_w_dis_array_tmp;
    }
    else
    {
        memcpy(&param_adr_weight_20_lut_array, 0xa5b6c, 0x80);
        memcpy(&param_adr_weight_02_lut_array, 0xa5bec, 0x80);
        memcpy(&param_adr_weight_12_lut_array, 0xa5c6c, 0x80);
        memcpy(&param_adr_weight_22_lut_array, 0xa5cec, 0x80);
        memcpy(&param_adr_weight_21_lut_array, 0xa5d6c, 0x80);
        $a1_1 = &data_a6010;
    }
    
    memcpy(&param_adr_centre_w_dis_array, $a1_1, 0x7c);
    uint32_t adr_ratio_1 = adr_ratio;
    
    if (adr_ratio_1 != 0x80)
        tisp_s_adr_str_internal(adr_ratio_1);
    
    tiziano_adr_gamma_refresh();
    ev_changed = 1;
    return 0;
}

