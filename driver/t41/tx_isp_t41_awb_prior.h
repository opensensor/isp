/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_PRIOR_H
#define TX_ISP_T41_AWB_PRIOR_H
#include "tx_isp_t41_awb_stats.h"

/* Build the estimator's calibrated CT prior and native pointer view.
 * The fixed offsets describe an ISP ABI, not a particular sensor's values.
 * Public calibration is copied by the runtime owner before this function.
 * Integer interpolation deliberately keeps OEM low-word products and its
 * signed floor division (the four temperature knots use unsigned division). */
static inline int t41_awb_prior_prepare(unsigned char *p, unsigned int pb,
		unsigned char *s, unsigned int sb, void *slots[45],
		unsigned short cluster_red[6], unsigned short cluster_blue[6])
{
	static const unsigned short offset[45] = {
		0xce2,0x34cc,0x392c,0x393c,0x4ec,0xc6c,0x38,0x74,0x870,0x1200,
		0xf0,0x28,0x3520,0xcd2,0xd60,0xd56,0x120,0xd0a,0x34e0,0x140,
		0x34ea,0x34fe,0,0,0x11d4,0x11dc,0x11e4,0x11e8,0x11ec,0x11f0,
		0x1c20,0x2a30,0x3138,0xc6b8,0xc9c8,0xccd8,0xce60,0xcef0,
		0xd1c0,0xd580,0xd904,0xdc88,0xe00c,0xe390,0xe714};
	static const unsigned char state_slot[45] = {
		0,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	unsigned int low, high, step, ev, day, night, shift, df, nf, floor, enabled;
	unsigned int left=0xd0, right=0xd0, n=0, i;
	if (!p || pb<0x12e1 || !s || sb<T41_AWB_STATE_BYTES || !slots ||
	    !cluster_red || !cluster_blue) return -1;
	low=t41_tmo_le32(p+0x1c); high=t41_tmo_le32(p+0x20);
	ev=t41_tmo_le32(s+0xea98);
	day=t41_tmo_le32(p+0xe0); night=t41_tmo_le32(p+0xe8);
	if (high<=low || high>0x3fffffff || ev>0x7fffffff || day>1 || night>1)
		return -1;
	step=(high-low)/3;
	if (!step) return -1;
	shift=t41_tmo_le16(p+0xcd4)&31;
	df=t41_tmo_le32(p+0xe4)<<shift; nf=t41_tmo_le32(p+0xec)<<shift;
	enabled=ev>=high ? night : day;
	floor=ev>=high ? nf : df;
	if (day) {
		left=right=0xc0;
		if (ev<low) left=right=0xb0;
		else if (ev<low+step) { left=0xb0; n=ev-low; }
		else if (ev>=high) {
			n=ev-high;
			if (n>=step) { n=step; left=right=night ? 0xd0 : 0xc0; }
			else if (night) right=0xd0;
			if (!night) enabled=n<step ? day : 0;
			floor=(unsigned int)((int)(df*step+((night ? nf : 65536U)-df)*n)/(int)step);
			if (n==step) floor=night ? nf : 65536;
		}
	} else {
		floor=65536;
		if (night && ev>=high) {
			unsigned int at=ev-high;
			floor=at>=step ? nf :
				(unsigned int)((int)((step<<16)-(65536U-nf)*at)/(int)step);
		}
	}
	t41_ae_put32(p+0x2c,t41_tmo_le32(p+0x28));
	t41_ae_put32(s+0x34bc,day); t41_ae_put32(s+0x34c0,df);
	t41_ae_put32(s+0x34c4,night); t41_ae_put32(s+0x34c8,nf);
	s[0x34cc]=ev>=high ? p[0x11f4] : 0; s[0x34cd]=p[0x11f5];
	t41_ae_put32(s+0x393c,enabled); t41_ae_put32(s+0x3940,floor);
	for (i=0;i<4;++i) {
		unsigned int a=t41_tmo_le32(p+left+i*4), b=t41_tmo_le32(p+right+i*4);
		t41_ae_put32(s+0x392c+i*4,left==right ? a : (a*step+(b-a)*n)/step);
	}
	for (i=0;i<45;++i) slots[i]=(state_slot[i] ? s : p)+offset[i];
	slots[22]=cluster_red; slots[23]=cluster_blue;
	return 0;
}
#endif
