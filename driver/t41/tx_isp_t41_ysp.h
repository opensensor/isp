/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_YSP_H
#define TX_ISP_T41_YSP_H
#include "tx_isp_t41_dpc.h"
#define T41_YSP_PARAM_BYTES 0xa78U
#define T41_YSP_STATE_BYTES 0x112U
#define T41_YSP_STATIC_WRITES 13U
#define T41_YSP_DYNAMIC_WRITES 77U

static inline int t41_ysp_interpolate(const unsigned char *p, unsigned int bytes,
		unsigned char *s, unsigned int state_bytes, unsigned int gain)
{
	/* Source and destination widths differ for the 32 sharpening fields. */
	static const struct {
		unsigned short target, source;
		unsigned char target_width, source_width, count;
	} runs[] = {
		{0x000,0x680,1,1,1}, {0x002,0x024,2,2,1},
		{0x004,0x693,1,1,11}, {0x010,0x03a,2,2,1},
		{0x012,0x70c,2,1,32}, {0x052,0x050,2,2,3},
		{0x058,0x86c,1,1,4}, {0x05c,0x89e,1,1,3},
		{0x060,0x092,2,2,1}, {0x062,0x8bf,1,1,2},
		{0x064,0x0a8,2,2,1}, {0x066,0x8d5,1,1,6},
		{0x06c,0x0be,2,2,67}, {0x0f2,0x917,1,1,32},
	};
	unsigned int i,j;
	if (!p || !s || bytes < T41_YSP_PARAM_BYTES ||
	    state_bytes < T41_YSP_STATE_BYTES || gain > (16U<<16)) return -1;
	for (i=0;i<sizeof(runs)/sizeof(runs[0]);++i)
		for (j=0;j<runs[i].count;++j) {
			unsigned int value = t41_dpc_interpolate(p+runs[i].source+j*runs[i].source_width*11,
				gain,runs[i].source_width);
			unsigned char *dest = s+runs[i].target+j*runs[i].target_width;
			if (runs[i].target_width==2) t41_dpc_put16(dest,value); else *dest=value;
		}
	return 0;
}

/* Apply to the saved calibration, not repeatedly to the last scaled values. */
static inline int t41_ysp_strength(const unsigned char *original, unsigned int original_bytes,
		unsigned char *p, unsigned int bytes, unsigned int strength)
{
	unsigned int i;
	if (!original || !p || original_bytes<T41_YSP_PARAM_BYTES ||
	    bytes<T41_YSP_PARAM_BYTES || strength>255) return -1;
	for (i=0;i<32*11;++i)
		p[0x70c+i]=tx_isp_tuning_ratio_u32(strength,original[0x70c+i],255);
	return 0;
}

static inline int t41_ysp_pack_static(const unsigned char *p, unsigned int bytes,
		struct t41_dpc_word *out, unsigned int capacity)
{
	unsigned int n=0,i;
	if (!p || !out || bytes<T41_YSP_PARAM_BYTES || capacity<T41_YSP_STATIC_WRITES) return -1;
#define E(a,v) do { out[n].address=(a); out[n++].value=(v); } while (0)
#define P(o) ((unsigned int)p[o])
	E(0x13004,(P(3)<<16 & 0x70000) | (P(4)<<20 & 0x300000) | (P(2)&3));
	E(0x1300c,(P(7)<<10 & 0xc00) | (P(8)<<20 & 0x300000) | (P(6)&3));
	E(0x13018,(P(0x1a)&3) | P(0x1c)<<21 | (P(0x1b)<<11 & 0x7800));
	E(0x13028,P(0x899)<<10 | P(0x898)); E(0x13030,P(0x89b)<<10 | P(0x89a));
	E(0x13038,P(0x89d)<<10 | P(0x89c) | (P(0x1e)<<21 & 0x3e00000));
	E(0x1303c,(P(0x20)<<5 & 0x60) | (P(0x21)<<9 & 0x600) | (P(0x1f)&1) | (P(0x22)<<14 & 0xffff));
	E(0x13044,(P(0x68c)<<8 & 0x700) | (P(0x68d)<<12 & 0x7000) | (P(0x68b)&0x1f));
	E(0x13048,(P(0x68f)<<4 & 0x70) | (P(0x690)<<8 & 0x700) | (P(0x68e)&7) |
		(P(0x691)<<12 & 0x7000) | (P(0x692)<<16 & 0xf0000));
	for (i=0;i<4;++i) E(0x13084+(i/2)*16+(i%2)*4,t41_tmo_le32(p+9+i*4));
#undef P
#undef E
	return n;
}

/* Decode the piecewise-linear threshold representation. Only cumulative
 * thresholds have the OEM 0xfff saturation; the first threshold does not. */
static inline unsigned int t41_ysp_threshold(unsigned int x, unsigned int cumulative)
{
	unsigned int shift,value,reciprocal;
	if (cumulative && x>=0xfff) value=0xffff;
	else if (x<0x400) value=x;
	else {
		shift=x<0xe00 ? (x>>9)-1 : 6;
		value=(x-(shift<<9))<<shift;
	}
	reciprocal=value ? ((value>>1)+8192)/value : 8192;
	return (reciprocal<<17 & 0x7ffe0000) | (value&0xffff);
}

static inline unsigned int t41_ysp_ramp_word(const unsigned char *s, unsigned int start_index)
{
	int start=s[0], step=(int)s[1]-128, endpoint=s[2];
	unsigned int i,word=0;
	if (step<0 && endpoint>start) endpoint=start;
	if (step>=0 && endpoint<start) endpoint=start;
	for (i=0;i<4;++i) {
		int value=start+(int)(start_index+i)*step;
		if (step<0 && value<endpoint) value=endpoint;
		if (step>=0 && value>endpoint) value=endpoint;
		word|=(unsigned int)value<<(i*8);
	}
	return word;
}

static inline int t41_ysp_pack_dynamic(const unsigned char *p, unsigned int bytes,
		const unsigned char *s, unsigned int state_bytes, struct t41_dpc_word *out,
		unsigned int capacity)
{
	static const unsigned int ramp_regs[]={0x1308c,0x1309c,0x130a4};
	unsigned int n=0,i,x;
	if (!p || !s || !out || bytes<T41_YSP_PARAM_BYTES ||
	    state_bytes<T41_YSP_STATE_BYTES || capacity<T41_YSP_DYNAMIC_WRITES) return -1;
#define E(a,v) do { out[n].address=(a); out[n++].value=(v); } while (0)
#define P(o) ((unsigned int)p[o])
#define S(o) ((unsigned int)s[o])
#define H(o) t41_tmo_le16(s+(o))
	E(0x13000,(P(1)<<16 & 0x1f0000) | (S(0)&7));
	E(0x13008,(P(5)<<15 & 0xffff) | S(4)<<19 | (H(2)&0x3fff));
	E(0x13010,H(0x52)&0xfff); E(0x13014,(H(0x56)<<15 & 0x7ff8000) | (H(0x54)&0xfff));
	E(0x1301c,S(0x5a)<<11 | S(0x58));
	E(0x13020,(S(0x5b)<<10 & 0x1fc00) | (P(0x1d)<<20 & 0x300000) | (S(0x59)&0x7f));
	E(0x13024,S(0x5d)<<12 | S(0x5c) | (S(0x5e)<<22 & 0x7c00000));
	E(0x1302c,(H(0x60)&0x1ff) | S(0x62)<<12 | (S(0x63)<<22 & 0x7c00000));
	E(0x13034,(H(0x64)&0x1ff) | S(0x66)<<12 | (S(0x67)<<22 & 0x7c00000));
	E(0x13040,(H(0x10)<<11 & 0x7ff800) | (P(0x19)<<25 & 0x6000000) | S(0xe)<<1);
	E(0x1304c,t41_tmo_le32(s+0x68)&0x7f7f7f7f);
	x=H(0x6c); E(0x13050,t41_ysp_threshold(x,0));
	x+=H(0x6e); E(0x13054,t41_ysp_threshold(x,1));
	x+=H(0x70); E(0x13058,t41_ysp_threshold(x,1));
	for (i=0;i<3;++i) {
		E(ramp_regs[i],t41_ysp_ramp_word(s+5+i*3,0));
		E(ramp_regs[i]+4,t41_ysp_ramp_word(s+5+i*3,4));
	}
	for (i=0;i<32;++i) E(0x130ac+i*4,(H(0x72+i*4)&0x3ff) | (H(0x74+i*4)<<16 & 0x3ff0000));
	for (i=0;i<16;++i) E(0x1312c+i*4,(H(0x12+i*4)<<2 & 0x3ff) | (H(0x14+i*4)<<18 & 0x3ff0000));
	for (i=0;i<8;++i) E(0x1316c+i*4,t41_tmo_le32(s+0xf2+i*4)&0x7f7f7f7f);
	E(0x1318c,P(0)&0xf);
#undef H
#undef S
#undef P
#undef E
	return n;
}
#endif
