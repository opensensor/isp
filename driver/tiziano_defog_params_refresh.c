#include "include/main.h"


  int32_t tiziano_defog_params_refresh()

{
    int32_t defog_rgbra_list_1 = defog_rgbra_list;
    void* $a1_1;
            return 0xffffffff;
    memcpy(&defog_rgbra_list, &data_a56b0[0x2d], 0x24);
    
    if (defog_rgbra_list_1 != 1)
    {
        if (defog_rgbra_list_1)
        
        memcpy(&data_aca90[6], &param_defog_weightlut20_tmp, 0x80);
        memcpy(&param_defog_weightlut02, &param_defog_weightlut02_tmp, 0x80);
        memcpy(&param_defog_weightlut12, &param_defog_weightlut12_tmp, 0x80);
        memcpy(&data_ac8fc[0xb], &param_defog_weightlut22_tmp, 0x80);
        memcpy(&param_defog_weightlut21, &param_defog_weightlut21_tmp, 0x80);
        memcpy(&param_defog_cent3_w_dis_array, &param_defog_cent3_w_dis_array_tmp, 0x60);
        $a1_1 = &param_defog_cent5_w_dis_array_tmp;
    }
    else
    {
        memcpy(&data_aca90[6], 0xa52f8, 0x80);
        memcpy(&param_defog_weightlut02, &data_a5378, 0x80);
        memcpy(&param_defog_weightlut12, &data_a53f8, 0x80);
        memcpy(&data_ac8fc[0xb], 0xa5478, 0x80);
        memcpy(&param_defog_weightlut21, 0xa54f8, 0x80);
        memcpy(&param_defog_cent3_w_dis_array, 0xa55b0, 0x60);
        $a1_1 = &data_a5610;
    }
    
    memcpy(&param_defog_cent5_w_dis_array, $a1_1, 0x7c);
    memcpy(&defog_t_par_list1, &data_a58e4_1[5], 0x2c);
    memcpy(&defog_t_par_list2, 0xa5924, 0x74);
    memcpy(&defog_manual_ctrl, 0xa5998, 0x1c);
    memcpy(&defog_ev_list, 0xa568c, 0x24);
    memcpy(&defog_trsy0_list, U"PPZZZPPPPFNMLKJIHFFIGOFDCBBA?>=<;:99LLKJIHGFE", 0x24);
    memcpy(&defog_trsy1_list, &data_a56b0_1[9], 0x24);
    memcpy(&defog_trsy2_list, &data_a56b0_2[0x12], 0x24);
    memcpy(&defog_trsy3_list, &data_a56b0_3[0x1b], 0x24);
    memcpy(&defog_trsy4_list, &data_a56b0_4[0x24], 0x24);
    memcpy(&param_defog_main_para_array, 0xa5788, 0x2c);
    memcpy(&param_defog_color_control_array, 0xa57b4, 0x38);
    memcpy(&param_defog_lc_s_array, 0xa57ec, 0x28);
    memcpy(&param_defog_lc_v_array, 0xa5814, 0x28);
    memcpy(&param_defog_cc_s_array, 0xa583c, 0x20);
    memcpy(&param_defog_cc_v_array, 0xa585c, 0x24);
    memcpy(&param_defog_dark_l1_array, 0xa5880, 0x28);
    memcpy(&param_defog_dark_l2_array, 0xa58a8, 0x28);
    memcpy(&param_defog_block_t_y_array[5], U"<Z", 0x14);
    memcpy(U"_PKAK<Z", U"PFFAL7Z", 0x14);
    memcpy(&defog_ev_list_wdr, 0xa59b4, 0x24);
    memcpy(&defog_trsy0_list_wdr, 0xa59d8, 0x24);
    memcpy(&defog_trsy1_list_wdr, &data_a59fc, 0x24);
    memcpy(&defog_trsy2_list_wdr, &data_a5a20, 0x24);
    memcpy(&defog_trsy3_list_wdr, &data_a5a44, 0x24);
    memcpy(&defog_trsy4_list_wdr, &data_a5a68, 0x24);
    memcpy(&param_defog_main_para_wdr_array, U"F", 0x2c);
    memcpy(&param_defog_block_t_x_wdr_array, 0xa5ab8, 0x14);
    memcpy(&param_defog_fpga_para_wdr_array, 0xa5acc, 0x40);
    memcpy(&param_defog_fpga_para_array, 0xa5b0c, 0x40);
    return 0;
}

