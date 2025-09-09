#include "include/main.h"


  int32_t tisp_defog_wdr_en(uint32_t arg1)

{
    defog_wdr_en = arg1;
    void* $v0;
    
    if (arg1)
    {
        defog_ev_list_now = &defog_ev_list_wdr;
        defog_trsy0_list_now = &defog_trsy0_list_wdr;
        defog_trsy1_list_now = &defog_trsy1_list_wdr;
        defog_trsy2_list_now = &defog_trsy2_list_wdr;
        defog_trsy3_list_now = &defog_trsy3_list_wdr;
        defog_trsy4_list_now = &defog_trsy4_list_wdr;
        param_defog_block_t_x_array_now = &param_defog_block_t_x_wdr_array;
        param_defog_fpga_para_array_now = &param_defog_fpga_para_wdr_array;
        $v0 = &param_defog_main_para_wdr_array;
    }
    else
    {
        defog_ev_list_now = &defog_ev_list;
        defog_trsy0_list_now = &defog_trsy0_list;
        defog_trsy1_list_now = &defog_trsy1_list;
        defog_trsy2_list_now = &defog_trsy2_list;
        defog_trsy3_list_now = &defog_trsy3_list;
        defog_trsy4_list_now = &defog_trsy4_list;
        param_defog_block_t_x_array_now = &param_defog_block_t_y_array[5];
        param_defog_fpga_para_array_now = &param_defog_fpga_para_array;
        $v0 = &param_defog_main_para_array;
    }
    
    param_defog_main_para_array_now = $v0;
    tiziano_defog_params_refresh();
    tiziano_defog_params_init();
    /* tailcall */
    return tiziano_defog_set_reg_params();
}

