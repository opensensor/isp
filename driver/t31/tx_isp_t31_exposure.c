/*
 * T31 exposure-policy adapter.
 *
 * The common library owns checked anti-flicker node generation.  This file
 * preserves the older T31 tuning ABI's u32 entries, repeated tail, 120-node
 * bound, and last-index convention.
 */
#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#else
#include <errno.h>
#include <string.h>
#endif

#include "tx_isp_t31_exposure.h"
#include "../include/tx_isp/tx_isp_exposure.h"

#ifdef __KERNEL__
#include "../common/tx_isp_exposure.c"
#endif

int tx_isp_t31_flicker_lut_build(u32 step_lines, u32 max_integration,
				 u32 *lut, u32 lut_capacity,
				 u32 *last_index)
{
	u16 nodes[TX_ISP_T31_FLICKER_LUT_ENTRIES];
	u32 expanded[TX_ISP_T31_FLICKER_LUT_ENTRIES];
	u64 coverage;
	u32 limited_max;
	u32 node_count;
	u32 index;
	int ret;

	if (!step_lines || !max_integration || !lut || !lut_capacity ||
	    lut_capacity > TX_ISP_T31_FLICKER_LUT_ENTRIES || !last_index)
		return -EINVAL;

	/*
	 * The T31 implementation clamps a long sequence to its 120-word ABI.
	 * Limit the common generator to the representable coverage so a full
	 * table is success, not ENOSPC.
	 */
	coverage = (u64)step_lines * lut_capacity;
	limited_max = max_integration;
	if (coverage < limited_max)
		limited_max = (u32)coverage;

	ret = tx_isp_flicker_nodes_build(step_lines, 1, limited_max,
					 nodes, lut_capacity, &node_count);
	if (ret)
		return ret;

	/*
	 * Stock publishes one node even when the first half-cycle extends past
	 * the nominal frame height.  Preserve that corner case.
	 */
	if (!node_count) {
		if (step_lines > 0xffffU)
			return -ERANGE;
		nodes[0] = (u16)step_lines;
		node_count = 1;
	}

	for (index = 0; index < node_count; ++index)
		expanded[index] = nodes[index];
	for (; index < lut_capacity; ++index)
		expanded[index] = nodes[node_count - 1U];

	memcpy(lut, expanded, lut_capacity * sizeof(*lut));
	*last_index = node_count - 1U;
	return 0;
}
