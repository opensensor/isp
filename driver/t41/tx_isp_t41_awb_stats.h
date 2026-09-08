/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_STATS_H
#define TX_ISP_T41_AWB_STATS_H
#include "tx_isp_t41_ae.h"

#define T41_AWB_STATE_BYTES 0xf54cU
#define T41_AWB_PLANE_BYTES 0x384U

/* H20250310a AWB DMA: eight packed records per zone, or one selected
 * luminance-class pair. Each pair contains independent global/neutral
 * selections, not Bayer phases. Preserve unselected classes and padding. */
static inline int t41_awb_statistics(const unsigned char *p, unsigned int pb,
		const unsigned char *dma, unsigned int db,
		unsigned char *s, unsigned int sb)
{
	unsigned int rows, cols, mode, selected, stride, r, c, bin, kind, plane;
	if (!p || pb < 0xcce || !dma || !s || sb < T41_AWB_STATE_BYTES)
		return -1;
	rows = t41_tmo_le16(p+0xc6e); cols = t41_tmo_le16(p+0xc72);
	mode = p[0xcca]; selected = p[0xccc];
	if (!rows || rows > 15 || !cols || cols > 15 || mode > 1 || selected > 3)
		return -1;
	stride = mode ? 32 : 128;
	if (db < rows * cols * stride) return -1;
	for (r=0; r<rows; ++r) for (c=0; c<cols; ++c)
		for (bin=0; bin<4; ++bin) {
			const unsigned char *src;
			if (mode && bin != selected) continue;
			src = dma + (r*cols+c)*stride + (mode ? 0 : bin*32);
			for (kind=0; kind<2; ++kind) {
				unsigned int w0=t41_tmo_le32(src+kind*16);
				unsigned int w1=t41_tmo_le32(src+kind*16+4);
				unsigned int w2=t41_tmo_le32(src+kind*16+8);
				unsigned int w3=t41_tmo_le32(src+kind*16+12);
				unsigned int values[5] = {w0 & 0x3fffff,
					((w1<<10)&0x3ffc00)|(w0>>22),
					((w2<<20)&0x300000)|(w1>>12),
					(w2>>2)&0x3fffff, ((w3<<8)&0x3f00)|(w2>>24)};
				unsigned int base = (kind ? 0x3944 : 0x7f94) + bin*0x1194;
				for (plane=0; plane<5; ++plane)
					t41_ae_put32(s+base+plane*T41_AWB_PLANE_BYTES+(r*15+c)*4,
						values[plane]);
			}
		}
	return 0;
}

/* Relative saturation is min(R,G,B)/max(R,G,B), or /mean(R,G,B).
 * Calibration selects precision and count gate; supplied gains are the
 * previous estimator gains. OEM multiplies/truncates to u32 before shifting.
 * Validate divisors in a dry pass so malformed inputs cannot half-update. */
static inline int t41_awb_saturation_weights(const unsigned char *p, unsigned int pb,
		const unsigned int red[225], const unsigned int green[225],
		const unsigned int blue[225], const unsigned int count[225],
		const unsigned int gains[2], unsigned int out[225])
{
	unsigned int rows, cols, precision, threshold, mode, pass, r, c;
	if (!p || pb < 0xd58 || !red || !green || !blue || !count || !gains || !out)
		return -1;
	rows=t41_tmo_le16(p+0xc6e); cols=t41_tmo_le16(p+0xc72);
	precision=t41_tmo_le16(p+0xcd2)&31;
	mode=t41_tmo_le16(p+0xd56); threshold=t41_tmo_le32(p);
	if (!rows || rows>15 || !cols || cols>15) return -1;
	for (pass=0; pass<2; ++pass) for (r=0; r<rows; ++r) for (c=0; c<cols; ++c) {
		unsigned int i=r*15+c, value=1;
		if (green[i] && count[i]>threshold) {
			unsigned int rv=(red[i]*gains[0])>>precision;
			unsigned int bv=(blue[i]*gains[1])>>precision;
			unsigned int lo=rv<bv ? rv : bv, den;
			if (green[i]<lo) lo=green[i];
			if (lo) {
				den=rv>bv ? rv : bv;
				if (green[i]>den) den=green[i];
				if (mode!=1) den=(rv+bv+green[i])/3;
				if (!den) return -1;
				value=(lo<<precision)/den;
			}
		}
		if (pass) out[i]=value;
	}
	return 0;
}
#endif
