#include "include/main.h"


  int32_t tiziano_awb_set_hardware_param()

{
    if (!awb_first)
    {
        awb_first = 1;
        system_reg_write(0xb004, 
            data_a9fc8_2 << 0x1c | data_a9fc4_1 << 0x10 | _awb_parameter | data_a9fc0_2 << 0xc);
        system_reg_write(0xb008, 
            data_a9fcc_1[3] << 0x18 | data_a9fcc_2[2] << 0x10 | data_a9fcc_3[0] | data_a9fcc_4[1] << 8);
        system_reg_write(0xb00c, 
            data_a9fcc_5[7] << 0x18 | data_a9fcc_6[6] << 0x10 | data_a9fcc_7[4] | data_a9fcc_8[5] << 8);
        system_reg_write(0xb010, 
            data_a9fcc_9[0xb] << 0x18 | data_a9fcc_10[0xa] << 0x10 | data_a9fcc_11[8] | data_a9fcc_12[9] << 8);
        system_reg_write(0xb014, data_a9fcc_13[0xe] << 0x10 | data_a9fcc_14[0xd] << 8 | data_a9fcc_15[0xc]);
        system_reg_write(0xb018, 
            data_a9fcc_16[0x12] << 0x18 | data_a9fcc_17[0x11] << 0x10 | data_a9fcc_18[0xf]
                | data_a9fcc_19[0x10] << 8);
        system_reg_write(0xb01c, 
            data_a9fcc_20[0x16] << 0x18 | data_a9fcc_21[0x15] << 0x10 | data_a9fcc_22[0x13]
                | data_a9fcc_23[0x14] << 8);
        system_reg_write(0xb020, 
            data_a9fcc_24[0x1a] << 0x18 | data_a9fcc_25[0x19] << 0x10 | data_a9fcc_26[0x17]
                | data_a9fcc_27[0x18] << 8);
        system_reg_write(0xb024, 
            data_a9fcc_28[0x1d] << 0x10 | data_a9fcc_29[0x1c] << 8 | data_a9fcc_30[0x1b]);
    }
    
    if (data_a9f68_5)
    {
        system_reg_write_awb(1, 0xb028, 0xfff0001);
        system_reg_write_awb(1, 0xb02c, 0xfff0001);
        system_reg_write_awb(1, 0xb030, 0x100);
        system_reg_write_awb(1, 0xb034, 0xffff0100);
    }
    else
    {
        uint32_t ModeFlag_1 = ModeFlag;
        int32_t $v0_47;
        
        if (ModeFlag_1 == 1)
        {
            system_reg_write_awb(1, 0xb028, 
                *(_awb_lowlight_rg_th + 4) << 0x10 | *_awb_lowlight_rg_th);
            system_reg_write_awb(1, 0xb02c, 0x3ff0001);
            $v0_47 = data_aa058_1;
        }
        else if (ModeFlag_1)
            $v0_47 = data_aa058_2;
        else
        {
            system_reg_write_awb(1, 0xb028, data_aa048_2 << 0x10 | data_aa044_2);
            system_reg_write_awb(1, 0xb02c, data_aa050_2 << 0x10 | data_aa04c_2);
            $v0_47 = data_aa058_3;
        }
        
        system_reg_write_awb(1, 0xb030, $v0_47 << 0x10 | data_aa054_1);
        system_reg_write_awb(1, 0xb034, data_aa060_1 << 0x10 | data_aa05c_1);
        private_clk_put();
    }
    
    return 0;
}

