#include "include/main.h"


  int32_t tisp_ae1_get_statistics(int32_t* arg1, int32_t arg2)

{
    uint32_t $t6 = arg2 >> 0x1c;
    int32_t $t0 = 0;
    int32_t result;
    
    while (true)
    {
        result = $t0 * 0x3c;
        
        if ($t0 == (arg2 & 0xf000) >> 0xc)
            break;
        
        int32_t $t1_1 = 0;
        void* $v1_1 = result + &IspAeStatic;
        int32_t* $v0_1 = arg1;
        
        while (true)
        {
            $v1_1 += 4;
            
            if ($t1_1 == $t6)
                break;
            
            $t1_1 += 1;
            *($v1_1 + 0x1d14) = *$v0_1 & 0x1fffff;
            int32_t $a2_3 = $v0_1[1];
            $v0_1 = &$v0_1[4];
            *($v1_1 + 0x2098) = ($a2_3 & 0x3ff) << 0xb | *($v0_1 - 0x10) >> 0x15;
            *($v1_1 + 0x241c) = (*($v0_1 - 0xc) & 0x7ffffc00) >> 0xa;
            *($v1_1 + 0x27a0) = (*($v0_1 - 8) & 0xfff) << 1 | *($v0_1 - 0xc) >> 0x1f;
            *($v1_1 + 0x2b24) = (*($v0_1 - 8) & 0x1fff000) >> 0xc;
        }
        
        $t0 += 1;
        arg1 = &arg1[$t6 * 4];
    }
    
    return result;
}

