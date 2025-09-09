#include "include/main.h"


  void check_csi_error() __noreturn

{
        int32_t* $v0_2 = (int32_t*)((char*)dump_csd  + 0xb8); // Fixed void pointer arithmetic
        int32_t $a2_1 = *($v0_2 + 0x20);
        int32_t $s3_1 = *($v0_2 + 0x24);
    while (true)
    {
        dump_csi_reg(dump_csd);
        
        if ($a2_1)

        
        if ($s3_1)
            isp_printf(0, 
                "width is %d, height is %d, imagesize is %d\n, snap num is %d, buf size is %d", 
                $s3_1);
    }
}

