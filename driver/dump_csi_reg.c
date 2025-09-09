#include "include/main.h"


  int32_t dump_csi_reg(void* arg1)

{
    int32_t entry_$a2;
    isp_printf(0, "%s[%d] do not support this interface\\n", entry_$a2);
    isp_printf(0, "%s:%d::linear mode\\n", **(arg1 + 0xb8));
    isp_printf(0, "%s:%d::wdr mode\\n", *(*(arg1 + 0xb8) + 4));
    isp_printf(0, "qbuffer null\\n", *(*(arg1 + 0xb8) + 8));
    isp_printf(0, "bank no free\\n", *(*(arg1 + 0xb8) + 0xc));
    isp_printf(0, "Failed to allocate vic device\\n", *(*(arg1 + 0xb8) + 0x10));
    isp_printf(0, "Failed to init isp module(%d.%d)\\n", *(*(arg1 + 0xb8) + 0x14));
    isp_printf(0, "&vsd->mlock", *(*(arg1 + 0xb8) + 0x18));
    isp_printf(0, "&vsd->snap_mlock", *(*(arg1 + 0xb8) + 0x1c));
    isp_printf(0, " %d, %d\\n", *(*(arg1 + 0xb8) + 0x20));
    isp_printf(0, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\\n", *(*(arg1 + 0xb8) + 0x24));
    isp_printf(0, "The parameter is invalid!\\n", *(*(arg1 + 0xb8) + 0x28));
    isp_printf(0, "vic_done_gpio%d", *(*(arg1 + 0xb8) + 0x2c));
    isp_printf(0, "register is 0x%x, value is 0x%x\\n", *(*(arg1 + 0xb8) + 0x30));
    /* tailcall */
    return isp_printf(0, "count is %d\\n", *(*(arg1 + 0xb8) + 0x34));
}

