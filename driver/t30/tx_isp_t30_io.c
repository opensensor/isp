/* T30 Apical MMIO access, sourced from the Ingenic 3.10.14 system_io.c. */

#include <linux/io.h>
#include <linux/types.h>

#include "tx_isp_t30_io.h"

static void __iomem *apical_io_base;

static void __iomem *tx_isp_t30_io_address(u32 address)
{
	return (void __iomem *)((u8 __iomem *)apical_io_base + address);
}

void system_isp_set_base_address(void __iomem *address)
{
	apical_io_base = address;
}

u32 system_isp_read_32(u32 address)
{
	return __raw_readl(tx_isp_t30_io_address(address));
}

u16 system_isp_read_16(u32 address)
{
	return __raw_readw(tx_isp_t30_io_address(address));
}

u8 system_isp_read_8(u32 address)
{
	return __raw_readb(tx_isp_t30_io_address(address));
}

void __iomem *system_isp_write_32(u32 address, u32 value)
{
	void __iomem *target = tx_isp_t30_io_address(address);

	__raw_writel(value, target);
	/* Recovered APICAL wrappers consume the OEM function's v0 residue. */
	return target;
}

void __iomem *system_isp_write_16(u32 address, u16 value)
{
	void __iomem *target = tx_isp_t30_io_address(address);

	__raw_writew(value, target);
	return target;
}

void __iomem *system_isp_write_8(u32 address, u8 value)
{
	void __iomem *target = tx_isp_t30_io_address(address);

	__raw_writeb(value, target);
	return target;
}
