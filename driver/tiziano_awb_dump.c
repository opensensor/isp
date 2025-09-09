#include "include/main.h"


  int32_t tiziano_awb_dump()

{
    if (DumpNum.32174)
        return &data_b0000_4;
    
    int32_t $ra;
    int32_t var_4_1 = $ra;
    DumpNum.32174 = 1;
    int32_t entry_$a2;
    isp_printf(2, &$LC0, entry_$a2);
    system_reg_read(0xb000);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb000);
    system_reg_read(0xb004);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb004);
    system_reg_read(0xb008);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb008);
    system_reg_read(0xb00c);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb00c);
    system_reg_read(0xb010);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb010);
    system_reg_read(0xb014);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb014);
    system_reg_read(0xb018);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb018);
    system_reg_read(0xb01c);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb01c);
    system_reg_read(0xb020);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb020);
    system_reg_read(0xb024);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb024);
    system_reg_read(0xb028);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb028);
    system_reg_read(0xb02c);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb02c);
    system_reg_read(0xb030);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb030);
    system_reg_read(0xb034);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb034);
    system_reg_read(0xb038);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb038);
    system_reg_read(0xb03c);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb03c);
    system_reg_read(0xb040);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb040);
    system_reg_read(0xb044);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb044);
    system_reg_read(0xb048);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb048);
    system_reg_read(0xb04c);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb04c);
    system_reg_read(0xb050);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb050);
    system_reg_read(0xb054);
    isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb054);
    system_reg_read(0xb058);
    /* tailcall */
    return isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0xb058);
}

