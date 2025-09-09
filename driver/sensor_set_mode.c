#include "include/main.h"


  uint32_t sensor_set_mode(char arg1, char* arg2)

{
    char var_20_21 = 1;
    uint32_t ispcore_1 = g_ispcore;
    int32_t $v0_1;
    
    if (ispcore_1)
        $v0_1 = *(ispcore_1 + 0x7c);
    
    uint32_t $a2;
    
    if (!ispcore_1 || !$v0_1)
        $a2 = var_20_22;
    else
    {
        void* $s1_1 = *(ispcore_1 + 0x120);
        int32_t $v0_2 = $v0_1(ispcore_1, 0x200000d, &var_20_23);
        $a2 = var_20_24;
        
        if (!$v0_2)
        {
            *(arg2 + 6) = *(ispcore_1 + 0xec);
            *(arg2 + 8) = *(ispcore_1 + 0xf0);
            *(arg2 + 2) = *($s1_1 + 0xb0);
            *(arg2 + 4) = *($s1_1 + 0xb2);
            *(arg2 + 0x28) = *($s1_1 + 0xa4);
            *(arg2 + 0x2c) = *($s1_1 + 0xb4);
            *(arg2 + 0x50) = *($s1_1 + 0xd8);
            *(arg2 + 0x54) = *($s1_1 + 0xda);
            *(arg2 + 0x30) = *($s1_1 + 0xb4);
            uint32_t result = *($s1_1 + 0xaa);
            *arg2 = arg1;
            *(arg2 + 0x34) = result;
            return result;
        }
    }
    
    return isp_printf(1, &$LC0, $a2);
}

