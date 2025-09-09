#include "include/main.h"


  int32_t tisp_code_create_tuning_node()

{
    uint32_t major_1 = major;
    uint32_t var_18_79;
    
    if (!major_1)
    {
        alloc_chrdev_region(&var_18_80, 0, 1, "%s[%d] do not support this interface\\n");
        major = var_18_81 >> 0x14;
    }
    else
    {
        uint32_t $a0 = major_1 << 0x14;
        var_18_82 = $a0;
        register_chrdev_region($a0, 1, "%s[%d] do not support this interface\\n");
    }
    
    cdev_init(&tisp_cdev, &tisp_fops);
    cdev_add(&tisp_cdev, var_18_83, 1);
    uint32_t $v0_3 = __class_create(&__this_module, "%s[%d] do not support this interface\\n", 0);
    uint32_t major_2 = major;
    cls = $v0_3;
    device_create($v0_3, 0, major_2 << 0x14, 0, "%s[%d] do not support this interface\\n");
    *tispPollValue = 0;
    __init_waitqueue_head(&dumpQueue, "%s:%d::linear mode\\n", 0);
    return 0;
}

