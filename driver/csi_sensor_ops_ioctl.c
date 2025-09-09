#include "include/main.h"


  int32_t csi_sensor_ops_ioctl(void* arg1, int32_t arg2)

{
    if (arg1 && arg1 < 0xfffff001)
    {
        if (arg2 != 0x200000e)
        {
            if (arg2 != 0x200000f)
            {
                if (arg2 == 0x200000c)
                    csi_core_ops_init(arg1, 1, 0x200000f);
            }
            else if (*(*(arg1 + 0x110) + 0x14) == 1)
                *(arg1 + 0x128) = 4;
        }
        else if (*(*(arg1 + 0x110) + 0x14) == 1)
            *(arg1 + 0x128) = 3;
    }
    
    return 0;
}

