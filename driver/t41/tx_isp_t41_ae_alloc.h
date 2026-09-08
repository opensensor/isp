/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AE_ALLOC_H
#define TX_ISP_T41_AE_ALLOC_H
#include "tx_isp_t41_ae.h"

struct t41_ae_allocation {
	unsigned int integration, again, dgain, saturated_frames;
	unsigned short settled;
};

/* Automatic branch of H20250310a tisp_ae_ev_alloc_calc. Inputs are exposure
 * in calibration precision, calibrated limits and the generated flicker
 * lattice. No sensor identity or register/gain-code encoding belongs here.
 * Manual component masks are rejected; variable-FPS list allocation is a
 * separate entry point and is not reproduced here. This scalar helper is
 * not yet wired into the live AE controller. */
static inline int t41_ae_auto_allocate(const unsigned char *p, unsigned int pbytes,
		const unsigned char *s, unsigned int sbytes,
		const unsigned char *cache, unsigned int cbytes,
		unsigned long long ev, unsigned int target,
		struct t41_ae_allocation *out)
{
	unsigned int q, unity, min_e, max_e, min_a, max_a, min_d, max_d;
	unsigned int a, d, e, base, anti, policy, short_scale, last, i, saturated = 0;
	unsigned long long normalized, threshold;
	unsigned int gain;
	struct t41_ae_allocation result;
	if (!p || !s || !cache || !out || pbytes != T41_AE_PARAM_BYTES ||
	    sbytes != T41_AE_STATE_BYTES || cbytes != 0x688 ||
	    cache[0x4f8] || cache[0x4f9] || cache[0x4fb] || cache[0x500])
		return -1;
	q = t41_tmo_le16(p + 0x6c0);
	anti = t41_tmo_le16(p + 0x7a0);
	policy = t41_tmo_le16(p + 0x7a2);
	short_scale = t41_tmo_le16(p + 0x7a4);
	min_e = t41_tmo_le32(cache + 0x260); max_e = t41_tmo_le32(cache + 0x270);
	min_a = t41_tmo_le32(cache + 0x264); max_a = t41_tmo_le32(cache + 0x274);
	min_d = t41_tmo_le32(cache + 0x26c); max_d = t41_tmo_le32(cache + 0x27c);
	last = s[0x2612];
	if (!q || q > 16 || anti > 1 || policy > 2 || !min_e || min_e > max_e ||
	    max_e > (~0U >> q) || !min_a || min_a > max_a || !min_d || min_d > max_d ||
	    (anti && last >= 120) || (anti && policy == 2 && (!target || target > (~0U >> q))))
		return -1;
	if (anti) for (i = 0; i <= (last ? last : 1); ++i) {
		unsigned int node = t41_tmo_le16(s + 0x2438 + i * 2);
		if (!node || (i && node < t41_tmo_le16(s + 0x2438 + (i - 1) * 2))) return -1;
	}
	unity = 1U << q;
	a = t41_ae_fixed_div(q, max_a, min_a);
	d = t41_ae_fixed_div(q, max_d, min_d);
	base = t41_ae_fixed_mul(q, min_a, min_d);
	if (!a || !d || !base) return -1;
	e = max_e << q;
	normalized = t41_ae_fixed_div64(q, ev, base);
	if (normalized < (min_e << q)) normalized = min_e << q;
	if (!anti) {
		if (ev <= t41_ae_fixed_mul(q, base, min_e << q)) {
			a = min_a; d = min_d; e = min_e << q;
		} else if (normalized > e) {
			threshold = t41_ae_fixed_mul64(q, e, a);
			if (!threshold) return -1;
			if (normalized > threshold) {
				if (normalized > t41_ae_fixed_mul64(q, threshold, d)) saturated = 1;
				else d = (unsigned int)t41_ae_fixed_div64(q, normalized, threshold);
			} else { a = (unsigned int)t41_ae_fixed_div64(q, normalized, e); d = unity; }
		} else {
			e = ((unsigned int)normalized >> q) << q;
			if (!e) return -1;
			gain = t41_ae_fixed_div64(q, normalized, e);
			if (gain > a) {
				if (t41_ae_fixed_mul(q, a, d) >= gain)
					d = t41_ae_fixed_div(q, (unsigned int)gain, a);
			} else { a = (unsigned int)gain; d = unity; }
		}
	} else {
		unsigned int first = t41_tmo_le16(s + 0x2438) << q;
		if (normalized < first) {
			if (policy) {
				if (first < e) e = first;
				a = unity; d = unity;
				if (policy != 1) {
					/* The caller supplies its luma target, not FPS:
					 * tisp_ae_tune ELF 0x27a20 stores s6 at sp+28. */
					unsigned int short_e = t41_ae_fixed_mul(q, first,
						t41_ae_fixed_div(q, short_scale << q, target << q)) >> q << q;
					if (short_e < e) e = short_e;
					if (!e) return -1;
					if (normalized >= e) {
						e = ((unsigned int)normalized >> q) << q;
						if (!e) return -1;
						a = t41_ae_fixed_div(q, (unsigned int)normalized, e);
					}
				}
			} else {
				unsigned int truncated = ((unsigned int)normalized >> q) << q;
				if (truncated < e) e = truncated;
				if (!e) return -1;
				gain = t41_ae_fixed_div(q, (unsigned int)normalized, e);
				if (gain > a) {
					if (t41_ae_fixed_mul(q, a, d) >= gain)
						d = t41_ae_fixed_div(q, (unsigned int)gain, a);
				} else { a = (unsigned int)gain; d = unity; }
			}
		} else {
			unsigned int bound = t41_ae_fixed_div(q,
				t41_tmo_le16(s + 0x2438 + last * 2) << q, min_e << q);
			/* The OEM decrements a 16-bit index before testing zero. Avoid
			 * its underflow and out-of-bounds read for a one-node lattice. */
			if (!last && (20U << q) + e < bound) return -1;
			while ((20U << q) + e < bound) {
				if (!last || !--last) { last = 1; break; }
				bound = t41_tmo_le16(s + 0x2438 + last * 2) << q;
			}
			for (i = 1; i <= last; ++i)
				if (normalized < (t41_tmo_le16(s + 0x2438 + i * 2) << q)) break;
			bound = t41_tmo_le16(s + 0x2438 + (i - 1) * 2) << q;
			if (bound < e) e = bound;
			if (!e) return -1;
			gain = t41_ae_fixed_div64(q, normalized, e);
			if (gain <= a) { a = (unsigned int)gain; d = unity; }
			else {
				unsigned int need_d = t41_ae_fixed_div(q, (unsigned int)gain, a);
				if (need_d < d) d = need_d;
				saturated = 1;
			}
		}
	}
	result.integration = e >> q;
	result.again = t41_ae_fixed_mul(q, a, min_a);
	result.dgain = t41_ae_fixed_mul(q, d, min_d);
	result.saturated_frames = saturated ? t41_tmo_le32(cache + 0x4a0) + 1 : 0;
	result.settled = t41_tmo_le16(s + 0x2178);
	if (saturated && result.saturated_frames > t41_tmo_le32(p + 0x64c)) result.settled = 1;
	*out = result;
	return 0;
}
#endif
