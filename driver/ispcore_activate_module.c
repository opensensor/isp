#include "include/main.h"


  int32_t ispcore_activate_module(void* arg1)

{
    int32_t result = 0xffffffea;
    
    if (arg1)
    {
        if (arg1 >= 0xfffff001)
            return 0xffffffea;
        
        void* $s0_1 = *(arg1 + 0xd4);
        result = 0xffffffea;
        
        if ($s0_1 && $s0_1 < 0xfffff001)
        {
            result = 0;
            
            if (*($s0_1 + 0xe8) == 1)
            {
                int32_t* $s2_1 = *(arg1 + 0xbc);
                int32_t i = 0;
                
                while (i < *(arg1 + 0xc0))
                {
                    if (private_clk_get_rate(*$s2_1) != 0xffff)
                        private_clk_set_rate(*$s2_1, isp_clk);
                    
                    private_clk_enable(*$s2_1);
                    i += 1;
                    $s2_1 = &$s2_1[1];
                }
                
                int32_t $a2_1 = 0;
                void* $a3_1;
                
                while (true)
                {
                    $a3_1 = $a2_1 * 0xc4;
                    
                    if ($a2_1 >= *($s0_1 + 0x154))
                        break;
                    
                    void* $v0_6 = $a3_1 + *($s0_1 + 0x150);
                    
                    if (*($v0_6 + 0x74) != 1)
                    {
                        isp_printf(2, "Err [VIC_INT] : mipi ch0 hcomp err !!!\\n", $a2_1);
                        return 0xffffffff;
                    }
                    
                    *($v0_6 + 0x74) = 2;
                    $a2_1 += 1;
                }
                
                void* $a0_3 = *($s0_1 + 0x1bc);
                (*($a0_3 + 0x40cc))($a0_3, 0x4000000, 0, $a3_1);
                void* $s2_2 = $s0_1 + 0x38;
                void* $s1_2 = *$s2_2;
                
                while (true)
                {
                    if (!$s1_2)
                        $s2_2 += 4;
                    else if ($s1_2 >= 0xfffff001)
                        $s2_2 += 4;
                    else
                    {
                        int32_t* $v0_10 = *(*($s1_2 + 0xc4) + 0x10);
                        
                        if (!$v0_10)
                            $s2_2 += 4;
                        else
                        {
                            int32_t $v0_11 = *$v0_10;
                            
                            if (!$v0_11)
                                $s2_2 += 4;
                            else
                            {
                                int32_t $v0_12 = $v0_11($s1_2);
                                $s2_2 += 4;
                                
                                if ($v0_12 && $v0_12 != 0xfffffdfd)
                                {
                                    isp_printf(2, "Err [VIC_INT] : mipi ch1 hcomp err !!!\\n", 
                                        *($s1_2 + 8));
                                    break;
                                }
                            }
                        }
                    }
                    
                    if ($s2_2 == $s0_1 + 0x78)
                        break;
                    
                    $s1_2 = *$s2_2;
                }
                
                *($s0_1 + 0xe8) = 2;
                return 0;
            }
        }
    }
    
    return result;
}

