#include "include/main.h"


  int32_t tisp_s_dpc_str_internal(int32_t arg1)

{
    wchar32* i = U"dP<<2(";
    char* $t0 = (char*)(&dpc_d_m1_dthres_array); // Fixed void pointer assignment
    char* $a3 = (char*)(&dpc_d_m3_dthres_array); // Fixed void pointer assignment
    char* $a1 = (char*)(&dpc_d_m1_fthres_array); // Fixed void pointer assignment
    char* $v1 = (char*)(&dpc_d_m3_fthres_array); // Fixed void pointer assignment
        int32_t $a2_13;
            int32_t $t9_1 = *(i - 0x24);
            int32_t $t9_6 = i[0x74];
    data_9ab24 = arg1;
    
    do
    {
        
        if ((uintptr_t)arg1 >= 0x81)
        {
            *$a1 = (((0x4b0 - $t9_1) * (arg1 - 0x80)) >> 7) + $t9_1;
            *$v1 = (((0x4b0 - $t9_6) * (arg1 - 0x80)) >> 7) + $t9_6;
            *$t0 = ((0x100 - arg1) * *i + arg1 * 5 - 0x280) >> 7;
            $a2_13 = (0x100 - arg1) * i[0x7d] + arg1 * 5 - 0x280;
        }
        else
        {
            uint32_t $a2_3 = (arg1 * *(i - 0x24)) >> 7;
            uint32_t $a2_6 = (arg1 * i[0x74]) >> 7;
            
            if ($a2_3 < 5)
                *$a1 = 5;
            else
                *$a1 = $a2_3;
            
            
            if ($a2_6 < 5)
                *$v1 = 5;
            else
                *$v1 = $a2_6;
            
            *$t0 = ((*i - 0x3e8) * arg1 + 0x1f400) >> 7;
            $a2_13 = (i[0x7d] - 0x3e8) * arg1 + 0x1f400;
        }
        
        i = &i[1];
        *$a3 = $a2_13 >> 7;
        $t0 += 4;
        $a3 += 4;
        $a1 += 4;
        $v1 += 4;
    } while ((uintptr_t)i != 0x9f02c);
    
    /* tailcall */
    return tisp_dpc_all_reg_refresh(data_9ab10_2 + 0x200);
}

