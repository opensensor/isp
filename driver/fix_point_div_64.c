#include "include/main.h"


  int32_t fix_point_div_64(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6)

{
    int32_t $t0 = arg3 - $lo_2;
    int32_t $a3_2 = arg4 - ($lo_1 + $hi_2) - (arg3 < $t0 ? 1 : 0);
    int32_t $v1_3 = 0;
    int32_t $s0_2 = 0;
    int32_t $s1_1 = 0;
        int32_t $v0_3 = $t0 >> 0x1f | $a3_2 << 1;
        int32_t $a1_2 = $s0_2 >> 0x1f | $s1_1 << 1;
    int32_t $v0;
    int32_t $v1;
    $v0 = div64_u64(arg3, arg4, arg5, arg6);
    int32_t $lo_1;
    int32_t $hi_1;
    $hi_1 = HIGHD($v1 * arg5 + arg6 * $v0);
    $lo_1 = LOWD($v1 * arg5 + arg6 * $v0);
    int32_t $lo_2;
    int32_t $hi_2;
    $hi_2 = HIGHD(arg5 * $v0);
    $lo_2 = LOWD(arg5 * $v0);
    
    while ($v1_3 != arg1)
    {
        $t0 <<= 1;
        $a3_2 = $v0_3;
        $s0_2 <<= 1;
        $s1_1 = $a1_2;
        
        if (arg6 >= $v0_3 && ($v0_3 != arg6 || arg5 >= $t0))
        {
            if (arg5 != $t0)
            {
                $v1_3 += 1;
                continue;
            }
            else
            {
                if (arg6 != $v0_3)
                {
                    $v1_3 += 1;
                    continue;
                }
                
                int32_t $v0_4;
                int32_t $v1_4;
                $v0_4 = __ashldi3($s0_2 | 1, $a1_2, arg1 - 1 - $v1_3);
                $s0_2 = $v0_4;
                break;
            }
        }
        
        int32_t $a1_3 = $t0 - arg5;
        int32_t $a0_6 = $t0 < $a1_3 ? 1 : 0;
        $s0_2 |= 1;
        $t0 = $a1_3;
        $a3_2 = $v0_3 - arg6 - $a0_6;
        $v1_3 += 1;
    }
    
    int32_t $v0_5;
    int32_t $v1_5;
    $v0_5 = __ashldi3($v0, $v1, arg1);
    return $v0_5 | $s0_2;
}

