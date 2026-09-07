/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_MDNS_H
#define TX_ISP_T41_MDNS_H
#include "tx_isp_t41_dpc.h"
#define T41_MDNS_PARAM_BYTES 0x61aU
#define T41_MDNS_STATE_BYTES 0x8cU
#define T41_MDNS_WRITES 128U

/* Eight point ramp with a flat prefix and an explicit final endpoint. */
static inline void t41_mdns_smp(unsigned int x0, unsigned int x1,
		unsigned int y0, unsigned int y1, unsigned int values[8])
{
	unsigned int i,start=x0<7 ? x0 : 6;
	int delta=(int)y1-(int)y0,step;
	if(x1<=start) x1=start+1;
	if(x1>=8) x1=7;
	step=(delta<0 ? -delta : delta)/(int)(x1-start);
	if(delta<=0) step=-step;
	for(i=0;i<7;++i) {
		int value=(int)y0+(i>x0 ? (int)(i-x0)*step : 0);
		if(delta>0 && value>(int)y1) value=y1;
		if(delta<=0 && value<(int)y1) value=y1;
		values[i]=value;
	}
	values[7]=y1;
}

/* Difference ramp: the slope is Q13, and bin spacing truncates separately. */
static inline unsigned int t41_mdns_dif(unsigned int x0, unsigned int x1,
		unsigned int y0, unsigned int y1, unsigned int values[8])
{
	unsigned int i,start=x0==255 ? 254 : x0,spacing=(256-x0)/7;
	int delta=(int)y1-(int)y0;
	unsigned int slope;
	if(x1<=start) x1=start+1;
	slope=((unsigned int)(delta<0 ? -delta : delta)<<13)/(x1-start);
	for(i=0;i<8;++i) {
		int change=(i*spacing*slope)/8192;
		int value=(int)y0+(delta>0 ? change : -change);
		if(delta>0 && value>(int)y1) value=y1;
		if(delta<=0 && value<(int)y1) value=y1;
		values[i]=value;
	}
	return start;
}

static inline unsigned int t41_mdns_four(const unsigned int *v)
{
	return (v[0]&255) | (v[1]<<8 & 0xff00) | (v[2]<<16 & 0xff0000) | v[3]<<24;
}

/* OEM strength uses an open valid-value interval. At strength <= 128,
 * values outside (0, maximum) become zero; above 128 they stay unchanged.
 * Thus this is not the unrestricted Q7 ratio used by demosaic/SDNS. */
static inline int t41_mdns_strength(const unsigned char *original, unsigned int original_bytes,
		unsigned char *p, unsigned int bytes, unsigned int strength, unsigned int wdr)
{
	static const unsigned short offsets[]={0,0x21,0x42,0xe7,0xc6};
	static const unsigned char maxima[]={128,128,128,180,255};
	unsigned int i,j,base=wdr ? 0x39c : 0;
	if(!original || !p || bytes<T41_MDNS_PARAM_BYTES || original_bytes<base+0xf2 ||
	   strength>255 || wdr>1) return -1;
	for(i=0;i<5;++i) for(j=0;j<11;++j) {
		unsigned int v=original[base+offsets[i]+j];
		unsigned int valid=v>0 && v<maxima[i];
		p[0x27e + offsets[i]+j]=strength<=128 ? ((valid ? v : 0)*strength)>>7 :
			v+(((strength-128)*(valid ? maxima[i]-v : 0))>>7);
	}
	return 0;
}
static inline int t41_mdns_interpolate(const unsigned char *p, unsigned int bytes,
		unsigned char *s, unsigned int state_bytes, unsigned int gain)
{
	static const struct { unsigned short target,source; unsigned char count; } runs[] = {
		{0x0,0x27e,3},
		{0x3,0xa,6},
		{0x9,0x29f,3},
		{0xc,0x4c,3},
		{0xf,0x2c0,3},
		{0x12,0x6d,3},
		{0x15,0x273,1},
		{0x16,0x2e1,3},
		{0x19,0x8e,17},
		{0x2a,0x302,4},
		{0x2e,0x155,14},
		{0x3c,0x32e,10},
		{0x46,0x1ef,4},
		{0x4a,0x39c,19},
		{0x5d,0x21b,3},
		{0x60,0x46d,5},
		{0x65,0x23c,2},
		{0x67,0x4a4,15},
		{0x76,0x252,3},
		{0x79,0x549,19},
	};
	unsigned int i,j;
	if(!p || !s || bytes<T41_MDNS_PARAM_BYTES || state_bytes<T41_MDNS_STATE_BYTES ||
	   gain>(16U<<16)) return -1;
	for(i=0;i<sizeof(runs)/sizeof(runs[0]);++i)
		for(j=0;j<runs[i].count;++j)
			s[runs[i].target+j]=t41_dpc_interpolate(p+runs[i].source+j*11,gain,1);
	return 0;
}

static inline int t41_mdns_pack_enable(unsigned char *p, unsigned int bytes,
		unsigned int channel, unsigned int memopt, struct t41_dpc_word *out,
		unsigned int capacity)
{
	unsigned int base=(channel+30)<<11;
	if(!p || !out || bytes<T41_MDNS_PARAM_BYTES || channel>1 || capacity<2) return -1;
	if(memopt==1 || memopt==2) {
		p[4]&=memopt==1 ? 0xfe : 0xfc;
		p[7]&=0xfe;
	}
	out[0].address=base+0x20;
	out[0].value=((unsigned int)p[1]<<4 & 0x10) | (p[0]&1);
	out[1].address=base+0x24;
	out[1].value=0x10000000 | ((unsigned int)p[3]<<4 & 0x10) | (p[2]&1) |
		((unsigned int)p[5]<<8 & 0x100) | ((unsigned int)p[6]<<12 & 0x1000) |
		((unsigned int)p[4]<<16 & 0x30000) | ((unsigned int)p[7]<<20 & 0x300000) |
		((unsigned int)p[8]<<24 & 0x1000000);
	return 2;
}

static inline int t41_mdns_pack(const unsigned char *p, unsigned int bytes,
		const unsigned char *s, unsigned int state_bytes, unsigned int channel,
		unsigned int width, unsigned int height, struct t41_dpc_word *out,
		unsigned int capacity)
{
	unsigned int n=0,i,j,base=(channel+30)<<11,values[8],cut,word;
	int trim_x,trim_y,inner_x,inner_y,lo0,hi0,lo1,hi1;
	if(!p || !s || !out || bytes<T41_MDNS_PARAM_BYTES || state_bytes<T41_MDNS_STATE_BYTES ||
	   channel>1 || !width || !height || width>65535 || height>65535 ||
	   capacity<T41_MDNS_WRITES || !s[4]) return -1;
	lo0=s[0x84]<129 ? s[0x84] : 128; hi0=s[0x85];
	lo1=s[0x86]<129 ? s[0x86] : 128; hi1=s[0x87];
	if(hi0-lo0<8) hi0+=8;
	if(hi1-lo1<8) hi1+=8;
	/* Reject OEM divide-by-zero cases before producing any transaction. */
	if(hi0==lo0 || hi1==lo1) return -1;
	trim_x=s[5]*s[4]+width%s[4]; trim_y=s[6]*s[4]+height%s[4];
	inner_x=(int)width-trim_x; inner_y=(int)height-trim_y;
#define E(a,v) do { out[n].address=base+(a); out[n++].value=(v); } while (0)
#define S(o) ((unsigned int)s[o])
#define P(o) ((unsigned int)p[o])
#define DIF(reg,field,extra,copies) do { \
	cut=t41_mdns_dif(S(field),S((field)+1),S((field)+2),S((field)+3),values); \
	for(j=0;j<(copies);++j) { \
		E((reg)+j*12,cut<<8 | (extra)); \
		E((reg)+j*12+4,t41_mdns_four(values)); \
		E((reg)+j*12+8,t41_mdns_four(values+4)); \
	} \
} while (0)
#define SMP(reg,field,copies) do { \
	t41_mdns_smp(S(field),S((field)+1),S((field)+2),S((field)+3),values); \
	for(j=0;j<(copies);++j) { \
		E((reg)+j*8,t41_mdns_four(values)); E((reg)+j*8+4,t41_mdns_four(values+4)); \
	} \
} while (0)
	E(0x68,height*((width+15)/16)); E(0x6c,(height*((width+31)/32))>>1);
	E(0x100,S(4)); E(0x104,((unsigned int)(trim_y/2)<<8 & 0xffff) | ((unsigned int)(trim_x/2)&255));
	E(0x108,((unsigned int)inner_x&0xffff) | (unsigned int)inner_y<<16);
	E(0x10c,((unsigned int)(inner_x/(int)s[4])&0xffff) | (unsigned int)(inner_y/(int)s[4])<<16);
	E(0x110,(S(7)<<8 & 0x300) | (S(8)<<16 & 0x30000) | (((S(4)+0x800)/(S(4)*2))&255));
	E(0x114,S(1)<<8 | S(3)<<24 | S(0) | (S(2)<<16 & 0xf0000) | (S(0x15)<<23 & 0x800000));
	E(0x118,S(0xa)<<8 | S(0xd)<<24 | S(9) | (S(0xb)<<16 & 0xf0000) | (S(0xc)<<20 & 0x700000) | (S(0xe)<<23 & 0x800000));
	E(0x11c,S(0x10)<<8 | S(0x13)<<24 | S(0xf) | (S(0x11)<<16 & 0xf0000) | (S(0x12)<<20 & 0x700000) | (S(0x14)<<23 & 0x800000));
	E(0x120,S(0x1b)<<16 | S(0x1a));
	DIF(0x124,0x1c,0,1); DIF(0x130,0x20,0,1);
	E(0x13c,(S(0x18)<<16 & 0xf0000) | (S(0x19)<<20 & 0x700000) | S(0x17)<<8 | S(0x16));
	E(0x150,S(0x42)<<8 | P(0x149)<<16 | S(0x41) | P(0x14a)<<24);
	for(i=0;i<6;++i) E(0x154+i*4,t41_tmo_le32(p+0x149+(i%2)*4));
	E(0x16c,t41_tmo_le32(p+0x151));
	DIF(0x170,0x2a,0,1); DIF(0x17c,0x30,S(0x2f)<<24 | (S(0x2e)<<16 & 0x30000),1);
	DIF(0x188,0x34,0,1);
	E(0x194,t41_tmo_le32(s+0x38));
	E(0x198,S(0x3d)<<8 | S(0x3e)<<16 | S(0x3c) | S(0x40)<<28 | (S(0x3f)<<24 & 0xf000000));
	E(0x1b0,S(0x4a)<<8 | S(0x50)<<16); E(0x1b4,t41_tmo_le32(s+0x4b));
	E(0x1b8,S(0x4f)*0x01010101U);
	E(0x1bc,((P(0x14a)+P(0x14b))>>1)<<8 | ((P(0x14b)+P(0x14c))>>1)<<16 |
		0x14 | ((P(0x14c)+P(0x14d))>>1)<<24);
	E(0x1c0,((P(0x14e)+P(0x14f))>>1)<<8 | ((P(0x14f)+P(0x150))>>1)<<16 | ((P(0x14d)+P(0x14e))>>1));
	E(0x1c4,(S(0x25)<<8 & 0x300) | (S(0x24)&3));
	SMP(0x1c8,0x26,2); DIF(0x1d8,0x46,0,2);
	E(0x1f0,(S(0x45)<<8 & 0x100) | S(0x44)<<16 | S(0x44) | (S(0x45)<<24 & 0x1000000));
	E(0x1f4,t41_tmo_le32(s+0x51)); E(0x1f8,t41_tmo_le32(s+0x55));
	SMP(0x1fc,0x59,2); E(0x20c,S(0x43));
	E(0x210,S(0x5e)<<8 | S(0x5f)<<16 | S(0x5d)); SMP(0x214,0x60,1);
	E(0x230,(S(0x66)&15) | S(0x64)<<8 | (S(0x67)<<16 & 0xf0000) | (S(0x65)<<20 & 0x700000));
	E(0x234,S(0x69)<<8 | P(0x149)<<16 | S(0x68) | P(0x14a)<<24);
	E(0x238,(S(0x6f)&3) | S(0x6f)<<8 | S(0x70)<<16);
	word=S(0x6f)*0x01010101U; E(0x23c,word); E(0x240,word);
	word=S(0x71)*0x01010101U; E(0x244,word); E(0x248,word);
	DIF(0x24c,0x6b,0,2); SMP(0x264,0x72,2);
	E(0x274,S(0x6a)); E(0x278,S(0x77)<<8 | S(0x78)<<16 | S(0x76)); SMP(0x27c,0x79,1);
	E(0x2a0,S(0x7e)<<8 | S(0x7f)<<16 | S(0x7d)); E(0x2a4,t41_tmo_le32(s+0x80));
	E(0x2a8,((unsigned int)(1024/(hi0-lo0))<<16 & 0xff0000) | ((unsigned int)hi0<<8 & 0xffff) | (unsigned int)lo0);
	E(0x2ac,((unsigned int)(1024/(hi1-lo1))<<16 & 0xff0000) | ((unsigned int)hi1<<8 & 0xffff) | (unsigned int)lo1);
	E(0x2b0,t41_tmo_le32(s+0x88)); E(0x0c,(P(9)&63)|64);
#undef SMP
#undef DIF
#undef P
#undef S
#undef E
	return n;
}
#endif
