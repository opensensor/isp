/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_CCM_H
#define TX_ISP_T41_CCM_H

#include "tx_isp_t41_tmo.h" /* unaligned LE readers */
#define T41_CCM_PARAM_BYTES 148U

static inline int t41_ccm_s16(const unsigned char *p)
{
	unsigned int v = t41_tmo_le16(p);
	return v < 32768 ? (int)v : (int)v - 65536;
}

/* OEM round_int64 rounds half toward positive infinity, including negatives. */
static inline int t41_ccm_round(long long v, unsigned int shift)
{
	return (int)((v + (1LL << (shift - 1))) >> shift);
}

static inline int t41_ccm_clip(int v)
{
	return v < -8192 ? -8192 : v > 8191 ? 8191 : v;
}

/* Pure CT/EV selection. Calibration has alternating constant / transition
 * intervals, not six independent matrices. No sensor or measured-frame input.
 */
static inline int t41_ccm_select(const unsigned char *p, unsigned int bytes,
		unsigned int ct, unsigned int ev, short *matrix, unsigned int *sat)
{
	unsigned int i, zone = 0, saturation, left, right, n = 0, d = 1;
	short result[9];
	if (!p || bytes != T41_CCM_PARAM_BYTES || !matrix || !sat || ct > 100000)
		return -1;
	for (i = 0; i < 6; ++i)
		if (!t41_tmo_le16(p + i * 2) ||
		    (i && t41_tmo_le16(p + i * 2) <= t41_tmo_le16(p + i * 2 - 2)))
			return -1;
	for (i = 0; i < 9; ++i)
		if ((i && t41_tmo_le32(p + 20 + i * 4) <= t41_tmo_le32(p + 16 + i * 4)) ||
		    t41_tmo_le16(p + 56 + i * 2) > 1024)
			return -1;
	for (i = 0; i < 36; ++i) {
		int v = t41_ccm_s16(p + 74 + i * 2);
		if (v < -8192 || v > 8191)
			return -1;
	}
	while (zone < 6 && ct > t41_tmo_le16(p + zone * 2))
		++zone;
	left = right = zone / 2;
	if (zone & 1) {
		right = left + 1;
		d = t41_tmo_le16(p + zone * 2) - t41_tmo_le16(p + zone * 2 - 2);
		n = ct - t41_tmo_le16(p + zone * 2 - 2);
	}
	for (i = 0; i < 9; ++i) {
		int a = t41_ccm_s16(p + 74 + left * 18 + i * 2);
		int b = t41_ccm_s16(p + 74 + right * 18 + i * 2);
		int value = a * (int)(d - n) + b * (int)n;
		/* CT interpolation uses signed division and half away from zero,
		 * unlike the separate fixed-shift saturation transform. */
		value = (value + (value > 0 ? (int)d / 2 : -(int)d / 2)) / (int)d;
		result[i] = t41_ccm_clip(value);
	}
	saturation = t41_tmo_le16(p + 72);
	for (i = 0; i < 9; ++i) {
		unsigned int high = t41_tmo_le32(p + 20 + i * 4);
		if (ev < high) {
			saturation = t41_tmo_le16(p + 56);
			if (i) {
				unsigned int low = t41_tmo_le32(p + 16 + i * 4);
				unsigned int a = t41_tmo_le16(p + 54 + i * 2);
				unsigned int b = t41_tmo_le16(p + 56 + i * 2);
				saturation = (unsigned int)t41_tmo_div((unsigned long long)a * (high - ev) +
					(unsigned long long)b * (ev - low) + (high - low) / 2, high - low);
			}
			break;
		}
	}
	for (i = 0; i < 9; ++i) matrix[i] = result[i];
	*sat = saturation;
	return 0;
}

static inline int t41_ccm_saturate(const short *matrix, unsigned int sat,
		unsigned int csc, const int *custom_luma, short *out)
{
	/* The OEM's Q16 luminance coefficients for its three CSC families. */
	static const int luma[3][3] = {
		{ 19595, 38470, 7471 }, { 13933, 46871, 4732 }, { 17216, 44434, 3886 },
	};
	/* Complements are quantized independently from the floating CSC
	 * coefficients. In family 2, round((1-Kg)*65536) != round(Kr*65536)+round(Kb*65536). */
	static const int complement[3][3] = {
		{ 45941, 27066, 58065 }, { 51603, 18665, 60804 }, { 48320, 21103, 61650 },
	};
	int transform[9], result[9];
	const int *w = csc < 6 ? luma[csc / 2] : custom_luma;
	unsigned int row, col, k;
	if (!matrix || !out || !w || sat > 1024)
		return -1;
	for (k = 0; k < 3; ++k)
		if (w[k] < 0 || w[k] > 65536)
			return -1;
	for (k = 0; k < 9; ++k)
		if (matrix[k] < -8192 || matrix[k] > 8191)
			return -1;
	for (row = 0; row < 3; ++row)
		for (col = 0; col < 3; ++col)
			transform[row * 3 + col] = row == col ?
				w[col] + t41_ccm_round((long long)(csc < 6 ? complement[csc/2][col] :
					w[(col+1)%3] + w[(col+2)%3]) * sat, 8) :
				w[col] - t41_ccm_round((long long)w[col] * sat, 8);
	for (row = 0; row < 3; ++row)
		for (col = 0; col < 3; ++col) {
			int sum = 0;
			for (k = 0; k < 3; ++k)
				sum += t41_ccm_round((int)(unsigned int)((long long)matrix[row*3+k] *
					transform[k*3+col]), 10);
			result[row*3+col] = t41_ccm_clip(t41_ccm_round(sum, 6));
		}
	for (k = 0; k < 9; ++k) out[k] = result[k];
	return 0;
}

static inline int t41_ccm_pack(const unsigned char *p, unsigned int bytes,
		const short *matrix, unsigned int *words)
{
	unsigned int i, distance;
	if (!p || bytes != T41_CCM_PARAM_BYTES || !matrix || !words)
		return -1;
	for (i = 0; i < 9; ++i)
		if (matrix[i] < -8192 || matrix[i] > 8191)
			return -1;
	for (i = 0; i < 4; ++i)
		words[i] = (matrix[i*2] & 0x3fff) | ((matrix[i*2+1] & 0x3fffU) << 16);
	words[4] = matrix[8] & 0x3fff;
	words[5] = ((p[16] & 1U) << 12) | ((unsigned int)p[17] << 16) | p[18];
	distance = p[17] > p[18] ? p[17] - p[18] : p[18] - p[17];
	words[6] = distance ? ((distance / 2 + 32) / distance) & 31 : 0;
	words[7] = (t41_tmo_le16(p + 12) & 0x1ff) | ((t41_tmo_le16(p + 14) & 0x1fff) << 16);
	return 0;
}
#endif
