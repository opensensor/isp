#include "include/main.h"


  int32_t tisp_code_destroy_tuning_node()

{
    uint32_t major_1 = major;
    cdev_del(&tisp_cdev);
    device_destroy(cls, major << 0x14);
    class_destroy(cls);
    int32_t result = unregister_chrdev_region(major_1 << 0x14, 1);
    major = 0;
    return result;
}

