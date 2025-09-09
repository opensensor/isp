#include "include/main.h"


  int32_t ispcore_slake_module(int32_t* arg1)

{
    int32_t result = 0xffffffea;
    
    if (arg1)
    {
        if (arg1 >= 0xfffff001)
            return 0xffffffea;
        
        void* $s0_1 = arg1[0x35];
        result = 0xffffffea;
        
        if ($s0_1 && $s0_1 < 0xfffff001)
        {
            int32_t $v0 = *($s0_1 + 0xe8);
            
            if ($v0 != 1)
            {
                if ($v0 >= 3)
                {
                    isp_printf(0, "Err [VIC_INT] : dma chid ovf  !!!\\n", "ispcore_slake_module");
                    ispcore_core_ops_init(arg1, 0);
                }
                
                int32_t $v0_2 = 0;
                
                while (true)
                {
                    void* $a2_1 = $v0_2 * 0xc4;
                    
                    if ($v0_2 >= *($s0_1 + 0x154))
                        break;
                    
                    $v0_2 += 1;
                    *($a2_1 + *($s0_1 + 0x150) + 0x74) = 1;
                }
                
                void* $a0_1 = *($s0_1 + 0x1bc);
                (*($a0_1 + 0x40cc))($a0_1, 0x4000001, 0);
                *($s0_1 + 0xe8) = 1;
                void* $s3_1 = $s0_1 + 0x38;
                void* $s2_1 = *$s3_1;
                int32_t $s0_3;
                
                while (true)
                {
                    if (!$s2_1)
                        $s3_1 += 4;
                    else if ($s2_1 >= 0xfffff001)
                        $s3_1 += 4;
                    else
                    {
                        void* $v0_6 = *(*($s2_1 + 0xc4) + 0x10);
                        
                        if (!$v0_6)
                            $s3_1 += 4;
                        else
                        {
                            int32_t $v0_7 = *($v0_6 + 4);
                            
                            if (!$v0_7)
                                $s3_1 += 4;
                            else
                            {
                                int32_t $v0_8 = $v0_7($s2_1);
                                
                                if (!$v0_8)
                                    $s3_1 += 4;
                                else
                                {
                                    if ($v0_8 != 0xfffffdfd)
                                    {
                                        isp_printf(2, "error handler!!!\\n", *($s2_1 + 8));
                                        $s0_3 = arg1[0x30];
                                        break;
                                    }
                                    
                                    $s3_1 += 4;
                                }
                            }
                        }
                    }
                    
                    if ($s0_1 + 0x78 == $s3_1)
                    {
                        $s0_3 = arg1[0x30];
                        break;
                    }
                    
                    $s2_1 = *$s3_1;
                }
                
                int32_t $s2_2 = $s0_3 - 1;
                int32_t* $s0_5 = arg1[0x2f] + ($s0_3 << 2);
                
                while (true)
                {
                    $s0_5 = &$s0_5[-1];
                    
                    if ($s2_2 < 0)
                        break;
                    
                    private_clk_disable(*$s0_5);
                    $s2_2 -= 1;
                }
            }
            
            return 0;
        }
    }
    
    return result;
}

