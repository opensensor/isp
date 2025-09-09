#include "include/main.h"


  int32_t tisp_dmsc_sharpness_set(char arg1)

{
    uint32_t $a0_2 = arg1;
    wchar_t* i = U"ZZZF2-(((ZZZF2-(((";
    char* $t1 = (char*)(&dmsc_sp_d_w_stren_array); // Fixed void pointer assignment
    char* $t0 = (char*)(&dmsc_sp_d_b_stren_array); // Fixed void pointer assignment
    char* $a3 = (char*)(&dmsc_sp_ud_w_stren_array); // Fixed void pointer assignment
    char* $a2 = (char*)(&dmsc_sp_ud_b_stren_array); // Fixed void pointer assignment
    char* $a1 = (char*)(&dmsc_uu_stren_array); // Fixed void pointer assignment
        uint32_t $v1_15;
            wchar_t $t7_1 = *i;
            int32_t $t7_2 = i[9];
            int32_t $t7_3 = i[0x53];
            int32_t $t7_4 = i[0x5c];
            int32_t $t7_5 = *(i - 0x108);
    data_9a44c = $a0_2;
    
    do
    {
        
        if ($(uintptr_t)a0_2 >= 0x81)
        {
            *$t1 = (((0x258 - $t7_1) * ($a0_2 - 0x80)) >> 7) + $t7_1;
            *$t0 = (((0x258 - $t7_2) * ($a0_2 - 0x80)) >> 7) + $t7_2;
            *$a3 = (((0x258 - $t7_3) * ($a0_2 - 0x80)) >> 7) + $t7_3;
            *$a2 = (((0x258 - $t7_4) * ($a0_2 - 0x80)) >> 7) + $t7_4;
            $v1_15 = (((0x320 - $t7_5) * ($a0_2 - 0x80)) >> 7) + $t7_5;
        }
        else
        {
            *$t1 = ($a0_2 * *i) >> 7;
            *$t0 = ($a0_2 * i[9]) >> 7;
            *$a3 = ($a0_2 * i[0x53]) >> 7;
            *$a2 = ($a0_2 * i[0x5c]) >> 7;
            $v1_15 = ($a0_2 * *(i - 0x108)) >> 7;
        }
        
        i = &i[1];
        *$a1 = $v1_15;
        $t1 += 4;
        $t0 += 4;
        $a3 += 4;
        $a2 += 4;
        $a1 += 4;
    } while (i != &data_9e0d0_1[9]);
    
    /* tailcall */
    return tisp_dmsc_all_reg_refresh(data_9a430_2);
}

