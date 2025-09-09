#include "include/main.h"


  int32_t subdev_sensor_ops_release_all_sensor(void* arg1)

{
    if (!arg1)
        return 0xffffffea;
    
    if (*(arg1 + 0xf4) == 1)
    {
        int32_t entry_$a2;
        isp_printf(2, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\\n", entry_$a2);
        return 0xffffffff;
    }
    
    int32_t* $v0_2 = *(arg1 + 0xdc);
    
    while (true)
    {
        if ($v0_2 == arg1 + 0xdc)
            return 0;
        
        void** $v1_1 = $v0_2[1];
        void* $a0 = *$v0_2;
        *($a0 + 4) = $v1_1;
        *$v1_1 = $a0;
        $v0_2[1] = 0x200200;
        int32_t $v1_2 = $v0_2[0xa];
        *$v0_2 = 0x100100;
        
        if ($v1_2 != 1)
        {
            if ($v1_2 != 2)
                break;
            
            $v0_2 = *(arg1 + 0xdc);
        }
        else
        {
            void* $s5_1 = *($v0_2 - 0x10);
            int32_t $a0_1 = *($s5_1 + 0x18);
            
            if ($a0_1)
                private_i2c_put_adapter($a0_1);
            
            private_i2c_unregister_device($s5_1);
            $v0_2 = *(arg1 + 0xdc);
        }
    }
    
    isp_printf(2, "The parameter is invalid!\\n", "subdev_sensor_ops_release_all_sensor");
    return 0xffffffea;
}

