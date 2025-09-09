#include "include/main.h"


  int32_t printf_func1(int32_t arg1, int32_t arg2)

{
    int32_t result = IntNum.(uintptr_t)32292_1 < 0x88b8 ? 1 : 0;
        int32_t $s1_1 = arg1;
        int32_t $a2_1 =
        char* i_2 = (char*)(&data_d220c); // Fixed void pointer assignment
    uint32_t IntNum.32292_1 = IntNum.32292;
    IntNum.32292 = IntNum.32292_1 + 1;
    
    if (result)
    {
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
        
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
                    i_2 = &data_d220c;
                    break;
                }
                case 1:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d2590;
                    break;
                }
                case 2:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d2914;
                    break;
                }
                case 3:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d2c98;
                    break;
                }
                case 4:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d301c;
                    break;
                }
                case 5:
                {
                    isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
                    i_2 = &data_d33a0;
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

