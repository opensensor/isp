#include "include/main.h"


  int32_t tisp_mscaler_mask_setreg(int32_t arg1, int32_t arg2, int32_t arg3, int24_t arg4, int32_t arg5, int16_t arg6, int16_t arg7, int32_t arg8, char arg9, uint32_t arg10, int16_t arg11, int16_t arg12, int16_t arg13, char arg14, char arg15, int32_t arg16, int16_t arg17, int16_t arg18, int32_t arg19, char arg20, uint32_t arg21, int16_t arg22, int16_t arg23, int16_t arg24, char arg25, char arg26, int32_t arg27, int16_t arg28, int16_t arg29, int32_t arg30, char arg31, uint32_t arg32, int16_t arg33, int16_t arg34, int16_t arg35, char arg36, char arg37, int32_t arg38, int16_t arg39, int16_t arg40, int32_t arg41, char arg42, uint32_t arg43, int16_t arg44, int16_t arg45, int16_t arg46, char arg47, char arg48, int32_t arg49, int16_t arg50, int16_t arg51, int32_t arg52, char arg53, uint32_t arg54, int16_t arg55, int16_t arg56, int16_t arg57, char arg58, char arg59, int32_t arg60, int16_t arg61, int16_t arg62, int32_t arg63)

{
    uint32_t $v1 = *arg1[2];
    uint32_t $fp = arg2;
    uint32_t $s5_1 = _setRightPart32(arg1);
    uint32_t var_34 = $v1;
    uint32_t var_38 = $fp;
    uint32_t msca_ch_en_1 = msca_ch_en;
    int32_t $s4 = 0 < system_reg_read(0x9968) ? 1 : 0;
    int32_t $s1;
    _setLeftPart32(arg1);
    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    
    if (!~msca_ch_en_1)
        msca_ch_en_1 = 0;
    
    msca_ch_en = msca_ch_en_1;
    
    if (arg1 != 1)
    {
        system_reg_write(0x9938, 0);
        system_reg_write(0x993c, 0);
        system_reg_write(0x9940, 0);
        $s1 = 0;
    }
    else
    {
        uint32_t var_34_1 = $v1;
        uint32_t var_38_1 = $fp;
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        system_reg_write(0x9938, $s5_1);
        system_reg_write(0x993c, *arg2[2] << 0x10 | arg3);
        system_yvu_or_yuv($s4, 0x9940, arg4 << 0x10 | *arg3[2]);
        $s1 = 1;
    }
    
    if (*arg4[2] != 1)
    {
        system_reg_write(0x9944, 0);
        system_reg_write(0x9948, 0);
        system_reg_write(0x994c, 0);
    }
    else
    {
        system_reg_write(0x9944, arg5);
        system_reg_write(0x9948, arg6 << 0x10 | arg7);
        system_yvu_or_yuv($s4, 0x994c, arg8);
        $s1 = 1;
    }
    
    uint32_t $v1_4;
    
    if (arg9 != 1)
    {
        system_reg_write(0x9950, 0);
        system_reg_write(0x9954, 0);
        system_reg_write(0x9958, 0);
        $v1_4 = arg15;
    }
    else
    {
        _setLeftPart32(arg10);
        $s1 = 1;
        system_reg_write(0x9950, _setRightPart32(arg10));
        system_reg_write(0x9954, arg11 << 0x10 | arg12);
        system_yvu_or_yuv($s4, 0x9958, arg14 << 0x10 | arg13);
        $v1_4 = arg15;
    }
    
    if ($v1_4 != 1)
    {
        system_reg_write(0x995c, 0);
        system_reg_write(0x9960, 0);
        system_reg_write(0x9964, 0);
    }
    else
    {
        system_reg_write(0x995c, arg16);
        system_reg_write(0x9960, arg17 << 0x10 | arg18);
        system_yvu_or_yuv($s4, 0x9964, arg19);
        $s1 = 1;
    }
    
    int32_t $s5_2 = 0 < system_reg_read(0x9a68) ? 1 : 0;
    uint32_t $v1_6;
    int32_t $s4_1;
    
    if (arg20 != 1)
    {
        system_reg_write(0x9a38, 0);
        system_reg_write(0x9a3c, 0);
        system_reg_write(0x9a40, 0);
        $s4_1 = 0;
        $v1_6 = arg26;
    }
    else
    {
        _setLeftPart32(arg21);
        $s4_1 = 1;
        system_reg_write(0x9a38, _setRightPart32(arg21));
        system_reg_write(0x9a3c, arg22 << 0x10 | arg23);
        system_yvu_or_yuv($s5_2, 0x9a40, arg25 << 0x10 | arg24);
        $v1_6 = arg26;
    }
    
    if ($v1_6 != 1)
    {
        system_reg_write(0x9a44, 0);
        system_reg_write(0x9a48, 0);
        system_reg_write(0x9a4c, 0);
    }
    else
    {
        system_reg_write(0x9a44, arg27);
        system_reg_write(0x9a48, arg28 << 0x10 | arg29);
        system_yvu_or_yuv($s5_2, 0x9a4c, arg30);
        $s4_1 = 1;
    }
    
    uint32_t $v1_8;
    
    if (arg31 != 1)
    {
        system_reg_write(0x9a50, 0);
        system_reg_write(0x9a54, 0);
        system_reg_write(0x9a58, 0);
        $v1_8 = arg37;
    }
    else
    {
        _setLeftPart32(arg32);
        $s4_1 = 1;
        system_reg_write(0x9a50, _setRightPart32(arg32));
        system_reg_write(0x9a54, arg33 << 0x10 | arg34);
        system_yvu_or_yuv($s5_2, 0x9a58, arg36 << 0x10 | arg35);
        $v1_8 = arg37;
    }
    
    if ($v1_8 != 1)
    {
        system_reg_write(0x9a5c, 0);
        system_reg_write(0x9a60, 0);
        system_reg_write(0x9a64, 0);
    }
    else
    {
        system_reg_write(0x9a5c, arg38);
        system_reg_write(0x9a60, arg39 << 0x10 | arg40);
        system_yvu_or_yuv($s5_2, 0x9a64, arg41);
        $s4_1 = 1;
    }
    
    int32_t $s5_3 = 0 < system_reg_read(0x9b68) ? 1 : 0;
    uint32_t $v1_10;
    int32_t $s2;
    
    if (arg42 != 1)
    {
        system_reg_write(0x9b38, 0);
        system_reg_write(0x9b3c, 0);
        system_reg_write(0x9b40, 0);
        $s2 = 0;
        $v1_10 = arg48;
    }
    else
    {
        _setLeftPart32(arg43);
        $s2 = 1;
        system_reg_write(0x9b38, _setRightPart32(arg43));
        system_reg_write(0x9b3c, arg44 << 0x10 | arg45);
        system_yvu_or_yuv($s5_3, 0x9b40, arg47 << 0x10 | arg46);
        $v1_10 = arg48;
    }
    
    if ($v1_10 != 1)
    {
        system_reg_write(0x9b44, 0);
        system_reg_write(0x9b48, 0);
        system_reg_write(0x9b4c, 0);
    }
    else
    {
        system_reg_write(0x9b44, arg49);
        system_reg_write(0x9b48, arg50 << 0x10 | arg51);
        system_yvu_or_yuv($s5_3, 0x9b4c, arg52);
        $s2 = 1;
    }
    
    uint32_t $v1_12;
    
    if (arg53 != 1)
    {
        system_reg_write(0x9b50, 0);
        system_reg_write(0x9b54, 0);
        system_reg_write(0x9b58, 0);
        $v1_12 = arg59;
    }
    else
    {
        _setLeftPart32(arg54);
        $s2 = 1;
        system_reg_write(0x9b50, _setRightPart32(arg54));
        system_reg_write(0x9b54, arg55 << 0x10 | arg56);
        system_yvu_or_yuv($s5_3, 0x9b58, arg58 << 0x10 | arg57);
        $v1_12 = arg59;
    }
    
    if ($v1_12 != 1)
    {
        system_reg_write(0x9b5c, 0);
        system_reg_write(0x9b60, 0);
        system_reg_write(0x9b64, 0);
    }
    else
    {
        system_reg_write(0x9b5c, arg60);
        system_reg_write(0x9b60, arg61 << 0x10 | arg62);
        system_yvu_or_yuv($s5_3, 0x9b64, arg63);
        $s2 = 1;
    }
    
    uint32_t $a1_45 = $s1 << 3 | $s4_1 << 4 | $s2 << 5 | (0xffffffc7 & msca_ch_en) | 0xe0000;
    msca_ch_en = $a1_45;
    system_reg_write(0x9804, $a1_45);
    return 0;
}

