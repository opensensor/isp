#ifndef TX_ISP_EXPOSURE_H
#define TX_ISP_EXPOSURE_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#endif

struct tx_isp_exposure_plan {
	u32 integration;
	u32 again;
};

/*
 * Scale a histogram target by a linear ISP gain.  This keeps AE from
 * compensating away an intentional pre-tone-map attenuation used to create
 * sensor headroom for a mains-synchronous shutter.
 */
int tx_isp_exposure_target_scale(u32 target, u32 gain, u32 gain_unity,
				 u32 *scaled_target);

/*
 * Build a rounded mains-synchronous integration sequence from the rational
 * line period step_numerator / step_denominator.  Values beyond the sensor
 * ABI's 16-bit line representation are omitted.  node_count is published
 * only on success.
 */
int tx_isp_flicker_nodes_build(u32 step_numerator,
			       u32 step_denominator,
			       u32 max_integration,
			       u16 *nodes, u32 node_capacity,
			       u32 *node_count);

/*
 * Split a desired linear exposure product into sensor integration and gain.
 *
 * Below the first anti-flicker node, an exposure may use arbitrary integration
 * unless flicker_floor is non-zero.  At and above the first node, the planner
 * selects the highest node not exceeding the unity-gain ideal, then assigns
 * the remainder to gain.  A non-zero flicker_floor is always enforced even
 * when it is not present in flicker_nodes.  All products and intermediate
 * divisions are checked in 64 bits; a result is published only after
 * validation succeeds.
 */
int tx_isp_exposure_plan_build(u64 desired_exposure,
			       u32 min_integration, u32 max_integration,
			       u32 min_again, u32 max_again,
			       const u16 *flicker_nodes,
			       u32 flicker_node_count,
			       u32 flicker_floor,
			       struct tx_isp_exposure_plan *plan);

#endif /* TX_ISP_EXPOSURE_H */
