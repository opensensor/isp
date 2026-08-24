#ifndef TX_ISP_T30_IO_H
#define TX_ISP_T30_IO_H

#include <linux/io.h>
#include <linux/types.h>

void system_isp_set_base_address(void __iomem *address);
u32 system_isp_read_32(u32 address);
u16 system_isp_read_16(u32 address);
u8 system_isp_read_8(u32 address);
void __iomem *system_isp_write_32(u32 address, u32 value);
void __iomem *system_isp_write_16(u32 address, u16 value);
void __iomem *system_isp_write_8(u32 address, u8 value);

#endif /* TX_ISP_T30_IO_H */
