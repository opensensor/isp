#include "include/main.h"


  int32_t isp_vic_interrupt_service_routine(void* arg1)

{
        return 1;
    int32_t* $s0 = (int32_t*)((char*)arg1  + 0xd4); // Fixed void pointer arithmetic
        int32_t* $v0_4 = (int32_t*)((char*)arg1  + 0xb8); // Fixed void pointer arithmetic
        int32_t $v1_7 = ~*($v0_4 + 0x1e8) & *($v0_4 + 0x1e0);
        int32_t $v1_10 = ~*($v0_4 + 0x1ec) & *($v0_4 + 0x1e4);
    if (!(uintptr_t)arg1 || (uintptr_t)arg1 >= 0xfffff001)
    
    
    if ($s0 && $(uintptr_t)s0 < 0xfffff001)
    {
        *((int32_t*)((char*)$v0_4 + 0x1f0)) = $v1_7; // Fixed void pointer dereference
        *(*(arg1 + 0xb8) + 0x1f4) = $v1_10;
        
        if (vic_start_ok)
        {
            int32_t entry_$a2;
            
            if ($v1_7 & 1)
            {
                *($s0 + 0x160) += 1;
                entry_$a2 = vic_framedone_irq_function($s0);
            }
            
            if ($v1_7 & 0x200)
            {
                data_b299c += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x400)
            {
                vic_err += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : hor err ch0 !!!!! 0x3a8 = 0x%08x\n", 
                    *(*(arg1 + 0xb8) + 0x3a8));
            }
            
            if ($v1_7 & 0x800)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x1000)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x2000)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x4000)
            {
                data_b2974 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x8000)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & isp_printf)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & &data_20000)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & &data_40000_1)
            {
                data_b2978 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & &data_80000_1)
            {
                data_b297c += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x100000)
            {
                data_b2980 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x200000)
            {
                data_b2984 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x400000)
            {
                data_b2988 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x800000)
            {
                data_b298c += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x1000000)
            {
                data_b2990 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x2000000)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x4000000)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x8000000)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x10000000)
            {
                data_b2994 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x20000000)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0x40000000)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 < 0)
            {
                data_b29a0 += 1;
                entry_$a2 =
            }
            
            if ($v1_10 & 1)
                entry_a2_1 = vic_mdma_irq_function($s0, 0);
            
            if ($v1_10 & 2)
                entry_a2_2 = vic_mdma_irq_function($s0, 1);
            
            if ($v1_10 & 4)
                entry_a2_3 =
            
            if ($v1_10 & 8)
            {
                data_b2998 += 1;
                entry_$a2 =
            }
            
            if ($v1_7 & 0xde00 && vic_start_ok == 1)
            {
                int32_t* $v0_70 = *($s0 + 0xb8);

                **((int32_t*)((char*)$s0 + 0xb8)) = 4; // Fixed void pointer dereference
                
                while (*$v0_70)
                {

                    $v0_70 = *($s0 + 0xb8);
                }
                
                $v0_70[0x41] = $v0_70[0x41];
                int32_t* $v0_71 = (int32_t*)((char*)$s0  + 0xb8); // Fixed void pointer arithmetic
                *((int32_t*)((char*)$v0_71 + 0x108)) = *($v0_71 + 0x108); // Fixed void pointer dereference
                **((int32_t*)((char*)$s0 + 0xb8)) = 1; // Fixed void pointer dereference
            }
        }
    }
    
    return 1;
}

