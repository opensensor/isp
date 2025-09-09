#include "include/main.h"


  int32_t tiziano_awb_set_hardware_param()

{
    if (!awb_first)
    {
        awb_first = 1;
        system_reg_write(0xb004, 
            data_a9fc8 << 0x1c | data_a9fc4 << 0x10 | _awb_parameter | data_a9fc0 << 0xc);
        system_reg_write(0xb008, 
            data_a9fcc[3] << 0x18 | data_a9fcc[2] << 0x10 | data_a9fcc[0] | data_a9fcc[1] << 8);
        system_reg_write(0xb00c, 
            data_a9fcc[7] << 0x18 | data_a9fcc[6] << 0x10 | data_a9fcc[4] | data_a9fcc[5] << 8);
        system_reg_write(0xb010, 
            data_a9fcc[0xb] << 0x18 | data_a9fcc[0xa] << 0x10 | data_a9fcc[8] | data_a9fcc[9] << 8);
        system_reg_write(0xb014, data_a9fcc[0xe] << 0x10 | data_a9fcc[0xd] << 8 | data_a9fcc[0xc]);
        system_reg_write(0xb018, 
            data_a9fcc[0x12] << 0x18 | data_a9fcc[0x11] << 0x10 | data_a9fcc[0xf]
                | data_a9fcc[0x10] << 8);
        system_reg_write(0xb01c, 
            data_a9fcc[0x16] << 0x18 | data_a9fcc[0x15] << 0x10 | data_a9fcc[0x13]
                | data_a9fcc[0x14] << 8);
        system_reg_write(0xb020, 
            data_a9fcc[0x1a] << 0x18 | data_a9fcc[0x19] << 0x10 | data_a9fcc[0x17]
                | data_a9fcc[0x18] << 8);
        system_reg_write(0xb024, 
            data_a9fcc[0x1d] << 0x10 | data_a9fcc[0x1c] << 8 | data_a9fcc[0x1b]);
    }
    
    if (data_a9f68_3)
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
            $v0_47 = data_aa058;
        }
        else if (ModeFlag_1)
            $v0_47 = data_aa058_1;
        else
        {
            system_reg_write_awb(1, 0xb028, data_aa048 << 0x10 | data_aa044);
            system_reg_write_awb(1, 0xb02c, data_aa050 << 0x10 | data_aa04c);
            $v0_47 = data_aa058;
        }
        
        system_reg_write_awb(1, 0xb030, $v0_47 << 0x10 | data_aa054_1);
        system_reg_write_awb(1, 0xb034, data_aa060_1 << 0x10 | data_aa05c_1);
        private_clk_put();
    }
    
    return 0;
}

