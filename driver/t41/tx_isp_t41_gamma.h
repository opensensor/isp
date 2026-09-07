/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_GAMMA_H
#define TX_ISP_T41_GAMMA_H
#include "tx_isp_t41_tmo.h"

#define T41_GAMMA_PARAM_BYTES 0x238U
#define T41_GAMMA_POINTS 129U

/* H20250310a tisp_gamma_interp_by_ev: ten EV knots and byte strengths.
 * The OEM numerator deliberately wraps at 32 bits. */
static inline int t41_gamma_strength(const unsigned char *p, unsigned int bytes,
		unsigned int ev, unsigned int high_ev, unsigned int *out)
{
	unsigned int i, value;
	if (!p || !out || bytes < T41_GAMMA_PARAM_BYTES)
		return -1;
	for (i = 1; i < 10; ++i)
		if (t41_tmo_le32(p + 0x104 + i * 4) <=
		    t41_tmo_le32(p + 0x100 + i * 4))
			return -1;
	value = p[0x237];
	for (i = 0; !high_ev && i < 10; ++i) {
		unsigned int high = t41_tmo_le32(p + 0x104 + i * 4);
		if (ev < high) {
			value = p[0x22e];
			if (i) {
				unsigned int low = t41_tmo_le32(p + 0x100 + i * 4);
				value = (p[0x22d + i] * (high - ev) +
					p[0x22e + i] * (ev - low) +
					((high - low) >> 1)) / (high - low);
			}
			break;
		}
	}
	*out = value;
	return 0;
}

/* Strength 255 selects the calibrated curve; zero selects the linear ramp.
 * The endpoint is 4095, not the 4096 implied by extending that ramp. */
static inline int t41_gamma_curve(const unsigned char *p, unsigned int bytes,
		unsigned int strength, unsigned short out[T41_GAMMA_POINTS])
{
	unsigned int i, scale = strength + (strength >> 7), accumulator = 0;
	if (!p || !out || bytes < T41_GAMMA_PARAM_BYTES || strength > 255)
		return -1;
	for (i = 0; i < 128; ++i)
		if (t41_tmo_le16(p + 0x12c + i * 2) > 4095)
			return -1;
	for (i = 0; i < 128; ++i) {
		out[i] = (t41_tmo_le16(p + 0x12c + i * 2) * scale +
			accumulator + 128U) >> 8;
		accumulator += 8192U - (scale << 5);
	}
	out[128] = 4095;
	return 0;
}
#endif
