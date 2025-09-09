#include "include/main.h"


  int32_t ispcore_core_ops_ioctl(void* arg1, int32_t arg2)

{
    int32_t result;
    int32_t $v0_3;
    
    if (arg2 == 0x1000000)
    {
        result = 0xffffffed;
        
        if (!arg1)
            isp_printf(2, &$LC0, "ispcore_core_ops_ioctl");
        else
        {
            void* $v0_5 = **(arg1 + 0xc4);
            result = 0xfffffdfd;
            
            if (!$v0_5)
                goto label_76990;
            
            $v0_3 = *($v0_5 + 4);
            
            if (!$v0_3)
                goto label_76990;
            
        label_7696c:
            int32_t result_1 = $v0_3();
            result = result_1;
            
            if (!result_1)
                goto label_76990;
            
            if (result == 0xfffffdfd)
            {
                result = 0xfffffdfd;
                goto label_76990;
            }
            
            isp_printf(2, &$LC0, "ispcore_core_ops_ioctl");
        }
    }
    else if (arg2 != 0x1000001)
    {
        result = 0;
    label_76990:
        void* $s2_1 = arg1 + 0x38;
        void* $a0 = *$s2_1;
        
        while (true)
        {
            if (!$a0)
                $s2_1 += 4;
            else
            {
                int32_t $v0_9;
                
                if (arg2 == 0x1000000)
                {
                    void* $v0_10 = **($a0 + 0xc4);
                    result = 0xfffffdfd;
                    
                    if (!$v0_10)
                        $s2_1 += 4;
                    else
                    {
                        $v0_9 = *($v0_10 + 4);
                        
                        if (!$v0_9)
                            $s2_1 += 4;
                        else
                        {
                        label_76a38:
                            result = $v0_9();
                        label_76a3c:
                            
                            if (!result)
                                $s2_1 += 4;
                            else
                            {
                                if (result != 0xfffffdfd)
                                    break;
                                
                                result = 0xfffffdfd;
                                $s2_1 += 4;
                            }
                        }
                    }
                }
                else
                {
                    if (arg2 != 0x1000001)
                        goto label_76a3c;
                    
                    void* $v0_8 = *(*($a0 + 0xc4) + 0xc);
                    
                    if ($v0_8)
                    {
                        $v0_9 = *($v0_8 + 4);
                        
                        if ($v0_9)
                            goto label_76a38;
                        
                        result = 0xfffffdfd;
                        $s2_1 += 4;
                    }
                    else
                    {
                        result = 0xfffffdfd;
                        $s2_1 += 4;
                    }
                }
            }
            
            if ($s2_1 == arg1 + 0x78)
            {
                if (result == 0xfffffdfd)
                    return 0;
                
                break;
            }
            
            $a0 = *$s2_1;
        }
    }
    else
    {
        if (arg1)
        {
            void* $v0_2 = *(*(arg1 + 0xc4) + 0xc);
            
            if (!$v0_2)
            {
                result = 0xfffffdfd;
                goto label_76990;
            }
            
            $v0_3 = *($v0_2 + 4);
            
            if ($v0_3)
                goto label_7696c;
            
            result = 0xfffffdfd;
            goto label_76990;
        }
        
        result = 0xffffffed;
        isp_printf(2, &$LC0, "ispcore_core_ops_ioctl");
    }
    return result;
}

