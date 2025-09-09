#include "include/main.h"


  int32_t isp_subdev_release_clks(void* arg1)

{
    int32_t* $s1 = *(arg1 + 0xbc);
    
    if ($s1)
    {
        int32_t* $s3_1 = $s1;
        int32_t i = 0;
        
        while (i < *(arg1 + 0xc0))
        {
            private_clk_put(*$s3_1);
            i += 1;
            $s3_1 = &$s3_1[1];
        }
        
        private_kfree($s1);
        *(arg1 + 0xbc) = 0;
    }
    
    return 0;
}

