#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/string.h>
#else
#include <errno.h>
#include <string.h>
#endif

#include "../include/tx_isp/tx_isp_region_allocator.h"

static struct tx_isp_region_node *
tx_isp_region_new_node(struct tx_isp_region_allocator *allocator)
{
	u32 index;

	for (index = 0; index < TX_ISP_REGION_MAX_NODES; index++) {
		struct tx_isp_region_node *node = &allocator->nodes[index];

		if (node->busy)
			continue;
		memset(node, 0, sizeof(*node));
		node->busy = 1;
		return node;
	}
	return NULL;
}

int tx_isp_region_allocator_init(struct tx_isp_region_allocator *allocator,
				 u32 base, u32 size)
{
	struct tx_isp_region_node *first;

	if (!allocator)
		return -EINVAL;
	memset(allocator, 0, sizeof(*allocator));
	if (!base || !size)
		return -EINVAL;
	if (base + size < base)
		return -ERANGE;

	allocator->base = base;
	allocator->size = size;
	first = tx_isp_region_new_node(allocator);
	if (!first)
		return -ENOSPC;
	first->addr = base;
	first->size = size;
	allocator->first = first;
	return 0;
}

u32 tx_isp_region_alloc(struct tx_isp_region_allocator *allocator, u32 size)
{
	struct tx_isp_region_node *node;
	u32 aligned_size;

	if (!allocator || !allocator->first || !size)
		return 0;
	if (size > ~(u32)0 - (TX_ISP_REGION_ALIGNMENT - 1U))
		return 0;
	aligned_size = (size + TX_ISP_REGION_ALIGNMENT - 1U) &
		       ~(TX_ISP_REGION_ALIGNMENT - 1U);

	for (node = allocator->first; node; node = node->next) {
		struct tx_isp_region_node *remainder;

		if (node->used || node->size < aligned_size)
			continue;
		if (node->size != aligned_size) {
			remainder = tx_isp_region_new_node(allocator);
			if (!remainder)
				return 0;
			remainder->addr = node->addr + aligned_size;
			remainder->size = node->size - aligned_size;
			remainder->prev = node;
			remainder->next = node->next;
			if (node->next)
				node->next->prev = remainder;
			node->next = remainder;
			node->size = aligned_size;
		}
		node->used = 1;
		allocator->used_size += node->size;
		return node->addr;
	}
	return 0;
}

int tx_isp_region_free(struct tx_isp_region_allocator *allocator, u32 addr)
{
	struct tx_isp_region_node *node;

	if (!allocator || !allocator->first || !addr)
		return -EINVAL;
	for (node = allocator->first; node; node = node->next) {
		if (node->used && node->addr == addr)
			break;
	}
	if (!node)
		return -ENOENT;

	node->used = 0;
	allocator->used_size -= node->size;
	node = allocator->first;
	while (node && node->next) {
		struct tx_isp_region_node *next = node->next;

		if (node->used || !next || next->used ||
		    node->addr + node->size != next->addr) {
			node = next;
			continue;
		}
		node->size += next->size;
		node->next = next->next;
		if (next->next)
			next->next->prev = node;
		memset(next, 0, sizeof(*next));
		/* Do not advance: another free successor may now be adjacent. */
	}
	return 0;
}
