#include "include/main.h"


  int32_t printf_func0(int32_t arg1, int32_t arg2)

{
    int32_t result = IntNum.(uintptr_t)32264_1 < 0x88b8 ? 1 : 0;
        int32_t $s1_1 = arg1;
        int32_t $a2_1 = isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        char* i_2 = (char*)(&IspAeStatic); // Fixed void pointer assignment
    uint32_t IntNum.32264_1 = IntNum.32264;
    IntNum.32264 = IntNum.32264_1 + 1;
    
    if (result)
    {
        
        while (true)
        {
            result = $s1_1 - 1;
            
            if (arg2 < $s1_1)
                break;
            
            switch (result)
            {
                case 0:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 5 arguments;
                    i_2 = &IspAeStatic;
                    break;
                }
                case 1:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d0878;
                    break;
                }
                case 2:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d0bfc;
                    break;
                }
                case 3:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d1304;
                    break;
                }
                case 4:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d1688;
                    break;
                }
                case 5:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d0f80;
                    break;
                }
                case 6:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d1a0c;
                    break;
                }
                case 7:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 2 arguments\n", $a2_1);
                    i_2 = &data_d1e0c;
                    break;
                }
            }
            
            char* i = (char*)(i_2); // Fixed void pointer assignment
            int32_t j = 0;
            int32_t $a2_4;
            
            do
            {
                char* i_1 = (char*)(i); // Fixed void pointer assignment
                int32_t $a2_3;
                
                do
                {
                    $a2_3 = isp_printf(1, 
                        "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n", *i_1);
                    j += 4;
                    i_1 = i + j;
                } while ((uintptr_t)j != 0x3c);
                
                i += 0x3c;
                $a2_4 =
                    isp_printf(); // Fixed: macro with no parameters, removed 2 arguments\n", $a2_3);
                j = 0;
            } while (i != i_2 + 0x384);
            
            $a2_1 = isp_printf(); // Fixed: macro with no parameters, removed 2 arguments\n", $a2_4);
            $s1_1 += 1;
        }
    }
    
    return result;
}

