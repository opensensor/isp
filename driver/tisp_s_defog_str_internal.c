#include "include/main.h"


  int32_t tisp_s_defog_str_internal(char* arg1)

{
    int32_t $v0_1 = **&param_defog_main_para_array_now;
    int32_t $v1 = 0x1f;
    uint32_t $t3 = *arg1;
    uint32_t defog_wdr_en_1 = defog_wdr_en;
    int32_t $t2_1 = $v1;
    wchar32* $t1 = U"PPZZZPPPPFNMLKJIHFFIGOFDCBBA?>=<;:99LLKJIHGFE";
    
    if ($(uintptr_t)v0_1 >= 0x1f)
        $v1 = $v0_1;
    
    defog_strength_attr = $t3;
    int32_t result;
    int32_t $t5_1;
    int32_t i;
    
    do
    {
        void* defog_trsy0_list_now_1;
        void* $t1_5;
        int32_t $t7_1;
        int32_t $s4_1;
        int32_t $s5_1;
        int32_t $s6_1;
        int32_t result_1;
        int32_t $t8_1;
        int32_t $t9_1;
        
        if (defog_wdr_en_1)
        {
            int32_t result_3;
            void* $t1_6;
            int32_t $t2_6;
            int32_t $t3_5;
            int32_t $t4_5;
            result_3 = defog_itp($t3, $t2_1, $t1[0xee]);
            result_1 = result_3;
            int32_t $v0_5;
            void* $t1_7;
            int32_t $t2_7;
            int32_t $t3_6;
            int32_t $t4_6;
            $v0_5 = $t4_5($t3_5, $t2_6, *($t1_6 + 0x34c));
            $s6_1 = $v0_5;
            int32_t $v0_6;
            void* $t1_8;
            int32_t $t2_8;
            int32_t $t3_7;
            int32_t $t4_7;
            $v0_6 = $t4_6($t3_6, $t2_7, *($t1_7 + 0x370));
            $s4_1 = $v0_6;
            int32_t $v0_7;
            $v0_7 = $t4_7($t3_7, $t2_8, *($t1_8 + 0x394));
            $s5_1 = $v0_7;
            result = result_1;
            defog_trsy0_list_now_1 = defog_trsy0_list_now;
        }
        else
        {
            int32_t result_2;
            void* $t1_1;
            int32_t $t2_2;
            int32_t $t3_1;
            int32_t $t4_1;
            result_2 = defog_itp($t3, $t2_1, *$t1);
            result_1 = result_2;
            int32_t $v0_2;
            void* $t1_2;
            int32_t $t2_3;
            int32_t $t3_2;
            int32_t $t4_2;
            $v0_2 = $t4_1($t3_1, $t2_2, *($t1_1 + 0x24));
            $s6_1 = $v0_2;
            int32_t $v0_3;
            void* $t1_3;
            int32_t $t2_4;
            int32_t $t3_3;
            int32_t $t4_3;
            $v0_3 = $t4_2($t3_2, $t2_3, *($t1_2 + 0x48));
            $s4_1 = $v0_3;
            int32_t $v0_4;
            void* $t1_4;
            int32_t $t2_5;
            int32_t $t3_4;
            int32_t $t4_4;
            $v0_4 = $t4_3($t3_3, $t2_4, *($t1_3 + 0x6c));
            $s5_1 = $v0_4;
            result = $t4_4($t3_4, $t2_5, *($t1_4 + 0x90));
            defog_trsy0_list_now_1 = defog_trsy0_list_now;
        }
        
        $t1 = $t1_5 + 4;
        *(((void**)((char*)defog_trsy0_list_now_1 + $t5_1))) = result_1; // Fixed void pointer dereference
        *(((void**)((char*)defog_trsy1_list_now + $t5_1))) = $s6_1; // Fixed void pointer dereference
        *(*($t9_1 - 0x538c) + $t5_1) = $s4_1;
        *(*($t8_1 - 0x5390) + $t5_1) = $s5_1;
        *(*($t7_1 - 0x5394) + $t5_1) = result;
    } while ($t5_1 + 4 != i);
    return result;
}

