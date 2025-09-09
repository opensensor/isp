#include "include/main.h"


  int32_t tiziano_defog_set_reg_params()

{
        int32_t $s0_1 = i * 0xa + 0x58000;
            int32_t $a0_1 = i + j;
    for (int32_t i = 0; (uintptr_t)i != 0x48; i += 4)
    {
        
        for (int32_t j = 0; (uintptr_t)j != 0x2d0; )
        {
            system_reg_write($s0_1, 
                *(&defog_block_air_light_r + $a0_1) << 0x18 | *(&defog_block_transmit_t + $a0_1)
                    | *(&defog_block_air_light_g + $a0_1) << 0x10
                    | *(&defog_block_air_light_b + $a0_1) << 8);
            j += 0x48;
            $s0_1 += 4;
        }
    }
    
    return 0x48;
}

