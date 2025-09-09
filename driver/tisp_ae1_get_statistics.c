#include "include/main.h"


  int32_t tisp_ae1_get_statistics(int32_t* arg1, int32_t arg2)

{
    uint32_t $t6 = arg2 >> 0x1c;
    int32_t $t0 = 0;
    int32_t result;
        int32_t $t1_1 = 0;
        char* $v1_1 = (char*)(result + &IspAeStatic); // Fixed void pointer assignment
        int32_t* $v0_1 = arg1;
            int32_t $a2_3 = $v0_1[1];
    
    while (true)
    {
        result = $t0 * 0x3c;
        
        if ($t0 == (arg2 & 0xf000) >> 0xc)
            break;
        
        
        while (true)
        {
            $v1_1 += 4;
            
            if ($t1_1 == $t6)
                break;
            
            $t1_1 += 1;
            *((int32_t*)((char*)$v1_1 + 0x1d14)) = *$v0_1 & 0x1fffff; // Fixed void pointer dereference
            $v0_1 = &$v0_1[4];
            *((int32_t*)((char*)$v1_1 + 0x2098)) = ($a2_3 & 0x3ff) << 0xb | *($v0_1 - 0x10) >> 0x15; // Fixed void pointer dereference
            *((int32_t*)((char*)$v1_1 + 0x241c)) = (*($v0_1 - 0xc) & 0x7ffffc00) >> 0xa; // Fixed void pointer dereference
            *((int32_t*)((char*)$v1_1 + 0x27a0)) = (*($v0_1 - 8) & 0xfff) << 1 | *($v0_1 - 0xc) >> 0x1f; // Fixed void pointer dereference
            *((int32_t*)((char*)$v1_1 + 0x2b24)) = (*($v0_1 - 8) & 0x1fff000) >> 0xc; // Fixed void pointer dereference
        }
        
        $t0 += 1;
        arg1 = &arg1[$t6 * 4];
    }
    
    return result;
}

