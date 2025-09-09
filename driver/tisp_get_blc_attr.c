#include "include/main.h"


  int32_t tisp_get_blc_attr(int32_t* arg1)

{
    int32_t $s1 = system_reg_read(8) & 0x1f;
    uint32_t $v0_2 = *(&var_68 + ($s1 << 1));
        int32_t var_38_2 = system_reg_read(0x1060) & 0xfff;
        int32_t $v0_15 = system_reg_read(0x1064);
        int32_t var_34_1 = $v0_15 & 0xfff;
        int32_t var_30_1 = $v0_15 >> 0x10 & 0xfff;
        int32_t $v0_18 = system_reg_read(0x1068);
        int32_t var_2c_1 = $v0_18 & 0xfff;
    void var_68;
    memcpy(&var_68, 0x7dc64, 0x30);
    
    if ($(uintptr_t)s1 >= 0x18)
        $s1 = 0;
    
    int32_t var_38;
    __builtin_memset(&var_38, 0, 0x14);
    int32_t var_24;
    tisp_g_module_control(&var_24);
    int32_t $v0_12;
    
    if ((var_24 & 9) == 9)
    {
        $v0_12 = $v0_18 >> 0x10 & 0xfff;
    }
    else
    {
        int32_t $v0_5 = system_reg_read(0x1018);
        int32_t var_38_1 = $v0_5 & 0xfff;
        int32_t var_34 = $v0_5 >> 0x10 & 0xfff;
        int32_t $v0_8 = system_reg_read(0x101c);
        int32_t var_30 = $v0_8 & 0xfff;
        int32_t var_2c = $v0_8 >> 0x10 & 0xfff;
        $v0_12 = system_reg_read(0x1020) & 0xfff;
    }
    
    int32_t var_28_7 = $v0_12;
    *arg1 = *(&var_68_11 + (($v0_2 & 7) << 2) + 0x30);
    arg1[1] = *(&var_68_12 + (($v0_2 >> 3 & 7) << 2) + 0x30);
    arg1[2] = *(&var_68_13 + (($v0_2 >> 6 & 7) << 2) + 0x30);
    arg1[3] = *(&var_68_14 + (($v0_2 >> 9 & 7) << 2) + 0x30);
    arg1[4] = *(&var_68_15 + (($v0_2 >> 0xc & 7) << 2) + 0x30);
    return 0;
}

