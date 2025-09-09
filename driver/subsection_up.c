#include "include/main.h"


  int32_t subsection_up(void* arg1, void* arg2, int32_t* arg3, int32_t arg4)

{
    int32_t* i = arg2 + 4;
    void* $a0 = arg1 + 4;
    *($a0 - 4) = 0;
    *($a0 + 0x1c) = 0xfff;
    int32_t j_1;
    int32_t $t0_1;
    
    do
    {
        int32_t $t9_1 = *i;
        int32_t* $t5_1 = arg3;
        int32_t $s0_1 = 0x2710;
        $t0_1 = 0;
        j_1 = 0;
        
        for (int32_t j = 0; j != 0x200; )
        {
            int32_t $t2_1 = *$t5_1;
            int32_t $v1_1 = $t2_1 - $t9_1;
            
            if ($t9_1 >= $t2_1)
                $v1_1 = $t9_1 - $t2_1;
            
            if ($v1_1 < $s0_1)
            {
                $t0_1 = 0;
                j_1 = j;
                goto label_2c3a0;
            }
            
            if ($v1_1 != $s0_1)
                j += 1;
            else
            {
                j_1 += j;
            label_2c3a0:
                $t0_1 += 1;
                $s0_1 = $v1_1;
                j += 1;
            }
            
            $t5_1 = &$t5_1[1];
        }
        
        if (0 >= $t0_1)
            $t0_1 = 1;
        
        i = &i[1];
        $a0 += 4;
        *($a0 - 4) = ((j_1 << 0xc) / $t0_1 + 0x800) / 0x1000 * arg4 - 1;
    } while (i != arg2 + 0x20);
    
    return ((j_1 << 0xc) / $t0_1 + 0x800) / 0x1000 * arg4 - 1;
}

