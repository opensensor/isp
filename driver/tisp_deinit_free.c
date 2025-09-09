#include "include/main.h"


  void* const tisp_deinit_free()

{
    int32_t $a0 = data_ca490;
    
    if ($a0)
    {
        private_kfree($a0);
        data_ca490 = 0;
    }
    
    int32_t $a0_1 = data_ca48c_3;
    void* void* result = (void*)&data_20000_3; // Fixed function pointer assignment
    
    if ($a0_1)
    {
        result = private_kfree($a0_1);
        data_ca48c = 0;
    }
    
    return result;
}

