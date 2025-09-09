#include "include/main.h"


  int32_t tisp_s_fcrop_control(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5)

{
    uint32_t msca_ch_en_1 = msca_ch_en;
    int32_t arg_0 = arg1;
    
    if (!~msca_ch_en_1)
        msca_ch_en_1 = 0;
    
    int32_t arg_4 = arg2;
    int32_t arg_8 = arg3;
    int32_t arg_c = arg4;
    msca_ch_en = msca_ch_en_1;
    uint32_t msca_ch_en_4;
    
    if (!(arg1 & 0xff))
    {
        isp_printf(2, "The parameter is invalid!\\n", arg3);
        msca_ch_en_4 = msca_ch_en;
    }
    else
    {
        data_b2e08_3 = arg3;
        data_b2e0c_3 = arg2;
        data_b2e10_4 = arg4;
        data_b2e04_2 = 1;
        data_b2e14_4 = arg5;
        system_reg_write(0x9860, arg3 << 0x10 | arg2);
        system_reg_write(0x9864, arg4 << 0x10 | arg5);
        uint32_t msca_ch_en_2 = msca_ch_en;
        
        if (msca_ch_en & 1)
        {
            system_reg_write(0x9904, 
                ((arg4 << 9) / data_b2de8_1) << 0x10 | ((arg5 << 9) / data_b2dec_1));
            msca_ch_en_2 = msca_ch_en;
        }
        
        uint32_t msca_ch_en_3 = msca_ch_en;
        
        if (msca_ch_en_2 & 2)
        {
            system_reg_write(0x9a04, 
                ((arg4 << 9) / data_b2db4_1) << 0x10 | ((arg5 << 9) / data_b2db8_1));
            msca_ch_en_3 = msca_ch_en;
        }
        
        if (!(msca_ch_en_3 & 4))
            msca_ch_en_4 = msca_ch_en;
        else
        {
            system_reg_write(0x9b04, 
                ((arg4 << 9) / data_b2d80_1) << 0x10 | ((arg5 << 9) / data_b2d84_1));
            msca_ch_en_4 = msca_ch_en;
        }
    }
    
    uint32_t $a1_15 = 0xf0000 | msca_ch_en_4;
    msca_ch_en = $a1_15;
    /* tailcall */
    return system_reg_write(0x9804, $a1_15);
}

