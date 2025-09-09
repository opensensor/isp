#include "include/main.h"


  int32_t* isp_core_tuning_init(int32_t arg1)

{
    int32_t* result;
    int32_t $a2;
        return nullptr;
    result = private_kmalloc(0x40d0, 0xd0);
    
    if (!result)
    {

    }
    
    memset(result, 0, 0x40d0);
    *result = arg1;
    private_spin_lock_init(&result[0x102e]);
    private_raw_mutex_init(&result[0x102e], 
        "width is %d, height is %d, imagesize is %d\\n, save num is %d, buf size is %d", 0);
    result[0x1031] = 1;
    result[0x1032] = &isp_core_tunning_fops;
    result[0x1033] = isp_core_tuning_event;
    return result;
}

