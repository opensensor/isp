/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_GIB_H
#define TX_ISP_T41_GIB_H

/* Normalize the remaining 12-bit range after calibrated black subtraction.
 * H20250310a uses a rounded Q12 reciprocal, capped at two times unity. */
static inline int t41_gib_self_gain(const unsigned short black[5],
		unsigned int infrared, unsigned int *out)
{
	unsigned int i, maximum = 0, quotient;
	if (!black || !out || infrared > 1) return -1;
	for (i = 0; i < 4 + infrared; ++i)
		if (black[i] > maximum) maximum = black[i];
	if (maximum >= 2048) { *out = 8192; return 0; }
	quotient = 33554432U / (4096-maximum);
	*out = (quotient >> 1) + (quotient & 1);
	return 0;
}

/* Compose AE's per-channel Q10 digital gains with black-range normalization.
 * Keep OEM u32 product truncation, rounded shifts and register range stages.
 * out: 0x803c shift, 0x8000 packed channels, 0x8004 packed channels. */
static inline int t41_gib_dgain(unsigned int self_gain, unsigned int infrared,
		const unsigned int gains[4], unsigned int out[3])
{
	unsigned int i, maximum = 0, value[4], shift, floor;
	if (!gains || !out || infrared > 1 || self_gain < 4096 || self_gain > 8192)
		return -1;
	for (i = 0; i < 4; ++i) {
		value[i] = ((unsigned long long)(self_gain*gains[i])+2048) >> 12;
		if ((i < 3 || infrared) && value[i] > maximum) maximum = value[i];
	}
	shift = maximum >= 131072 ? 3 : maximum >= 65536 ? 2 : maximum >= 32768 ? 1 : 0;
	floor = 1024U << shift;
	for (i = 0; i < 4; ++i) {
		if (shift == 3) value[i] = 16384;
		else if (shift) value[i] = (value[i] + (1U << (shift-1))) >> shift;
		if (value[i] < floor) value[i] = floor;
	}
	out[0] = shift;
	out[1] = (value[0] & 32767) | ((value[1] & 32767) << 16);
	out[2] = (value[2] & 32767) | ((value[3] & 32767) << 16);
	return 0;
}
#endif
