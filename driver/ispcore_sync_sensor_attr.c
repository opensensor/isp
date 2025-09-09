#include "include/main.h"


  int32_t ispcore_sync_sensor_attr(void* arg1, int32_t arg2, int32_t arg3)

{
        int32_t* $s0_1 = (int32_t*)((char*)arg1  + 0xd4); // Fixed void pointer arithmetic
                return 0;
    if (arg1 && (uintptr_t)arg1 < 0xfffff001)
    {
        
        if ($s0_1 && $(uintptr_t)s0_1 < 0xfffff001)
        {
            if (!(uintptr_t)arg2)
            {
                memset($s0_1 + 0xec, arg2, 0x4c);
            }
            
            memcpy($s0_1 + 0xec, arg2, 0x4c);
            int32_t $a1 = *($s0_1 + 0x12c);
            int32_t var_68_16 = *($s0_1 + 0x124);
            int32_t var_64_1_4 = *($s0_1 + 0x128);
            int32_t* $v0_3 = (int32_t*)((char*)$s0_1  + 0x120); // Fixed void pointer arithmetic
            int32_t var_3c_1_4 = $a1;
            int32_t var_4c_1_1 = *($v0_3 + 0x94);
            uint32_t $a2 = *($v0_3 + 0xb2);
            int32_t var_48_1_2 = *($v0_3 + 0x98);
            uint16_t var_2a_1_1 = $a2;
            int16_t var_38_1_4 = *($v0_3 + 0xa4);
            int16_t var_36_1 = *($v0_3 + 0xa6);
            int16_t var_34_1_5 = *($v0_3 + 0xa8);
            int16_t var_32_1 = *($v0_3 + 0xaa);
            int16_t var_2c_1_4 = *($v0_3 + 0xb0);
            int16_t var_28_1_2 = *($v0_3 + 0xb4);
            int16_t var_26_1 = *($v0_3 + 0xb6);
            int16_t var_24_1_1 = *($v0_3 + 0xb8);
            int16_t var_22_1 = *($v0_3 + 0xba);
            *((int32_t*)((char*)$v0_3 + 0xbc)) = ($a1 & 0xffff) * 0xf4240 / ($a1 >> 0x10) / $a2; // Fixed void pointer dereference
            int32_t* $v0_4 = (int32_t*)((char*)$s0_1  + 0x120); // Fixed void pointer arithmetic
            int16_t var_20_1_2 = *($v0_4 + 0xbc);
            int16_t var_1e_1 = *($v0_4 + 0xd8);
            int16_t var_1c_1_1 = *($v0_4 + 0xda);
            int32_t var_14_1_4 = *($v0_4 + 0xe0);
            tiziano_sync_sensor_attr(&var_68_17);
            return 0;
        }
    }
    

    return 0xffffffea;
}

