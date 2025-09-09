#include "include/main.h"


  int32_t tisp_ae0_get_statistics(int32_t* arg1, int32_t arg2)

{
    uint32_t $t6 = arg2 >> 0x1c;
    int32_t $t0 = 0;
        int32_t $t1_1 = 0;
        void* $v1_1 = result + &IspAeStatic;
        int32_t* $v0_1 = arg1;
            int32_t temp1_1 = $t1_1;
            int32_t $a3_1 = ($v0_1[1] & 0x3ff) << 0xb;
            int32_t $a2_5 = *$v0_1;
    int32_t result;
    
    while (true)
    {
        result = $t0 * 0x3c;
        
        if ($t0 == (arg2 & 0xf000) >> 0xc)
            break;
        
        
        while (true)
        {
            $t1_1 += 1;
            
            if (temp1_1 == $t6)
                break;
            
            *$v1_1 = *$v0_1 & 0x1fffff;
            $v1_1 += 4;
            $v0_1 = &$v0_1[4];
            *(((void**)((char*)$v1_1 + 0x380))) = $a3_1 | $a2_5 >> 0x15; // Fixed void pointer dereference
            *(((void**)((char*)$v1_1 + 0x704))) = (*($v0_1 - 0xc) & 0x7ffffc00) >> 0xa; // Fixed void pointer dereference
            *(((void**)((char*)$v1_1 + 0xe0c))) = (*($v0_1 - 8) & 0xfff) << 1 | *($v0_1 - 0xc) >> 0x1f; // Fixed void pointer dereference
            *(((void**)((char*)$v1_1 + 0x1190))) = (*($v0_1 - 8) & 0x1fff000) >> 0xc; // Fixed void pointer dereference
            *(((void**)((char*)$v1_1 + 0xa88))) = (*($v0_1 - 4) & 0x3fff) << 7 | *($v0_1 - 8) >> 0x19; // Fixed void pointer dereference
        }
        
        $t0 += 1;
        arg1 = &arg1[$t6 * 4];
    }
    
    return result;
}

