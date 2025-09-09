#include "include/main.h"


  int32_t tisp_defog_param_array_get(int32_t arg1, int32_t arg2, int32_t* arg3)

{
        int32_t var_18_1 = arg1;
    if (arg1 - (uintptr_t)0x35a >= 0x26)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xffffffff;
    }
    
    int32_t* $a1_1;
    int32_t $s1_1;
    
    switch (arg1)
    {
        case 0x35a:
        {
            $a1_1 = &data_aca90[6];
            $s1_1 = 0x80;
            break;
        }
        case 0x35b:
        {
            $a1_1 = &param_defog_weightlut02;
            $s1_1 = 0x80;
            break;
        }
        case 0x35c:
        {
            $a1_1 = &param_defog_weightlut12;
            $s1_1 = 0x80;
            break;
        }
        case 0x35d:
        {
            $a1_1 = &data_ac8fc[0xb];
            $s1_1 = 0x80;
            break;
        }
        case 0x35e:
        {
            $a1_1 = &param_defog_weightlut21;
            $s1_1 = 0x80;
            break;
        }
        case 0x35f:
        {
            $a1_1 = &param_defog_col_ct_array;
            $s1_1 = 0x38;
            break;
        }
        case 0x360:
        {
            $a1_1 = &param_defog_cent3_w_dis_array;
            $s1_1 = 0x60;
            break;
        }
        case 0x361:
        {
            $a1_1 = &param_defog_cent5_w_dis_array;
            $s1_1 = 0x7c;
            break;
        }
        case 0x362:
        {
            $a1_1 = &defog_ev_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x363:
        {
            $a1_1 = &defog_trsy0_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x364:
        {
            $a1_1 = &defog_trsy1_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x365:
        {
            $a1_1 = &defog_trsy2_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x366:
        {
            $a1_1 = &defog_trsy3_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x367:
        {
            $a1_1 = &defog_trsy4_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x368:
        {
            $a1_1 = &defog_rgbra_list;
            $s1_1 = 0x24;
            break;
        }
        case 0x369:
        {
            $a1_1 = &param_defog_main_para_array;
            $s1_1 = 0x2c;
            break;
        }
        case 0x36a:
        {
            $a1_1 = &param_defog_color_control_array;
            $s1_1 = 0x38;
            break;
        }
        case 0x36b:
        {
            $a1_1 = &param_defog_lc_s_array;
            $s1_1 = 0x28;
            break;
        }
        case 0x36c:
        {
            $a1_1 = &param_defog_lc_v_array;
            $s1_1 = 0x28;
            break;
        }
        case 0x36d:
        {
            $a1_1 = &param_defog_cc_s_array;
            $s1_1 = 0x20;
            break;
        }
        case 0x36e:
        {
            $a1_1 = &param_defog_cc_v_array;
            $s1_1 = 0x24;
            break;
        }
        case 0x36f:
        {
            $a1_1 = &param_defog_dark_l1_array;
            $s1_1 = 0x28;
            break;
        }
        case 0x370:
        {
            $a1_1 = &param_defog_dark_l2_array;
            $s1_1 = 0x28;
            break;
        }
        case 0x371:
        {
            $a1_1 = &param_defog_block_t_y_array[5];
            $s1_1 = 0x14;
            break;
        }
        case 0x372:
        {
            $a1_1 = U"_PKAK<Z";
            $s1_1 = 0x14;
            break;
        }
        case 0x373:
        {
            $a1_1 = &defog_t_par_list1;
            $s1_1 = 0x2c;
            break;
        }
        case 0x374:
        {
            $a1_1 = &defog_t_par_list2;
            $s1_1 = 0x74;
            break;
        }
        case 0x375:
        {
            $a1_1 = &defog_manual_ctrl;
            $s1_1 = 0x1c;
            break;
        }
        case 0x376:
        {
            $a1_1 = &defog_ev_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x377:
        {
            $a1_1 = &defog_trsy0_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x378:
        {
            $a1_1 = &defog_trsy1_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x379:
        {
            $a1_1 = &defog_trsy2_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x37a:
        {
            $a1_1 = &defog_trsy3_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x37b:
        {
            $a1_1 = &defog_trsy4_list_wdr;
            $s1_1 = 0x24;
            break;
        }
        case 0x37c:
        {
            $a1_1 = &param_defog_main_para_wdr_array;
            $s1_1 = 0x2c;
            break;
        }
        case 0x37d:
        {
            $a1_1 = &param_defog_block_t_x_wdr_array;
            $s1_1 = 0x14;
            break;
        }
        case 0x37e:
        {
            $a1_1 = &param_defog_fpga_para_wdr_array;
            $s1_1 = 0x40;
            break;
        }
        case 0x37f:
        {
            $a1_1 = &param_defog_fpga_para_array;
            $s1_1 = 0x40;
            break;
        }
    }
    
    memcpy(arg2, $a1_1, $s1_1);
    *arg3 = $s1_1;
    return 0;
}

