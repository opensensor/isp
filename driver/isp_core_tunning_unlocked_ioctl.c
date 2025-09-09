#include "include/main.h"


  int32_t isp_core_tunning_unlocked_ioctl(void* arg1, int32_t arg2, int32_t arg3)

{
    int32_t* $s0 = *(*(*(arg1 + 0x70) + 0xc8) + 0x1bc);
    int32_t $v0_2 = 0xffffffff;
    
    if ($s0[0x1031] == 3)
    {
        int32_t var_20_9;
        int32_t $s0_1;
        
        if (arg2 != 0xc008561b)
        {
            int32_t* $a0_2;
            int32_t* $a1_1;
            int32_t $a2;
            
            if (arg2 == 0xc008561c)
            {
                int32_t $v0_3;
                $v0_3 = private_copy_from_user(&var_20_10, arg3, 8);
                $a1_1 = &var_20_11;
                
                if ($v0_3)
                    return 0xfffffff2;
                
                $a0_2 = $s0;
                goto label_189b0;
            }
            
            int32_t $v0_6;
            $v0_6 = private_copy_from_user(&var_20_12, arg3, 0xc);
            int32_t $v0_10;
            
            if ($v0_6)
            {
            label_189f8:
                $s0_1 = 0xfffffff2;
            label_18a00:
                $v0_10 = $s0_1 ^ 0xfffffdfd;
            }
            else
            {
                void var_1c_6;
                $a1_1 = &var_1c_7;
                $a0_2 = $s0;
                
                if (!var_20_13)
                {
                label_189b0:
                    $s0_1 = apical_isp_core_ops_s_ctrl($a0_2, $a1_1, $a2);
                    goto label_18a00;
                }
                
                int32_t $v0_9 = apical_isp_core_ops_g_ctrl($a0_2, $a1_1);
                $s0_1 = $v0_9;
                
                if ($v0_9)
                    $v0_10 = $s0_1 ^ 0xfffffdfd;
                
                if (!$v0_9 || $s0_1 == 0xfffffdfd)
                {
                    if (!private_copy_to_user(arg3, &var_20_14, 0xc))
                        goto label_18a00;
                    
                    goto label_189f8;
                }
            }
            
            if (!$v0_10)
                return 0;
            
            return $s0_1;
        }
        
        $v0_2 = 0xfffffff2;
        
        if (!private_copy_from_user(&var_20_15, arg3, 8))
        {
            $v0_2 = apical_isp_core_ops_g_ctrl($s0, &var_20_16);
            $s0_1 = $v0_2;
            
            if (!$v0_2 || $v0_2 == 0xfffffdfd)
            {
                if (private_copy_to_user(arg3, &var_20_17, 8))
                    return 0xfffffff2;
                
                goto label_18a00;
            }
        }
    }
    
    return $v0_2;
}

