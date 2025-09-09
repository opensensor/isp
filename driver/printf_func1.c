#include "include/main.h"


  int32_t printf_func1(int32_t arg1, int32_t arg2)

{
    uint32_t IntNum.32292_1 = IntNum.32292;
    IntNum.32292 = IntNum.32292_1 + 1;
    int32_t result = IntNum.32292_1 < 0x88b8 ? 1 : 0;
    
    if (result)
    {
        int32_t $s1_1 = arg1;
        int32_t $a2_1 =
            isp_printf(1, "%s[%d] VIC do not support this format %d\\n", IntNum.32292_1 + 1);
        void* i_2 = &data_d220c;
        
        while (true)
        {
            result = $s1_1 - 1;
            
            if (arg2 < $s1_1)
                break;
            
            switch (result)
            {
                case 0:
                {
                    isp_printf(1, "flags = 0x%08x, jzflags = %p,0x%08x", $a2_1);
                    i_2 = &data_d220c_1;
                    break;
                }
                case 1:
                {
                    isp_printf(1, "Can not support this frame mode!!!\\n", $a2_1);
                    i_2 = &data_d2590;
                    break;
                }
                case 2:
                {
                    isp_printf(1, "sensor type is BT1120!\\n", $a2_1);
                    i_2 = &data_d2914;
                    break;
                }
                case 3:
                {
                    isp_printf(1, "VIC_CTRL : %08x\\n", $a2_1);
                    i_2 = &data_d2c98;
                    break;
                }
                case 4:
                {
                    isp_printf(1, "not support the gpio mode!\\n", $a2_1);
                    i_2 = &data_d301c;
                    break;
                }
                case 5:
                {
                    isp_printf(1, "sensor type is BT601!\\n", $a2_1);
                    i_2 = &data_d33a0;
                    break;
                }
            }
            
            void* i = i_2;
            int32_t j = 0;
            int32_t $a2_4;
            
            do
            {
                void* i_1 = i;
                int32_t $a2_3;
                
                do
                {
                    $a2_3 = isp_printf(1, 
                        "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\\n", *i_1);
                    j += 4;
                    i_1 = i + j;
                } while (j != 0x3c);
                
                i += 0x3c;
                $a2_4 =
                    isp_printf(1, "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n", $a2_3);
                j = 0;
            } while (i != i_2 + 0x384);
            
            $a2_1 = isp_printf(1, "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n", $a2_4);
            $s1_1 += 1;
        }
    }
    
    return result;
}

