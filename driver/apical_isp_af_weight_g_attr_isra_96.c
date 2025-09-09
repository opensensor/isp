#include "include/main.h"


  int32_t apical_isp_af_weight_g_attr.isra.96(int32_t* arg1)

{
    void* $v0;
    int32_t $a2;
    $v0 = private_kmalloc(0x384, 0xd0);
    
    if (!$v0)
    {
        isp_printf(1, "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\\n", $a2);
        return 0xffffffff;
    }
    
    int32_t result = tisp_g_af_weight($v0);
    int32_t i = 0;
    
    if (!result)
    {
        int32_t $a2_1 = 0;
        char var_f8_15[0xe8];
        
        do
        {
            for (int32_t j = 0; j != 0xf; )
            {
                void* $a1_2 = &var_f8_16[j + i];
                char $a0_4 = *($v0 + (j << 2) + $a2_1);
                j += 1;
                *$a1_2 = $a0_4;
            }
            
            i += 0xf;
            $a2_1 = i << 2;
        } while (i != 0xe1);
        
        private_copy_to_user(*arg1, &var_f8_17, 0xe1);
    }
    else
        isp_printf(1, 
            "width is %d, height is %d, imagesize is %d\\n, snap num is %d, buf size is %d", 
            "apical_isp_af_weight_g_attr");
    
    private_kfree($v0);
    return result;
}

