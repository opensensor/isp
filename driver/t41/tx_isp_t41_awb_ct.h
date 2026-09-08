/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_CT_H
#define TX_ISP_T41_AWB_CT_H
#include "tx_isp_t41_awb_stats.h"

/* Two OEM interpolation domains: table ordinates need an initial fractional
 * shift; the second bilinear pass already has that shift. Preserve the
 * intermediate fixed-point multiply/divide, not a floating-point lerp. */
static inline int t41_awb_ct_lerp(unsigned int precision, unsigned int fraction,
		unsigned int x, unsigned int a, unsigned int b,
		unsigned int left, unsigned int right, unsigned int table_values,
		unsigned int *out)
{
	unsigned int denominator, distance, delta, base;
	if (!out || precision>31 || fraction>31 || b<=a) return -1;
	denominator=(b-a)<<fraction;
	if (!denominator) return -1;
	distance=x-(a<<fraction);
	delta=right>=left ? right-left : left-right;
	base=left;
	if (table_values) { delta<<=fraction; base<<=fraction; }
	delta=t41_ae_fixed_div(precision,t41_ae_fixed_mul(precision,delta,distance),denominator);
	*out=right>=left ? base+delta : base-delta;
	return 0;
}

/* Calibrated 15x15 reciprocal-temperature surface. Inputs are calibrated
 * R/G and B/G in the parameter-selected fractional domain. Endpoint paths
 * avoid the OEM's unnecessary one-past-axis/table reads at the last knot.
 * They are mathematically the same endpoint value, with the same u32 shifts.
 * Return CT explicitly; the runtime owner chooses when to publish history. */
static inline int t41_awb_ct_calculate(const unsigned char *p, unsigned int pb,
		unsigned int red, unsigned int blue, unsigned int *ct)
{
	unsigned int precision, fraction, x[15], y[15], ix=0, iy=0, i, a, b, mired;
	const unsigned char *map;
	if (!p || pb<0xcd6 || !ct) return -1;
	precision=t41_tmo_le16(p+0xcd2); fraction=t41_tmo_le16(p+0xcd4);
	if (precision>31 || !fraction || fraction>31) return -1;
	for (i=0;i<15;++i) {
		x[i]=t41_tmo_le32(p+0x38+i*4); y[i]=t41_tmo_le32(p+0x74+i*4);
		if (x[i]>(~0U>>fraction) || y[i]>(~0U>>fraction) ||
		    (i && (x[i]<=x[i-1] || y[i]<=y[i-1]))) return -1;
	}
	if (red<(x[0]<<fraction)) red=x[0]<<fraction;
	if (red>(x[14]<<fraction)) red=x[14]<<fraction;
	if (blue<(y[0]<<fraction)) blue=y[0]<<fraction;
	if (blue>(y[14]<<fraction)) blue=y[14]<<fraction;
	while (ix<14 && red>=(x[ix+1]<<fraction)) ++ix;
	while (iy<14 && blue>=(y[iy+1]<<fraction)) ++iy;
	map=p+0x870+(iy*15+ix)*4;
	if (ix==14) {
		a=t41_tmo_le32(map)<<fraction;
		if (iy==14) mired=a;
		else if (t41_awb_ct_lerp(precision,fraction,blue,y[iy],y[iy+1],
			t41_tmo_le32(map),t41_tmo_le32(map+60),1,&mired)) return -1;
	} else {
		if (t41_awb_ct_lerp(precision,fraction,red,x[ix],x[ix+1],
			t41_tmo_le32(map),t41_tmo_le32(map+4),1,&a)) return -1;
		if (iy==14) mired=a;
		else {
			if (t41_awb_ct_lerp(precision,fraction,red,x[ix],x[ix+1],
				t41_tmo_le32(map+60),t41_tmo_le32(map+64),1,&b)) return -1;
			if (t41_awb_ct_lerp(precision,fraction,blue,y[iy],y[iy+1],a,b,0,&mired)) return -1;
		}
	}
	*ct=mired ? ((1U<<(fraction-1))+
		t41_ae_fixed_div(fraction,1000000U<<fraction,mired))>>fraction : 5000;
	return 0;
}
#endif
