#include "include/main.h"


/* Must be check the return value */
__must_check int private_driver_get_interface(void)
{
    pfaces = get_driver_common_interfaces();
    if(pfaces && (pfaces->flags_0 != (unsigned int)printk || pfaces->flags_0 != pfaces->flags_1)){
        printk("flags = 0x%08x, printk = %p", pfaces->flags_0, printk);
        return -1;
    }else
        return 0;
}
EXPORT_SYMBOL(private_driver_get_interface);