#include "include/main.h"


  void* const tisp_deinit_free()

{
    int32_t $a0 = data_ca490_15;
    
    if ($a0)
    {
        private_kfree($a0);
        data_ca490_16 = 0;
    }
    
    int32_t $a0_1 = data_ca48c_6;
    void* const result = &data_20000_17;
    
    if ($a0_1)
    {
        result = private_kfree($a0_1);
        data_ca48c_7 = 0;
    }
    
    return result;
}

