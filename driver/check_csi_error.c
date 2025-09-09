#include "include/main.h"


  void check_csi_error() __noreturn

{
    while (true)
    {
        dump_csi_reg(dump_csd);
        void* $v0_2 = *(dump_csd + 0xb8);
        int32_t $a2_1 = *($v0_2 + 0x20);
        int32_t $s3_1 = *($v0_2 + 0x24);
        
        if ($a2_1)
            isp_printf(0, "snapraw", $a2_1);
        
        if ($s3_1)
            isp_printf(0, 
                "width is %d, height is %d, imagesize is %d\\n, snap num is %d, buf size is %d", 
                $s3_1);
    }
}

