/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_BCSH_H
#define TX_ISP_T41_BCSH_H
#include "tx_isp_t41_ccm.h"
#define T41_BCSH_PARAM_BYTES 360U
#define T41_BCSH_WORDS 29U

static inline int t41_bcsh_round(long long v, unsigned int q)
{
	long long half = 1LL << (q-1);
	return v < 0 ? -(int)((-v+half) >> q) : (int)((v+half) >> q);
}
static inline int t41_bcsh_s32(const unsigned char *p)
{
	return (int)t41_tmo_le32(p);
}
static inline unsigned int t41_bcsh_itp(unsigned int x, unsigned int lo,
		unsigned int hi, unsigned int a, unsigned int b)
{
	if (lo >= hi || x <= lo) return a;
	if (x >= hi) return b;
	return ((hi-x)*a + (x-lo)*b + (hi-lo)/2) / (hi-lo);
}
static inline void t41_bcsh_multiply(int *out, const int *a, const int *b, unsigned int q)
{
	unsigned int row, col, k;
	for (row = 0; row < 3; ++row)
		for (col = 0; col < 3; ++col) {
			long long sum = 0;
			for (k = 0; k < 3; ++k) sum += (long long)a[row*3+k] * b[k*3+col];
			out[row*3+col] = t41_bcsh_round(sum, q);
		}
}

/* Automatic calibration path at neutral B/C/S/H API controls (128 each).
 * The CT/EV inputs and all sensor coefficients remain live. No captured
 * register bank. Non-neutral API control support is a separate interface.
 */
static inline int t41_bcsh_compute(const unsigned char *p, unsigned int bytes,
		unsigned int ct, unsigned int ev, const unsigned char *csc,
		unsigned int csc_bytes, unsigned int *words)
{
	static const unsigned short offsets[11] = {
		0x42, 0x54, 0x6c, 0x7e, 0x90, 0xa2, 0xbe, 0xd0, 0xe2, 0xf4, 0x106,
	};
	unsigned short h[85] = {0};
	unsigned int i, j, index = 0, zone = 0, v[11], low, high;
	int matrix[9], forward[9], inverse[9], tmp[9], result[9], bias[3];
#define BH(off) h[(off)/2]
#define BL(off) t41_tmo_le16(p+(off))
	if (!p || bytes != T41_BCSH_PARAM_BYTES || !csc || csc_bytes != 92 ||
	    !words || ct > 32767 || p[0x166] > 1)
		return -1;
	for (i = 0; i < 9; ++i)
		if (i && t41_tmo_le32(p+i*4) <= t41_tmo_le32(p+i*4-4)) return -1;
	for (i = 0; i < 6; ++i)
		if (!BL(0x24+i*2) || (i && BL(0x24+i*2) <= BL(0x22+i*2))) return -1;
	for (i = 0; i < 9; ++i) {
		forward[i] = t41_bcsh_s32(csc+4+i*4);
		inverse[i] = t41_bcsh_s32(csc+48+i*4);
		if (forward[i] < -262144 || forward[i] > 262144 ||
		    inverse[i] < -262144 || inverse[i] > 262144) return -1;
	}
	while (index < 8 && ev >= t41_tmo_le32(p+index*4)) ++index;
	for (i = 0; i < 11; ++i) {
		if (!index) v[i] = BL(offsets[i]);
		else v[i] = t41_bcsh_itp(ev, t41_tmo_le32(p+index*4-4),
			t41_tmo_le32(p+index*4), BL(offsets[i]+index*2-2), BL(offsets[i]+index*2));
	}
	/* Optional CT-dependent RGB matrix, selected before CSC conversion. */
	while (zone < 6 && ct > BL(0x24+zone*2)) ++zone;
	for (i = 0; i < 9; ++i) {
		unsigned int a = 0x11e + (zone/2)*18 + i*2;
		matrix[i] = t41_ccm_s16(p+a);
		if (zone & 1) {
			int d = BL(0x24+zone*2) - BL(0x22+zone*2);
			int n = ct - BL(0x22+zone*2);
			int value = matrix[i]*(d-n) + t41_ccm_s16(p+a+18)*n;
			matrix[i] = (value + (value > 0 ? d/2 : -d/2)) / d;
		}
		if (!p[0x166]) matrix[i] = i%4 == 0 ? 1024 : 0;
	}
	t41_bcsh_multiply(tmp, forward, matrix, 10);
	t41_bcsh_multiply(result, tmp, inverse, 16);
	for (i = 0; i < 9; ++i) result[i] = t41_bcsh_round(result[i], 6);
	for (i = 0; i < 3; ++i) bias[i] = (int)BL(0x118+i*2) - 1024;
	for (i = 0; i < 3; ++i) {
		long long sum = 0;
		for (j = 0; j < 3; ++j) sum += (int)(unsigned int)((long long)forward[i*3+j]*bias[j]);
		BH(0x6a+i*4) = t41_bcsh_round(sum, 16) + (i ? 1024 : csc[40]*4 + v[10]);
	}
	BH(0x50) = BH(0x58) = csc[42]*4;
	BH(0x52) = BH(0x5a) = csc[43] == 255 ? 1023 : 940;
	BH(0x56) = (csc[43]-csc[42])*4;
	BH(0x5c) = BH(0x64) = csc[44]*4;
	BH(0x5e) = BH(0x66) = csc[45] == 255 ? 1023 : 960;
	BH(0x62) = (csc[45]-csc[44])*4;
	BH(0x68) = (256-csc[40])*4;
	BH(0x6c) = BH(0x70) = 1024;
	/* Chroma/luminance thresholds. Reversed pairs have zero slope. */
	for (i = 0; i < 3; ++i) {
		low = i == 2 ? v[0] : BL(0x32+i*6);
		high = i == 2 ? v[1] : BL(0x34+i*6);
		BH(0x74+i*6) = low < high ? 1024 / (high-low) : 0;
		BH(0x76+i*6) = low < high ? low : high;
		BH(0x78+i*6) = low < high ? high : low;
	}
	BH(0x86) = BL(0xb4);
	BH(0x8e) = v[6]; BH(0x90) = v[7]; BH(0x92) = v[8]; BH(0x94) = v[9];
	if (v[6] >= v[7] || v[9] < v[8]) {
		BH(0x88) = 0; BH(0x8a) = 1024; BH(0x8c) = 0;
		BH(0x8e) = 0; BH(0x90) = 1023; BH(0x92) = 0; BH(0x94) = 1023;
	} else {
		BH(0x88) = v[6] ? (v[8]<<10)/v[6] : 0;
		BH(0x8a) = ((v[9]-v[8])<<10)/(v[7]-v[6]);
		BH(0x8c) = v[7] != 1023 ? ((1023-v[9])<<10)/(1023-v[7]) : 0;
	}
	BH(0x9e) = v[2]; BH(0xa0) = v[3]; BH(0xa2) = v[4]; BH(0xa4) = v[5];
	BH(0x9a) = BL(0x68); BH(0x9c) = BL(0x6a);
	if (BH(0x9a) >= BH(0x9c) || v[3] < v[2] || v[5] < v[4]) {
		BH(0x9a) = 0; BH(0x9c) = 1;
		BH(0x9e) = BH(0xa0) = BH(0xa2) = BH(0xa4) = 1024;
	} else {
		BH(0x96) = (v[3]-v[2])/(BH(0x9c)-BH(0x9a));
		BH(0x98) = (v[5]-v[4])/(BH(0x9c)-BH(0x9a));
	}
	for (i = 0; i < 6; ++i)
		words[i] = ((BH(0x50+i*4)&1023U)<<16) | (BH(0x52+i*4)&1023);
	for (i = 0; i < 3; ++i)
		words[6+i] = (BH(0x68+i*4)&2047) | ((BH(0x6a+i*4)&2047U)<<16);
	for (i = 0; i < 3; ++i) {
		words[9+i*2] = (result[i*3]&16383) | ((result[i*3+1]&16383U)<<16);
		words[10+i*2] = result[i*3+2]&16383;
	}
	for (i = 0; i < 3; ++i) {
		words[15+i*2] = ((BH(0x74+i*6)&1023U)<<16) | (p[0x30+i*6]&1);
		words[16+i*2] = (BH(0x76+i*6)&1023) | ((BH(0x78+i*6)&1023U)<<16);
	}
	words[21] = (BH(0x86)&1) | ((BH(0x88)&32767U)<<16);
	words[22] = (BH(0x8a)&32767) | ((BH(0x8c)&32767U)<<16);
	words[23] = (BH(0x8e)&1023) | ((BH(0x90)&1023U)<<16);
	words[24] = (BH(0x92)&1023) | ((BH(0x94)&1023U)<<16);
	words[25] = (p[0x66]&1) | ((BH(0x96)<<3)&65535) | ((BH(0x98)&8191U)<<16);
	words[26] = (BH(0x9a)&1023) | ((BH(0x9c)&1023U)<<16);
	words[27] = ((BH(0x9e)&8191U)<<16) | (BH(0xa0)&8191);
	words[28] = ((BH(0xa2)&8191U)<<16) | (BH(0xa4)&8191);
#undef BH
#undef BL
	return 0;
}
#endif
