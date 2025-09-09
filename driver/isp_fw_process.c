#include "include/main.h"


  int32_t isp_fw_process()

{
    return 0;
    while (!private_kthread_should_stop())
        tisp_fw_process();
    
}

