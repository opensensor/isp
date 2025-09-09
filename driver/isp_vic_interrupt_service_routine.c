#include "include/main.h"


  int32_t isp_vic_interrupt_service_routine(void* arg1)

{
    if (!arg1 || arg1 >= 0xfffff001)
        return 1;
    
    void* $s0 = *(arg1 + 0xd4);
    
    if ($s0 && $s0 < 0xfffff001)
    {
        void* $v0_4 = *(arg1 + 0xb8);
        int32_t $v1_7 = ~*($v0_4 + 0x1e8) & *($v0_4 + 0x1e0);
        int32_t $v1_10 = ~*($v0_4 + 0x1ec) & *($v0_4 + 0x1e4);
        *($v0_4 + 0x1f0) = $v1_7;
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
                data_b299c_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : frame asfifo ovf!!!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x400)
            {
                vic_err += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : hor err ch0 !!!!! 0x3a8 = 0x%08x\\n", 
                    *(*(arg1 + 0xb8) + 0x3a8));
            }
            
            if ($v1_7 & 0x800)
            {
                data_b29a0_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : hor err ch1 !!!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x1000)
            {
                data_b29a0_3 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : hor err ch2 !!!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x2000)
            {
                data_b29a0_4 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : hor err ch3 !!!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x4000)
            {
                data_b2974_1 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : ver err ch0 !!!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x8000)
            {
                data_b29a0_5 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : ver err ch1 !!!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & isp_printf)
            {
                data_b29a0_6 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : ver err ch2 !!!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & &data_20000_4)
            {
                data_b29a0_7 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : ver err ch3 !!!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & &data_40000_1)
            {
                data_b2978_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : hvf err !!!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & &data_80000_3)
            {
                data_b297c_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : dvp hcomp err!!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x100000)
            {
                data_b2980_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : dma syfifo ovf!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x200000)
            {
                data_b2984_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : control limit err!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x400000)
            {
                data_b2988_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : image syfifo ovf !!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x800000)
            {
                data_b298c_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : mipi fid asfifo ovf!!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x1000000)
            {
                data_b2990_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : mipi ch0 hcomp err !!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x2000000)
            {
                data_b29a0_8 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : mipi ch1 hcomp err !!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x4000000)
            {
                data_b29a0_9 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : mipi ch2 hcomp err !!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x8000000)
            {
                data_b29a0_10 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : mipi ch3 hcomp err !!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x10000000)
            {
                data_b2994_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : mipi ch0 vcomp err !!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x20000000)
            {
                data_b29a0_11 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : mipi ch1 vcomp err !!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0x40000000)
            {
                data_b29a0_12 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : mipi ch2 vcomp err !!!\\n", entry_$a2);
            }
            
            if ($v1_7 < 0)
            {
                data_b29a0_13 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : mipi ch3 vcomp err !!!\\n", entry_$a2);
            }
            
            if ($v1_10 & 1)
                entry_$a2 = vic_mdma_irq_function($s0, 0);
            
            if ($v1_10 & 2)
                entry_$a2 = vic_mdma_irq_function($s0, 1);
            
            if ($v1_10 & 4)
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : dma arb trans done ovf!!!\\n", entry_$a2);
            
            if ($v1_10 & 8)
            {
                data_b2998_2 += 1;
                entry_$a2 = isp_printf(1, "Err [VIC_INT] : dma chid ovf  !!!\\n", entry_$a2);
            }
            
            if ($v1_7 & 0xde00 && vic_start_ok == 1)
            {
                isp_printf(1, "error handler!!!\\n", entry_$a2);
                **($s0 + 0xb8) = 4;
                int32_t* $v0_70 = *($s0 + 0xb8);
                
                while (*$v0_70)
                {
                    isp_printf(1, "addr ctl is 0x%x\\n", *$v0_70);
                    $v0_70 = *($s0 + 0xb8);
                }
                
                $v0_70[0x41] = $v0_70[0x41];
                void* $v0_71 = *($s0 + 0xb8);
                *($v0_71 + 0x108) = *($v0_71 + 0x108);
                **($s0 + 0xb8) = 1;
            }
        }
    }
    
    return 1;
}

