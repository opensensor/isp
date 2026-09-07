/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_H
#define TX_ISP_T41_AWB_H

#include "../include/tx_isp/tx_isp_math.h"

/* H20250310a AWB-relative ABI. No sensor coefficients or register captures. */
#define T41_AWB_HW_PARAM_BYTES 0xe86U

static inline unsigned int t41_awb_u16(const unsigned char *p)
{
	return p[0] | ((unsigned int)p[1] << 8);
}

/* set_hardware_param 0x2f9c4: 0x18004..0x18024, then 0x1804c.
 * Validate before producing any output. The caller owns MMIO and trigger. */
static inline int t41_awb_geometry(const unsigned char *p, unsigned int bytes,
				   unsigned int out[10])
{
	unsigned int i, j;
	if (!p || !out || bytes < T41_AWB_HW_PARAM_BYTES ||
	    t41_awb_u16(p + 0xc6c) > 4095 ||
	    t41_awb_u16(p + 0xc70) > 4095 ||
	    !t41_awb_u16(p + 0xc6e) || t41_awb_u16(p + 0xc6e) > 15 ||
	    !t41_awb_u16(p + 0xc72) || t41_awb_u16(p + 0xc72) > 15 ||
	    t41_awb_u16(p + 0xcc8) > 255 ||
	    t41_awb_u16(p + 0xcca) > 1 || t41_awb_u16(p + 0xccc) > 3)
		return -1;
	for (i = 0; i < 30; ++i)
		if (t41_awb_u16(p + 0xc74 + i * 2) > 255)
			return -1;
	out[0] = t41_awb_u16(p + 0xc6c) |
		(t41_awb_u16(p + 0xc6e) << 12) |
		(t41_awb_u16(p + 0xc70) << 16) |
		(t41_awb_u16(p + 0xc72) << 28);
	for (i = 0; i < 8; ++i) {
		const unsigned char *sizes = p + 0xc74 + (i / 4) * 30 + (i % 4) * 8;
		out[i + 1] = 0;
		for (j = 0; j < (i % 4 == 3 ? 3U : 4U); ++j)
			out[i + 1] |= t41_awb_u16(sizes + j * 2) << (j * 8);
	}
	out[9] = 0x01010103U | (t41_awb_u16(p + 0xcc8) << 12) |
		(t41_awb_u16(p + 0xcca) << 28) | (t41_awb_u16(p + 0xccc) << 25);
	return 0;
}

/* set_regional_threshold 0x2f744 / set_lum_th_freq 0x2f8d0.
 * Eleven u16 gain knots per table, log2 gain in Q16, nearest-step rounding
 * from the shared interpolation primitive. Keep byte offsets byte-based. */
static inline int t41_awb_thresholds(const unsigned char *p, unsigned int bytes,
		unsigned int gain_q16, unsigned short values[11], unsigned int out[5])
{
	unsigned int i, index = gain_q16 >> 16, fraction = gain_q16 & 65535;
	if (!p || !values || !out || bytes < T41_AWB_HW_PARAM_BYTES)
		return -1;
	for (i = 0; i < 11; ++i) {
		const unsigned char *table = p + 0xd94 + i * 22;
		values[i] = index >= 10 ? t41_awb_u16(table + 20) :
			tx_isp_lerp_pair_u32(t41_awb_u16(table + index * 2),
				t41_awb_u16(table + index * 2 + 2), fraction);
	}
	for (i = 0; i < 4; ++i)
		out[i] = values[i * 2] | ((unsigned int)values[i * 2 + 1] << 16);
	out[4] = values[8] | ((unsigned int)values[9] << 8) |
		((unsigned int)values[10] << 16) | (t41_awb_u16(p + 0xcc6) << 24);
	return 0;
}
#endif
