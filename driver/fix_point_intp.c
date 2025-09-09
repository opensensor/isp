#include "include/main.h"


  int32_t fix_point_intp(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5)

{
        int32_t $s2_1 = arg3 - arg2;
            int32_t $v0_2;
            int32_t $v1_1;
            int32_t $lo_1;
            int32_t $hi_1;
            int32_t $s1_1 = 0;
            int32_t $a0_1 = -($lo_1);
            int32_t $a2_2 = -(($v1_1 * $s2_1 + $hi_1)) - (0 < $a0_1 ? 1 : 0);
            char i_1;
                int32_t $v0_7 = $a0_1 >> 0x1f | $a2_2 << 1;
                    int32_t $v1_6 = $a0_1 - $s2_1;
                    int32_t $a2_3 = $a0_1 < $v1_6 ? 1 : 0;
    if (arg3 < arg2)

    
    if (arg3 != arg2)
    {
        
        if (arg4 != arg5)
        {
            $v0_2 = div64_u64(0, 0, $s2_1, 0);
            $hi_1 = HIGHD($s2_1 * $v0_2);
            $lo_1 = LOWD($s2_1 * $v0_2);
            
            for (uint32_t i = 0; i < arg1; i = i_1)
            {
                $a0_1 <<= 1;
                $a2_2 = $v0_7;
                $s1_1 <<= 1;
                
                if ($v0_7 || $s2_1 < $a0_1)
                {
                    $s1_1 |= 1;
                    $a0_1 = $v1_6;
                    $a2_2 = $v0_7 - $a2_3;
                    i_1 = i + 1;
                }
                else
                {
                    if ($s2_1 == $a0_1)
                    {
                        $s1_1 = ($s1_1 | 1) << ((arg1 - 1 - i) & 0x1f);
                        break;
                    }
                    
                    i_1 = i + 1;
                }
            }
            
            return $v0_2 - ($s1_1 | __ashldi3($v0_2, $v1_1, arg1));
        }
    }
    else if (arg4 != arg5)
    {
        return 0;

    }
    
    return arg4;
}

