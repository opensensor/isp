#include "include/main.h"


  int32_t tiziano_ae_set_hardware_param(int32_t arg1, int32_t* arg2, int32_t arg3)

{
    int32_t $a1_6 = arg2[3] << 0x1c | arg2[2] << 0x10 | *arg2 | arg2[1] << 0xc;
    int32_t $s5_6 = arg2[7] << 0x18 | arg2[6] << 0x10 | arg2[4] | arg2[5] << 8;
    int32_t $s4_6 = arg2[0xb] << 0x18 | arg2[0xa] << 0x10 | arg2[8] | arg2[9] << 8;
    int32_t $s3_6 = arg2[0xf] << 0x18 | arg2[0xe] << 0x10 | arg2[0xc] | arg2[0xd] << 8;
    int32_t $s7_3 = arg2[0x12] << 0x10 | arg2[0x11] << 8 | arg2[0x10];
    int32_t $s2_6 = arg2[0x16] << 0x18 | arg2[0x15] << 0x10 | arg2[0x13] | arg2[0x14] << 8;
    int32_t $s1_6 = arg2[0x1a] << 0x18 | arg2[0x19] << 0x10 | arg2[0x17] | arg2[0x18] << 8;
    int32_t $s0_6 = arg2[0x1e] << 0x18 | arg2[0x1d] << 0x10 | arg2[0x1b] | arg2[0x1c] << 8;
    int32_t $s6_3 = arg2[0x21] << 0x10 | arg2[0x20] << 8 | arg2[0x1f];
    int32_t $v1_18 = arg2[0x23];
    int32_t $a3_1 = arg2[0x25] << 0x14 | arg2[0x24] << 0x10;
    int32_t $a2_11 = arg2[0x22];
    uint32_t $v1_21;
    
    if ($v1_18 < 0xff)
    {
        $a3_1 |= $a2_11;
        $v1_21 = (($v1_18 << 1) / 3) << 8;
    }
    else
        $v1_21 = $v1_18 << 8 | $a2_11;
    
    int32_t $a2_12 = $v1_21 | $a3_1;
    int32_t $a1_7;
    
    if (!arg1)
    {
        if (arg3)
            $a1_7 = 0xa028;
        else
        {
            system_reg_write(0xa004, $a1_6);
            system_reg_write(0xa008, $s5_6);
            system_reg_write(0xa00c, $s4_6);
            system_reg_write(0xa010, $s3_6);
            system_reg_write(0xa014, $s7_3);
            system_reg_write(0xa018, $s2_6);
            system_reg_write(0xa01c, $s1_6);
            system_reg_write(0xa020, $s0_6);
            system_reg_write(0xa024, $s6_3);
            $a1_7 = 0xa028;
        }
        
        system_reg_write_ae(1, $a1_7, $a2_12);
    }
    else if (arg1 == 1)
    {
        if (arg3)
            $a1_7 = 0xa828;
        else
        {
            system_reg_write(0xa804, $a1_6);
            system_reg_write(0xa808, $s5_6);
            system_reg_write(0xa80c, $s4_6);
            system_reg_write(0xa810, $s3_6);
            system_reg_write(0xa814, $s7_3);
            system_reg_write(0xa818, $s2_6);
            system_reg_write(0xa81c, $s1_6);
            system_reg_write(0xa820, $s0_6);
            system_reg_write(0xa824, $s6_3);
            $a1_7 = 0xa828;
        }
        
        system_reg_write_ae(2, $a1_7, $a2_12);
    }
    return 0;
}

