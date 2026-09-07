/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AE_H
#define TX_ISP_T41_AE_H
#include "tx_isp_t41_tmo.h"

#define T41_AE_PARAM_BYTES 0x910U
#define T41_AE_STATE_BYTES 0x2618U
#define T41_AE_ZONE_COUNT 225U
#define T41_AE_TARGET_KNOTS 15U

static inline void t41_ae_put32(unsigned char *p, unsigned int v)
{
	p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24;
}

/* Packed T41 DMA record -> six 15-stride planes. Validate all lengths and
 * dimensions before writing. Padding zones and unrelated state stay intact. */
static inline int t41_ae_statistics(const unsigned char *dma,
		unsigned int bytes, unsigned int rows, unsigned int cols,
		unsigned char *state, unsigned int state_bytes)
{
	unsigned int r, c;
	if (!dma || !state || state_bytes != T41_AE_STATE_BYTES ||
	    !rows || rows > 15 || !cols || cols > 15 ||
	    bytes < rows * cols * 16)
		return -1;
	state[0x219e] = (t41_tmo_le32(dma + 12) >> 28) & 3;
	for (r = 0; r < rows; ++r) for (c = 0; c < cols; ++c) {
		unsigned int at = (r * 15 + c) * 4;
		const unsigned char *p = dma + (r * cols + c) * 16;
		unsigned int a = t41_tmo_le32(p), b = t41_tmo_le32(p + 4);
		unsigned int d = t41_tmo_le32(p + 8), e = t41_tmo_le32(p + 12);
		t41_ae_put32(state + 0x800 + at, a & 0x3fffff);
		t41_ae_put32(state + 0xb84 + at, ((b << 10) & 0x3ffc00) | (a >> 22));
		t41_ae_put32(state + 0xf08 + at, ((d << 20) & 0x300000) | (b >> 12));
		t41_ae_put32(state + 0x128c + at, (d >> 2) & 0x3fff);
		t41_ae_put32(state + 0x1610 + at, t41_tmo_le16(p + 10) & 0x3fff);
		t41_ae_put32(state + 0x1994 + at, ((e << 2) & 0x3ffffc) | (d >> 30));
	}
	return 0;
}

/* OEM drops exposure's fractional bits before interpolating. Its divider
 * only supports a 32-bit knot difference; reject larger differences. */
static inline int t41_ae_long_target(unsigned long long exposure,
		const unsigned long long *knots, const unsigned short *targets,
		unsigned int shift, unsigned int *out)
{
	unsigned int i, a, b, delta;
	unsigned long long x;
	if (!knots || !targets || !out || shift > 31)
		return -1;
	for (i = 0; i < T41_AE_TARGET_KNOTS; ++i)
		if (knots[i] > (~0ULL >> shift) ||
		    (i && (knots[i] <= knots[i-1] ||
			   knots[i] - knots[i-1] > 0xffffffffULL)))
			return -1;
	x = exposure >> shift;
	if (x <= knots[0]) { *out = targets[0]; return 0; }
	if (x >= knots[14]) { *out = targets[14]; return 0; }
	for (i = 0; i < 13 && x > knots[i+1]; ++i) { }
	a = targets[i]; b = targets[i+1];
	delta = (unsigned int)t41_tmo_div((unsigned long long)(a > b ? a-b : b-a) *
		(x - knots[i]), knots[i+1] - knots[i]);
	*out = a > b ? a-delta : a+delta;
	return 0;
}

/* These operations intentionally retain H20250310a's 32-bit truncations. */
static inline unsigned int t41_ae_fixed_div(unsigned int shift,
		unsigned int a, unsigned int b)
{
	unsigned int i, rem = a % b, frac = 0;
	for (i = 0; i < shift; ++i) {
		rem <<= 1; frac <<= 1;
		if (rem >= b) { rem -= b; frac |= 1; }
	}
	return frac | (a / b << shift);
}
static inline unsigned int t41_ae_fixed_mul(unsigned int shift,
		unsigned int a, unsigned int b)
{
	unsigned int mask = (1U << shift) - 1;
	return (a & mask) * (b >> shift) + (a >> shift) * (b & mask) +
		((a >> shift) * (b >> shift) << shift) +
		(((a & mask) * (b & mask)) >> shift);
}

struct t41_ae_meter {
	unsigned int mean, foreground, background, bright_q, dark_q;
};

/* Optional calibration target compensation, not a scene-specific preset.
 * The three target factors multiply as u32 before a signed /16384 and a
 * u16 store. The EV product wraps at 64 bits before its seven-bit shift. */
static inline int t41_ae_target_tables(const unsigned char *p, unsigned int bytes,
		unsigned long long *knots, unsigned short *targets)
{
	unsigned int i, adjust;
	if (!p || !knots || !targets || bytes != T41_AE_PARAM_BYTES)
		return -1;
	adjust = t41_tmo_le16(p+0x7c4);
	if (adjust > 1) return -1;
	for (i = 0; i < T41_AE_TARGET_KNOTS; ++i) {
		unsigned long long ev = t41_tmo_le32(p+0x5d0+i*8) |
			((unsigned long long)t41_tmo_le32(p+0x5d4+i*8) << 32);
		unsigned int target = t41_tmo_le16(p+0x76e + i*2);
		if (adjust) {
			unsigned int scale = t41_tmo_le16(p+0x7c6);
			unsigned int product = ((target*scale) >> 7) *
				t41_tmo_le16(p+0x7d4+i*2) * t41_tmo_le16(p+0x7c8);
			long long signed_product = product;
			if (product & 0x80000000U) signed_product -= 0x100000000LL;
			target = (unsigned short)(signed_product / 16384);
			ev = (ev*scale) >> 7;
			if (!ev) ev = 1;
			if (!target) target = 1;
		}
		knots[i] = ev; targets[i] = target;
	}
	return 0;
}

/* Calibrated zone-weighted RGB metering, with OEM highlight compensation.
 * ROI blending is not handled here; callers must not enable it silently.
 * State includes the current histogram and grid cell sizes, not sensor data.
 * No caller-visible output is changed on an invalid or empty sample. */
static inline int t41_ae_weight_mean(const unsigned char *p, unsigned int bytes,
		const unsigned char *s, unsigned int state_bytes,
		struct t41_ae_meter *out)
{
	unsigned int rows, cols, shift, divisor, mode, dark_weight, high_weight;
	unsigned int r, c, numerator = 0, denominator = 0, bright = 0, dark = 0;
	unsigned int fn = 0, fd = 0, bn = 0, bd = 0;
	struct t41_ae_meter v;
	if (!p || !s || !out || bytes != T41_AE_PARAM_BYTES ||
	    state_bytes != T41_AE_STATE_BYTES)
		return -1;
	rows = t41_tmo_le16(p+0x70a); cols = t41_tmo_le16(p+0x70e);
	shift = t41_tmo_le16(p+0x6c0); divisor = t41_tmo_le16(p+0x7b8);
	mode = t41_tmo_le32(p+0x68c);
	if (!rows || rows > 15 || !cols || cols > 15 ||
	    !divisor || !shift || shift > 16 || mode > 1)
		return -1;
	dark_weight = mode ? t41_tmo_le16(p+0x7b4) : 1;
	high_weight = mode ? t41_tmo_le16(p+0x7b6) : 1;
	if (!dark_weight || !high_weight)
		return -1;
	for (r = 0; r < rows; ++r) for (c = 0; c < cols; ++c) {
		unsigned int z = r*15+c, at = z*4;
		unsigned int area = t41_tmo_le16(s+0x21fc+r*2) *
			t41_tmo_le16(s+0x221a+c*2);
		unsigned int a = t41_tmo_le32(s+0x128c+at);
		unsigned int b = t41_tmo_le32(s+0x1610+at);
		unsigned int red = t41_tmo_le32(s+0x800+at);
		unsigned int sum = red + t41_tmo_le32(s+0xb84+at) +
			t41_tmo_le32(s+0xf08+at);
		unsigned int w = p[0x82e + z], f = p[0x4ee + z];
		unsigned int extra = (dark_weight-1)*a/divisor;
		if (!area || f > 8) return -1;
		fn += sum*f; fd += area*f;
		bn += sum*(8-f); bd += area*(8-f);
		bright += w*b; dark += w*a+extra;
		denominator += area*w+extra;
		numerator += sum*w+(dark_weight-1)*red/divisor;
	}
	if (!denominator || !fd || !bd || !(denominator << shift)) return -1;
	v.mean = numerator / denominator;
	if (!v.mean) v.mean = 1;
	v.foreground = fn/fd; v.background = bn/bd;
	v.bright_q = t41_ae_fixed_div(shift, bright << shift, denominator << shift);
	v.dark_q = t41_ae_fixed_div(shift, dark << shift, denominator << shift);
	if (mode) {
		unsigned int i = t41_tmo_le16(p+0x712), start, sum = 0, area, factor;
		unsigned int strength = t41_ae_fixed_div(16, (high_weight-1) << 16, divisor << 16);
		i = i < 255 ? i*2/3 : 255;
		start = i;
		/* The denominator is fixed at the starting threshold, not per bin. */
		denominator = (256-i) << 16;
		for (; i < 256; ++i) {
			factor = t41_ae_fixed_div(16, (i-start) << 16, denominator);
			factor = t41_ae_fixed_mul(16, strength,
				t41_ae_fixed_mul(16, factor, factor));
			sum += t41_ae_fixed_mul(5, t41_tmo_le32(s+i*4) << 5, factor >> 11) >> 5;
		}
		area = rows*cols*t41_tmo_le16(s+0x21fc)*t41_tmo_le16(s+0x221a);
		if (!area) return -1;
		v.mean = (t41_ae_fixed_div(shift, area+sum, area)*v.mean) >> shift;
	}
	if (v.mean > 255) v.mean = 255;
	*out = v;
	return 0;
}
#endif
