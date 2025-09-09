#include "include/main.h"


  int32_t Tiziano_awb_set_gain(void* arg1, int32_t arg2, int32_t* arg3)

{
    uint32_t $a1 = *(arg1 + 0x10);
    int32_t $v0 = *(arg1 + 0x14);
    int32_t $a2 = *arg3;
    uint32_t var_20 = $a1;
    int32_t $v0_1 = fix_point_mult2_32(arg2, $a1 << (arg2 & 0x1f), $a2);
    int32_t $s0_1 = 1 << ((arg2 - 1) & 0x1f);
    int32_t $a2_1 = arg3[1];
    int32_t $v0_4 = fix_point_mult2_32(arg2, $v0 << (arg2 & 0x1f), $a2_1);
    uint32_t $a2_2 = var_20;
    uint32_t $v0_6 = ($s0_1 + $v0_4) >> (arg2 & 0x1f);
    uint32_t var_1c_1 = $v0_6;
    int32_t tisp_wb_attr_1 = tisp_wb_attr;
    uint32_t var_1c_2;
    var_20 = ($v0_1 + $s0_1) >> (arg2 & 0x1f);
    data_b5a48 = isp_printf / $a2_2;
    data_b5a4c = isp_printf / $v0_6;
    
    if (tisp_wb_attr_1 - 1 < 8)
    {
        var_20 = data_b5a38;
        var_1c_2 = data_b5a3c;
    }
    else if (tisp_wb_attr_1 == 9)
    {
        var_20 = ((data_b5a38 + 0x40) * $a2_2) >> 6;
        var_1c_2 = ((data_b5a3c + 0x40) * $v0_6) >> 6;
    }
    int32_t var_28;
    JZ_Isp_Awb_Awbg2reg(&var_20_13, &var_28_1);
    
    if (!awb_frz)
    {
        int32_t var_24;
        system_reg_write_awb(2, 0x1804, var_28);
        system_reg_write_awb(2, 0x1808, var_24);
        system_reg_write_awb(2, 0x180c, var_28);
        system_reg_write_awb(2, 0x1810, var_24);
        tisp_rdns_awb_gain_updata(*var_28[2], *var_24[2]);
    }
    
    awb_moa = 0;
    return &data_b0000_2;
}

