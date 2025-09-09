#include "include/main.h"


  int32_t tisp_channel_attr_set(int32_t arg1, int32_t* arg2)

{
    int32_t tispinfo_1 = tispinfo;
    int32_t var_34_15 = arg2[2];
    int32_t var_38_21 = arg2[1];
    int32_t var_3c_7 = *arg2;
    int32_t var_40_21 = arg2[7];
    int32_t var_44_15 = arg2[6];
    int32_t var_48_19 = arg2[5];
    int32_t var_4c_7 = arg2[4];
    int32_t var_50_12 = arg2[3];
    int32_t var_54_3 = arg2[0xc];
    int32_t var_58_4 = arg2[0xb];
    int32_t var_5c_2 = arg2[0xa];
    int32_t var_60_7 = arg2[9];
    int32_t var_64_13 = arg2[8];
    int32_t var_68_13 = data_b2f34_2;
    isp_printf(0, "not support the gpio mode!\\n", arg1);
    
    if (!arg1)
        memcpy(&ds0_attr, arg2, 0x34);
    else if (arg1 == 1)
        memcpy(&ds1_attr, arg2, 0x34);
    else if (arg1 == 2)
        memcpy(&ds2_attr, arg2, 0x34);
    
    int32_t tispinfo_2 = tispinfo;
    int32_t $s2 = data_b2f34_3;
    int32_t $a1_2;
    
    if (!data_b2e04_1)
    {
        data_b2e08_1 = 0;
        data_b2e0c_1 = 0;
        data_b2e10_2 = tispinfo_2;
        data_b2e14_2 = $s2;
        $a1_2 = 0;
    }
    else
    {
        int32_t tispinfo_3 = data_b2e10_3;
        int32_t $v1_1 = data_b2e08_2;
        int32_t $a0_1 = data_b2e14_3;
        int32_t $a1_1 = data_b2e0c_2;
        
        if (tispinfo_2 < tispinfo_3 + $v1_1 || $s2 < $a0_1 + $a1_1)
        {
            isp_printf(2, "sensor type is BT656!\\n", "tisp_channel_attr_set");
            return 0xffffffff;
        }
        
        tispinfo_2 = tispinfo_3;
        $s2 = $a0_1;
        $a1_2 = $v1_1 << 0x10 | $a1_1;
    }
    
    system_reg_write(0x9860, $a1_2);
    system_reg_write(0x9864, tispinfo_2 << 0x10 | $s2);
    int32_t tispinfo_4;
    int32_t $s7_1;
    
    if (!*arg2)
    {
        arg2[1] = tispinfo_2;
        arg2[2] = $s2;
        $s7_1 = $s2;
        tispinfo_4 = tispinfo_2;
    }
    else
    {
        tispinfo_4 = arg2[1];
        $s7_1 = arg2[2];
    }
    
    int32_t $s1_2 = (arg1 + 0x99) << 8;
    system_reg_write($s1_2, tispinfo_4 << 0x10 | $s7_1);
    system_reg_write($s1_2 + 4, ((tispinfo_2 << 9) / tispinfo_4) << 0x10 | (($s2 << 9) / $s7_1));
    
    if (!arg2[3])
    {
        arg2[4] = 0;
        arg2[5] = 0;
        arg2[6] = tispinfo_4;
        arg2[7] = $s7_1;
    }
    else
    {
        int32_t tispinfo_6 = arg2[6];
        int32_t $a1_9 = arg2[4];
        int32_t $v0_20 = arg2[7];
        int32_t $a2_1 = arg2[5];
        
        if (tispinfo_4 < tispinfo_6 + $a1_9 || $s7_1 < $v0_20 + $a2_1)
        {
            int32_t var_58_1_1 = $a2_1;
            int32_t var_64_1_3 = $a1_9;
            int32_t var_54_1_1 = $s7_1;
            int32_t var_5c_1_1 = $v0_20;
            int32_t tispinfo_5 = tispinfo_4;
            int32_t tispinfo_7 = tispinfo_6;
            isp_printf(2, "sensor type is BT601!\\n", "tisp_channel_attr_set");
            return 0xffffffff;
        }
        
        tispinfo_4 = tispinfo_6;
        $s7_1 = $v0_20;
    }
    
    system_reg_write($s1_2 + 0x2c, tispinfo_4 << 0x10 | $s7_1);
    system_reg_write($s1_2 + 0x28, arg2[4] << 0x10 | arg2[5]);
    system_reg_write($s1_2 + 0x80, tispinfo_4);
    system_reg_write($s1_2 + 0x98, tispinfo_4);
    return 0;
}

