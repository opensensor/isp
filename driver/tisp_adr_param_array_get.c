#include "include/main.h"


  int32_t tisp_adr_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0x380 >= 0x2c)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    int32_t* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x380:
        {
            $a1_1 = &param_adr_para_array;
            $s1_1 = 0x20;
            break;
        }
        case 0x381:
        {
            $a1_1 = &param_adr_weight_20_lut_array;
            $s1_1 = 0x80;
            break;
        }
        case 0x382:
        {
            $a1_1 = &param_adr_weight_02_lut_array;
            $s1_1 = 0x80;
            break;
        }
        case 0x383:
        {
            $a1_1 = &param_adr_weight_12_lut_array;
            $s1_1 = 0x80;
            break;
        }
        case 0x384:
        {
            $a1_1 = &param_adr_weight_22_lut_array;
            $s1_1 = 0x80;
            break;
        }
        case 0x385:
        {
            $a1_1 = &param_adr_weight_21_lut_array;
            $s1_1 = 0x80;
            break;
        }
        case 0x386:
        {
            $a1_1 = &param_adr_ctc_kneepoint_array;
            $s1_1 = 0x44;
            break;
        }
        case 0x387:
        {
            $a1_1 = &param_adr_min_kneepoint_array;
            $s1_1 = 0x5c;
            break;
        }
        case 0x388:
        {
            $a1_1 = &param_adr_map_kneepoint_array;
            $s1_1 = 0x5c;
            break;
        }
        case 0x389:
        {
            $a1_1 = &param_adr_coc_kneepoint_y1_array;
            $s1_1 = 0x30;
            break;
        }
        case 0x38a:
        {
            $a1_1 = &param_adr_coc_kneepoint_y2_array;
            $s1_1 = 0x30;
            break;
        }
        case 0x38b:
        {
            $a1_1 = &param_adr_coc_kneepoint_y3_array;
            $s1_1 = 0x30;
            break;
        }
        case 0x38c:
        {
            $a1_1 = &param_adr_coc_kneepoint_y4_array;
            $s1_1 = 0x30;
            break;
        }
        case 0x38d:
        {
            $a1_1 = &param_adr_coc_kneepoint_y5_array;
            $s1_1 = 0x30;
            break;
        }
        case 0x38e:
        {
            $a1_1 = &param_adr_coc_adjust_array;
            $s1_1 = 0x38;
            break;
        }
        case 0x38f:
        {
            $a1_1 = &param_adr_centre_w_dis_array;
            $s1_1 = 0x7c;
            break;
        }
        case 0x390:
        {
            $a1_1 = &param_adr_stat_block_hist_diff_array;
            $s1_1 = 0x10;
            break;
        }
        case 0x391:
        {
            $a1_1 = &adr_tm_base_lut;
            $s1_1 = 0x24;
            break;
        }
        case 0x392:
        {
            $a1_1 = &param_adr_gam_x_array;
            $s1_1 = 0x102;
            break;
        }
        case 0x393:
        {
            $a1_1 = &param_adr_gam_y_array;
            $s1_1 = 0x102;
            break;
        }
        case 0x394:
        {
            $a1_1 = &adr_ctc_map2cut_y;
            $s1_1 = 0x24;
            break;
        }
        case 0x395:
        {
            $a1_1 = &adr_light_end;
            $s1_1 = 0x74;
            break;
        }
        case 0x396:
        {
            $a1_1 = &adr_block_light;
            $s1_1 = 0x3c;
            break;
        }
        case 0x397:
        {
            $a1_1 = &adr_map_mode;
            $s1_1 = 0x2c;
            break;
        }
        case 0x398:
        {
            $a1_1 = &histSub_4096_diff;
            $s1_1 = 0x20;
            break;
        }
        case 0x399:
        {
            $a1_1 = &param_adr_tool_control_array;
            $s1_1 = 0x38;
            break;
        }
        case 0x39a:
        {
            $a1_1 = &adr_ev_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x39b:
        {
            $a1_1 = &adr_ligb_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x39c:
        {
            $a1_1 = &adr_mapb1_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x39d:
        {
            $a1_1 = &adr_mapb2_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x39e:
        {
            $a1_1 = &adr_mapb3_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x39f:
        {
            $a1_1 = &adr_mapb4_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x3a0:
        {
            $a1_1 = &adr_ctc_map2cut_y_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3a1:
        {
            $a1_1 = &adr_light_end_wdr;
            $s1_1 = 0x74;
            break;
        }
        case 0x3a2:
        {
            $a1_1 = &adr_block_light_wdr;
            $s1_1 = 0x3c;
            break;
        }
        case 0x3a3:
        {
            $a1_1 = &adr_map_mode_wdr;
            $s1_1 = 0x2c;
            break;
        }
        case 0x3a4:
        {
            $a1_1 = &adr_ev_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3a5:
        {
            $a1_1 = &adr_ligb_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3a6:
        {
            $a1_1 = &adr_mapb1_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3a7:
        {
            $a1_1 = &adr_mapb2_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3a8:
        {
            $a1_1 = &adr_mapb3_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3a9:
        {
            $a1_1 = &adr_mapb4_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3aa:
        {
            $a1_1 = &adr_blp2_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x3ab:
        {
            $a1_1 = &adr_blp2_list;
            $s1_1 = 0x24;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

