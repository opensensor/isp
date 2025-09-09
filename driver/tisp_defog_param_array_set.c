#include "include/main.h"


  int32_t tisp_defog_param_array_set(int32_t arg1, int32_t arg2, int32_t* arg3)

{
    if (arg1 - 0x35a >= 0x26)
    {
        int32_t var_18_1_19 = arg1;
        isp_printf(2, "sensor type is BT1120!\\n", "tisp_defog_param_array_set");
        return 0xffffffff;
    }
    
    switch (arg1)
    {
        case 0x35a:
        {
            memcpy(&data_aca90_12[6]);
            *arg3 = 0x80;
            break;
        }
        case 0x35b:
        {
            memcpy(&param_defog_weightlut02);
            *arg3 = 0x80;
            break;
        }
        case 0x35c:
        {
            memcpy(&param_defog_weightlut12);
            *arg3 = 0x80;
            break;
        }
        case 0x35d:
        {
            memcpy(&data_ac8fc_17[0xb]);
            *arg3 = 0x80;
            break;
        }
        case 0x35e:
        {
            memcpy(&param_defog_weightlut21);
            *arg3 = 0x80;
            break;
        }
        case 0x35f:
        {
            memcpy(&param_defog_col_ct_array);
            *arg3 = 0x38;
            break;
        }
        case 0x360:
        {
            memcpy(&param_defog_cent3_w_dis_array, arg2, 0x60);
            *arg3 = 0x60;
            break;
        }
        case 0x361:
        {
            memcpy(&param_defog_cent5_w_dis_array, arg2, 0x7c);
            *arg3 = 0x7c;
            break;
        }
        case 0x362:
        {
            memcpy(&defog_ev_list);
            *arg3 = 0x24;
            break;
        }
        case 0x363:
        {
            memcpy(&defog_trsy0_list);
            *arg3 = 0x24;
            break;
        }
        case 0x364:
        {
            memcpy(&defog_trsy1_list);
            *arg3 = 0x24;
            break;
        }
        case 0x365:
        {
            memcpy(&defog_trsy2_list);
            *arg3 = 0x24;
            break;
        }
        case 0x366:
        {
            memcpy(&defog_trsy3_list);
            *arg3 = 0x24;
            break;
        }
        case 0x367:
        {
            memcpy(&defog_trsy4_list);
            *arg3 = 0x24;
            break;
        }
        case 0x368:
        {
            memcpy(&defog_rgbra_list, arg2, 0x24);
            
            if (defog_rgbra_list)
                *arg3 = 0x24;
            else
            {
                memcpy(&param_defog_cent3_w_dis_array, &param_defog_cent3_w_dis_array_tmp, 0x60);
                memcpy(&param_defog_cent5_w_dis_array, &param_defog_cent5_w_dis_array_tmp, 0x7c);
                memcpy(&data_ac8fc_18[0xb], &param_defog_weightlut22_tmp, 0x80);
                memcpy(&param_defog_weightlut12, &param_defog_weightlut12_tmp, 0x80);
                memcpy(&param_defog_weightlut21, &param_defog_weightlut21_tmp, 0x80);
                memcpy(&param_defog_weightlut02, &param_defog_weightlut02_tmp, 0x80);
                memcpy(&data_aca90_13[6], &param_defog_weightlut20_tmp, 0x80);
                *arg3 = 0x24;
            }
            break;
        }
        case 0x369:
        {
            memcpy(&param_defog_main_para_array);
            *arg3 = 0x2c;
            break;
        }
        case 0x36a:
        {
            memcpy(&param_defog_color_control_array);
            *arg3 = 0x38;
            break;
        }
        case 0x36b:
        {
            memcpy(&param_defog_lc_s_array);
            *arg3 = 0x28;
            break;
        }
        case 0x36c:
        {
            memcpy(&param_defog_lc_v_array);
            *arg3 = 0x28;
            break;
        }
        case 0x36d:
        {
            memcpy(&param_defog_cc_s_array, arg2, 0x20);
            *arg3 = 0x20;
            break;
        }
        case 0x36e:
        {
            memcpy(&param_defog_cc_v_array);
            *arg3 = 0x24;
            break;
        }
        case 0x36f:
        {
            memcpy(&param_defog_dark_l1_array);
            *arg3 = 0x28;
            break;
        }
        case 0x370:
        {
            memcpy(&param_defog_dark_l2_array);
            *arg3 = 0x28;
            break;
        }
        case 0x371:
        {
            memcpy(&param_defog_block_t_y_array[5]);
            *arg3 = 0x14;
            break;
        }
        case 0x372:
        {
            memcpy(U"_PKAK<Z");
            *arg3 = 0x14;
            break;
        }
        case 0x373:
        {
            memcpy(&defog_t_par_list1);
            *arg3 = 0x2c;
            break;
        }
        case 0x374:
        {
            memcpy(&defog_t_par_list2, arg2, 0x74);
            *arg3 = 0x74;
            break;
        }
        case 0x375:
        {
            memcpy(&defog_manual_ctrl, arg2, 0x1c);
            *arg3 = 0x1c;
            break;
        }
        case 0x376:
        {
            memcpy(&defog_ev_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x377:
        {
            memcpy(&defog_trsy0_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x378:
        {
            memcpy(&defog_trsy1_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x379:
        {
            memcpy(&defog_trsy2_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x37a:
        {
            memcpy(&defog_trsy3_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x37b:
        {
            memcpy(&defog_trsy4_list_wdr);
            *arg3 = 0x24;
            break;
        }
        case 0x37c:
        {
            memcpy(&param_defog_main_para_wdr_array);
            *arg3 = 0x2c;
            break;
        }
        case 0x37d:
        {
            memcpy(&param_defog_block_t_x_wdr_array);
            *arg3 = 0x14;
            break;
        }
        case 0x37e:
        {
            memcpy(&param_defog_fpga_para_wdr_array, arg2, 0x40);
            *arg3 = 0x40;
            break;
        }
        case 0x37f:
        {
            memcpy(&param_defog_fpga_para_array, arg2, 0x40);
            tiziano_defog_params_init();
            *arg3 = 0x40;
            break;
        }
    }
    
    return 0;
}

