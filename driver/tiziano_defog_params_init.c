#include "include/main.h"


  int32_t tiziano_defog_params_init()

{
    int32_t $a1_114 = (data_aca90[3] & 0x1f) << 8 | (data_aca90[4] & 0x1f) << 0x10
    int32_t $a1_146 = (data_acb10[3] & 0x1f) << 8 | (data_acb10[4] & 0x1f) << 0x10
    int32_t $a1_174 = (data_ac8fc[4] & 0x1f) << 8 | (data_ac8fc[5] & 0x1f) << 0x10
    int32_t $a1_178 = (data_ac8fc[8] & 0x1f) << 8 | (data_ac8fc[9] & 0x1f) << 0x10
    char* param_defog_main_para_array_now_1 = (char*)(param_defog_main_para_array_now); // Fixed void pointer assignment
    char* param_defog_main_para_array_now_2 = (char*)(param_defog_main_para_array_now); // Fixed void pointer assignment
    int32_t $v0_329 = (*(param_defog_main_para_array_now_2 + 0xc) & 0xf) << 2
    char* param_defog_main_para_array_now_3 = (char*)(param_defog_main_para_array_now); // Fixed void pointer assignment
    int32_t $a1_253 = (*(param_defog_main_para_array_now_3 + 0x20) & 0x1f) << 8
    int32_t $a1_260 = (data_acc14[4] & 0x3f) << 8 | (data_acc14[5] & 0x3f) << 0x10
    char* param_defog_fpga_para_array_now_1 = (char*)(param_defog_fpga_para_array_now); // Fixed void pointer assignment
    return 0;
    system_reg_write(0x5850, 
        (data_ac84c[0] & 0x7fff) << 0x10 | (param_defog_cent3_w_dis_array & 0x7fff));
    system_reg_write(0x5854, (data_ac84c[2] & 0x7fff) << 0x10 | (data_ac84c[1] & 0x7fff));
    system_reg_write(0x5858, (data_ac84c[4] & 0x7fff) << 0x10 | (data_ac84c[3] & 0x7fff));
    system_reg_write(0x585c, (data_ac864 & 0x7fff) << 0x10 | (data_ac860 & 0x7fff));
    system_reg_write(0x5860, (data_ac86c & 0x7fff) << 0x10 | (data_ac868 & 0x7fff));
    system_reg_write(0x5864, (data_ac874 & 0x7fff) << 0x10 | (data_ac870 & 0x7fff));
    system_reg_write(0x5868, (data_ac87c & 0x7fff) << 0x10 | (data_ac878 & 0x7fff));
    system_reg_write(0x586c, (data_ac884 & 0x7fff) << 0x10 | (data_ac880 & 0x7fff));
    system_reg_write(0x5870, (data_ac88c & 0x7fff) << 0x10 | (data_ac888 & 0x7fff));
    system_reg_write(0x5874, (data_ac894 & 0x7fff) << 0x10 | (data_ac890 & 0x7fff));
    system_reg_write(0x5878, (data_ac89c & 0x7fff) << 0x10 | (data_ac898 & 0x7fff));
    system_reg_write(0x587c, (data_ac8a4 & 0x7fff) << 0x10 | (data_ac8a0 & 0x7fff));
    system_reg_write(0x5880, 
        (data_ac7d0 & 0x7fff) << 0x10 | (param_defog_cent5_w_dis_array & 0x7fff));
    system_reg_write(0x5884, (data_ac7d8 & 0x7fff) << 0x10 | (data_ac7d4 & 0x7fff));
    system_reg_write(0x5888, (data_ac7e0 & 0x7fff) << 0x10 | (data_ac7dc & 0x7fff));
    system_reg_write(0x588c, (data_ac7e8 & 0x7fff) << 0x10 | (data_ac7e4 & 0x7fff));
    system_reg_write(0x5890, (data_ac7f0 & 0x7fff) << 0x10 | (data_ac7ec & 0x7fff));
    system_reg_write(0x5894, (data_ac7f8 & 0x7fff) << 0x10 | (data_ac7f4 & 0x7fff));
    system_reg_write(0x5898, (data_ac800 & 0x7fff) << 0x10 | (data_ac7fc & 0x7fff));
    system_reg_write(0x589c, (data_ac808 & 0x7fff) << 0x10 | (data_ac804 & 0x7fff));
    system_reg_write(0x58a0, (data_ac810 & 0x7fff) << 0x10 | (data_ac80c & 0x7fff));
    system_reg_write(0x58a4, (data_ac818 & 0x7fff) << 0x10 | (data_ac814 & 0x7fff));
    system_reg_write(0x58a8, (data_ac820 & 0x7fff) << 0x10 | (data_ac81c & 0x7fff));
    system_reg_write(0x58ac, (data_ac828 & 0x7fff) << 0x10 | (data_ac824 & 0x7fff));
    system_reg_write(0x58b0, (data_ac830 & 0x7fff) << 0x10 | (data_ac82c & 0x7fff));
    system_reg_write(0x58b4, (data_ac838 & 0x7fff) << 0x10 | (data_ac834 & 0x7fff));
    system_reg_write(0x58b8, (data_ac840 & 0x7fff) << 0x10 | (data_ac83c & 0x7fff));
    system_reg_write(0x58bc, data_ac844 & 0x7fff);
    system_reg_write(0x58c0, 
        (data_aca2c & 0x1f) << 8 | (data_aca30 & 0x1f) << 0x10 | (param_defog_weightlut02 & 0x1f)
            | (data_aca34 & 0x1f) << 0x18);
    system_reg_write(0x58c4, 
        (data_aca3c & 0x1f) << 8 | (data_aca40 & 0x1f) << 0x10 | (data_aca38 & 0x1f)
            | (data_aca44 & 0x1f) << 0x18);
    system_reg_write(0x58c8, 
        (data_aca4c & 0x1f) << 8 | (data_aca50 & 0x1f) << 0x10 | (data_aca48 & 0x1f)
            | (data_aca54 & 0x1f) << 0x18);
    system_reg_write(0x58cc, 
        (data_aca5c & 0x1f) << 8 | (data_aca60 & 0x1f) << 0x10 | (data_aca58 & 0x1f)
            | (data_aca64 & 0x1f) << 0x18);
    system_reg_write(0x58d0, 
        (data_aca6c & 0x1f) << 8 | (data_aca70 & 0x1f) << 0x10 | (data_aca68 & 0x1f)
            | (data_aca74 & 0x1f) << 0x18);
    system_reg_write(0x58d4, 
        (data_aca7c & 0x1f) << 8 | (data_aca80 & 0x1f) << 0x10 | (data_aca78 & 0x1f)
            | (data_aca84 & 0x1f) << 0x18);
    system_reg_write(0x58d8, 
        (data_aca8c & 0x1f) << 8 | (data_aca90[0] & 0x1f) << 0x10 | (data_aca88 & 0x1f)
            | (data_aca90[1] & 0x1f) << 0x18);
        | (data_aca90[2] & 0x1f) | (data_aca90[5] & 0x1f) << 0x18;
    system_reg_write(0x58dc, $a1_114);
    system_reg_write(0x58e0, 
        (data_acaac & 0x1f) << 8 | (data_acab0 & 0x1f) << 0x10 | (data_aca90[6] & 0x1f)
            | (data_acab4 & 0x1f) << 0x18);
    system_reg_write(0x58e4, 
        (data_acabc & 0x1f) << 8 | (data_acac0 & 0x1f) << 0x10 | (data_acab8 & 0x1f)
            | (data_acac4 & 0x1f) << 0x18);
    system_reg_write(0x58e8, 
        (data_acacc & 0x1f) << 8 | (data_acad0 & 0x1f) << 0x10 | (data_acac8 & 0x1f)
            | (data_acad4 & 0x1f) << 0x18);
    system_reg_write(0x58ec, 
        (data_acadc & 0x1f) << 8 | (data_acae0 & 0x1f) << 0x10 | (data_acad8 & 0x1f)
            | (data_acae4 & 0x1f) << 0x18);
    system_reg_write(0x58f0, 
        (data_acaec & 0x1f) << 8 | (data_acaf0 & 0x1f) << 0x10 | (data_acae8 & 0x1f)
            | (data_acaf4 & 0x1f) << 0x18);
    system_reg_write(0x58f4, 
        (data_acafc & 0x1f) << 8 | (data_acb00 & 0x1f) << 0x10 | (data_acaf8 & 0x1f)
            | (data_acb04 & 0x1f) << 0x18);
    system_reg_write(0x58f8, 
        (data_acb0c & 0x1f) << 8 | (data_acb10[0] & 0x1f) << 0x10 | (data_acb08 & 0x1f)
            | (data_acb10[1] & 0x1f) << 0x18);
        | (data_acb10[2] & 0x1f) | (data_acb10[5] & 0x1f) << 0x18;
    system_reg_write(0x58fc, $a1_146);
    system_reg_write(0x5900, 
        (data_ac8ac & 0x1f) << 8 | (data_ac8b0 & 0x1f) << 0x10 | (param_defog_weightlut21 & 0x1f)
            | (data_ac8b4 & 0x1f) << 0x18);
    system_reg_write(0x5904, 
        (data_ac8bc & 0x1f) << 8 | (data_ac8c0 & 0x1f) << 0x10 | (data_ac8b8 & 0x1f)
            | (data_ac8c4 & 0x1f) << 0x18);
    system_reg_write(0x5908, 
        (data_ac8cc & 0x1f) << 8 | (data_ac8d0 & 0x1f) << 0x10 | (data_ac8c8 & 0x1f)
            | (data_ac8d4 & 0x1f) << 0x18);
    system_reg_write(0x590c, 
        (data_ac8dc & 0x1f) << 8 | (data_ac8e0 & 0x1f) << 0x10 | (data_ac8d8 & 0x1f)
            | (data_ac8e4 & 0x1f) << 0x18);
    system_reg_write(0x5910, 
        (data_ac8ec & 0x1f) << 8 | (data_ac8f0 & 0x1f) << 0x10 | (data_ac8e8 & 0x1f)
            | (data_ac8f4 & 0x1f) << 0x18);
    system_reg_write(0x5914, 
        (data_ac8fc[0] & 0x1f) << 8 | (data_ac8fc[1] & 0x1f) << 0x10 | (data_ac8f8 & 0x1f)
            | (data_ac8fc[2] & 0x1f) << 0x18);
        | (data_ac8fc[3] & 0x1f) | (data_ac8fc[6] & 0x1f) << 0x18;
    system_reg_write(0x5918, $a1_174);
        | (data_ac8fc[7] & 0x1f) | (data_ac8fc[0xa] & 0x1f) << 0x18;
    system_reg_write(0x591c, $a1_178);
    system_reg_write(0x5920, 
        (data_ac9ac & 0x1f) << 8 | (data_ac9b0 & 0x1f) << 0x10 | (param_defog_weightlut12 & 0x1f)
            | (data_ac9b4 & 0x1f) << 0x18);
    system_reg_write(0x5924, 
        (data_ac9bc & 0x1f) << 8 | (data_ac9c0 & 0x1f) << 0x10 | (data_ac9b8 & 0x1f)
            | (data_ac9c4 & 0x1f) << 0x18);
    system_reg_write(0x5928, 
        (data_ac9cc & 0x1f) << 8 | (data_ac9d0 & 0x1f) << 0x10 | (data_ac9c8 & 0x1f)
            | (data_ac9d4 & 0x1f) << 0x18);
    system_reg_write(0x592c, 
        (data_ac9dc & 0x1f) << 8 | (data_ac9e0 & 0x1f) << 0x10 | (data_ac9d8 & 0x1f)
            | (data_ac9e4 & 0x1f) << 0x18);
    system_reg_write(0x5930, 
        (data_ac9ec & 0x1f) << 8 | (data_ac9f0 & 0x1f) << 0x10 | (data_ac9e8 & 0x1f)
            | (data_ac9f4 & 0x1f) << 0x18);
    system_reg_write(0x5934, 
        (data_ac9fc & 0x1f) << 8 | (data_aca00 & 0x1f) << 0x10 | (data_ac9f8 & 0x1f)
            | (data_aca04 & 0x1f) << 0x18);
    system_reg_write(0x5938, 
        (data_aca0c & 0x1f) << 8 | (data_aca10 & 0x1f) << 0x10 | (data_aca08 & 0x1f)
            | (data_aca14 & 0x1f) << 0x18);
    system_reg_write(0x593c, 
        (data_aca1c & 0x1f) << 8 | (data_aca20 & 0x1f) << 0x10 | (data_aca18 & 0x1f)
            | (data_aca24 & 0x1f) << 0x18);
    system_reg_write(0x5940, 
        (data_ac92c & 0x1f) << 8 | (data_ac930 & 0x1f) << 0x10 | (data_ac8fc[0xb] & 0x1f)
            | (data_ac934 & 0x1f) << 0x18);
    system_reg_write(0x5944, 
        (data_ac93c & 0x1f) << 8 | (data_ac940 & 0x1f) << 0x10 | (data_ac938 & 0x1f)
            | (data_ac944 & 0x1f) << 0x18);
    system_reg_write(0x5948, 
        (data_ac94c & 0x1f) << 8 | (data_ac950 & 0x1f) << 0x10 | (data_ac948 & 0x1f)
            | (data_ac954 & 0x1f) << 0x18);
    system_reg_write(0x594c, 
        (data_ac95c & 0x1f) << 8 | (data_ac960 & 0x1f) << 0x10 | (data_ac958 & 0x1f)
            | (data_ac964 & 0x1f) << 0x18);
    system_reg_write(0x5950, 
        (data_ac96c & 0x1f) << 8 | (data_ac970 & 0x1f) << 0x10 | (data_ac968 & 0x1f)
            | (data_ac974 & 0x1f) << 0x18);
    system_reg_write(0x5954, 
        (data_ac97c & 0x1f) << 8 | (data_ac980 & 0x1f) << 0x10 | (data_ac978 & 0x1f)
            | (data_ac984 & 0x1f) << 0x18);
    system_reg_write(0x5958, 
        (data_ac98c & 0x1f) << 8 | (data_ac990 & 0x1f) << 0x10 | (data_ac988 & 0x1f)
            | (data_ac994 & 0x1f) << 0x18);
    system_reg_write(0x595c, 
        (data_ac99c & 0x1f) << 8 | (data_ac9a0 & 0x1f) << 0x10 | (data_ac998 & 0x1f)
            | (data_ac9a4 & 0x1f) << 0x18);
    system_reg_write(0x5a00, 
        *(param_defog_main_para_array_now_1 + 4) << 8 | *param_defog_main_para_array_now_1);
        | (*(param_defog_main_para_array_now_2 + 0x10) & 0x1f) << 8
        | (*(param_defog_main_para_array_now_2 + 8) & 1)
        | (*(param_defog_main_para_array_now_2 + 0x14) & 0x1f) << 0x10;
    system_reg_write(0x5a04, 
        $v0_329 | (*(param_defog_main_para_array_now_2 + 0x18) & 0x1f) << 0x18);
        | (*(param_defog_main_para_array_now_3 + 0x24) & 0x1f) << 0x10
        | (*(param_defog_main_para_array_now_3 + 0x1c) & 0x1f)
        | (*(param_defog_main_para_array_now_3 + 0x28) & 0x1f) << 0x18;
    system_reg_write(0x5a08, $a1_253);
    system_reg_write(0x5a10, 
        param_defog_color_control_array | data_acc14[1] << 0x18 | data_acc10 << 8
            | data_acc14[0] << 0x10);
        | (data_acc14[3] & 0x3f) | (data_acc14[6] & 0x3f) << 0x18;
    system_reg_write(0x5a14, $a1_260);
    system_reg_write(0x5a18, (data_acc14[7] & 0x3f) << 0x10 | data_acc14[2]);
    system_reg_write(0x5a1c, 
        (data_acc38 & 7) << 8 | (data_acc3c & 7) << 0x10 | (data_acc34 & 7)
            | (data_acc40 & 7) << 0x18);
    system_reg_write(0x5a20, 
        (data_acbe8 & 7) << 8 | (data_acbec & 7) << 0x10 | (param_defog_lc_s_array & 7)
            | (data_acbf0 & 7) << 0x18);
    system_reg_write(0x5a24, 
        (data_acbf8 & 0x1f) << 8 | (data_acbfc & 0x1f) << 0x10 | (data_acbf4 & 0x1f));
    system_reg_write(0x5a28, 
        (data_acc04 & 0x3f) << 8 | (data_acc08 & 0x3f) << 0x10 | (data_acc00 & 0x3f));
    system_reg_write(0x5a30, 
        (data_acbc0 & 7) << 8 | (data_acbc4 & 7) << 0x10 | (param_defog_lc_v_array & 7)
            | (data_acbc8 & 7) << 0x18);
    system_reg_write(0x5a34, data_acbd0 << 8 | data_acbd4 << 0x10 | data_acbcc);
    system_reg_write(0x5a38, 
        (data_acbdc & 0x3f) << 8 | (data_acbe0 & 0x3f) << 0x10 | (data_acbd8 & 0x3f));
    system_reg_write(0x5a40, (data_acba0 & 7) << 8 | (param_defog_cc_s_array & 7));
    system_reg_write(0x5a44, 
        (data_acba8 & 0x1f) << 8 | (data_acbac & 0x1f) << 0x10 | (data_acba4 & 0x1f));
    system_reg_write(0x5a48, 
        (data_acbb4 & 0x3f) << 8 | (data_acbb8 & 0x3f) << 0x10 | (data_acbb0 & 0x3f));
    system_reg_write(0x5a50, 
        (data_acb7c & 7) << 8 | (data_acb80 & 7) << 0x10 | (param_defog_cc_v_array & 7));
    system_reg_write(0x5a54, data_acb88 << 8 | data_acb8c << 0x10 | data_acb84);
    system_reg_write(0x5a58, 
        (data_acb94 & 0x3f) << 8 | (data_acb98 & 0x3f) << 0x10 | (data_acb90 & 0x3f));
    system_reg_write(0x5a60, 
        (data_acb54 & 7) << 8 | (data_acb58 & 7) << 0x10 | (param_defog_dark_l1_array & 7));
    system_reg_write(0x5a64, data_acb60 << 8 | data_acb64 << 0x10 | data_acb5c);
    system_reg_write(0x5a68, 
        (data_acb6c & 0x3f) << 8 | (data_acb70 & 0x3f) << 0x10 | (data_acb68 & 0x3f)
            | (data_acb74 & 0x3f) << 0x18);
    system_reg_write(0x5a70, 
        (data_acb2c & 7) << 8 | (data_acb30 & 7) << 0x10 | (param_defog_dark_l2_array & 7));
    system_reg_write(0x5a74, data_acb38 << 8 | data_acb3c << 0x10 | data_acb34);
    system_reg_write(0x5a78, 
        (data_acb44 & 0x3f) << 8 | (data_acb48 & 0x3f) << 0x10 | (data_acb40 & 0x3f)
            | (data_acb4c & 0x3f) << 0x18);
    system_reg_write(0x5b10, 
        *(param_defog_fpga_para_array_now_1 + 8) << 0x10
            | *(param_defog_fpga_para_array_now_1 + 4));
}

