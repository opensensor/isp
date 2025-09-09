#include "include/main.h"


  int32_t tisp_adr_param_array_set(int32_t arg1, int32_t* arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
        return 0xffffffff;
    if (arg1 - (uintptr_t)0x380 >= 0x2c)
    {

    }
    
    switch (arg1)
    {
        case 0x380:
        {
            memcpy(&param_adr_para_array);
            *arg3 = 0x20;
            break;
        }
        case 0x381:
        {
            memcpy(&param_adr_weight_20_lut_array);
            *arg3 = 0x80;
            break;
        }
        case 0x382:
        {
            memcpy(&param_adr_weight_02_lut_array);
            *arg3 = 0x80;
            break;
        }
        case 0x383:
        {
            memcpy(&param_adr_weight_12_lut_array);
            *arg3 = 0x80;
            break;
        }
        case 0x384:
        {
            memcpy(&param_adr_weight_22_lut_array);
            *arg3 = 0x80;
            break;
        }
        case 0x385:
        {
            memcpy(&param_adr_weight_21_lut_array);
            *arg3 = 0x80;
            break;
        }
        case 0x386:
        {
            memcpy(&param_adr_ctc_kneepoint_array, arg2, 0x44);
            *arg3 = 0x44;
            break;
        }
        case 0x387:
        {
            memcpy(&param_adr_min_kneepoint_array);
            *arg3 = 0x5c;
            break;
        }
        case 0x388:
        {
            memcpy(&param_adr_map_kneepoint_array);
            *arg3 = 0x5c;
            break;
        }
        case 0x389:
        {
            memcpy(&param_adr_coc_kneepoint_y1_array);
            *arg3 = 0x30;
            break;
        }
        case 0x38a:
        {
            memcpy(&param_adr_coc_kneepoint_y2_array);
            *arg3 = 0x30;
            break;
        }
        case 0x38b:
        {
            memcpy(&param_adr_coc_kneepoint_y3_array);
            *arg3 = 0x30;
            break;
        }
        case 0x38c:
        {
            memcpy(&param_adr_coc_kneepoint_y4_array);
            *arg3 = 0x30;
            break;
        }
        case 0x38d:
        {
            memcpy(&param_adr_coc_kneepoint_y5_array);
            *arg3 = 0x30;
            break;
        }
        case 0x38e:
        {
            memcpy(&param_adr_coc_adjust_array, arg2, 0x38);
            *arg3 = 0x38;
            break;
        }
        case 0x38f:
        {
            memcpy(&param_adr_centre_w_dis_array, arg2, 0x7c);
            *arg3 = 0x7c;
            break;
        }
        case 0x390:
        {
            memcpy(&param_adr_stat_block_hist_diff_array, arg2, 0x10);
            *arg3 = 0x10;
            break;
        }
        case 0x391:
        {
            memcpy(&adr_tm_base_lut);
            *arg3 = 0x24;
            break;
        }
        case 0x392:
        {
            memcpy(&param_adr_gam_x_array);
            *arg3 = 0x102;
            break;
        }
        case 0x393:
        {
            memcpy(&param_adr_gam_y_array);
            *arg3 = 0x102;
            break;
        }
        case 0x394:
        {
            memcpy(&adr_ctc_map2cut_y);
            *arg3 = 0x24;
            break;
        }
        case 0x395:
        {
            memcpy(&adr_light_end);
            *arg3 = 0x74;
            break;
        }
        case 0x396:
        {
            memcpy(&adr_block_light);
            *arg3 = 0x3c;
            break;
        }
        case 0x397:
        {
            memcpy(&adr_map_mode);
            *arg3 = 0x2c;
            break;
        }
        case 0x398:
        {
            memcpy(&histSub_4096_diff);
            *arg3 = 0x20;
            break;
        }
        case 0x399:
        {
            int32_t* $a0_9 = &param_adr_tool_control_array;
                int32_t i_1 = i;
            
            for (int32_t i = 0; (uintptr_t)i != 0xe; )
            {
                i += 1;
                
                if (i_1 != 1)
                    *$a0_9 = *arg2;
                
                arg2 = &arg2[1];
                $a0_9 = &$a0_9[1];
            }
            
            if (param_adr_tool_control_array)
                *arg3 = 0x38;
            else
            {
                memcpy(&param_adr_centre_w_dis_array, &param_adr_centre_w_dis_array_tmp, 0x7c);
                memcpy(&param_adr_weight_20_lut_array, &param_adr_weight_20_lut_array_tmp, 0x80);
                memcpy(&param_adr_weight_02_lut_array, &param_adr_weight_02_lut_array_tmp, 0x80);
                memcpy(&param_adr_weight_12_lut_array, &param_adr_weight_12_lut_array_tmp, 0x80);
                memcpy(&param_adr_weight_22_lut_array, &param_adr_weight_22_lut_array_tmp, 0x80);
                memcpy(&param_adr_weight_21_lut_array, &param_adr_weight_21_lut_array_tmp, 0x80);
                *arg3 = 0x38;
            }
            break;
        }
        case 0x39a:
        {
            memcpy(&adr_ev_list);
            *arg3 = 0x24;
            break;
        }
        case 0x39b:
        {
            memcpy(&adr_ligb_list);
            *arg3 = 0x24;
            break;
        }
        case 0x39c:
        {
            memcpy(&adr_mapb1_list);
            *arg3 = 0x24;
            break;
        }
        case 0x39d:
        {
            memcpy(&adr_mapb2_list);
            *arg3 = 0x24;
            break;
        }
        case 0x39e:
        {
            memcpy(&adr_mapb3_list);
            *arg3 = 0x24;
            break;
        }
        case 0x39f:
        {
            memcpy(&adr_mapb4_list);
            *arg3 = 0x24;
            break;
        }
        case 0x3a0:
        {
            memcpy(&adr_ctc_map2cut_y_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x3a1:
        {
            memcpy(&adr_light_end_wdr);
            *arg3 = 0x74;
            break;
        }
        case 0x3a2:
        {
            memcpy(&adr_block_light_wdr);
            *arg3 = 0x3c;
            break;
        }
        case 0x3a3:
        {
            memcpy(&adr_map_mode_wdr);
            *arg3 = 0x2c;
            break;
        }
        case 0x3a4:
        {
            memcpy(&adr_ev_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x3a5:
        {
            memcpy(&adr_ligb_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x3a6:
        {
            memcpy(&adr_mapb1_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x3a7:
        {
            memcpy(&adr_mapb2_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x3a8:
        {
            memcpy(&adr_mapb3_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x3a9:
        {
            memcpy(&adr_mapb4_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x3aa:
        {
            memcpy(&adr_blp2_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x3ab:
        {
            memcpy(&adr_blp2_list, arg2, 0x24);
            tiziano_adr_params_init();
            ev_changed = 1;
            *arg3 = 0x24;
            break;
        }
    }
    
    return 0;
}

