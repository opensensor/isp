/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_SENSOR_GAIN_H
#define TX_ISP_T41_SENSOR_GAIN_H
#include "../include/tx_isp/tx_isp_math.h"

typedef unsigned int (*t41_sensor_allocator)(unsigned int, unsigned char, unsigned int *);
struct t41_sensor_gain {
	unsigned int code, log2_q16, linear_q10;
};

/* The sensor owns its register encoding and quantization. Never treat its
 * code as a linear gain: T41 modules use different, often nonlinear LUTs.
 * H20250310a sensor_alloc_analog_gain passes log2 Q16 to attr.alloc_again.
 */
static inline int t41_sensor_gain_allocate(unsigned int requested_q10,
		unsigned int max_log2_q16, t41_sensor_allocator allocate,
		struct t41_sensor_gain *out)
{
	struct t41_sensor_gain result;
	unsigned int request;
	if (!out || !allocate || requested_q10 < 1024 || max_log2_q16 > (16U << 16))
		return -1;
	request = tx_isp_log2_fixed_u32(requested_q10, 10, 16);
	/* A non-power-of-two ceiling loses precision on its Q10 round trip.
	 * Still allow the allocator to select its exact maximum LUT entry. */
	if (requested_q10 >= tx_isp_exp2_u32(max_log2_q16, 16, 10) ||
	    request > max_log2_q16)
		request = max_log2_q16;
	result.code = ~0U;
	result.log2_q16 = allocate(request, 16, &result.code);
	if (result.code > 65535 || result.log2_q16 > request)
		return -1;
	result.linear_q10 = tx_isp_exp2_u32(result.log2_q16, 16, 10);
	if (result.linear_q10 < 1024)
		return -1;
	*out = result;
	return 0;
}
#endif
