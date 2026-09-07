/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_YDNS_H
#define TX_ISP_T41_YDNS_H
#include "tx_isp_t41_dpc.h"
#define T41_YDNS_PARAM_BYTES 0xf5U
#define T41_YDNS_STATE_BYTES 0x16U
#define T41_YDNS_WRITES 8U
static inline int t41_ydns_interpolate(const unsigned char *p, unsigned int bytes,
		unsigned char *s, unsigned int state_bytes, unsigned int gain)
{
	unsigned int i;
	if (!p || !s || bytes<T41_YDNS_PARAM_BYTES || state_bytes<T41_YDNS_STATE_BYTES || gain>(16U<<16)) return -1;
	s[12]=t41_dpc_interpolate(p+3,gain,1); s[13]=t41_dpc_interpolate(p+14,gain,1);
	for(i=0;i<12;++i) s[i]=t41_dpc_interpolate(p+0x19+i*11,gain,1);
	for(i=14;i<22;++i) s[i]=t41_dpc_interpolate(p+0x9d+(i-14)*11,gain,1);
	return 0;
}
static inline int t41_ydns_pack(const unsigned char *p, unsigned int bytes,
		const unsigned char *s, unsigned int state_bytes, unsigned int channel,
		struct t41_dpc_word *out, unsigned int capacity)
{
	unsigned int n=0,base=(channel+0x100)<<8;
	if (!p || !s || !out || bytes<T41_YDNS_PARAM_BYTES || state_bytes<T41_YDNS_STATE_BYTES ||
	    channel>1 || capacity<T41_YDNS_WRITES) return -1;
#define E(a,v) do { out[n].address=base+(a); out[n++].value=(v); } while (0)
#define S(o) ((unsigned int)s[o])
	E(8,p[0]); E(0x10,((unsigned int)p[2]<<4 & 0x10) | (p[1]&1));
	E(0x20,(S(1)<<8 & 0x300) | (S(2)<<12 & 0x3000) | S(0));
	E(0x24,t41_tmo_le32(s+3)); E(0x28,t41_tmo_le32(s+7));
	E(0x40,S(12)<<8 | S(13)<<16 | S(11));
	E(0x44,t41_tmo_le32(s+14)); E(0x48,t41_tmo_le32(s+18));
#undef S
#undef E
	return n;
}
#endif
