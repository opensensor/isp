#include "include/main.h"


  int32_t tiziano_defog_algorithm()

{
    if (defog_frm_num < 2)
        defog_update_paras = 0;
    else if (!data_acd70_1)
        defog_update_paras = 0;
    else
        defog_update_paras = 1;
    
    data_acd78_1 = 0x528;
    data_acd7c_1 = 0x12c;
    data_acd80_1 = 0xfa0;
    
    if (!data_acd68_1)
        data_acd84_1 = 0;
    else
        data_acd84_2 = 1;
    
    if (!data_acd6c_1)
        data_acd88_1 = 0;
    else
        data_acd88_2 = 1;
    
    int32_t $a0_5 = data_acd58_1;
    
    if ($a0_5 == 1)
    {
        uint32_t ev_now_1 = ev_now;
        defog_manual_ctrl = $a0_5;
        data_ceab8_1 = ev_now_1;
    }
    
    TizianoDefogStructMe = &defog_block_hist_info;
    data_cd4a4_1 = &defog_sum_block_r;
    data_cd4a8_1 = &defog_sum_block_g;
    data_cd4ac_1 = &defog_sum_block_b;
    data_cd4b0_1 = &defog_block_mean_y_last;
    wchar32 (* param_defog_block_t_x_array_now_1)[0x2] = param_defog_block_t_x_array_now;
    data_cd4f8_1 = &defog_manual_ctrl;
    data_cd4b4_1 = param_defog_block_t_x_array_now_1;
    data_cd4b8_1 = U"_PKAK<Z";
    data_cd4bc_1 = &defog_block_area_div;
    data_cd4c0_1 = &defog_block_area_index;
    void* param_defog_fpga_para_array_now_1 = param_defog_fpga_para_array_now;
    data_cd4fc_1 = &defog_update_paras;
    data_cd4c4_1 = param_defog_fpga_para_array_now_1;
    data_cd4c8_1 = &defog_block_transmit_t;
    data_cd4cc_1 = &defog_block_air_light_r;
    data_cd4d0_1 = &defog_block_air_light_g;
    data_cd4d4_1 = &defog_block_air_light_b;
    int32_t i = 0;
    data_cd4d8_1 = defog_ev_list_now;
    data_cd4dc_1 = defog_trsy0_list_now;
    data_cd4e0_1 = defog_trsy1_list_now;
    data_cd4e4_1 = defog_trsy2_list_now;
    data_cd4e8_1 = defog_trsy3_list_now;
    data_cd4ec_1 = defog_trsy4_list_now;
    data_cd4f0_1 = &defog_t_par_list1;
    data_cd4f4_1 = &defog_t_par_list2;
    
    for (; i < 0x50; i += 1)
    {
        char var_58_14[0x50];
        var_58_15[i] = *(&data_cd4b0_2 + i);
    }
    
    tisp_defog_soft_process(TizianoDefogStructMe, data_cd4a4_2, data_cd4a8_2, data_cd4ac_2);
    uint32_t $v0_1 = defog_frm_num + 1;
    
    if ($v0_1 == isp_printf)
        $v0_1 = 0x80;
    
    defog_frm_num = $v0_1;
    return 0;
}

