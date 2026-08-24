/* T30 locking and reserved-memory adapter for the shared region allocator. */

#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/types.h>

#include "../include/tx_isp/tx_isp_region_allocator.h"
#include "tx_isp_t30_videobuf.h"

extern void private_get_isp_priv_mem(unsigned int *physical_address,
				     unsigned int *size);

static struct tx_isp_region_allocator isp_region;
static struct mutex isp_region_lock;

#include "../common/tx_isp_region_allocator.c"

void isp_mem_init(void)
{
	u32 base = 0;
	u32 size = 0;

	/* OEM ispmem is 0x1ac bytes in the matching MIPS32 module. */
	BUILD_BUG_ON(sizeof(isp_region) + sizeof(isp_region_lock) != 0x1ac);
	private_get_isp_priv_mem(&base, &size);
	mutex_init(&isp_region_lock);
	tx_isp_region_allocator_init(&isp_region, base, size);
}

unsigned int isp_malloc_buffer(unsigned int size)
{
	u32 address;

	mutex_lock(&isp_region_lock);
	address = tx_isp_region_alloc(&isp_region, size);
	mutex_unlock(&isp_region_lock);
	return address;
}

void isp_free_buffer(unsigned int address)
{
	mutex_lock(&isp_region_lock);
	tx_isp_region_free(&isp_region, address);
	mutex_unlock(&isp_region_lock);
}
