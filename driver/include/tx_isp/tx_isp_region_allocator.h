#ifndef TX_ISP_REGION_ALLOCATOR_H
#define TX_ISP_REGION_ALLOCATOR_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#endif

/* Ingenic T23/T30/T40/T41 use twenty descriptors and 4 KiB blocks. */
#define TX_ISP_REGION_MAX_NODES 20U
#define TX_ISP_REGION_ALIGNMENT 4096U

struct tx_isp_region_node {
	u8 used;
	u8 busy;
	u16 reserved;
	struct tx_isp_region_node *prev;
	struct tx_isp_region_node *next;
	u32 addr;
	u32 size;
};

struct tx_isp_region_allocator {
	u32 base;
	u32 size;
	u32 used_size;
	struct tx_isp_region_node nodes[TX_ISP_REGION_MAX_NODES];
	struct tx_isp_region_node *first;
};

int tx_isp_region_allocator_init(struct tx_isp_region_allocator *allocator,
				 u32 base, u32 size);
u32 tx_isp_region_alloc(struct tx_isp_region_allocator *allocator, u32 size);
int tx_isp_region_free(struct tx_isp_region_allocator *allocator, u32 addr);

#endif /* TX_ISP_REGION_ALLOCATOR_H */
