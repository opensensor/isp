/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_AWB_MESH_H
#define TX_ISP_AWB_MESH_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
typedef uint32_t u32;
typedef uint64_t u64;
#endif

#define TX_ISP_AWB_MESH_SIZE 15U

/* OEM knot order: full-low, outer-low, full-high, outer-high. */
struct tx_isp_awb_ct_prior {
	u32 knots[4];
	u32 floor_q8;
};

struct tx_isp_awb_ct_config {
	u32 ev_low, ev_high;
	u32 day[4], transition[4], night[4];
	u32 day_enabled, night_enabled;
	u32 day_floor_q8, night_floor_q8;
};

int tx_isp_awb_ct_prior_build(const struct tx_isp_awb_ct_config *config,
			    u32 ev, struct tx_isp_awb_ct_prior *prior);
u32 tx_isp_awb_ct_weight(const struct tx_isp_awb_ct_prior *prior, u32 kelvin);

/* Sensor calibration, not a scene-specific gain preset. Axes are calibrated
 * R/G and B/G in Q8; mesh weights are Q8. Hardware/statistics layout stays in
 * the SoC adapter. Validate once before feeding any samples. */
struct tx_isp_awb_mesh {
	const u32 *red_axis;
	const u32 *blue_axis;
	const u32 *weights;
	u32 red_calibration_q10;
	u32 blue_calibration_q10;
	u32 red_bias_q10;
	u32 blue_bias_q10;
	/* Optional sensor-owned reciprocal-temperature mesh (mired), on the
	 * same ratio axes. A NULL pointer leaves existing estimators unchanged. */
	const u32 *ct_mired;
	struct tx_isp_awb_ct_prior ct_prior;
};

struct tx_isp_awb_accumulator {
	u64 red, green, blue;
	u32 samples, weight;
};

int tx_isp_awb_mesh_validate(const struct tx_isp_awb_mesh *mesh);
/* Returns 1 for accepted, 0 for rejected. Sums must fit 26 bits; spatial
 * weight is a byte. Dark/clipped sample selection belongs to the adapter. */
int tx_isp_awb_mesh_add(const struct tx_isp_awb_mesh *mesh,
		      struct tx_isp_awb_accumulator *sum,
		      u32 red, u32 green, u32 blue, u32 spatial_weight);
/* No output on insufficient evidence or out-of-range gains. */
int tx_isp_awb_mesh_result(const struct tx_isp_awb_mesh *mesh,
			 const struct tx_isp_awb_accumulator *sum,
			 u32 minimum_samples, u32 *red_q10, u32 *blue_q10);
#endif
