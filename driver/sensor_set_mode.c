#include "include/main.h"


  uint32_t sensor_set_mode(char arg1, char* arg2)

{
    char var_20 = 1;
    uint32_t ispcore_1 = g_ispcore;
    int32_t $v0_1;
    uint32_t $a2;
        int32_t* $s1_1 = (int32_t*)((char*)ispcore_1  + 0x120); // Fixed void pointer arithmetic
        int32_t $v0_2 = $v0_1(ispcore_1, 0x200000d, &var_20);
            uint32_t result = *($s1_1 + 0xaa);
            return result;
    
    if (ispcore_1)
        $v0_1 = *(ispcore_1 + 0x7c);
    
    
    if (!ispcore_1 || !$v0_1)
        $a2 = var_20;
    else
    {
        $a2 = var_20;
        
        if (!$v0_2)
        {
            *((int32_t*)((char*)arg2 + 6)) = *(ispcore_1 + 0xec); // Fixed void pointer dereference
            *((int32_t*)((char*)arg2 + 8)) = *(ispcore_1 + 0xf0); // Fixed void pointer dereference
            *((int32_t*)((char*)arg2 + 2)) = *($s1_1 + 0xb0); // Fixed void pointer dereference
            *((int32_t*)((char*)arg2 + 4)) = *($s1_1 + 0xb2); // Fixed void pointer dereference
            *((int32_t*)((char*)arg2 + 0x28)) = *($s1_1 + 0xa4); // Fixed void pointer dereference
            *((int32_t*)((char*)arg2 + 0x2c)) = *($s1_1 + 0xb4); // Fixed void pointer dereference
            *((int32_t*)((char*)arg2 + 0x50)) = *($s1_1 + 0xd8); // Fixed void pointer dereference
            *((int32_t*)((char*)arg2 + 0x54)) = *($s1_1 + 0xda); // Fixed void pointer dereference
            *((int32_t*)((char*)arg2 + 0x30)) = *($s1_1 + 0xb4); // Fixed void pointer dereference
            *arg2 = arg1;
            *((int32_t*)((char*)arg2 + 0x34)) = result; // Fixed void pointer dereference
        }
    }
    
    return
}

