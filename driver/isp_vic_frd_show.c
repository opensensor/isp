#include "include/main.h"


  int32_t isp_vic_frd_show(void* arg1)

{
    void* $v0 = *(arg1 + 0x3c);
    
    if ($v0 && $v0 < 0xfffff001)
    {
        void* $v0_1 = *($v0 + 0xd4);
        
        if ($v0_1 && $v0_1 < 0xfffff001)
        {
            *($v0_1 + 0x164) = 0;
            int32_t i = 0;
            int32_t $a3_1 = 0;
            
            do
            {
                int32_t $a1_2 = *(&vic_err + i);
                i += 4;
                $a3_1 += $a1_2;
            } while (i != 0x34);
            
            int32_t $a2 = *($v0_1 + 0x160);
            *($v0_1 + 0x164) = $a3_1;
            int32_t $v0_3 = private_seq_printf(arg1, " %d, %d\\n", $a2);
            int32_t var_20_1_1 = data_b29a0_1;
            int32_t var_24_1 = data_b299c_1;
            int32_t var_28_1 = data_b2998_1;
            int32_t var_2c_1 = data_b2994_1;
            int32_t var_30_1 = data_b2990_1;
            int32_t var_34_1 = data_b298c_1;
            int32_t var_38_1 = data_b2988_1;
            int32_t var_3c_1 = data_b2984_1;
            int32_t var_40_1 = data_b2980_1;
            int32_t var_44_1 = data_b297c_1;
            int32_t var_48_1 = data_b2978_1;
            return $v0_3 + private_seq_printf(arg1, 
                "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\\n", vic_err);
        }
    }
    
    int32_t entry_$a2;
    isp_printf(2, "The parameter is invalid!\\n", entry_$a2);
    return 0;
}

