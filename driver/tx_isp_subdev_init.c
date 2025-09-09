#include "include/main.h"


  int32_t tx_isp_subdev_init(int32_t* arg1, void* arg2, int32_t arg3)

{
        return 0xffffffea;
    if (!(uintptr_t)arg1 || !(uintptr_t)arg2)
    {
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    *((int32_t*)((char*)arg2 + 0xc4)) = arg3; // Fixed void pointer dereference
    
    if (tx_isp_module_init(arg1, arg2))
    {
        return 0xfffffff4;
        isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
    }
    
    char* $s1_1 = arg1[0x16];
    
    if ($s1_1)
    {
        int32_t result_1;
        int32_t $a2_1;
        int32_t result = result_1;
            return result;
        result_1 = tx_isp_request_irq(arg1, arg2 + 0x80);
        
        if (result_1)
        {
            isp_printf(); // Fixed: macro with no parameters, removed 4 arguments;
            tx_isp_module_deinit(arg2);
        }
        
        int32_t* $v0_2;
        int32_t result_4;
        int32_t* $s3_2;
        
        while (true)
        {
                goto label_1f840;
            result_4 = result;
            
            if (result >= arg1[0x32])
            {
                $s3_2 = nullptr;
            }
            
            $v0_2 = private_platform_get_resource(arg1, 0x200, result_4);
            
            if ($v0_2)
            {
                char* $v1_1 = $v0_2[2];
                uint32_t $a1_1 = *$v1_1;
                uint32_t $at_1;
                    uint32_t temp1_1 = $a1_1;
                char const* const $a0_2 = "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n";
                
                while (true)
                {
                    $at_1 = *$a0_2;
                    $v1_1 = &$v1_1[1];
                    $a0_2 = &$a0_2[1];
                    
                    if ($at_1 != $a1_1)
                        break;
                    
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
            goto label_1fb80;
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            result = 0xfffffff0;
        }
        
        int32_t $a0_19 = *$v0_22;
        int32_t $v0_23;
        $v0_23 = private_ioremap($a0_19, $v0_22[1] + 1 - $a0_19);
        *((int32_t*)((char*)arg2 + 0xb8)) = $v0_23; // Fixed void pointer dereference
        
        if (!$v0_23)
        {
            goto label_1fb60;
            isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            result = 0xfffffffa;
        }
        
    label_1f840:
        *((int32_t*)((char*)arg2 + 0xb4)) = $s3_2; // Fixed void pointer dereference
        uint32_t $v0_5 = *$s1_1;
        
        if ($v0_5 != 1)
        {
                return 0;
            if ($v0_5 != 2)
            {
                isp_printf(); // Fixed: macro with no parameters, removed 3 arguments;
            }
            
            *((int32_t*)((char*)arg2 + 0xc0)) = $s1_1[4]; // Fixed void pointer dereference
            int32_t result_3 = isp_subdev_init_clks(arg2, *($s1_1 + 8));
            result = result_3;
            
            if (!result_3)
                return 0;
            
            goto label_1fb44;
        }
        
        *((int32_t*)((char*)arg2 + 0xc0)) = $s1_1[4]; // Fixed void pointer dereference
        int32_t result_2 = isp_subdev_init_clks(arg2, *($s1_1 + 8));
        result = result_2;
        
        if (result_2)
        {
            int32_t $a0_15 = *$s3_2;
            return result;
            isp_printf(); // Fixed: macro with no parameters, removed 4 arguments);
        label_1fb44:
            isp_printf(); // Fixed: macro with no parameters, removed 2 arguments!\n", 0x77d);
            private_iounmap(*(arg2 + 0xb8));
        label_1fb60:
            private_release_mem_region($a0_15, $s3_2[1] + 1 - $a0_15);
        label_1fb80:
            tx_isp_free_irq(arg2 + 0x80);
            tx_isp_module_deinit(arg2);
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
                goto label_1fb44;
            $s2_1 = $v0_7;
            
            if (!$v0_7)
            {
                isp_printf(); // Fixed: macro with no parameters, removed 3 arguments);
            label_1fad0:
                isp_subdev_release_clks(arg2);
                result = 0xfffffff4;
            }
            
            memset($v0_7, 0, *(arg2 + 0xc8) * 0x24);
            int32_t $a0_7 = 0;
            int32_t $v1_11 = 0;
            
            while (true)
            {
                int32_t $a2_6 = $v1_11 << 1;
                    char $a1_12 = *(*($s1_1 + 0x10) + $a2_6 + 1);
                
                if ($v1_11 >= $s1_1[0xc])
                    break;
                
                $v1_11 += 1;
                
                if (*(*($s1_1 + 0x10) + $a2_6) == 2)
                {
                    void** $v0_13 = $a0_7 * 0x24 + $s2_1;
                    $v0_13[1] = $a0_7;
                    *$v0_13 = arg2;
                    $a0_7 += 1;
                    *((int32_t*)((char*)$v0_13 + 5)) = *(*($s1_1 + 0x10) + $a2_6); // Fixed void pointer dereference
                    *((int32_t*)((char*)$v0_13 + 7)) = 2; // Fixed void pointer dereference
                    *((int32_t*)((char*)$v0_13 + 6)) = $a1_12; // Fixed void pointer dereference
                    $v0_13[5] = 0;
                }
            }
            
            goto label_1f9cc;
        }
        
    label_1f9cc:
        uint32_t $a0_8 = *(arg2 + 0xca);
        
        if (!$a0_8)
        {
            *((int32_t*)((char*)arg2 + 0xcc)) = $s2_1; // Fixed void pointer dereference
            *((int32_t*)((char*)arg2 + 0xd0)) = 0; // Fixed void pointer dereference
        }
        else
        {
            int32_t $v0_14 = private_kmalloc($a0_8 * 0x24, 0xd0);
                goto label_1fad0;
            
            if (!$s2_1)
            {
                isp_printf(); // Fixed: macro with no parameters, removed 3 arguments);
                private_kfree(0);
            }
            
            memset($v0_14, 0, *(arg2 + 0xca) * 0x24);
            int32_t $a0_11 = 0;
            int32_t $v1_12 = 0;
            
            while (true)
            {
                int32_t $a2_10 = $v1_12 << 1;
                    char $a1_19 = *(*($s1_1 + 0x10) + $a2_10 + 1);
                
                if ($v1_12 >= $s1_1[0xc])
                    break;
                
                $v1_12 += 1;
                
                if (*(*($s1_1 + 0x10) + $a2_10) == 1)
                {
                    void** $v0_20 = $a0_11 * 0x24 + $v0_14;
                    $v0_20[1] = $a0_11;
                    *$v0_20 = arg2;
                    $a0_11 += 1;
                    *((int32_t*)((char*)$v0_20 + 5)) = *(*($s1_1 + 0x10) + $a2_10); // Fixed void pointer dereference
                    *((int32_t*)((char*)$v0_20 + 7)) = 2; // Fixed void pointer dereference
                    *((int32_t*)((char*)$v0_20 + 6)) = $a1_19; // Fixed void pointer dereference
                    $v0_20[5] = 0;
                }
            }
            
            *((int32_t*)((char*)arg2 + 0xcc)) = $s2_1; // Fixed void pointer dereference
            *((int32_t*)((char*)arg2 + 0xd0)) = $v0_14; // Fixed void pointer dereference
        }
    }
    
    return 0;
}

