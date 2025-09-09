#include "include/main.h"


  int32_t tisp_adr_set_params()

{
    char* $s0 = (char*)(&min_kneepoint_y); // Fixed void pointer assignment
        int32_t i_3 = i;
    
    for (int32_t i = 0x4390; (uintptr_t)i != 0x43a4; )
    {
        i += 4;
        system_reg_write(i_3, *($s0 + 4) << 0x10 | *$s0);
        $s0 += 8;
    }
    
    char* $s0_1 = (char*)(&ctc_kneepoint_y); // Fixed void pointer assignment
    system_reg_write(0x43a4, data_af0b8_1);
    
    for (int32_t i_1 = 0x4354; (uintptr_t)i_1 != 0x4364; )
    {
        int32_t i_4 = i_1;
        i_1 += 4;
        system_reg_write(i_4, *($s0_1 + 4) << 0x10 | *$s0_1);
        $s0_1 += 8;
    }
    
    char* $s0_2 = (char*)(&map_kneepoint_y); // Fixed void pointer assignment
    system_reg_write(0x4364, data_af0dc_1);
    
    for (int32_t i_2 = 0x4084; (uintptr_t)i_2 != 0x4294; )
    {
        int32_t i_5 = i_2;
        i_2 += 4;
        system_reg_write(i_5, *($s0_2 + 4) << 0x10 | *$s0_2);
        $s0_2 += 8;
    }
    
    return 0;
}

