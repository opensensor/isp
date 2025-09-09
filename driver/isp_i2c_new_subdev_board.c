#include "include/main.h"


  int32_t isp_i2c_new_subdev_board(int32_t arg1, void* arg2, int32_t arg3)

{
    private_request_module(1, arg2, arg3);
    
    if (*(arg2 + 0x16))
    {
        void* $v0_1 = private_i2c_new_device(arg1, arg2);
        
        if ($v0_1)
        {
            void* $v0_2 = *($v0_1 + 0x1c);
            
            if ($v0_2 && private_try_module_get(*($v0_2 + 0x2c)))
            {
                int32_t result = private_i2c_get_clientdata($v0_1);
                private_module_put(*(*($v0_1 + 0x1c) + 0x2c));
                
                if (result)
                    return result;
            }
            
            private_i2c_unregister_device($v0_1);
        }
    }
    
    return 0;
}

