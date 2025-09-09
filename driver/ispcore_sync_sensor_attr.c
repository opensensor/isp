#include "include/main.h"


  int32_t ispcore_sync_sensor_attr(void* arg1, int32_t arg2, int32_t arg3)

{
    if (arg1 && arg1 < 0xfffff001)
    {
        void* $s0_1 = *(arg1 + 0xd4);
        
        if ($s0_1 && $s0_1 < 0xfffff001)
        {
            if (!arg2)
            {
                memset($s0_1 + 0xec, arg2, 0x4c);
                return 0;
            }
            
            memcpy($s0_1 + 0xec, arg2, 0x4c);
            int32_t $a1 = *($s0_1 + 0x12c);
            int32_t var_68_33 = *($s0_1 + 0x124);
            int32_t var_64_1_7 = *($s0_1 + 0x128);
            void* $v0_3 = *($s0_1 + 0x120);
            int32_t var_3c_1_6 = $a1;
            int32_t var_4c_1_5 = *($v0_3 + 0x94);
            uint32_t $a2 = *($v0_3 + 0xb2);
            int32_t var_48_1_10 = *($v0_3 + 0x98);
            uint16_t var_2a_1_1 = $a2;
            int16_t var_38_1_16 = *($v0_3 + 0xa4);
            int16_t var_36_1_1 = *($v0_3 + 0xa6);
            int16_t var_34_1_13 = *($v0_3 + 0xa8);
            int16_t var_32_1_1 = *($v0_3 + 0xaa);
            int16_t var_2c_1_12 = *($v0_3 + 0xb0);
            int16_t var_28_1_6 = *($v0_3 + 0xb4);
            int16_t var_26_1_1 = *($v0_3 + 0xb6);
            int16_t var_24_1_3 = *($v0_3 + 0xb8);
            int16_t var_22_1_1 = *($v0_3 + 0xba);
            *($v0_3 + 0xbc) = ($a1 & 0xffff) * 0xf4240 / ($a1 >> 0x10) / $a2;
            void* $v0_4 = *($s0_1 + 0x120);
            int16_t var_20_1_6 = *($v0_4 + 0xbc);
            int16_t var_1e_1_1 = *($v0_4 + 0xd8);
            int16_t var_1c_1_4 = *($v0_4 + 0xda);
            int32_t var_14_1_5 = *($v0_4 + 0xe0);
            tiziano_sync_sensor_attr(&var_68_34);
            return 0;
        }
    }
    
    isp_printf(2, "not support the gpio mode!\\n", arg3);
    return 0xffffffea;
}

