/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_CDNS_H
#define TX_ISP_T41_CDNS_H
#include "tx_isp_t41_dpc.h"
#define T41_CDNS_PARAM_BYTES 0x161U
#define T41_CDNS_STATE_BYTES 0x20U
#define T41_CDNS_WRITES 12U
static inline int t41_cdns_interpolate(const unsigned char *p, unsigned int bytes,
		unsigned char *s, unsigned int state_bytes, unsigned int gain)
{
	unsigned int i;
	if (!p || !s || bytes<T41_CDNS_PARAM_BYTES ||
	    state_bytes<T41_CDNS_STATE_BYTES || gain>(16U<<16)) return -1;
	s[26]=t41_dpc_interpolate(p+1,gain,1); s[27]=t41_dpc_interpolate(p+12,gain,1);
	for (i=0;i<26;++i) s[i]=t41_dpc_interpolate(p+0x17+i*11,gain,1);
	for (i=28;i<32;++i) s[i]=t41_dpc_interpolate(p+0x135+(i-28)*11,gain,1);
	return 0;
}
static inline int t41_cdns_pack(const unsigned char *p, unsigned int bytes,
		const unsigned char *s, unsigned int state_bytes, unsigned int channel,
		struct t41_dpc_word *out, unsigned int capacity)
{
	unsigned int n=0,i,base=(channel+0x380)<<7,reciprocal;
	int lower,upper;
	if (!p || !s || !out || bytes<T41_CDNS_PARAM_BYTES || state_bytes<T41_CDNS_STATE_BYTES ||
	    channel>1 || capacity<T41_CDNS_WRITES) return -1;
#define E(a,v) do { out[n].address=base+(a); out[n++].value=(v); } while (0)
#define S(o) ((unsigned int)s[o])
	E(8,p[0]); E(0x10,S(0)&15);
	for (i=0;i<8;++i) E(0x20+i*4,S(1+i*3) | S(2+i*3)<<8 | S(3+i*3)<<16);
	E(0x40,S(25) | S(26)<<8 | S(27)<<16 | (S(28)<<24 & 0x3000000) | (S(29)<<28 & 0x30000000));
	/* These two thresholds are signed bytes, unlike the surrounding fields. */
	lower=(int)(s[30]^128)-128; upper=(int)(s[31]^128)-128;
	if (upper<lower) upper=lower;
	reciprocal=upper==lower ? 0 : 4080/(upper-lower);
	E(0x50,((unsigned int)lower&255) | ((unsigned int)upper&255)<<8 | (reciprocal<<16 & 0xfff0000));
#undef S
#undef E
	return n;
}
#endif
