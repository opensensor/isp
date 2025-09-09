#include "include/main.h"


  int32_t tisp_channel_start(int32_t arg1, int32_t* arg2 @ $s7)

{
    uint32_t msca_ch_en_1 = msca_ch_en;
    
    if (!~msca_ch_en_1)
        msca_ch_en_1 = 0;
    
    msca_ch_en = 1 << (arg1 & 0x1f) | msca_ch_en_1;
    uint32_t msca_dmaout_arb_1 = msca_dmaout_arb;
    uint32_t $v0_1 = 0xe;
    
    if (~msca_dmaout_arb_1)
        $v0_1 = msca_dmaout_arb_1 | 0xe;
    
    msca_dmaout_arb = $v0_1;
    int32_t* var_c_4 = arg2;
    uint32_t msca_dmaout_arb_2;
    
    if (arg1 == 1)
    {
        arg2 = &ds1_attr;
        msca_dmaout_arb_2 = msca_dmaout_arb;
    }
    else if (arg1 == 2)
    {
        arg2 = &ds2_attr;
        msca_dmaout_arb_2 = msca_dmaout_arb;
    }
    else if (!arg1)
    {
        arg2 = &ds0_attr;
        msca_dmaout_arb_2 = msca_dmaout_arb;
    }
    else
    {
        int32_t entry_$a2;
        isp_printf(2, "Can not support this frame mode!!!\\n", entry_$a2);
        msca_dmaout_arb_2 = msca_dmaout_arb;
    }
    
    system_reg_write(0x9818, msca_dmaout_arb_2);
    int32_t $v0_2;
    int32_t tispinfo_1;
    
    if (arg2[8] != 1)
    {
        tispinfo_1 = tispinfo;
        $v0_2 = data_b2f34_1;
    }
    else
    {
        tispinfo_1 = data_b2e10_1;
        $v0_2 = data_b2e14_1;
    }
    
    int32_t var_38_20 = arg1 + 0x98;
    int32_t $s1_3;
    
    if (arg2[1] << 1 < tispinfo_1 || arg2[2] << 1 < $v0_2)
    {
        int32_t $s3 = (arg1 + 0x98) << 8;
        system_reg_write($s3 + 0x1c0, 0x40080);
        system_reg_write($s3 + 0x1c4, 0x40080);
        system_reg_write($s3 + 0x1c8, 0x40080);
        system_reg_write($s3 + 0x1cc, 0x40080);
        $s1_3 = 1 << ((arg1 + 8) & 0x1f) | 1 << ((arg1 + 0xb) & 0x1f) | msca_ch_en;
    }
    else
    {
        int32_t $s3_1 = (arg1 + 0x98) << 8;
        system_reg_write($s3_1 + 0x1c0, 0x200);
        system_reg_write($s3_1 + 0x1c4, 0);
        system_reg_write($s3_1 + 0x1c8, 0x200);
        system_reg_write($s3_1 + 0x1cc, 0);
        $s1_3 = ~(1 << ((arg1 + 8) & 0x1f) | 1 << ((arg1 + 0xb) & 0x1f)) & msca_ch_en;
    }
    
    msca_ch_en = $s1_3;
    uint32_t $a1_1 = 0xf0000 | msca_ch_en;
    msca_ch_en = $a1_1;
    system_reg_write(0x9804, $a1_1);
    system_reg_read(0x9864);
    int32_t $v0_11 = system_reg_read(0x9860);
    int32_t $s2 = (arg1 + 0x98) << 8;
    int32_t $v0_13 = system_reg_read($s2 + 0x180);
    int32_t $v0_14 = system_reg_read($s2 + 0x198);
    int32_t $v0_15 = system_reg_read($s2 + 0x128);
    int32_t $v0_16 = system_reg_read($s2 + 0x12c);
    int32_t $v0_17 = system_reg_read($s2 + 0x104);
    int32_t $v0_18 = system_reg_read($s2 + 0x100);
    int32_t var_48_18 = arg2[2];
    int32_t var_4c_6 = arg2[1];
    int32_t var_58_3 = $v0_18;
    int32_t var_64_12 = $v0_15;
    int32_t tispinfo_2 = tispinfo_1;
    int32_t var_60_6 = $v0_16;
    int32_t var_68_12 = $v0_14;
    int32_t var_6c_10 = $v0_13;
    int32_t var_70_14 = $v0_11;
    int32_t var_50_9 = $v0_2;
    int32_t var_5c_1 = $v0_17;
    isp_printf(0, "sensor type is BT1120!\\n", arg1);
    return 0;
}

