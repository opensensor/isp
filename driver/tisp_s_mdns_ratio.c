#include "include/main.h"


  int32_t tisp_s_mdns_ratio(int32_t arg1)

{
    uint32_t mdns_wdr_en_1 = mdns_wdr_en;
    char* $v0 = (char*)(&data_a1788); // Fixed void pointer assignment
    int32_t i = 0;
    int32_t $t0 = (uintptr_t)arg1 < 0x81 ? 1 : 0;
        char* $t9_2 = (char*)(mdns_y_sad_ave_thres_array_now + i); // Fixed void pointer assignment
        int32_t $v1_13;
        void* $t9_10;
            int32_t $v1_14 = *($v0 - 0xd8);
            uint32_t $v1_15;
                int32_t $a1_13 = 0xc8 - $v1_14;
    data_9ab00 = arg1;
    
    do
    {
        
        if (mdns_wdr_en_1)
        {
            
            if ($t0)
                $v1_15 = (arg1 * $v1_14) >> 7;
            else
            {
                
                if ($(uintptr_t)v1_14 >= 0xc8)
                    $a1_13 = 0;
                
                $v1_15 = (($a1_13 * (arg1 - 0x80)) >> 7) + $v1_14;
            }
            
            *$t9_2 = $v1_15;
            int32_t $v1_17 = *($v0 - 0x90);
            uint32_t $v1_18;
            
            if ($t0)
                $v1_18 = (arg1 * $v1_17) >> 7;
            else
            {
                int32_t $a1_16 = 0xc8 - $v1_17;
                
                if ($(uintptr_t)v1_17 >= 0xc8)
                    $a1_16 = 0;
                
                $v1_18 = (($a1_16 * (arg1 - 0x80)) >> 7) + $v1_17;
            }
            
            *((int32_t*)((char*)mdns_y_sta_ave_thres_array_now + i)) = $v1_18; // Fixed void pointer dereference
            int32_t $v1_20 = *($v0 - 0xb4);
            uint32_t $v1_21;
            
            if ($t0)
                $v1_21 = (arg1 * $v1_20) >> 7;
            else
            {
                int32_t $a1_19 = 0xc8 - $v1_20;
                
                if ($(uintptr_t)v1_20 >= 0xc8)
                    $a1_19 = 0;
                
                $v1_21 = (($a1_19 * (arg1 - 0x80)) >> 7) + $v1_20;
            }
            
            *((int32_t*)((char*)mdns_y_sad_ass_thres_array_now + i)) = $v1_21; // Fixed void pointer dereference
            int32_t $v1_23 = *($v0 - 0x6c);
            uint32_t $v1_24;
            
            if ($t0)
                $v1_24 = (arg1 * $v1_23) >> 7;
            else
            {
                int32_t $a1_22 = 0xc8 - $v1_23;
                
                if ($(uintptr_t)v1_23 >= 0xc8)
                    $a1_22 = 0;
                
                $v1_24 = (($a1_22 * (arg1 - 0x80)) >> 7) + $v1_23;
            }
            
            *((int32_t*)((char*)mdns_y_sta_ass_thres_array_now + i)) = $v1_24; // Fixed void pointer dereference
            $v1_13 = *$v0;
            $t9_10 = mdns_y_ref_wei_b_min_array_now + i;
        }
        else
        {
            int32_t $v1_1 = *($v0 - 0x10d0);
            uint32_t $v1_2;
                int32_t $a1_1 = 0xc8 - $v1_1;
            
            if ($t0)
                $v1_2 = (arg1 * $v1_1) >> 7;
            else
            {
                
                if ($(uintptr_t)v1_1 >= 0xc8)
                    $a1_1 = 0;
                
                $v1_2 = (($a1_1 * (arg1 - 0x80)) >> 7) + $v1_1;
            }
            
            *$t9_2 = $v1_2;
            int32_t $v1_4 = *($v0 - 0xff8);
            uint32_t $v1_5;
            
            if ($t0)
                $v1_5 = (arg1 * $v1_4) >> 7;
            else
            {
                int32_t $a1_4 = 0xc8 - $v1_4;
                
                if ($(uintptr_t)v1_4 >= 0xc8)
                    $a1_4 = 0;
                
                $v1_5 = (($a1_4 * (arg1 - 0x80)) >> 7) + $v1_4;
            }
            
            *((int32_t*)((char*)mdns_y_sta_ave_thres_array_now + i)) = $v1_5; // Fixed void pointer dereference
            int32_t $v1_7 = *($v0 - 0x1064);
            uint32_t $v1_8;
            
            if ($t0)
                $v1_8 = (arg1 * $v1_7) >> 7;
            else
            {
                int32_t $a1_7 = 0xc8 - $v1_7;
                
                if ($(uintptr_t)v1_7 >= 0xc8)
                    $a1_7 = 0;
                
                $v1_8 = (($a1_7 * (arg1 - 0x80)) >> 7) + $v1_7;
            }
            
            *((int32_t*)((char*)mdns_y_sad_ass_thres_array_now + i)) = $v1_8; // Fixed void pointer dereference
            int32_t $v1_10 = *($v0 - 0xfb0);
            uint32_t $v1_11;
            
            if ($t0)
                $v1_11 = (arg1 * $v1_10) >> 7;
            else
            {
                int32_t $a1_10 = 0xc8 - $v1_10;
                
                if ($(uintptr_t)v1_10 >= 0xc8)
                    $a1_10 = 0;
                
                $v1_11 = (($a1_10 * (arg1 - 0x80)) >> 7) + $v1_10;
            }
            
            *((int32_t*)((char*)mdns_y_sta_ass_thres_array_now + i)) = $v1_11; // Fixed void pointer dereference
            $v1_13 = *($v0 - 0xdec);
            $t9_10 = mdns_y_ref_wei_b_min_array_now + i;
        }
        
        uint32_t $v1_27;
        
        if ($t0)
            $v1_27 = (arg1 * $v1_13) >> 7;
        else
        {
            int32_t $a1_25 = 0xc8 - $v1_13;
            
            if ($(uintptr_t)v1_13 >= 0xc8)
                $a1_25 = 0;
            
            $v1_27 = (($a1_25 * (arg1 - 0x80)) >> 7) + $v1_13;
        }
        
        i += 4;
        *$t9_10 = $v1_27;
        $v0 += 4;
    } while ((uintptr_t)i != 0x24);
    
    tisp_mdns_all_reg_refresh(data_9a9d0_1);
    /* tailcall */
    return tisp_mdns_reg_trigger();
}

