/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_TMO_MAP_H
#define TX_ISP_T41_TMO_MAP_H

#include "tx_isp_t41_tmo.h"
#include "tx_isp_t41_tmo_kernels.h"

#define T41_TMO_COLS 25U
#define T41_TMO_ROWS 15U
#define T41_TMO_TILES (T41_TMO_COLS * T41_TMO_ROWS)
#define T41_TMO_BINS 10U
#define T41_TMO_SAMPLES (T41_TMO_TILES * T41_TMO_BINS)

/* ISA-scalar translation of the H20250310a local histogram filter. Input
 * is tile-major, output is bin-major. All Q shifts, lane-width truncations,
 * zero padding and temporal rounding are deliberate. No sensor identity or
 * captured output is involved. Callers own serialization / complete frames.
 * Modes 0/1 select spatial/range kernels from calibration; mode 2's separate
 * adaptive kernel selector is not yet recovered and is rejected explicitly.
 */
static inline int t41_tmo_map(const unsigned char *p, unsigned int bytes,
		unsigned int shift, const unsigned int *sums,
		const unsigned int *counts, const unsigned int *previous,
		int first_frame, unsigned int *out)
{
	unsigned int tile, bin, radius, mode;
	if (!p || bytes != T41_TMO_PARAM_BYTES || !sums || !counts ||
	    !previous || !out || out == sums || out == counts ||
	    shift > 9 || p[0x1742] > 4 || p[0x1739] > 1 || p[0x173c] > 1)
		return -1;
	radius = p[0x1742];
	mode = p[0x1739];
	if (mode == 1 && (p[0x173b] > 10 || p[0x173a] > 14))
		return -1;
	for (tile = 0; tile < T41_TMO_TILES; ++tile) {
		if (!mode && (p[0x230 + tile] > 10 || p[0x3a7 + tile] > 14))
			return -1;
	}
	for (tile = 0; tile < T41_TMO_SAMPLES; ++tile) {
		if (sums[tile] > 65535 || counts[tile] > 65535 ||
		    (!first_frame && previous[tile] > 4095))
			return -1;
	}
	for (tile = 0; tile < T41_TMO_TILES; ++tile) {
		unsigned int n[T41_TMO_BINS] = { 0 };
		unsigned int s[T41_TMO_BINS] = { 0 };
		unsigned int sk = mode ? p[0x173b] : p[0x230 + tile];
		unsigned int rk = mode ? p[0x173a] : p[0x3a7 + tile];
		unsigned int extra = radius == 4 ? 2 : 0;
		int row = tile / T41_TMO_COLS, col = tile % T41_TMO_COLS;
		int dy, dx;
		for (dy = -(int)radius; dy <= (int)radius; ++dy) {
			int y = row + dy;
			if (y < 0 || y >= (int)T41_TMO_ROWS)
				continue;
			for (dx = -(int)radius; dx <= (int)radius; ++dx) {
				int x = col + dx;
				unsigned int ax = dx < 0 ? -dx : dx;
				unsigned int ay = dy < 0 ? -dy : dy;
				unsigned int weight, src, swap;
				if (x < 0 || x >= (int)T41_TMO_COLS)
					continue;
				if (ax > ay) { swap = ax; ax = ay; ay = swap; }
				weight = t41_tmo_spatial_kernels[sk][ay * (ay + 1) / 2 + ax];
				if (!weight)
					continue;
				src = (y * T41_TMO_COLS + x) * T41_TMO_BINS;
				for (bin = 0; bin < T41_TMO_BINS; ++bin) {
					unsigned int count = counts[src + bin];
					unsigned int luma = ((sums[src + bin] << 9) +
						count * (819U * bin)) >> shift;
					n[bin] += weight * count;
					s[bin] += (weight * luma) >> extra;
				}
			}
		}
		for (bin = 0; bin < T41_TMO_BINS; ++bin) {
			n[bin] >>= 8;
			s[bin] >>= 10 - extra;
		}
		for (bin = 0; bin < T41_TMO_BINS; ++bin) {
			unsigned long long numerator = 0, denominator = 0;
			unsigned int value, divisor, index = bin * T41_TMO_TILES + tile;
			int k;
			for (k = -2; k <= 2; ++k) {
				int b = (int)bin + k;
				unsigned int w = t41_tmo_range_kernels[rk][k < 0 ? -k : k];
				if (b < 0 || b >= (int)T41_TMO_BINS)
					continue;
				numerator += (w * s[b]) >> 1;
				denominator += w * n[b];
			}
			divisor = (unsigned int)(denominator >> (shift + 5));
			if (!divisor)
				divisor = 1;
			value = (((unsigned int)(numerator >> 2) / divisor) >> 1) & 32767;
			if (p[0x173c]) {
				unsigned int floor = (bin * 4095 + 5) / 10;
				if (value < floor)
					value = floor;
			}
			if (value > 4095)
				value = 4095;
			if (!first_frame)
				value = (previous[index] * 15 + value) >> 4;
			out[index] = value;
		}
	}
	return 0;
}

#endif
