#include "include/main.h"


  int32_t mbus_to_bayer_write(int32_t arg1)

{
        int32_t var_10_1 = arg1;
    if (arg1 - (uintptr_t)0x3001 >= 0x14)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    else
    {
        int32_t $a1_1;
        
        switch (arg1)
        {
            case 0x3001:
            case 0x3003:
            case 0x3004:
            case 0x3005:
            case 0x3006:
            case 0x3007:
            case 0x3008:
            case 0x300b:
            {
                $a1_1 = 1;
                break;
            }
            case 0x3002:
            case 0x3009:
            case 0x300a:
            case 0x3011:
            {
                $a1_1 = 2;
                break;
            }
            case 0x300c:
            case 0x300e:
            case 0x3010:
            case 0x3013:
            {
                $a1_1 = 3;
                break;
            }
            case 0x300d:
            case 0x300f:
            case 0x3012:
            case 0x3014:
            {
                $a1_1 = 0;
                break;
            }
        }
        
        system_reg_write(8, $a1_1);
    }
    
    return 0;
}

