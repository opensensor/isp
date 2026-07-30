#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/math64.h>
#else
#include <errno.h>
#endif

#include "../include/tx_isp/tx_isp_exposure.h"

static u64 tx_isp_exposure_div_round(u64 numerator, u32 denominator)
{
#ifdef __KERNEL__
	return div64_u64(numerator + denominator / 2, denominator);
#else
	return (numerator + denominator / 2) / denominator;
#endif
}

int tx_isp_exposure_target_scale(u32 target, u32 gain, u32 gain_unity,
				 u32 *scaled_target)
{
	u64 scaled;

	if (!target || !gain || !gain_unity || !scaled_target)
		return -EINVAL;
	scaled = tx_isp_exposure_div_round((u64)target * gain, gain_unity);
	if (!scaled || scaled > (u32)~0U)
		return -ERANGE;
	*scaled_target = (u32)scaled;
	return 0;
}

int tx_isp_flicker_nodes_build(u32 step_numerator,
			       u32 step_denominator,
			       u32 max_integration,
			       u16 *nodes, u32 node_capacity,
			       u32 *node_count)
{
	u64 multiplier;
	u64 next;
	u32 count = 0;

	if (!step_denominator || step_numerator < step_denominator ||
	    !nodes || !node_capacity || !node_count)
		return -EINVAL;

	for (multiplier = 1; ; ++multiplier) {
		next = tx_isp_exposure_div_round(
			(u64)step_numerator * multiplier,
			step_denominator);
		if (next > max_integration || next > 0xffffU)
			break;
		if (count == node_capacity)
			return -ENOSPC;
		nodes[count++] = (u16)next;
	}

	*node_count = count;
	return 0;
}

int tx_isp_exposure_plan_build(u64 desired_exposure,
			       u32 min_integration, u32 max_integration,
			       u32 min_again, u32 max_again,
			       const u16 *flicker_nodes,
			       u32 flicker_node_count,
			       u32 flicker_floor,
			       struct tx_isp_exposure_plan *plan)
{
	struct tx_isp_exposure_plan result;
	u64 min_exposure;
	u64 max_exposure;
	u64 ideal_integration;
	u64 gain;
	u32 integration;
	u32 index;

	if (!plan || !min_integration || min_integration > max_integration ||
	    !min_again || min_again > max_again ||
	    (flicker_node_count && !flicker_nodes) ||
	    flicker_floor > max_integration)
		return -EINVAL;

	for (index = 0; index < flicker_node_count; ++index) {
		if (!flicker_nodes[index] ||
		    (index && flicker_nodes[index] <= flicker_nodes[index - 1]))
			return -EINVAL;
	}

	min_exposure = (u64)min_integration * min_again;
	max_exposure = (u64)max_integration * max_again;
	if (desired_exposure < min_exposure)
		desired_exposure = min_exposure;
	if (desired_exposure > max_exposure)
		desired_exposure = max_exposure;

	ideal_integration =
		tx_isp_exposure_div_round(desired_exposure, min_again);
	if (flicker_floor && ideal_integration < flicker_floor) {
		integration = flicker_floor;
	} else if (!flicker_node_count ||
		   ideal_integration < flicker_nodes[0]) {
		if (ideal_integration < min_integration)
			integration = min_integration;
		else if (ideal_integration > max_integration)
			integration = max_integration;
		else
			integration = (u32)ideal_integration;
	} else {
		integration = flicker_nodes[0];
		for (index = 1; index < flicker_node_count; ++index) {
			if (flicker_nodes[index] > ideal_integration ||
			    flicker_nodes[index] > max_integration)
				break;
			integration = flicker_nodes[index];
		}
	}

	if (flicker_floor && integration < flicker_floor)
		integration = flicker_floor;
	if (integration < min_integration)
		integration = min_integration;
	if (integration > max_integration)
		integration = max_integration;
	gain = tx_isp_exposure_div_round(desired_exposure, integration);
	if (gain < min_again)
		gain = min_again;
	if (gain > max_again)
		gain = max_again;

	result.integration = integration;
	result.again = (u32)gain;
	*plan = result;
	return 0;
}
