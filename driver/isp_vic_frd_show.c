#include "include/main.h"


  int32_t isp_vic_frd_show(void* arg1)

{
    int32_t* $v0 = (int32_t*)((char*)arg1  + 0x3c); // Fixed void pointer arithmetic
        int32_t* $v0_1 = (int32_t*)((char*)$v0  + 0xd4); // Fixed void pointer arithmetic
            int32_t i = 0;
            int32_t $a3_1 = 0;
                int32_t $a1_2 = *(&vic_err + i);
            int32_t $a2 = *($v0_1 + 0x160);
            int32_t $v0_3 = private_seq_printf(arg1, " %d, %d\n", $a2);
            int32_t var_20_1 = data_b29a0;
            int32_t var_24_1 = data_b299c;
            int32_t var_28_1 = data_b2998;
            int32_t var_2c_1 = data_b2994;
            int32_t var_30_1 = data_b2990;
            int32_t var_34_1 = data_b298c;
            int32_t var_38_1 = data_b2988;
            int32_t var_3c_1 = data_b2984;
            int32_t var_40_1 = data_b2980;
            int32_t var_44_1 = data_b297c;
            int32_t var_48_1 = data_b2978;
    
    if ($v0 && $(uintptr_t)v0 < 0xfffff001)
    {
        
        if ($v0_1 && $(uintptr_t)v0_1 < 0xfffff001)
        {
            *((int32_t*)((char*)$v0_1 + 0x164)) = 0; // Fixed void pointer dereference
            
            do
            {
                i += 4;
                $a3_1 += $a1_2;
            } while ((uintptr_t)i != 0x34);
            
            *((int32_t*)((char*)$v0_1 + 0x164)) = $a3_1; // Fixed void pointer dereference
            return $v0_3 + private_seq_printf(arg1, 
                "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", vic_err);
        }
    }
    
    int32_t entry_a2;

    return 0;
}

