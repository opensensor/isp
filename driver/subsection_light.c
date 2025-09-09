#include "include/main.h"


  int32_t subsection_light(int32_t* arg1, int32_t* arg2, int32_t arg3, int32_t arg4)

{
        int32_t $v1_1 = *arg2;
        int32_t $t0_1 = *arg1;
    do
    {
        int32_t $v1_2;
        int32_t $lo_1;
        int32_t $hi_1;
        
        if ($v1_1 >= $t0_1)
        {
            $hi_1 = HIGHD($v1_1 * 0x3e8);
            $lo_1 = LOWD($v1_1 * 0x3e8);
            $v1_2 = $v1_1 - $t0_1;
        }
        else
        {
            $hi_1 = HIGHD($t0_1 * 0x3e8);
            $lo_1 = LOWD($t0_1 * 0x3e8);
            $v1_2 = $t0_1 - $v1_1;
        }
        
        int32_t $lo_2;
        int32_t $hi_2;
        $hi_2 = HIGHD(($hi_1 << 0x20 | $lo_1) - $v1_2 * arg3);
        $lo_2 = LOWD(($hi_1 << 0x20 | $lo_1) - $v1_2 * arg3);
        arg2 = &arg2[1];
        arg1 = &arg1[1];
        arg4 += 4;
        *(arg4 - 4) = $lo_2 / 0x3e8;
    } while (arg2 != &arg2[9]);
    
    return 0x3e8;
}

