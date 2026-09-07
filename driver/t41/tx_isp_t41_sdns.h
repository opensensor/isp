/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_SDNS_H
#define TX_ISP_T41_SDNS_H
#include "tx_isp_t41_dpc.h"
#define T41_SDNS_PARAM_BYTES 0x384U
#define T41_SDNS_STATE_BYTES 0x4cU
#define T41_SDNS_STATIC_WRITES 29U
#define T41_SDNS_DYNAMIC_WRITES 27U

static inline int t41_sdns_interpolate(const unsigned char *p, unsigned int bytes,
		unsigned char *s, unsigned int state_bytes, unsigned int gain,
		unsigned int strength)
{
	unsigned int i;
	if (!p || !s || bytes<T41_SDNS_PARAM_BYTES || state_bytes<T41_SDNS_STATE_BYTES ||
	    gain>(16U<<16) || strength>255) return -1;
	s[0]=t41_dpc_interpolate(p+0x6e,gain,1); s[1]=t41_dpc_interpolate(p+0x79,gain,1);
	s[2]=t41_dpc_interpolate(p+0xb,gain,1); s[3]=t41_dpc_interpolate(p+0x16,gain,1);
	t41_dpc_put16(s+4,t41_dpc_interpolate(p+0x58,gain,2));
	for (i=6;i<=65;++i) s[i]=t41_dpc_interpolate(p+0x84+(i-6)*11,gain,1);
	for (i=66;i<=74;++i) s[i]=t41_dpc_interpolate(p+0x320+(i-66)*11,gain,1);
	for (i=16;i<=31;++i) s[i]=tx_isp_tuning_ratio_u32(strength,s[i],16);
	s[74]=tx_isp_tuning_ratio_u32(strength,s[74],200);
	return 0;
}

static inline int t41_sdns_pack_static(const unsigned char *p, unsigned int bytes,
		unsigned int channel, struct t41_dpc_word *out, unsigned int capacity)
{
	unsigned int n=0,i,j,base=(channel+0x50)<<10;
	if (!p || !out || bytes<T41_SDNS_PARAM_BYTES || channel>1 || capacity<T41_SDNS_STATIC_WRITES) return -1;
#define E(a,v) do { out[n].address=base+(a); out[n++].value=(v); } while (0)
#define P(o) ((unsigned int)p[o])
	E(0,P(9)<<16 | P(8)); E(0x20,P(0x21)<<8 | P(0x42));
	for (j=0;j<2;++j) {
		unsigned int offset=0x22+j*15;
		for (i=0;i<7;++i) E(0x100+j*32+i*4,P(offset+i*2+1)<<18 | P(offset+i*2)<<2);
		E(0x11c+j*32,P(offset+14)<<2);
	}
	E(0x194,t41_tmo_le32(p+0x318)); E(0x198,t41_tmo_le32(p+0x31c));
	for (i=0;i<3;++i) E(0x16c+i*4,P(0x49+i*2)<<18 | P(0x48+i*2)<<2);
	E(0x178,P(0x4e)<<2); E(0x17c,t41_tmo_le32(p+0x4f)); E(0x180,t41_tmo_le32(p+0x53));
	E(0x8c,t41_tmo_le32(p)); E(0x90,t41_tmo_le32(p+4)); E(0x34,P(0x44)<<16 | P(0x43));
#undef P
#undef E
	return n;
}

static inline int t41_sdns_pack_dynamic(const unsigned char *p, unsigned int bytes,
		const unsigned char *s, unsigned int state_bytes, unsigned int channel,
		struct t41_dpc_word *out, unsigned int capacity)
{
	unsigned int n=0,i,base=(channel+0x50)<<10;
	if (!p || !s || !out || bytes<T41_SDNS_PARAM_BYTES || state_bytes<T41_SDNS_STATE_BYTES ||
	    channel>1 || capacity<T41_SDNS_DYNAMIC_WRITES) return -1;
#define E(a,v) do { out[n].address=base+(a); out[n++].value=(v); } while (0)
#define P(o) ((unsigned int)p[o])
#define S(o) ((unsigned int)s[o])
	E(4,S(1)<<20 | S(0)<<2); E(0xc,S(3)<<18 | S(2)<<1);
	E(0x10,t41_tmo_le32(s+4)); E(0x184,t41_tmo_le32(s+8)); E(0x188,t41_tmo_le32(s+0xc));
	for (i=0;i<8;++i) E(0x140+i*4,t41_tmo_le32(s+0x10+i*4));
	E(0x1c,S(0x32)<<8 | S(0x30)); E(0x18,S(0x31)<<8 | P(0x41)<<16 | S(0x41));
	for (i=0;i<3;++i) E(0x160+i*4,S(0x34+i*2)<<18 | S(0x33+i*2)<<2);
	E(0x18c,t41_tmo_le32(s+0x39)); E(0x190,t41_tmo_le32(s+0x3d));
	E(0x94,S(0x42)); E(0x30,S(0x43)<<18 | P(0x47)<<4); E(0x38,S(0x45)<<18 | S(0x44)<<2);
	E(0x24,S(0x46)<<18 | P(0x45)); E(0x28,S(0x48)<<18 | S(0x47)<<2);
	E(0x2c,S(0x49)<<8 | P(0x46) | S(0x4a)<<18);
	E(0x88,P(0xa)<<16 | P(0x40)<<24);
#undef S
#undef P
#undef E
	return n;
}
#endif
