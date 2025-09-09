#include "include/main.h"


  int32_t tisp_s_adr_str_internal(uint32_t arg1)

{
    uint32_t adr_wdr_en_1 = adr_wdr_en;
    char* $v0 = (char*)(&data_a663c); // Fixed void pointer assignment
    int32_t i = 0;
    int32_t $t5 = (uintptr_t)arg1 < 0x81 ? 1 : 0;
        uint32_t $v1_21;
        uint32_t $a2_2;
        uint32_t histSub_4096_diff_2;
        uint32_t $s3_2;
        int32_t $v1_10;
            int32_t $a3_3 = *$v0;
    adr_ratio = arg1;
    
    do
    {
        
        if (adr_wdr_en_1)
        {
            
            if ($t5)
            {
                histSub_4096_diff_2 = (arg1 * $a3_3) >> 7;
                $s3_2 = (arg1 * *($v0 + 0x24)) >> 7;
                $a2_2 = (arg1 * *($v0 + 0x48)) >> 7;
                $v1_21 = (arg1 * *($v0 + 0x6c)) >> 7;
            }
            else
            {
                int32_t $v1_12 = 0x190 - $a3_3;
                int32_t $s3_5 = *($v0 + 0x24);
                int32_t $a1_5 = 0x1f4 - $s3_5;
                int32_t $a2_5 = *($v0 + 0x48);
                int32_t $v1_18 = 0x258 - $a2_5;
                int32_t $a1_7 = 0x258 - $v1_10;
                
                if ($(uintptr_t)a3_3 >= 0x190)
                    $v1_12 = 0;
                
                histSub_4096_diff_2 = (($v1_12 * (arg1 - 0x80)) >> 7) + $a3_3;
                
                if ($(uintptr_t)s3_5 >= 0x1f4)
                    $a1_5 = 0;
                
                $s3_2 = (($a1_5 * (arg1 - 0x80)) >> 7) + $s3_5;
                
                if ($(uintptr_t)a2_5 >= 0x258)
                    $v1_18 = 0;
                
                $a2_2 = (($v1_18 * (arg1 - 0x80)) >> 7) + $a2_5;
                $v1_10 = *($v0 + 0x6c);
            label_5cf94:
                
                if ($(uintptr_t)v1_10 >= 0x258)
                    $a1_7 = 0;
                
                $v1_21 = (($a1_7 * (arg1 - 0x80)) >> 7) + $v1_10;
            }
        }
        else
        {
            int32_t $a3_1 = *($v0 - 0x1d8);
                int32_t $v1_1 = 0x190 - $a3_1;
                int32_t $s3_1 = *($v0 - 0x1b4);
                int32_t $a1_2 = 0x1f4 - $s3_1;
                int32_t $a2_1 = *($v0 - 0x190);
                int32_t $v1_7 = 0x258 - $a2_1;
                goto label_5cf94;
            
            if (!$t5)
            {
                
                if ($(uintptr_t)a3_1 >= 0x190)
                    $v1_1 = 0;
                
                histSub_4096_diff_2 = (($v1_1 * (arg1 - 0x80)) >> 7) + $a3_1;
                
                if ($(uintptr_t)s3_1 >= 0x1f4)
                    $a1_2 = 0;
                
                $s3_2 = (($a1_2 * (arg1 - 0x80)) >> 7) + $s3_1;
                
                if ($(uintptr_t)a2_1 >= 0x258)
                    $v1_7 = 0;
                
                $a2_2 = (($v1_7 * (arg1 - 0x80)) >> 7) + $a2_1;
                $v1_10 = *($v0 - 0x16c);
            }
            
            histSub_4096_diff_2 = (arg1 * $a3_1) >> 7;
            $s3_2 = (arg1 * *($v0 - 0x1b4)) >> 7;
            $a2_2 = (arg1 * *($v0 - 0x190)) >> 7;
            $v1_21 = (arg1 * *($v0 - 0x16c)) >> 7;
        }
        uint32_t histSub_4096_diff_1 = histSub_4096_diff;
        
        if (histSub_4096_diff_1 >= histSub_4096_diff_2)
            histSub_4096_diff_2 = histSub_4096_diff_1;
        
        *((int32_t*)((char*)adr_mapb1_list_now + i)) = histSub_4096_diff_2; // Fixed void pointer dereference
        uint32_t $s4_3 = data_ae7e4_1;
        
        if ($s4_3 >= $s3_2)
            $s3_2 = $s4_3;
        
        *((int32_t*)((char*)adr_mapb2_list_now + i)) = $s3_2; // Fixed void pointer dereference
        uint32_t $s3_8 = data_ae7e8_1;
        
        if ($s3_8 >= $a2_2)
            $a2_2 = $s3_8;
        
        *((int32_t*)((char*)adr_mapb3_list_now + i)) = $a2_2; // Fixed void pointer dereference
        uint32_t $a3_7 = data_ae7ec_1;
        char* $a1_17 = (char*)(adr_mapb4_list_now + i); // Fixed void pointer assignment
        
        if ($a3_7 >= $v1_21)
            $v1_21 = $a3_7;
        
        i += 4;
        *$a1_17 = $v1_21;
        $v0 += 4;
    } while ((uintptr_t)i != 0x24);
    
    tiziano_adr_params_init();
    ev_changed = 1;
    return &data_d0000;
}

