#include "include/main.h"


  int32_t* JZ_Isp_Get_Awb_Statistics(int32_t* arg1, int32_t arg2)

{
    uint32_t $t1_1 = arg2 >> 0x1c;
    int32_t $t0 = 0;
    int32_t* result;
    
    while (true)
    {
        result = arg1;
        
        if ($t0 == ((arg2 & 0xf000) >> 0xc) * 0x3c)
            break;
        
        int32_t $t2_1 = 0;
        
        while (true)
        {
            int32_t $v1_1 = $t0 + $t2_1;
            
            if ($t2_1 == $t1_1 << 2)
                break;
            
            *(&awb_array_r + $v1_1) = *result & 0x1fffff;
            $t2_1 += 4;
            *(&awb_array_g + $v1_1) = (result[1] & 0x3ff) << 0xb | *result >> 0x15;
            *(&awb_array_b + $v1_1) = (result[1] & 0x7ffffc00) >> 0xa;
            *(&awb_array_ir + $v1_1) = (result[2] & 0xfffff) << 1 | result[1] >> 0x1f;
            int32_t $a2_16 = result[3];
            result = &result[4];
            *(&awb_array_p + $v1_1) = ($a2_16 & 1) << 0xc | *(result - 8) >> 0x14;
        }
        
        arg1 = &arg1[$t1_1 * 4];
        $t0 += 0x3c;
    }
    
    return result;
}

