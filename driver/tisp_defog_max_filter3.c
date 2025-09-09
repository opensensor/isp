#include "include/main.h"


  char (*)[0xb8] tisp_defog_max_filter3(void* arg1, int32_t arg2)

{
    char var_d0[0xb8];
    int32_t $v1_1;
        int32_t $t3_1 = i - 1;
        int32_t $t6_1;
        int32_t $s1_1;
    
    for (int32_t i = 0; (uintptr_t)i != 0xa; )
    {
        
        if (!i)
        {
            $t3_1 = 0;
            $t6_1 = i + 1;
            $s1_1 = $t3_1 * 0x12;
        }
        else
        {
            $t6_1 = 9;
            
            if (i != 9)
            {
                $t6_1 = i + 1;
                $s1_1 = $t3_1 * 0x12;
            }
            else
                $s1_1 = $t3_1 * 0x12;
        }
        
        for (int32_t j = 0; (uintptr_t)j != 0x12; )
        {
            int32_t $a2_1 = j - 1;
            int32_t $s2_1;
            int32_t k;
            
            if (!j)
            {
                $a2_1 = 0;
                k = j + 1;
                $s2_1 = $s1_1 + $a2_1;
            }
            else
            {
                k = 0x11;
                
                if ((uintptr_t)j != 0x11)
                {
                    k = j + 1;
                    $s2_1 = $s1_1 + $a2_1;
                }
                else
                    $s2_1 = $s1_1 + $a2_1;
            }
            
            int32_t $t4_1 = $t3_1;
            int32_t $t1_1 = 0;
            uint32_t $t0_1 = 0;
            
            while ($t6_1 >= $t4_1)
            {
                char* $a2_5 = arg1 + $s2_1 + $t1_1;
                    char $t5_3 = *$a2_5;
                
                while (k >= $a2_1 - arg1 - $s2_1 - $t1_1 + $a2_5)
                {
                    
                    if ($t5_3 < $t0_1)
                        $t5_3 = $t0_1;
                    
                    $t0_1 = $t5_3;
                    $a2_5 = &$a2_5[1];
                }
                
                $t4_1 += 1;
                $t1_1 += 0x12;
            }
            
            uint8_t* $a2_7 = &var_d0[j + i * 0x12];
            j += 1;
            *$a2_7 = $t0_1;
        }
        
        i += 1;
        $v1_1 = arg2 + 0xb4;
    }
    
    char (* result)[0xb8] = &var_d0_1;
    
    do
    {
    return result;
        arg2 += 1;
        *(arg2 - 1) = *result;
        result = &(*result)[1];
    } while (arg2 != $v1_1);
    
}

