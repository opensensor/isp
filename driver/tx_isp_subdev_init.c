#include "include/main.h"


  int32_t tx_isp_subdev_init(int32_t* arg1, void* arg2, int32_t arg3)

{
    if (!arg1 || !arg2)
    {
        isp_printf(2, tiziano_wdr_gamma_refresh, "tx_isp_subdev_init");
        return 0xffffffea;
    }
    
    *(arg2 + 0xc4) = arg3;
    
    if (tx_isp_module_init(arg1, arg2))
    {
        isp_printf(2, "&vsd->snap_mlock", *arg1);
        return 0xfffffff4;
    }
    
    char* $s1_1 = arg1[0x16];
    
    if ($s1_1)
    {
        int32_t result_1;
        int32_t $a2_1;
        result_1 = tx_isp_request_irq(arg1, arg2 + 0x80);
        int32_t result = result_1;
        
        if (result_1)
        {
            isp_printf(2, " %d, %d\\n", $a2_1);
            tx_isp_module_deinit(arg2);
            return result;
        }
        
        int32_t* $v0_2;
        int32_t result_4;
        int32_t* $s3_2;
        
        while (true)
        {
            result_4 = result;
            
            if (result >= arg1[0x32])
            {
                $s3_2 = nullptr;
                goto label_1f840;
            }
            
            $v0_2 = private_platform_get_resource(arg1, 0x200, result_4);
            
            if ($v0_2)
            {
                char* $v1_1 = $v0_2[2];
                char const* const $a0_2 = "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\\n";
                uint32_t $a1_1 = *$v1_1;
                uint32_t $at_1;
                
                while (true)
                {
                    $at_1 = *$a0_2;
                    $v1_1 = &$v1_1[1];
                    $a0_2 = &$a0_2[1];
                    
                    if ($at_1 != $a1_1)
                        break;
                    
                    uint32_t temp1_1 = $a1_1;
                    $a1_1 = *$v1_1;
                    
                    if (!temp1_1)
                    {
                        $a1_1 = $at_1;
                        break;
                    }
                }
                
                if (!($a1_1 - $at_1))
                    break;
                
                result += 1;
            }
            else
                result += 1;
        }
        
        int32_t $a0_18 = *$v0_2;
        int32_t $a2_11 = arg1[0xf];
        
        if (!$a2_11)
            $a2_11 = arg1[6];
        
        int32_t* $v0_22 = private_request_mem_region($a0_18, $v0_2[1] + 1 - $a0_18, $a2_11);
        $s3_2 = $v0_22;
        
        if (!$v0_22)
        {
            isp_printf(2, "The parameter is invalid!\\n", "tx_isp_subdev_init");
            result = 0xfffffff0;
            goto label_1fb80;
        }
        
        int32_t $a0_19 = *$v0_22;
        int32_t $v0_23;
        $v0_23 = private_ioremap($a0_19, $v0_22[1] + 1 - $a0_19);
        *(arg2 + 0xb8) = $v0_23;
        
        if (!$v0_23)
        {
            isp_printf(2, "vic_done_gpio%d", "tx_isp_subdev_init");
            result = 0xfffffffa;
            goto label_1fb60;
        }
        
    label_1f840:
        *(arg2 + 0xb4) = $s3_2;
        uint32_t $v0_5 = *$s1_1;
        
        if ($v0_5 != 1)
        {
            if ($v0_5 != 2)
            {
                isp_printf(0, tiziano_wdr_params_refresh, result_4);
                return 0;
            }
            
            *(arg2 + 0xc0) = $s1_1[4];
            int32_t result_3 = isp_subdev_init_clks(arg2, *($s1_1 + 8));
            result = result_3;
            
            if (!result_3)
                return 0;
            
            goto label_1fb44;
        }
        
        *(arg2 + 0xc0) = $s1_1[4];
        int32_t result_2 = isp_subdev_init_clks(arg2, *($s1_1 + 8));
        result = result_2;
        
        if (result_2)
        {
            isp_printf(2, "register is 0x%x, value is 0x%x\\n", *(arg2 + 8));
        label_1fb44:
            isp_printf(2, "Can\'t output the width(%d)!\\n", 0x77d);
            private_iounmap(*(arg2 + 0xb8));
        label_1fb60:
            int32_t $a0_15 = *$s3_2;
            private_release_mem_region($a0_15, $s3_2[1] + 1 - $a0_15);
        label_1fb80:
            tx_isp_free_irq(arg2 + 0x80);
            tx_isp_module_deinit(arg2);
            return result;
        }
        
        for (int32_t i = 0; i < $s1_1[0xc]; i += 1)
        {
            if (*(*($s1_1 + 0x10) + (i << 1)) != 2)
                *(arg2 + 0xca) += 1;
            else
                *(arg2 + 0xc8) += 1;
        }
        
        uint32_t $a0_4 = *(arg2 + 0xc8);
        int32_t $s2_1 = 0;
        
        if ($a0_4)
        {
            int32_t $v0_7 = private_kmalloc($a0_4 * 0x24, 0xd0);
            $s2_1 = $v0_7;
            
            if (!$v0_7)
            {
                isp_printf(2, "count is %d\\n", *(arg2 + 8));
            label_1fad0:
                isp_subdev_release_clks(arg2);
                result = 0xfffffff4;
                goto label_1fb44;
            }
            
            memset($v0_7, 0, *(arg2 + 0xc8) * 0x24);
            int32_t $a0_7 = 0;
            int32_t $v1_11 = 0;
            
            while (true)
            {
                int32_t $a2_6 = $v1_11 << 1;
                
                if ($v1_11 >= $s1_1[0xc])
                    break;
                
                $v1_11 += 1;
                
                if (*(*($s1_1 + 0x10) + $a2_6) == 2)
                {
                    void** $v0_13 = $a0_7 * 0x24 + $s2_1;
                    $v0_13[1] = $a0_7;
                    *$v0_13 = arg2;
                    $a0_7 += 1;
                    *($v0_13 + 5) = *(*($s1_1 + 0x10) + $a2_6);
                    char $a1_12 = *(*($s1_1 + 0x10) + $a2_6 + 1);
                    *($v0_13 + 7) = 2;
                    *($v0_13 + 6) = $a1_12;
                    $v0_13[5] = 0;
                }
            }
            
            goto label_1f9cc;
        }
        
    label_1f9cc:
        uint32_t $a0_8 = *(arg2 + 0xca);
        
        if (!$a0_8)
        {
            *(arg2 + 0xcc) = $s2_1;
            *(arg2 + 0xd0) = 0;
        }
        else
        {
            int32_t $v0_14 = private_kmalloc($a0_8 * 0x24, 0xd0);
            
            if (!$s2_1)
            {
                isp_printf(2, "snapraw", *(arg2 + 8));
                private_kfree(0);
                goto label_1fad0;
            }
            
            memset($v0_14, 0, *(arg2 + 0xca) * 0x24);
            int32_t $a0_11 = 0;
            int32_t $v1_12 = 0;
            
            while (true)
            {
                int32_t $a2_10 = $v1_12 << 1;
                
                if ($v1_12 >= $s1_1[0xc])
                    break;
                
                $v1_12 += 1;
                
                if (*(*($s1_1 + 0x10) + $a2_10) == 1)
                {
                    void** $v0_20 = $a0_11 * 0x24 + $v0_14;
                    $v0_20[1] = $a0_11;
                    *$v0_20 = arg2;
                    $a0_11 += 1;
                    *($v0_20 + 5) = *(*($s1_1 + 0x10) + $a2_10);
                    char $a1_19 = *(*($s1_1 + 0x10) + $a2_10 + 1);
                    *($v0_20 + 7) = 2;
                    *($v0_20 + 6) = $a1_19;
                    $v0_20[5] = 0;
                }
            }
            
            *(arg2 + 0xcc) = $s2_1;
            *(arg2 + 0xd0) = $v0_14;
        }
    }
    
    return 0;
}

