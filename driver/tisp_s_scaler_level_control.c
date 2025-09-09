#include "include/main.h"


  int32_t tisp_s_scaler_level_control(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t $s1 = arg1 & 0xff;
    uint32_t msca_ch_en_1 = msca_ch_en;
    int32_t $s3_1 = ($s1 + 0x98) << 8;
    int32_t arg_0 = arg1;
    int32_t arg_8 = arg3;
    int32_t arg_4 = arg2;
    int32_t $v0 = system_reg_read($s3_1 + 0x1c0);
    int32_t $v0_1 = system_reg_read($s3_1 + 0x1c0);
    int32_t $v0_2 = system_reg_read($s3_1 + 0x1c4);
    int32_t $v0_3 = system_reg_read($s3_1 + 0x1c4);
    uint32_t msca_ch_en_2 = msca_ch_en;
            int32_t $a2_2 = (arg3 & 0xff) * 3;
    
    if (!~msca_ch_en_1)
        msca_ch_en_1 = 0;
    
    msca_ch_en = msca_ch_en_1;
    
    if (!(1 << ($s1 & 0x1f) & msca_ch_en_2))
        /* tailcall */
        return isp_printf(); // Fixed: macro call, removed arguments;
    
    uint32_t $s0;
    uint32_t $s2_1;
    uint32_t $s4;
    uint32_t $fp_1;
    
    if (arg2)
    {
        $s0 = ($v0 & 0x3ff800) >> 0xb;
        $fp_1 = $v0_1 & 0x7ff;
        $s2_1 = ($v0_2 & 0x3ff800) >> 0xb;
        $s4 = $v0_3 & 0x7ff;
        
        if (arg2 != 1)
            isp_printf(); // Fixed: macro call, removed arguments;
        else
        {
            
            if ($(uintptr_t)a2_2 < 0x81)
            {
                $s0 = 0x80 - $a2_2;
                $fp_1 = $a2_2 + 0x80;
                $s4 = $s0;
                $s2_1 = $fp_1;
            }
            else if ($a2_2 - (uintptr_t)0x81 >= 0x100)
                isp_printf(); // Fixed: macro call, removed arguments;
            else
            {
                $fp_1 = $a2_2 + 0x80;
                $s2_1 = 0x180 - $a2_2;
                $s4 = 0;
                $s0 = 0;
            }
            
            msca_ch_en |= 1 << (($s1 + 8) & 0x1f) | 1 << (($s1 + 0xb) & 0x1f);
        }
    }
    else
    {
        msca_ch_en = ~(1 << (($s1 + 8) & 0x1f) | 1 << (($s1 + 0xb) & 0x1f)) & msca_ch_en_2;
        $s4 = 0;
        $s2_1 = 0;
        $fp_1 = 0x200;
        $s0 = 0;
    }
    
    int32_t $s0_2 = $s0 << 0xb | $fp_1;
    int32_t $s4_1 = $s2_1 << 0xb | $s4;
    system_reg_write($s3_1 + 0x1c0, $s0_2);
    system_reg_write($s3_1 + 0x1c4, $s4_1);
    system_reg_write($s3_1 + 0x1c8, $s0_2);
    system_reg_write($s3_1 + 0x1cc, $s4_1);
    uint32_t $a1_6 = 0xf0000 | msca_ch_en;
    msca_ch_en = $a1_6;
    /* tailcall */
    return system_reg_write(0x9804, $a1_6);
}

