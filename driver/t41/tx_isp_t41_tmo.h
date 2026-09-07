/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_TMO_H
#define TX_ISP_T41_TMO_H

#define T41_TMO_PARAM_BYTES 6044U
#define T41_TMO_CURVE_ENTRIES 201U

#ifdef __KERNEL__
#include <linux/math64.h>
#define t41_tmo_div(n, d) div64_u64((n), (d))
#else
#define t41_tmo_div(n, d) ((n) / (d))
#endif
static inline unsigned int t41_tmo_le16(const unsigned char *p)
{
	return p[0] | ((unsigned int)p[1] << 8);
}

static inline unsigned int t41_tmo_le32(const unsigned char *p)
{
	return t41_tmo_le16(p) | (t41_tmo_le16(p + 2) << 16);
}

/* Sensor-owned global curves, H20250310a tisp_tmo_ev_interp. Check the
 * complete table before touching the destination. Offsets are TMO-relative. */
static inline int t41_tmo_curve(const unsigned char *p, unsigned int bytes,
			      unsigned int ev, unsigned short *out)
{
	static const unsigned short offsets[10] = {
		4950, 5416, 1416, 1818, 2220, 2622, 3024, 3426, 3828, 4230,
	};
	unsigned int i, j, index = 0, weight = 256;
	if (!p || !out || bytes != T41_TMO_PARAM_BYTES ||
	    t41_tmo_le16(p + 0x55c) != 0)
		return -1;
	for (i = 0; i < 10; ++i) {
		if (i && t41_tmo_le32(p + 1312 + i * 4) <=
			 t41_tmo_le32(p + 1308 + i * 4))
			return -1;
		for (j = 0; j < T41_TMO_CURVE_ENTRIES; ++j)
			if (t41_tmo_le16(p + offsets[i] + j * 2) > 8191)
				return -1;
	}
	for (i = 0; i < 9; ++i) {
		unsigned int low = t41_tmo_le32(p + 1312 + i * 4);
		unsigned int high = t41_tmo_le32(p + 1316 + i * 4);
		if (ev < high) {
			index = i;
			if (ev > low)
				weight = (unsigned int)t41_tmo_div(
					(unsigned long long)(high - ev) << 8, high - low);
			break;
		}
		index = i + 1;
	}
	/* Above the final EV knot use the final curve, not its left neighbour. */
	for (i = 0; i < T41_TMO_CURVE_ENTRIES; ++i) {
		unsigned int a = t41_tmo_le16(p + offsets[index] + i * 2);
		unsigned int b = index == 9 ? a :
			t41_tmo_le16(p + offsets[index + 1] + i * 2);
		out[i] = (a * weight + b * (256 - weight)) >> 8;
	}
	return 0;
}

#endif
