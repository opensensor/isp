#include "include/main.h"


  int32_t tiziano_defog_init(int32_t arg1, int32_t arg2)

{
    int32_t $s7 = arg2 * arg1;
    void* $v0_1;
    defog_frm_num = 0;
    
    if (defog_wdr_en)
    {
        defog_ev_list_now = &defog_ev_list_wdr;
        defog_trsy0_list_now = &defog_trsy0_list_wdr;
        defog_trsy1_list_now = &defog_trsy1_list_wdr;
        defog_trsy2_list_now = &defog_trsy2_list_wdr;
        defog_trsy3_list_now = &defog_trsy3_list_wdr;
        defog_trsy4_list_now = &defog_trsy4_list_wdr;
        param_defog_block_t_x_array_now = &param_defog_block_t_x_wdr_array;
        param_defog_fpga_para_array_now = &param_defog_fpga_para_wdr_array;
        $v0_1 = &param_defog_main_para_wdr_array;
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
        $v0_1 = &param_defog_main_para_array;
    }
    
    param_defog_main_para_array_now = $v0_1;
    tiziano_defog_params_refresh();
    
    if ((uintptr_t)arg2 == 0x2d0)
    {
        memcpy(&defog_block_sizem, &block_sizem_720, 0x2c);
        *defog_block_area_index = 0;
    }
    else if ((uintptr_t)arg2 == 0x438)
    {
        memcpy(&defog_block_sizem, &block_sizem_1080, 0x2c);
        *defog_block_area_index = 0;
    }
    else if ((uintptr_t)arg2 == 0x510)
    {
        memcpy(&defog_block_sizem, &block_sizem_1296, 0x2c);
        *defog_block_area_index = 6;
    }
    else if ((uintptr_t)arg2 == 0x5a0)
    {
        memcpy(&defog_block_sizem, &block_sizem_1440, 0x2c);
        *defog_block_area_index = 0;
    }
    else if ((uintptr_t)arg2 == 0x780)
    {
        memcpy(&defog_block_sizem, &block_sizem_1920, 0x2c);
        *defog_block_area_index = 0;
    }
    else if ((uintptr_t)arg2 != 0x798)
    {
        int32_t* $a0 = &defog_block_sizem;
        int32_t i = 1;
        int32_t $v0_3 = arg2 % 0xa;
            int32_t $v1_3 = arg2 / 0xa + *$a0;
        *defog_block_area_index = $v0_3;
        
        do
        {
            
            if ($v0_3 >= i)
                $v1_3 += 1;
            
            i += 1;
            $a0[1] = $v1_3;
            $a0 = &$a0[1];
        } while ((uintptr_t)i != 0xb);
    }
    else
    {
        memcpy(&defog_block_sizem, &block_sizem_1944, 0x2c);
        *defog_block_area_index = 4;
    }
    
    if ((uintptr_t)arg1 == 0x500)
    {
        memcpy(&defog_block_sizen, &block_sizen_1280, 0x4c);
        *((int32_t*)((char*)defog_block_area_index + 4)) = 2; // Fixed void pointer dereference
    }
    else if ((uintptr_t)arg1 == 0x780)
    {
        memcpy(&defog_block_sizen, &block_sizen_1920, 0x4c);
        *((int32_t*)((char*)defog_block_area_index + 4)) = 0xc; // Fixed void pointer dereference
    }
    else if ((uintptr_t)arg1 == 0x900)
    {
        memcpy(&defog_block_sizen, &block_sizen_2304, 0x4c);
        *((int32_t*)((char*)defog_block_area_index + 4)) = 0; // Fixed void pointer dereference
    }
    else if ((uintptr_t)arg1 == 0xa00)
    {
        memcpy(&defog_block_sizen, &block_sizen_2560, 0x4c);
        *((int32_t*)((char*)defog_block_area_index + 4)) = 4; // Fixed void pointer dereference
    }
    else if ((uintptr_t)arg1 != 0xa20)
    {
        int32_t* $a0_1 = &defog_block_sizen;
        int32_t i_1 = 1;
        int32_t $v0_5 = arg1 % 0x12;
            int32_t $v1_6 = arg1 / 0x12 + *$a0_1;
        *((int32_t*)((char*)defog_block_area_index + 4)) = $v0_5; // Fixed void pointer dereference
        
        do
        {
            
            if ($v0_5 >= i_1)
                $v1_6 += 1;
            
            i_1 += 1;
            $a0_1[1] = $v1_6;
            $a0_1 = &$a0_1[1];
        } while ((uintptr_t)i_1 != 0x13);
    }
    else
    {
        memcpy(&defog_block_sizen, &block_sizen_2592, 0x4c);
        *((int32_t*)((char*)defog_block_area_index + 4)) = 0; // Fixed void pointer dereference
    }
    
    if ($(uintptr_t)s7 == 0xe1000)
    {
        memcpy(&param_defog_cent3_w_dis_array_tmp, &weight3_1280_720, 0x60);
        memcpy(&param_defog_cent5_w_dis_array_tmp, &weight5_1280_720, 0x7c);
        memcpy(&param_defog_weightlut22_tmp, &wei22_16_9, 0x80);
        memcpy(&param_defog_weightlut12_tmp, &wei12_16_9, 0x80);
        memcpy(&param_defog_weightlut21_tmp, &wei21_16_9, 0x80);
        memcpy(&param_defog_weightlut20_tmp, &wei20_16_9, 0x80);
        memcpy(&param_defog_weightlut02_tmp, &wei02_16_9, 0x80);
        defog_block_area_div = 0xc78031;
        data_acd9c = 0xca4587;
        data_acd98 = 0xca4f85;
        data_acda0 = 0xcd1ed9;
    }
    else if ($(uintptr_t)s7 == 0x1fa400)
    {
        memcpy(&param_defog_cent3_w_dis_array_tmp, &weight3_1920_1080, 0x60);
        memcpy(&param_defog_cent5_w_dis_array_tmp, &weight5_1920_1080, 0x7c);
        memcpy(&param_defog_weightlut22_tmp, &wei22_16_9, 0x80);
        memcpy(&param_defog_weightlut12_tmp, &wei12_16_9, 0x80);
        memcpy(&param_defog_weightlut21_tmp, &wei21_16_9, 0x80);
        memcpy(&param_defog_weightlut20_tmp, &wei20_16_9, 0x80);
        memcpy(&param_defog_weightlut02_tmp, &wei02_16_9, 0x80);
        defog_block_area_div = 0x59e7fd;
        data_acd98 = 0x5ac11e;
        data_acd9c = 0x5abd19;
        data_acda0 = 0x5b983e;
    }
    else if ($(uintptr_t)s7 == 0x2d9000)
    {
        memcpy(&param_defog_cent3_w_dis_array_tmp, &weight3_2304_1296, 0x60);
        memcpy(&param_defog_cent5_w_dis_array_tmp, &weight5_2304_1296, 0x7c);
        memcpy(&param_defog_weightlut22_tmp, &wei22_16_9, 0x80);
        memcpy(&param_defog_weightlut12_tmp, &wei12_16_9, 0x80);
        memcpy(&param_defog_weightlut21_tmp, &wei21_16_9, 0x80);
        memcpy(&param_defog_weightlut20_tmp, &wei20_16_9, 0x80);
        memcpy(&param_defog_weightlut02_tmp, &wei02_16_9, 0x80);
        defog_block_area_div = 0x3e86e2;
        data_acd9c = 0x3f02f8;
        data_acd98 = 0x3f03f0;
        data_acda0 = 0x3f80fe;
    }
    else if ($(uintptr_t)s7 == 0x384000)
    {
        memcpy(&param_defog_cent3_w_dis_array_tmp, &weight3_2560_1440, 0x60);
        memcpy(&param_defog_cent5_w_dis_array_tmp, &weight5_2560_1440, 0x7c);
        memcpy(&param_defog_weightlut22_tmp, &wei22_16_9, 0x80);
        memcpy(&param_defog_weightlut12_tmp, &wei12_16_9, 0x80);
        memcpy(&param_defog_weightlut21_tmp, &wei21_16_9, 0x80);
        memcpy(&param_defog_weightlut20_tmp, &wei20_16_9, 0x80);
        memcpy(&param_defog_weightlut02_tmp, &wei02_16_9, 0x80);
        defog_block_area_div = 0x329202;
        data_acd98 = 0x32ed2d;
        data_acd9c = 0x32ebe9;
        data_acda0 = 0x3347b6;
    }
    else if ($(uintptr_t)s7 == 0x4b0000)
    {
        memcpy(&param_defog_cent3_w_dis_array_tmp, &weight3_2560_1920, 0x60);
        memcpy(&param_defog_cent5_w_dis_array_tmp, &weight5_2560_1920, 0x7c);
        memcpy(&param_defog_weightlut22_tmp, 0x9ac84, 0x80);
        memcpy(&param_defog_weightlut12_tmp, &wei12_4_3, 0x80);
        memcpy(&param_defog_weightlut21_tmp, &wei21_4_3, 0x80);
        memcpy(&param_defog_weightlut20_tmp, &wei20_4_3, 0x80);
        memcpy(&param_defog_weightlut02_tmp, &wei02_4_3, 0x80);
        data_acd98 = 0x2642c5;
        defog_block_area_div = 0x25fe46;
        data_acd9c = 0x2630ef;
        data_acda0 = 0x2675c9;
    }
    else if ($(uintptr_t)s7 != 0x4ce300)
    {
        uint32_t $lo_3 = arg2 / 0xa;
        int32_t $s7_1 = ($lo_3 + 1) * (arg1 / 0x12 + 1);
        int32_t $a1_5 = $s7_1 - ($lo_3 + 1);
        int32_t $s2_3 = $s7_1 - (arg1 / 0x12 + 1);
        int32_t $v0_8;
        int32_t $a1_6;
        int32_t $v0_9;
        int32_t $a1_7;
        defog_3x3_5x5_params_init(arg1, arg2);
        defog_block_area_div = fix_point_div_64(0, $lo_3 + 1, 0, 0x10, $s7_1, 0);
        $v0_8 = fix_point_div_64(0, $a1_5, 0, 0x10, $a1_5, 0);
        data_acd98 = $v0_8;
        $v0_9 = fix_point_div_64(0, $a1_6, 0, 0x10, $s2_3, 0);
        data_acd9c = $v0_9;
        data_acda0 = fix_point_div_64(0, $a1_7, 0, 0x10, $s2_3 - $lo_3, 0);
    }
    else
    {
        memcpy(&param_defog_cent3_w_dis_array_tmp, &weight3_2592_1944, 0x60);
        memcpy(&param_defog_cent5_w_dis_array_tmp, &weight5_2592_1944, 0x7c);
        memcpy(&param_defog_weightlut22_tmp, 0x9ac84, 0x80);
        memcpy(&param_defog_weightlut12_tmp, &wei12_4_3, 0x80);
        memcpy(&param_defog_weightlut21_tmp, &wei21_4_3, 0x80);
        memcpy(&param_defog_weightlut20_tmp, &wei20_4_3, 0x80);
        memcpy(&param_defog_weightlut02_tmp, &wei02_4_3, 0x80);
        defog_block_area_div = 0x2515bd;
        data_acd98 = 0x2557ab;
        data_acd9c = 0x2546ad;
        data_acda0 = 0x2588f2;
    }
    
    if (!defog_rgbra_list)
    {
        memcpy(&param_defog_cent3_w_dis_array, &param_defog_cent3_w_dis_array_tmp, 0x60);
        memcpy(&param_defog_cent5_w_dis_array, &param_defog_cent5_w_dis_array_tmp, 0x7c);
        memcpy(&data_ac8fc[0xb], &param_defog_weightlut22_tmp, 0x80);
        memcpy(&param_defog_weightlut12, &param_defog_weightlut12_tmp, 0x80);
        memcpy(&param_defog_weightlut21, &param_defog_weightlut21_tmp, 0x80);
        memcpy(&data_aca90[6], &param_defog_weightlut20_tmp, 0x80);
        memcpy(&param_defog_weightlut02, &param_defog_weightlut02_tmp, 0x80);
    }
    
    system_reg_write(0x5b04, 0);
    system_reg_write(0x5b0c, 0xffffffff);
    system_reg_write(0x5b00, 0);
    system_reg_write(0x5800, (data_accc0_1 & 0xfff) << 0x10 | (defog_block_sizem & 0xfff));
    system_reg_write(0x5804, (data_accc8_1 & 0xfff) << 0x10 | (data_accc4_1 & 0xfff));
    system_reg_write(0x5808, (data_accd0_1 & 0xfff) << 0x10 | (data_acccc_1 & 0xfff));
    system_reg_write(0x580c, (data_accd8_1 & 0xfff) << 0x10 | (data_accd4_1 & 0xfff));
    system_reg_write(0x5810, (data_acce0_1 & 0xfff) << 0x10 | (data_accdc_1 & 0xfff));
    system_reg_write(0x5814, data_acce4_1 & 0xfff);
    system_reg_write(0x5820, (data_acc74_1 & 0xfff) << 0x10 | (defog_block_sizen & 0xfff));
    system_reg_write(0x5824, (data_acc7c_1 & 0xfff) << 0x10 | (data_acc78_1 & 0xfff));
    system_reg_write(0x5828, (data_acc84_1 & 0xfff) << 0x10 | (data_acc80_1 & 0xfff));
    system_reg_write(0x582c, (data_acc8c_1 & 0xfff) << 0x10 | (data_acc88_1 & 0xfff));
    system_reg_write(0x5830, (data_acc94_1 & 0xfff) << 0x10 | (data_acc90_1 & 0xfff));
    system_reg_write(0x5834, (data_acc9c_1 & 0xfff) << 0x10 | (data_acc98_1 & 0xfff));
    system_reg_write(0x5838, (data_acca4_1 & 0xfff) << 0x10 | (data_acca0_1 & 0xfff));
    system_reg_write(0x583c, (data_accac_1 & 0xfff) << 0x10 | (data_acca8_1 & 0xfff));
    system_reg_write(0x5840, (data_accb4_1 & 0xfff) << 0x10 | (data_accb0_1 & 0xfff));
    system_reg_write(0x5844, data_accb8_1 & 0xfff);
    tiziano_defog_params_init();
    tiziano_defog_set_reg_params();
    system_irq_func_set(0x14, tiziano_defog_interrupt_static);
    tisp_event_set_cb(3, tisp_defog_process);
    return 0;
}

