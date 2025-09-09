#include "include/main.h"


  char (*)[0xbc] tisp_defog_img_filter5(void* arg1, int32_t arg2, void* arg3)

{
    int32_t $v1 = 0;
    int32_t $v0 = 0;
    char var_e0_3[0xbc];
    
    while (true)
    {
        for (int32_t i = 0; i != 0x12; )
        {
            int32_t $s4_1 = 0 < i ? 1 : 0;
            int32_t $t5_2 = ($v1 - $v0) * 0x12 + i - $s4_1;
            int32_t j = -($v0);
            int32_t $t4_1 = 0;
            int32_t $t1_1 = 0;
            int32_t $s2_1 = 0;
            
            do
            {
                void* $v0_6 = arg3 + ((j * 3 - $s4_1) << 2);
                char* $t0_2 = arg1 + $t5_2 + $t4_1;
                
                do
                {
                    int32_t $s5_1 = *($v0_6 + 0x10);
                    uint32_t $s7_1 = *$t0_2;
                    $t0_2 = &$t0_2[1];
                    $t1_1 += $s5_1;
                    $s2_1 += $s7_1 * $s5_1;
                    $v0_6 += 4;
                } while ((0 < (i ^ 0x11) ? 1 : 0) >= -($s4_1) - arg1 - $t5_2 - $t4_1 + $t0_2);
                
                j += 1;
                $t4_1 += 0x12;
            } while ((0 < ($v1 ^ 9) ? 1 : 0) >= j);
            
            void* $t2_2 = &var_e0_4[i + $v1 * 0x12];
            i += 1;
            *$t2_2 = (($t1_1 >> 1) + $s2_1) / $t1_1;
        }
        
        $v1 += 1;
        
        if ($v1 == 0xa)
            break;
        
        $v0 = 0 < $v1 ? 1 : 0;
    }
    
    char (* result)[0xbc] = &var_e0_5;
    int32_t $v1_1 = arg2 + 0xb4;
    
    do
    {
        arg2 += 1;
        *(arg2 - 1) = *result;
        result = &(*result)[1];
    } while ($v1_1 != arg2);
    
    return result;
}

