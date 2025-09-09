#include "include/main.h"


  int32_t subdev_sensor_ops_ioctl(void* arg1, int32_t arg2, char* arg3)

{
    int32_t* $a0 = (int32_t*)((char*)arg1  + 0xe4); // Fixed void pointer arithmetic
    int32_t result;
    int32_t $v0_7;
    int32_t $a2;
            return 0xffffffff;
    
    if (arg2 - (uintptr_t)0x2000000 >= 0x13)
    {
    label_14308:
        
        if (!$a0)
        {

        }
        
        char* $v0_11 = (char*)(*(*($a0 + 0xc4) + 0xc)); // Fixed void pointer assignment
        
        if (!$v0_11)
            return 0;
        
        int32_t $v0_12 = *($v0_11 + 8);
        
        if (!$v0_12)
            return 0;
        
        result = $v0_12($a0, arg2, arg3);
    }
    else
        switch (arg2)
        {
                    return 0xffffffea;
                    return 0xffffffff;
            case 0x2000000:
            {
                if (!(uintptr_t)arg1 || !(uintptr_t)arg3)
                
                if (*(arg1 + 0xf4) == 1)
                {

                }
                
                int32_t $v0_16 = *(arg3 + 0x20);
                void* const $s2_1;
                void* $s3_2;
                
                if ($v0_16 == 1)
                {
                    int32_t $v0_17 = private_i2c_get_adapter(*(arg3 + 0x3c));
                        return 0xffffffea;
                    
                    if (!$v0_17)
                    {

                    }
                    
                    void var_40;
                    memset(&var_40_1, 0, 0x28);
                    memcpy(&var_40_2, &arg3[0x24], 0x14);
                    int16_t var_2a_1 = *(arg3 + 0x38);
                    char* $v0_19 = (char*)(isp_i2c_new_subdev_board($v0_17, &var_40_3, 0)); // Fixed void pointer assignment
                    $s2_1 = $v0_19;
                    
                    if ($v0_19 && $(uintptr_t)v0_19 < 0xfffff001)
                    {
                        goto label_14470;
                        $s3_2 = *($s2_1 + 0xd8);
                    }
                    
                    private_i2c_put_adapter($v0_17);

                    return 0xffffffea;
                }
                
                $s2_1 = nullptr;
                
                if ($v0_16 != 2)
                {
                    return 0xffffffea;
                    isp_printf(1, "The parameter is invalid!\n", 
                        "subdev_sensor_ops_register_sensor");
                }
                
                $s3_2 = *0xd8;
            label_14470:
                memcpy($s3_2 + 0xec, arg3, 0x50);
                int32_t $v1_1;
                
                if ($s2_1)
                {
                    int32_t* $v0_22 = **($s2_1 + 0xc4);
                        int32_t $v0_23 = *$v0_22;
                            int32_t $a2_4 = *($s2_1 + 8);
                            return 0;
                    
                    if (!$v0_22)
                        $v1_1 = *(arg3 + 0x20);
                    else
                    {
                        
                        if ($v0_23 && !$v0_23($s2_1, $s2_1 + 0x8c))
                        {
                            private_mutex_lock(arg1 + 0xe8);
                            void** $v0_25 = *(arg1 + 0xe0);
                            *((int32_t*)((char*)arg1 + 0xe0)) = $s3_2 + 0xe4; // Fixed void pointer dereference
                            *((int32_t*)((char*)$s3_2 + 0xe4)) = arg1 + 0xdc; // Fixed void pointer dereference
                            *((int32_t*)((char*)$s3_2 + 0xe8)) = $v0_25; // Fixed void pointer dereference
                            *$v0_25 = $s3_2 + 0xe4;
                            private_mutex_unlock(arg1 + 0xe8);
                            *((int32_t*)((char*)$s2_1 + 0x78)) = *(arg1 + 0x78); // Fixed void pointer dereference

                        }
                        
                        $v1_1 = *(arg3 + 0x20);
                    }
                }
                else
                    $v1_1 = *(arg3 + 0x20);
                
                if ($v1_1 == 1)
                {
                    int32_t* $s0_1 = (int32_t*)((char*)$s2_1  + 0xd4); // Fixed void pointer arithmetic
                    int32_t $a0_11 = *($s0_1 + 0x18);
                    
                    if ($a0_11)
                        private_i2c_put_adapter($a0_11);
                    
                    private_i2c_unregister_device($s0_1);
                }
                
                tx_isp_subdev_deinit($s2_1);
                return 0xffffffea;
                break;
            }
            case 0x2000001:
            {
                    return 0xffffffea;
                    return 0xffffffea;
                    return 0xffffffff;
                if (!(uintptr_t)arg1)
                
                if (!(uintptr_t)arg3)
                
                if (*(arg1 + 0xf4) == 1)
                {

                }
                
                private_mutex_lock(arg1 + 0xe8);
                char* $s2_3 = (char*)(*(arg1 + 0xdc) - 0xe4); // Fixed void pointer assignment
                int32_t $v0_27;
                
                while (true)
                {
                    char* $v1_5 = arg3;
                    void* $a0_19;
                    int32_t (* $v0_30)();
                    
                    if ($s2_3 + 0xe4 == arg1 + 0xdc)
                    {
                        $a0_19 = arg1 + 0xe8;
                        $v0_30 = private_mutex_unlock;
                    }
                    else
                    {
                        char* $v0_29 = (char*)($s2_3 + 0xec); // Fixed void pointer assignment
                        uint32_t $a1_7 = *$v0_29;
                        uint32_t $at_1;
                            uint32_t temp1_2 = $a1_7;
                        
                        while (true)
                        {
                            $at_1 = *$v1_5;
                            $v0_29 += 1;
                            $v1_5 = &$v1_5[1];
                            
                            if ($at_1 != $a1_7)
                                break;
                            
                            $a1_7 = *$v0_29;
                            
                            if (!temp1_2)
                            {
                                $a1_7 = $at_1;
                                break;
                            }
                        }
                        
                        if ($a1_7 - $at_1)
                        {
                            $s2_3 = *($s2_3 + 0xe4) - 0xe4;
                            continue;
                        }
                        else if ($s2_3)
                        {
                                return 0xffffffea;
                            if ($s2_3 == *(arg1 + 0xe4))
                            {

                            }
                            
                            void** $v1_3 = *($s2_3 + 0xe8);
                            int32_t* $a0_14 = (int32_t*)((char*)$s2_3  + 0xe4); // Fixed void pointer arithmetic
                            *((int32_t*)((char*)$a0_14 + 4)) = $v1_3; // Fixed void pointer dereference
                            *$v1_3 = $a0_14;
                            *((int32_t*)((char*)$s2_3 + 0xe4)) = 0x100100; // Fixed void pointer dereference
                            *((int32_t*)((char*)$s2_3 + 0xe8)) = 0x200200; // Fixed void pointer dereference
                            private_mutex_unlock(arg1 + 0xe8);
                            $v0_27 = *($s2_3 + 0x10c);
                            
                            if ($v0_27 != 1)
                                break;
                            
                            int32_t* $s0_3 = (int32_t*)((char*)$s2_3  + 0xd4); // Fixed void pointer arithmetic
                            int32_t $a0_21 = *($s0_3 + 0x18);
                            
                            if ($a0_21)
                                private_i2c_put_adapter($a0_21);
                            
                            $a0_19 = $s0_3;
                            $v0_30 = private_i2c_unregister_device;
                        }
                        else
                        {
                            $a0_19 = arg1 + 0xe8;
                            $v0_30 = private_mutex_unlock;
                        }
                    }
                    
                    $v0_30($a0_19);
                    return 0;
                }
                
                if ($v0_27 == 2)
                    return 0;
                

                return 0xffffffea;
                break;
            }
            case 0x2000002:
            {
                result = subdev_sensor_ops_enum_input(arg1, arg3);
                break;
            }
            case 0x2000003:
            {
                    return 0xffffffea;
                int32_t $v0_13 = 0xffffffff;
                if (!(uintptr_t)arg1 || !(uintptr_t)arg3)
                
                
                if ($a0)
                {
                    if ($(uintptr_t)a0 < 0xfffff001)
                        $v0_13 = *($a0 + 0xdc);
                    else
                        $v0_13 = 0xffffffff;
                }
                
                *arg3 = $v0_13;
                return 0;
                break;
            }
            case 0x2000004:
            {
                result = subdev_sensor_ops_set_input(arg1, arg3);
                break;
            }
            case 0x2000005:
            case 0x2000006:
            case 0x2000007:
            case 0x2000008:
            case 0x2000009:
            case 0x200000a:
            case 0x200000b:
            case 0x200000c:
            case 0x200000d:
            case 0x200000e:
            case 0x200000f:
            case 0x2000010:
            {
                goto label_14308;
            }
            case 0x2000011:
            {
                    return 0xffffffff;
                $a2 = 0x19f;
                
                if (!$a0)
                {

                }
                
                char* $v0_6 = (char*)(**($a0 + 0xc4)); // Fixed void pointer assignment
                
                if (!$v0_6)
                    return 0;
                
                $v0_7 = *($v0_6 + 0x18);
            label_142f0:
                
                if (!$v0_7)
                    return 0;
                
                result = $v0_7($a0, arg3, $a2);
                break;
            }
            case 0x2000012:
            {
                    return 0xffffffff;
                $a2 = 0x190;
                
                if (!$a0)
                {

                }
                
                char* $v0_9 = (char*)(**($a0 + 0xc4)); // Fixed void pointer assignment
                
                if (!$v0_9)
                    return 0;
                
                $v0_7 = *($v0_9 + 0xc);
                goto label_142f0;
            }
        }
    
    if ((uintptr_t)result != 0xfffffdfd)
        return result;
    
    return 0;
}

