#include "include/main.h"


  int32_t apical_isp_ae_s_roi.isra.36(int32_t* arg1)

{
    int32_t* $v0;
    int32_t $a2;
        return 0xffffffff;
    $v0 = private_kmalloc(0x384, 0xd0);
    
    if (!$v0)
    {

    }
    
    int32_t $a1_1 = *arg1;
    int32_t result = 0xffffffff;
    
    if ($a1_1)
    {
        char var_f8[0xec];
        int32_t i = 0;
        int32_t $a2_1 = 0;
                uint32_t $a1_2 = var_f8[j + i];
                    goto label_158f0;
        private_copy_from_user(&var_f8, $a1_1, 0xe1);
        
        do
        {
            for (int32_t j = 0; (uintptr_t)j != 0xf; )
            {
                
                if ($a1_2 >= 9)
                {

                    result = 0xffffffff;
                }
                
                uint32_t* $a0_6 = $v0 + (j << 2) + $a2_1;
                j += 1;
                *$a0_6 = $a1_2;
            }
            
            i += 0xf;
            $a2_1 = i << 2;
        } while ((uintptr_t)i != 0xe1);
        
        tisp_s_aeroi_weight($v0);
        result = 0;
    }
    
label_158f0:
    private_kfree($v0);
    return result;
}

