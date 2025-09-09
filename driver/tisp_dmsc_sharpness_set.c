#include "include/main.h"


  int32_t tisp_dmsc_sharpness_set(char arg1)

{
    uint32_t $a0_2 = arg1;
    data_9a44c_1 = $a0_2;
    wchar32* i = U"ZZZF2-(((ZZZF2-(((";
    void* $t1 = &dmsc_sp_d_w_stren_array;
    void* $t0 = &dmsc_sp_d_b_stren_array;
    void* $a3 = &dmsc_sp_ud_w_stren_array;
    void* $a2 = &dmsc_sp_ud_b_stren_array;
    void* $a1 = &dmsc_uu_stren_array;
    
    do
    {
        uint32_t $v1_15;
        
        if ($a0_2 >= 0x81)
        {
            wchar32 $t7_1 = *i;
            int32_t $t7_2 = i[9];
            *$t1 = (((0x258 - $t7_1) * ($a0_2 - 0x80)) >> 7) + $t7_1;
            int32_t $t7_3 = i[0x53];
            *$t0 = (((0x258 - $t7_2) * ($a0_2 - 0x80)) >> 7) + $t7_2;
            int32_t $t7_4 = i[0x5c];
            *$a3 = (((0x258 - $t7_3) * ($a0_2 - 0x80)) >> 7) + $t7_3;
            int32_t $t7_5 = *(i - 0x108);
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
    return tisp_dmsc_all_reg_refresh(data_9a430_5);
}

