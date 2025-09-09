#include "include/main.h"


  int32_t tiziano_defog_params_init()

{
    system_reg_write(0x5850, 
        (data_ac84c_1[0] & 0x7fff) << 0x10 | (param_defog_cent3_w_dis_array & 0x7fff));
    system_reg_write(0x5854, (data_ac84c_2[2] & 0x7fff) << 0x10 | (data_ac84c_3[1] & 0x7fff));
    system_reg_write(0x5858, (data_ac84c_4[4] & 0x7fff) << 0x10 | (data_ac84c_5[3] & 0x7fff));
    system_reg_write(0x585c, (data_ac864_1 & 0x7fff) << 0x10 | (data_ac860_1 & 0x7fff));
    system_reg_write(0x5860, (data_ac86c_1 & 0x7fff) << 0x10 | (data_ac868_1 & 0x7fff));
    system_reg_write(0x5864, (data_ac874_1 & 0x7fff) << 0x10 | (data_ac870_1 & 0x7fff));
    system_reg_write(0x5868, (data_ac87c_1 & 0x7fff) << 0x10 | (data_ac878_1 & 0x7fff));
    system_reg_write(0x586c, (data_ac884_1 & 0x7fff) << 0x10 | (data_ac880_1 & 0x7fff));
    system_reg_write(0x5870, (data_ac88c_1 & 0x7fff) << 0x10 | (data_ac888_1 & 0x7fff));
    system_reg_write(0x5874, (data_ac894_1 & 0x7fff) << 0x10 | (data_ac890_1 & 0x7fff));
    system_reg_write(0x5878, (data_ac89c_1 & 0x7fff) << 0x10 | (data_ac898_1 & 0x7fff));
    system_reg_write(0x587c, (data_ac8a4_1 & 0x7fff) << 0x10 | (data_ac8a0_1 & 0x7fff));
    system_reg_write(0x5880, 
        (data_ac7d0_1 & 0x7fff) << 0x10 | (param_defog_cent5_w_dis_array & 0x7fff));
    system_reg_write(0x5884, (data_ac7d8_1 & 0x7fff) << 0x10 | (data_ac7d4_1 & 0x7fff));
    system_reg_write(0x5888, (data_ac7e0_1 & 0x7fff) << 0x10 | (data_ac7dc_1 & 0x7fff));
    system_reg_write(0x588c, (data_ac7e8_1 & 0x7fff) << 0x10 | (data_ac7e4_1 & 0x7fff));
    system_reg_write(0x5890, (data_ac7f0_1 & 0x7fff) << 0x10 | (data_ac7ec_1 & 0x7fff));
    system_reg_write(0x5894, (data_ac7f8_1 & 0x7fff) << 0x10 | (data_ac7f4_1 & 0x7fff));
    system_reg_write(0x5898, (data_ac800_1 & 0x7fff) << 0x10 | (data_ac7fc_1 & 0x7fff));
    system_reg_write(0x589c, (data_ac808_1 & 0x7fff) << 0x10 | (data_ac804_1 & 0x7fff));
    system_reg_write(0x58a0, (data_ac810_1 & 0x7fff) << 0x10 | (data_ac80c_1 & 0x7fff));
    system_reg_write(0x58a4, (data_ac818_1 & 0x7fff) << 0x10 | (data_ac814_1 & 0x7fff));
    system_reg_write(0x58a8, (data_ac820_1 & 0x7fff) << 0x10 | (data_ac81c_1 & 0x7fff));
    system_reg_write(0x58ac, (data_ac828_1 & 0x7fff) << 0x10 | (data_ac824_1 & 0x7fff));
    system_reg_write(0x58b0, (data_ac830_1 & 0x7fff) << 0x10 | (data_ac82c_1 & 0x7fff));
    system_reg_write(0x58b4, (data_ac838_1 & 0x7fff) << 0x10 | (data_ac834_1 & 0x7fff));
    system_reg_write(0x58b8, (data_ac840_1 & 0x7fff) << 0x10 | (data_ac83c_1 & 0x7fff));
    system_reg_write(0x58bc, data_ac844_1 & 0x7fff);
    system_reg_write(0x58c0, 
        (data_aca2c_1 & 0x1f) << 8 | (data_aca30_1 & 0x1f) << 0x10 | (param_defog_weightlut02 & 0x1f)
            | (data_aca34_1 & 0x1f) << 0x18);
    system_reg_write(0x58c4, 
        (data_aca3c_1 & 0x1f) << 8 | (data_aca40_1 & 0x1f) << 0x10 | (data_aca38_1 & 0x1f)
            | (data_aca44_1 & 0x1f) << 0x18);
    system_reg_write(0x58c8, 
        (data_aca4c_1 & 0x1f) << 8 | (data_aca50_1 & 0x1f) << 0x10 | (data_aca48_1 & 0x1f)
            | (data_aca54_1 & 0x1f) << 0x18);
    system_reg_write(0x58cc, 
        (data_aca5c_1 & 0x1f) << 8 | (data_aca60_1 & 0x1f) << 0x10 | (data_aca58_1 & 0x1f)
            | (data_aca64_1 & 0x1f) << 0x18);
    system_reg_write(0x58d0, 
        (data_aca6c_1 & 0x1f) << 8 | (data_aca70_1 & 0x1f) << 0x10 | (data_aca68_1 & 0x1f)
            | (data_aca74_1 & 0x1f) << 0x18);
    system_reg_write(0x58d4, 
        (data_aca7c_1 & 0x1f) << 8 | (data_aca80_1 & 0x1f) << 0x10 | (data_aca78_1 & 0x1f)
            | (data_aca84_1 & 0x1f) << 0x18);
    system_reg_write(0x58d8, 
        (data_aca8c_1 & 0x1f) << 8 | (data_aca90_1[0] & 0x1f) << 0x10 | (data_aca88_1 & 0x1f)
            | (data_aca90_2[1] & 0x1f) << 0x18);
    int32_t $a1_114 = (data_aca90_3[3] & 0x1f) << 8 | (data_aca90_4[4] & 0x1f) << 0x10
        | (data_aca90_5[2] & 0x1f) | (data_aca90_6[5] & 0x1f) << 0x18;
    system_reg_write(0x58dc, $a1_114);
    system_reg_write(0x58e0, 
        (data_acaac_1 & 0x1f) << 8 | (data_acab0_1 & 0x1f) << 0x10 | (data_aca90_7[6] & 0x1f)
            | (data_acab4_1 & 0x1f) << 0x18);
    system_reg_write(0x58e4, 
        (data_acabc_1 & 0x1f) << 8 | (data_acac0_1 & 0x1f) << 0x10 | (data_acab8_1 & 0x1f)
            | (data_acac4_1 & 0x1f) << 0x18);
    system_reg_write(0x58e8, 
        (data_acacc_1 & 0x1f) << 8 | (data_acad0_1 & 0x1f) << 0x10 | (data_acac8_1 & 0x1f)
            | (data_acad4_1 & 0x1f) << 0x18);
    system_reg_write(0x58ec, 
        (data_acadc_1 & 0x1f) << 8 | (data_acae0_1 & 0x1f) << 0x10 | (data_acad8_1 & 0x1f)
            | (data_acae4_1 & 0x1f) << 0x18);
    system_reg_write(0x58f0, 
        (data_acaec_1 & 0x1f) << 8 | (data_acaf0_1 & 0x1f) << 0x10 | (data_acae8_1 & 0x1f)
            | (data_acaf4_1 & 0x1f) << 0x18);
    system_reg_write(0x58f4, 
        (data_acafc_1 & 0x1f) << 8 | (data_acb00_1 & 0x1f) << 0x10 | (data_acaf8_1 & 0x1f)
            | (data_acb04_1 & 0x1f) << 0x18);
    system_reg_write(0x58f8, 
        (data_acb0c_1 & 0x1f) << 8 | (data_acb10_1[0] & 0x1f) << 0x10 | (data_acb08_1 & 0x1f)
            | (data_acb10_2[1] & 0x1f) << 0x18);
    int32_t $a1_146 = (data_acb10_3[3] & 0x1f) << 8 | (data_acb10_4[4] & 0x1f) << 0x10
        | (data_acb10_5[2] & 0x1f) | (data_acb10_6[5] & 0x1f) << 0x18;
    system_reg_write(0x58fc, $a1_146);
    system_reg_write(0x5900, 
        (data_ac8ac_1 & 0x1f) << 8 | (data_ac8b0_1 & 0x1f) << 0x10 | (param_defog_weightlut21 & 0x1f)
            | (data_ac8b4_1 & 0x1f) << 0x18);
    system_reg_write(0x5904, 
        (data_ac8bc_1 & 0x1f) << 8 | (data_ac8c0_1 & 0x1f) << 0x10 | (data_ac8b8_1 & 0x1f)
            | (data_ac8c4_1 & 0x1f) << 0x18);
    system_reg_write(0x5908, 
        (data_ac8cc_1 & 0x1f) << 8 | (data_ac8d0_1 & 0x1f) << 0x10 | (data_ac8c8_1 & 0x1f)
            | (data_ac8d4_1 & 0x1f) << 0x18);
    system_reg_write(0x590c, 
        (data_ac8dc_1 & 0x1f) << 8 | (data_ac8e0_1 & 0x1f) << 0x10 | (data_ac8d8_1 & 0x1f)
            | (data_ac8e4_1 & 0x1f) << 0x18);
    system_reg_write(0x5910, 
        (data_ac8ec_1 & 0x1f) << 8 | (data_ac8f0_1 & 0x1f) << 0x10 | (data_ac8e8_1 & 0x1f)
            | (data_ac8f4_1 & 0x1f) << 0x18);
    system_reg_write(0x5914, 
        (data_ac8fc_1[0] & 0x1f) << 8 | (data_ac8fc_2[1] & 0x1f) << 0x10 | (data_ac8f8_1 & 0x1f)
            | (data_ac8fc_3[2] & 0x1f) << 0x18);
    int32_t $a1_174 = (data_ac8fc_4[4] & 0x1f) << 8 | (data_ac8fc_5[5] & 0x1f) << 0x10
        | (data_ac8fc_6[3] & 0x1f) | (data_ac8fc_7[6] & 0x1f) << 0x18;
    system_reg_write(0x5918, $a1_174);
    int32_t $a1_178 = (data_ac8fc_8[8] & 0x1f) << 8 | (data_ac8fc_9[9] & 0x1f) << 0x10
        | (data_ac8fc_10[7] & 0x1f) | (data_ac8fc_11[0xa] & 0x1f) << 0x18;
    system_reg_write(0x591c, $a1_178);
    system_reg_write(0x5920, 
        (data_ac9ac_1 & 0x1f) << 8 | (data_ac9b0_1 & 0x1f) << 0x10 | (param_defog_weightlut12 & 0x1f)
            | (data_ac9b4_1 & 0x1f) << 0x18);
    system_reg_write(0x5924, 
        (data_ac9bc_1 & 0x1f) << 8 | (data_ac9c0_1 & 0x1f) << 0x10 | (data_ac9b8_1 & 0x1f)
            | (data_ac9c4_1 & 0x1f) << 0x18);
    system_reg_write(0x5928, 
        (data_ac9cc_1 & 0x1f) << 8 | (data_ac9d0_1 & 0x1f) << 0x10 | (data_ac9c8_1 & 0x1f)
            | (data_ac9d4_1 & 0x1f) << 0x18);
    system_reg_write(0x592c, 
        (data_ac9dc_1 & 0x1f) << 8 | (data_ac9e0_1 & 0x1f) << 0x10 | (data_ac9d8_1 & 0x1f)
            | (data_ac9e4_1 & 0x1f) << 0x18);
    system_reg_write(0x5930, 
        (data_ac9ec_1 & 0x1f) << 8 | (data_ac9f0_1 & 0x1f) << 0x10 | (data_ac9e8_1 & 0x1f)
            | (data_ac9f4_1 & 0x1f) << 0x18);
    system_reg_write(0x5934, 
        (data_ac9fc_1 & 0x1f) << 8 | (data_aca00_1 & 0x1f) << 0x10 | (data_ac9f8_1 & 0x1f)
            | (data_aca04_1 & 0x1f) << 0x18);
    system_reg_write(0x5938, 
        (data_aca0c_1 & 0x1f) << 8 | (data_aca10_1 & 0x1f) << 0x10 | (data_aca08_1 & 0x1f)
            | (data_aca14_1 & 0x1f) << 0x18);
    system_reg_write(0x593c, 
        (data_aca1c_1 & 0x1f) << 8 | (data_aca20_1 & 0x1f) << 0x10 | (data_aca18_1 & 0x1f)
            | (data_aca24_1 & 0x1f) << 0x18);
    system_reg_write(0x5940, 
        (data_ac92c_1 & 0x1f) << 8 | (data_ac930_1 & 0x1f) << 0x10 | (data_ac8fc_12[0xb] & 0x1f)
            | (data_ac934_1 & 0x1f) << 0x18);
    system_reg_write(0x5944, 
        (data_ac93c_1 & 0x1f) << 8 | (data_ac940_1 & 0x1f) << 0x10 | (data_ac938_1 & 0x1f)
            | (data_ac944_1 & 0x1f) << 0x18);
    system_reg_write(0x5948, 
        (data_ac94c_1 & 0x1f) << 8 | (data_ac950_1 & 0x1f) << 0x10 | (data_ac948_1 & 0x1f)
            | (data_ac954_1 & 0x1f) << 0x18);
    system_reg_write(0x594c, 
        (data_ac95c_1 & 0x1f) << 8 | (data_ac960_1 & 0x1f) << 0x10 | (data_ac958_1 & 0x1f)
            | (data_ac964_1 & 0x1f) << 0x18);
    system_reg_write(0x5950, 
        (data_ac96c_1 & 0x1f) << 8 | (data_ac970_1 & 0x1f) << 0x10 | (data_ac968_1 & 0x1f)
            | (data_ac974_1 & 0x1f) << 0x18);
    system_reg_write(0x5954, 
        (data_ac97c_1 & 0x1f) << 8 | (data_ac980_1 & 0x1f) << 0x10 | (data_ac978_1 & 0x1f)
            | (data_ac984_1 & 0x1f) << 0x18);
    system_reg_write(0x5958, 
        (data_ac98c_1 & 0x1f) << 8 | (data_ac990_1 & 0x1f) << 0x10 | (data_ac988_1 & 0x1f)
            | (data_ac994_1 & 0x1f) << 0x18);
    system_reg_write(0x595c, 
        (data_ac99c_1 & 0x1f) << 8 | (data_ac9a0_1 & 0x1f) << 0x10 | (data_ac998_1 & 0x1f)
            | (data_ac9a4_1 & 0x1f) << 0x18);
    void* param_defog_main_para_array_now_1 = param_defog_main_para_array_now;
    system_reg_write(0x5a00, 
        *(param_defog_main_para_array_now_1 + 4) << 8 | *param_defog_main_para_array_now_1);
    void* param_defog_main_para_array_now_2 = param_defog_main_para_array_now;
    int32_t $v0_329 = (*(param_defog_main_para_array_now_2 + 0xc) & 0xf) << 2
        | (*(param_defog_main_para_array_now_2 + 0x10) & 0x1f) << 8
        | (*(param_defog_main_para_array_now_2 + 8) & 1)
        | (*(param_defog_main_para_array_now_2 + 0x14) & 0x1f) << 0x10;
    system_reg_write(0x5a04, 
        $v0_329 | (*(param_defog_main_para_array_now_2 + 0x18) & 0x1f) << 0x18);
    void* param_defog_main_para_array_now_3 = param_defog_main_para_array_now;
    int32_t $a1_253 = (*(param_defog_main_para_array_now_3 + 0x20) & 0x1f) << 8
        | (*(param_defog_main_para_array_now_3 + 0x24) & 0x1f) << 0x10
        | (*(param_defog_main_para_array_now_3 + 0x1c) & 0x1f)
        | (*(param_defog_main_para_array_now_3 + 0x28) & 0x1f) << 0x18;
    system_reg_write(0x5a08, $a1_253);
    system_reg_write(0x5a10, 
        param_defog_color_control_array | data_acc14_1[1] << 0x18 | data_acc10_1 << 8
            | data_acc14_2[0] << 0x10);
    int32_t $a1_260 = (data_acc14_3[4] & 0x3f) << 8 | (data_acc14_4[5] & 0x3f) << 0x10
        | (data_acc14_5[3] & 0x3f) | (data_acc14_6[6] & 0x3f) << 0x18;
    system_reg_write(0x5a14, $a1_260);
    system_reg_write(0x5a18, (data_acc14_7[7] & 0x3f) << 0x10 | data_acc14_8[2]);
    system_reg_write(0x5a1c, 
        (data_acc38_1 & 7) << 8 | (data_acc3c_1 & 7) << 0x10 | (data_acc34_1 & 7)
            | (data_acc40_1 & 7) << 0x18);
    system_reg_write(0x5a20, 
        (data_acbe8_1 & 7) << 8 | (data_acbec_1 & 7) << 0x10 | (param_defog_lc_s_array & 7)
            | (data_acbf0_1 & 7) << 0x18);
    system_reg_write(0x5a24, 
        (data_acbf8_1 & 0x1f) << 8 | (data_acbfc_1 & 0x1f) << 0x10 | (data_acbf4_1 & 0x1f));
    system_reg_write(0x5a28, 
        (data_acc04_1 & 0x3f) << 8 | (data_acc08_1 & 0x3f) << 0x10 | (data_acc00_1 & 0x3f));
    system_reg_write(0x5a30, 
        (data_acbc0_1 & 7) << 8 | (data_acbc4_1 & 7) << 0x10 | (param_defog_lc_v_array & 7)
            | (data_acbc8_1 & 7) << 0x18);
    system_reg_write(0x5a34, data_acbd0_1 << 8 | data_acbd4_1 << 0x10 | data_acbcc_1);
    system_reg_write(0x5a38, 
        (data_acbdc_1 & 0x3f) << 8 | (data_acbe0_1 & 0x3f) << 0x10 | (data_acbd8_1 & 0x3f));
    system_reg_write(0x5a40, (data_acba0_1 & 7) << 8 | (param_defog_cc_s_array & 7));
    system_reg_write(0x5a44, 
        (data_acba8_1 & 0x1f) << 8 | (data_acbac_1 & 0x1f) << 0x10 | (data_acba4_1 & 0x1f));
    system_reg_write(0x5a48, 
        (data_acbb4_1 & 0x3f) << 8 | (data_acbb8_1 & 0x3f) << 0x10 | (data_acbb0_1 & 0x3f));
    system_reg_write(0x5a50, 
        (data_acb7c_1 & 7) << 8 | (data_acb80_1 & 7) << 0x10 | (param_defog_cc_v_array & 7));
    system_reg_write(0x5a54, data_acb88_1 << 8 | data_acb8c_1 << 0x10 | data_acb84_1);
    system_reg_write(0x5a58, 
        (data_acb94_1 & 0x3f) << 8 | (data_acb98_1 & 0x3f) << 0x10 | (data_acb90_1 & 0x3f));
    system_reg_write(0x5a60, 
        (data_acb54_1 & 7) << 8 | (data_acb58_1 & 7) << 0x10 | (param_defog_dark_l1_array & 7));
    system_reg_write(0x5a64, data_acb60_1 << 8 | data_acb64_1 << 0x10 | data_acb5c_1);
    system_reg_write(0x5a68, 
        (data_acb6c_1 & 0x3f) << 8 | (data_acb70_1 & 0x3f) << 0x10 | (data_acb68_1 & 0x3f)
            | (data_acb74_1 & 0x3f) << 0x18);
    system_reg_write(0x5a70, 
        (data_acb2c_1 & 7) << 8 | (data_acb30_1 & 7) << 0x10 | (param_defog_dark_l2_array & 7));
    system_reg_write(0x5a74, data_acb38_1 << 8 | data_acb3c_1 << 0x10 | data_acb34_1);
    system_reg_write(0x5a78, 
        (data_acb44_1 & 0x3f) << 8 | (data_acb48_1 & 0x3f) << 0x10 | (data_acb40_1 & 0x3f)
            | (data_acb4c_1 & 0x3f) << 0x18);
    void* param_defog_fpga_para_array_now_1 = param_defog_fpga_para_array_now;
    system_reg_write(0x5b10, 
        *(param_defog_fpga_para_array_now_1 + 8) << 0x10
            | *(param_defog_fpga_para_array_now_1 + 4));
    return 0;
}

