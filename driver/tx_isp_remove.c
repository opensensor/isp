#include "include/main.h"


  int32_t tx_isp_remove(int32_t arg1)

{
    void* $v0 = private_platform_get_drvdata();
    private_misc_deregister($v0 + 0xc);
    private_proc_remove(*($v0 + 0x11c));
    tx_isp_unregister_platforms($v0 + 0x84);
    private_platform_set_drvdata(arg1, 0);
    private_kfree($v0);
    return 0;
}

