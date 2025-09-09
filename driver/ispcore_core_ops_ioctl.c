#include "include/main.h"


  int32_t ispcore_core_ops_ioctl(void* arg1, int32_t arg2)

{
    int32_t result;
    int32_t $v0_3;
            char* $v0_5 = (char*)(**(arg1 + 0xc4)); // Fixed void pointer assignment
                goto label_76990;
                goto label_76990;
            int32_t result_1 = $v0_3();
                goto label_76990;
                goto label_76990;
    
    if ((uintptr_t)arg2 == 0x1000000)
    {
        result = 0xffffffed;
        
        if (!(uintptr_t)arg1)
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        else
        {
            result = 0xfffffdfd;
            
            if (!$v0_5)
            
            $v0_3 = *($v0_5 + 4);
            
            if (!$v0_3)
            
        label_7696c:
            result = result_1;
            
            if (!result_1)
            
            if ((uintptr_t)result == 0xfffffdfd)
            {
                result = 0xfffffdfd;
            }
            
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        }
    }
    else if ((uintptr_t)arg2 != 0x1000001)
    {
        char* $s2_1 = (char*)(arg1 + 0x38); // Fixed void pointer assignment
        char* $a0 = (char*)(*$s2_1); // Fixed void pointer assignment
                int32_t $v0_9;
                    char* $v0_10 = (char*)(**($a0 + 0xc4)); // Fixed void pointer assignment
        result = 0;
    label_76990:
        
        while (true)
        {
            if (!$a0)
                $s2_1 += 4;
            else
            {
                
                if ((uintptr_t)arg2 == 0x1000000)
                {
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
                                if ((uintptr_t)result != 0xfffffdfd)
                                    break;
                                
                                result = 0xfffffdfd;
                                $s2_1 += 4;
                            }
                        }
                    }
                }
                else
                {
                        goto label_76a3c;
                    char* $v0_8 = (char*)(*(*($a0 + 0xc4) + 0xc)); // Fixed void pointer assignment
                            goto label_76a38;
                    if ((uintptr_t)arg2 != 0x1000001)
                    
                    
                    if ($v0_8)
                    {
                        $v0_9 = *($v0_8 + 4);
                        
                        if ($v0_9)
                        
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
                    return 0;
                if ((uintptr_t)result == 0xfffffdfd)
                
                break;
            }
            
            $a0 = *$s2_1;
        }
    }
    else
    {
            char* $v0_2 = (char*)(*(*(arg1 + 0xc4) + 0xc)); // Fixed void pointer assignment
                goto label_76990;
        if (arg1)
        {
            
            if (!$v0_2)
            {
                result = 0xfffffdfd;
            }
            
            $v0_3 = *($v0_2 + 4);
            
            if ($v0_3)
                goto label_7696c;
            
            result = 0xfffffdfd;
            goto label_76990;
        }
        
        result = 0xffffffed;
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    return result;
}

