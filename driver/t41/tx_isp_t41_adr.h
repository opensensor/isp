/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_ADR_H
#define TX_ISP_T41_ADR_H
#include "tx_isp_t41_dpc.h"
#include "tx_isp_t41_ae.h"
#include "../include/tx_isp/tx_isp_math.h"
#define T41_ADR_PARAM_BYTES 0xa60U
#define T41_ADR_STATE_BYTES 0x320U
#define T41_ADR_STATS_BYTES 0x2748U
#define T41_ADR_WORK_BYTES 0x474U

static inline void t41_adr_put16(unsigned char *p,unsigned int v);

/* Global dark-range metering: round(255*exp(-i*i/(2*50*50))). */
static inline unsigned int t41_adr_dark_weight(unsigned int i)
{
	unsigned int x,term=1U<<30,value=term,n;
	if(i>=128) return 0;
	/* Taylor at x/16, then four squarings. Unlike the exposure LUT's
	 * piecewise-linear exponent, this preserves half-integer rounding. */
	x=t41_tmo_div(((unsigned long long)i*i<<30)+40000,80000);
	for(n=1;n<=8;++n) {
		term=t41_tmo_div((unsigned long long)term*x,(unsigned long long)n<<30);
		if(n&1) value-=term; else value+=term;
	}
	for(n=0;n<4;++n) value=((unsigned long long)value*value+(1ULL<<29))>>30;
	return ((unsigned long long)value*255+(1ULL<<29))>>30;
}

/* Higher-precision logarithm for generating geometric thresholds. This is
 * not a replacement for the deliberately quantized ISP exposure log ABI. */
static inline unsigned int t41_adr_log2_q24(unsigned int value)
{
	unsigned int top=0,n=value,fraction=0,i;
	if(!value) return 0;
	while(n>>1) { n>>=1; ++top; }
	n=value<<(31-top);
	for(i=0;i<24;++i) {
		unsigned long long square=(unsigned long long)n*n;
		fraction<<=1;
		if(square & (1ULL<<63)) { ++fraction; n=square>>32; }
		else n=square>>31;
	}
	return (top<<24)|fraction;
}

/* Rounded inverse-Gaussian radii: 8192*ln(31/(i+1/2)), i=0..30.
 * Generates all 31 OEM thresholds without a captured constant table. */
static inline unsigned int t41_adr_radial_threshold(unsigned int i)
{
	unsigned int log;
	if(i>=31) return 0;
	log=t41_adr_log2_q24(62)-t41_adr_log2_q24(2*i+1);
	return ((unsigned long long)log*744261118U+(1ULL<<40))>>41;
}

/* 256*(x/8)^(power/10), rounded. Hardware's twelve abscissae are
 * 0..8,16,32,64. Generate its power-law banks, not per-sensor LUTs. */
static inline unsigned int t41_adr_power_knot(unsigned int x,unsigned int power)
{
	int log,whole; unsigned int fraction,term=1U<<30,value=term,n,shift;
	if(!x || power>18) return 0;
	log=(int)t41_adr_log2_q24(x)-(3<<24);
	log=(int)((long long)log*power/10);
	whole=log/(1<<24); if(log<0 && (log&0xffffff)) --whole;
	fraction=((unsigned long long)(log-whole*(1<<24))*744261118U)>>24;
	for(n=1;n<=12;++n) {
		term=t41_tmo_div((unsigned long long)term*fraction,(unsigned long long)n<<30);
		value+=term;
	}
	shift=22-whole;
	return ((unsigned long long)value+(1ULL<<(shift-1)))>>shift;
}

static inline int t41_adr_hardware_parameters(unsigned char *p,unsigned int bytes)
{
	unsigned int i,j; int mode,step;
	if(!p || bytes<T41_ADR_PARAM_BYTES) return -1;
	mode=(short)t41_tmo_le16(p+0xa36); step=mode==2 ? 4 : mode==1 ? 3 : 2;
	t41_adr_put16(p+0x128,640/step);
	for(i=1;i<6;++i) t41_adr_put16(p+0x128+i*2,(1024*(10+((int)i-3)*step)+5)/10);
	for(j=0;j<5;++j) for(i=0;i<12;++i)
		t41_adr_put16(p+0x19e + j*24+i*2,t41_adr_power_knot(i<=8 ? i : 8U<<(i-8),10+((int)j-2)*step));
	for(i=0;i<2;++i) {
		int value=(short)t41_tmo_le16(p+0xa2e + i*2);
		if(value<0) value=0; else if(value>128) value=128;
		t41_adr_put16(p+0xa2e + i*2,value); t41_adr_put16(p+0x134+i*2,value);
	}
	t41_adr_put16(p+0x138,t41_tmo_le16(p+0xa32)); t41_adr_put16(p+0x13a,t41_tmo_le16(p+0xa34));
	for(i=0;i<18;++i) {
		int value=1024;
		if((short)t41_tmo_le16(p+0xa38)==1) {
			int first=(short)t41_tmo_le16(p+0xa3a);
			if((int)i<=first) value=(short)t41_tmo_le16(p+0xa42);
			else {
				int slope=0,dx=0,dy=0,distance=1;
				/* A negative first knot would read before the output array. */
				if(!i) return -1;
				for(j=1;j<4;++j) if((int)i<=(short)t41_tmo_le16(p+0xa3a+j*2)) {
					int previous=(short)t41_tmo_le16(p+0xa38+j*2);
					dx=(short)(t41_tmo_le16(p+0xa3a+j*2)-previous);
					dy=(short)(t41_tmo_le16(p+0xa42+j*2)-t41_tmo_le16(p+0xa40+j*2));
					distance=(short)(i-previous); if(dx) slope=(short)(dy/dx);
					break;
				}
				value=t41_tmo_le16(p+0x214+i*2)+slope+((short)(dy-slope*dx)>=distance);
			}
		}
		t41_adr_put16(p+0x216+i*2,value);
	}
	return 0;
}

static inline unsigned int t41_adr_pair(const unsigned char *p,unsigned int mask)
{ return (t41_tmo_le16(p)&mask)|((t41_tmo_le16(p+2)&mask)<<16); }

static inline int t41_adr_pack_geometry(const unsigned char *p,unsigned int bytes,
		unsigned int channel,struct t41_dpc_word *out,unsigned int capacity)
{
	unsigned int i,base=(channel+18)<<11;
	if(!p || !out || bytes<T41_ADR_PARAM_BYTES || channel>1 || capacity<7) return -1;
	for(i=0;i<7;++i) out[i].address=base+0x30+i*4;
	out[0].value=t41_adr_pair(p+0x13c,0x1fff);
	out[1].value=t41_adr_pair(p+0x140,0x1fff);
	out[2].value=t41_tmo_le16(p+0x144)&0x1fff;
	for(i=0;i<3;++i) out[i+3].value=t41_adr_pair(p+0x146+4*i,0x1fff);
	out[6].value=t41_tmo_le16(p+0x152)&0x1fff;
	return 7;
}

static inline int t41_adr_pack_spatial(const unsigned char *p,unsigned int bytes,
		unsigned int channel,struct t41_dpc_word *out,unsigned int capacity)
{
	unsigned int i,base=(channel+18)<<11;
	if(!p || !out || bytes<T41_ADR_PARAM_BYTES || channel>1 || capacity<56) return -1;
	for(i=0;i<40;++i) {
		out[i].address=base+0x4c+4*i;
		out[i].value=t41_tmo_le32(p+0x27f+4*i)&0x1f1f1f1f;
	}
	for(i=0;i<15;++i) { out[40+i].address=base+0x49c+4*i; out[40+i].value=t41_tmo_le32(p+0x23a+4*i); }
	out[55].address=base+0x4d8; out[55].value=t41_tmo_le16(p+0x276);
	return 56;
}

static inline int t41_adr_pack_controls(const unsigned char *p,unsigned int bytes,
		unsigned int channel,struct t41_dpc_word *out,unsigned int capacity)
{
	unsigned int base=(channel+18)<<11,n=0,i,flags=0;
	if(!p || !out || bytes<T41_ADR_PARAM_BYTES || channel>1 || capacity<61) return -1;
#define ADR_EMIT(a,v) do { out[n].address=(a); out[n++].value=(v); } while(0)
	ADR_EMIT(base+0x14,t41_tmo_le16(p+0x128));
	ADR_EMIT(base+0x4dc,t41_adr_pair(p+0x12a,0xfff));
	ADR_EMIT(base+0x4e0,t41_adr_pair(p+0x12e,0xfff));
	ADR_EMIT(base+0x1c,p[0x132]);
	ADR_EMIT(base+0x18,p[0x134]);
	ADR_EMIT(base+0x1c,p[0x136]);
	ADR_EMIT(base+0x10,t41_adr_pair(p+0x138,0xfff));
	for(i=0;i<7;++i) flags|=(p[0x278+i]&1)<<(i*4);
	ADR_EMIT(base+8,flags);
	ADR_EMIT(base+0x20,(t41_tmo_le16(p+0xa0e)&1)|((t41_tmo_le16(p+0xa10)&1)<<1));
	ADR_EMIT(base+0x24,t41_adr_pair(p+0xa12,0xfff));
	ADR_EMIT(base+0x28,t41_tmo_le16(p+0xa16)&1);
	ADR_EMIT(base+0x4e8,t41_adr_pair(p+0xa18,0xfff));
	ADR_EMIT(base+0x4ec,t41_adr_pair(p+0xa1c,0xfff));
	ADR_EMIT(base+0x4f0,p[0xa20]|((unsigned int)p[0xa22]<<16));
	ADR_EMIT(base+0x4f4,p[0xa24]|((unsigned int)p[0xa26]<<16));
	ADR_EMIT(base+0x4f8,t41_adr_pair(p+0xa28,15));
	ADR_EMIT(base+0x4fc,t41_tmo_le16(p+0xa2c)&15);
	for(i=0;i<30;++i) ADR_EMIT(base+0x3f4+4*i,t41_adr_pair(p+0x19e + 4*i,0x3fff));
	ADR_EMIT(base+0x46c,t41_adr_pair(p+0xa04,0xfff));
	ADR_EMIT(base+0x470,t41_adr_pair(p+0xa08,0xfff));
	ADR_EMIT(base+0x474,t41_tmo_le16(p+0xa0c)&0xfff);
	for(i=0;i<9;++i) ADR_EMIT(base+0x478+4*i,t41_adr_pair(p+0x216+4*i,0xfff));
	/* These two registers are shared, unlike the channel-relative controls. */
	ADR_EMIT(0x9550,(t41_tmo_le16(p+0xa4a)&3)|((t41_tmo_le16(p+0xa4c)&0xfff)<<16));
	ADR_EMIT(0x9554,t41_adr_pair(p+0xa4e,0xfff));
#undef ADR_EMIT
	return n;
}

/* Busy is sampled before starting this complete write-only RAM transaction.
 * There is no direct register-window alias for these 194 payload words. */
static inline int t41_adr_pack_curve(const unsigned char *p,unsigned int bytes,
		const unsigned char *s,unsigned int state_bytes,unsigned int busy,
		struct t41_dpc_word *out,unsigned int capacity)
{
	unsigned int i,n=3;
	if(!p || !s || !out || bytes<T41_ADR_PARAM_BYTES || state_bytes<T41_ADR_STATE_BYTES || capacity<199) return -1;
	if(busy) return 0;
	out[0].address=0x50400; out[0].value=0xec;
	out[1].address=0x50404; out[1].value=0xc20000;
	out[2].address=0x50300; out[2].value=0x101;
	for(i=0;i<8;++i) out[n++].value=t41_adr_pair(s+0x44+4*i,0xfff);
	for(i=0;i<10;++i) out[n++].value=t41_adr_pair(p+0x174+4*i,0xfff);
	out[n++].value=t41_tmo_le16(p+0x19c)&0xfff;
	for(i=0;i<7;++i) out[n++].value=t41_adr_pair(s+0x64+4*i,0xfff);
	for(i=0;i<168;++i) out[n++].value=t41_adr_pair(s+0x80+4*i,0xfff);
	for(i=3;i<n;++i) out[i].address=0x50304;
	out[n].address=0x50300; out[n++].value=0x102;
	out[n].address=0x50408; out[n++].value=1;
	return n;
}
static inline void t41_adr_put16(unsigned char *p,unsigned int v)
{ p[0]=v; p[1]=v>>8; }

static inline void t41_adr_put32(unsigned char *p,unsigned int v)
{ p[0]=v; p[1]=v>>8; p[2]=v>>16; p[3]=v>>24; }

static inline unsigned int t41_adr_bits(const unsigned char *p,unsigned int bit,unsigned int width)
{
	unsigned int word=bit/32,shift=bit%32,value=t41_tmo_le32(p+word*4)>>shift;
	if(shift+width>32) value|=t41_tmo_le32(p+(word+1)*4)<<(32-shift);
	return value&((1U<<width)-1);
}

/* DMA is row-major; software neighborhoods are column-major. Twenty local
 * bins are packed at 18 bits, and global triples use 63 of each 64 bits. */
static inline int t41_adr_unpack(const unsigned char *dma,unsigned int dma_bytes,
		unsigned char *s,unsigned int bytes)
{
	unsigned int row,col,i;
	if(!dma || !s || dma_bytes<0xaa0 || bytes<T41_ADR_STATS_BYTES) return -1;
	for(row=0;row<4;++row) for(col=0;col<6;++col) {
		const unsigned char *record=dma+(row*6+col)*56;
		unsigned int cell=col*4+row;
		for(i=0;i<20;++i) t41_adr_put32(s+0x800+cell*80+4*i,t41_adr_bits(record+8,i*18,18));
		t41_adr_put32(s+0x1580+cell*4,t41_adr_bits(record+8,360,12));
		t41_adr_put32(s+0x15e0+cell*4,t41_adr_bits(record+8,372,12));
		t41_adr_put32(s+0x14c0+cell*4,t41_tmo_le32(record+56)&0x1fffffff);
		t41_adr_put32(s+0x1520+cell*4,t41_tmo_le32(record+60)&0x1fffffff);
	}
	for(i=0;i<512;++i) t41_adr_put32(s+i*4,t41_adr_bits(dma+0x548+(i/3)*8,(i%3)*21,21));
	return 0;
}

static inline int t41_adr_statistics(unsigned char *s,unsigned int bytes,
		unsigned int width,unsigned int height)
{
	static const unsigned char first[14]={0,1,2,4,6,8,9,10,12,13,15,16,18,19};
	static const unsigned char count[14]={1,1,2,2,2,1,1,2,1,2,1,2,1,1};
	unsigned int i,j,total,area,sum1=0,sum2=0,cumulative=0;
	if(!s || bytes<T41_ADR_STATS_BYTES || !width || !height ||
	   width>65535 || height>65535 || width>0x7fffffffU/height) return -1;
	total=width*height/4; if(total<5120) total=5120;
	area=total/24;
	for(i=0;i<24;++i) {
		unsigned int a=t41_tmo_le32(s+0x14c0+i*4),b=t41_tmo_le32(s+0x1520+i*4);
		sum1+=a; sum2+=b;
		t41_adr_put32(s+0x2680+i*4,t41_tmo_le32(s+0x1580+i*4));
		t41_adr_put32(s+0x26e0+i*4,t41_tmo_le32(s+0x15e0+i*4));
		t41_adr_put32(s+0x25c0+i*4,(int)a/(int)area);
		t41_adr_put32(s+0x2620+i*4,(int)b/(int)area);
	}
	t41_adr_put32(s+0x2740,(int)sum1/(int)total);
	t41_adr_put32(s+0x2744,(int)sum2/(int)total);
	for(i=0;i<512;++i) {
		int normalized;
		cumulative+=t41_tmo_le32(s+i*4);
		if((int)cumulative>=(int)total) cumulative=total;
		normalized=(int)(cumulative<<8)/(int)(total>>8);
		normalized=(int)((unsigned int)normalized*10000U)/65536;
		t41_adr_put32(s+0x1640+i*4,normalized);
	}
	for(i=0;i<24;++i) for(j=0;j<14;++j) {
		unsigned int value=t41_tmo_le32(s+0x800+i*80+first[j]*4);
		if(count[j]==2) value+=t41_tmo_le32(s+0x800+i*80+(first[j]+1)*4);
		t41_adr_put32(s+0xf80+i*56+j*4,(int)(value*10000U)/(int)area);
	}
	return 0;
}

/* Quantized Gaussian distance class. Hardware stores signed 16-bit limits;
 * the sub-eight-pixel distances are deliberately discarded before squaring. */
static inline unsigned int t41_adr_distance(int y,int x,int cy,int cx,
		const unsigned char *limits)
{
	unsigned int dy=(y>=cy ? y-cy : cy-y)>>3;
	unsigned int dx=(x>=cx ? x-cx : cx-x)>>3;
	int i,distance=(int)(dx*dx+dy*dy);
	for(i=30;i>=0;--i) if((short)t41_tmo_le16(limits+i*2)>=distance) return i+1;
	return 0;
}

/* Integrate neighboring radial classes over the central grid cell. Empty
 * classes inherit the previous class; populated ones use rounded means.
 * Scratch belongs to the initialization caller, never a frame allocation. */
static inline int t41_adr_spatial(unsigned char *p,unsigned int bytes,
		unsigned int width,unsigned int height,unsigned int scratch[8][32])
{
	unsigned int gx=(width+3)/6,gy=(height+2)/4,x,y,i,j;
	unsigned int hx=(gx+1)/2,hy=(gy+1)/2;
	static const unsigned char counts[5]={2,1,0,0,0};
	static const unsigned char sums[5]={7,6,4,3,5};
	if(!p || !scratch || bytes<T41_ADR_PARAM_BYTES || !width || !height ||
	   width>8192 || height>8192) return -1;
	for(i=0;i<8;++i) for(j=0;j<32;++j) scratch[i][j]=0;
	for(y=2*gy;y<=3*gy;++y) for(x=2*gx;x<=3*gx;++x) {
		unsigned int a=t41_adr_distance(y,x,gy+hy,gx+hx,p+0x23a);
		unsigned int b=t41_adr_distance(y,x,2*gy+hy,gx+hx,p+0x23a);
		unsigned int c=t41_adr_distance(y,x,gy+hy,2*gx+hx,p+0x23a);
		++scratch[0][a]; ++scratch[1][b]; ++scratch[2][c];
		scratch[3][a]+=t41_adr_distance(y,x,hy,hx,p+0x23a);
		scratch[4][a]+=t41_adr_distance(y,x,gy+hy,hx,p+0x23a);
		scratch[5][a]+=t41_adr_distance(y,x,hy,gx+hx,p+0x23a);
		scratch[6][b]+=t41_adr_distance(y,x,2*gy+hy,hx,p+0x23a);
		scratch[7][c]+=t41_adr_distance(y,x,hy,2*gx+hx,p+0x23a);
	}
	for(j=0;j<5;++j) for(i=0;i<32;++i) {
		unsigned int n=scratch[counts[j]][i];
		p[0x27f+j*32+i]=n ? (scratch[sums[j]][i]+n/2)/n :
			(i ? p[0x27f+j*32+i-1] : 0);
	}
	return 0;
}

static inline int t41_adr_geometry(unsigned char *p,unsigned int bytes,
		unsigned int width,unsigned int height,unsigned int scratch[8][32])
{
	unsigned int i,gx=(width/6)&~1U,gy=(height/4)&~1U;
	unsigned int hx=(gx+1)/2,hy=(gy+1)/2,radius=((hx<hy ? hx : hy)*3+1)/2;
	if(!p || !scratch || bytes<T41_ADR_PARAM_BYTES || !width || !height ||
	   width>8192 || height>8192) return -1;
	for(i=0;i<5;++i) t41_adr_put16(p+0x13c+i*2,i==4 ? height : i*gy);
	for(i=0;i<7;++i) t41_adr_put16(p+0x146+i*2,i==6 ? width : i*gx);
	for(i=0;i<31;++i) t41_adr_put16(p+0x23a+i*2,
		((unsigned long long)t41_adr_radial_threshold(i)*radius*radius+0x20000)>>18);
	return t41_adr_spatial(p,bytes,width,height,scratch);
}

/* Elliptical Gaussian metering on a six-by-four grid. The five OEM arrays
 * are arithmetic coordinate sequences, not sensor calibration. Retain its
 * Q10/Q16 divisions and quantized exponent so small weights agree exactly. */
static inline int t41_adr_gaussian(const unsigned char *p,unsigned int bytes,
		short previous[4],int weights[24])
{
	int sigma,cx,cy,mode,cache_mode,step,i,maximum=0;
	unsigned int log2e=t41_ae_fixed_div(16,0x385b0000,0x27100000);
	if(!p || !previous || !weights || bytes<T41_ADR_PARAM_BYTES) return -1;
	sigma=(short)t41_tmo_le16(p+0x9d8)+80;
	cx=(short)t41_tmo_le16(p+0x9da); cy=(short)t41_tmo_le16(p+0x9dc);
	mode=(short)t41_tmo_le16(p+0x9de);
	if(sigma<80) sigma=80; else if(sigma>1104) sigma=1104;
	if(cx<0) cx=0; else if(cx>200) cx=200;
	if(cy<0) cy=0; else if(cy>320) cy=320;
	cache_mode=mode<0 ? 0 : mode>5 ? 5 : mode;
	if(previous[0]==sigma && previous[1]==cx && previous[2]==cy && previous[3]==cache_mode) return 0;
	step=mode<=0 ? 100 : mode==2 ? 140 : mode==3 ? 160 : 120;
	for(i=0;i<24;++i) {
		int x=(i/4)*100-250-(cx-100),y=(3-2*(i%4))*step/2-(cy-step);
		unsigned int distance=x*x+y*y,exponent,exp;
		if(distance>500000) distance=500000;
		exponent=t41_ae_fixed_mul(16,
			t41_ae_fixed_div(10,distance<<10,(unsigned int)(2*sigma*sigma)<<10)<<6,log2e);
		if((int)exponent>=0xf0000) exponent=0xeffff;
		exp=tx_isp_exp2_u32(exponent,16,16);
		if(!exp) return -1;
		weights[i]=(int)t41_ae_fixed_div(16,0x27100000,exp)>>16;
	}
	for(i=9;i<=12;++i) if(weights[i]>maximum) maximum=weights[i];
	if(!maximum) return -1;
	for(i=0;i<24;++i) weights[i]=weights[i]*10000/maximum;
	previous[0]=sigma; previous[1]=cx; previous[2]=cy; previous[3]=cache_mode;
	return 1;
}

static inline unsigned long long t41_adr_le64(const unsigned char *p)
{ return t41_tmo_le32(p)|((unsigned long long)t41_tmo_le32(p+4)<<32); }

/* Signed knots, signed ordinate, wrapping 64-bit product and signed rounded
 * division. The OEM adds half the interval even on descending ordinates. */
static inline int t41_adr_interpolate64(long long x,const unsigned char *knots,
		const unsigned char *values,unsigned int count)
{
	unsigned int i;
	for(i=0;i<count;++i) {
		long long high=(long long)t41_adr_le64(knots+i*8);
		int b=(short)t41_tmo_le16(values+i*2),a;
		unsigned long long interval,numerator,magnitude,quotient;
		long long low;
		if(x>high) continue;
		if(!i) return b;
		low=(long long)t41_adr_le64(knots+(i-1)*8);
		if(high==low) return b;
		a=(short)t41_tmo_le16(values+(i-1)*2);
		interval=(unsigned long long)high-(unsigned long long)low;
		numerator=((unsigned long long)x-(unsigned long long)low)*(unsigned long long)(long long)(b-a)+
			(unsigned long long)((long long)interval>>1);
		magnitude=(long long)numerator<0 ? 0ULL-numerator : numerator;
		quotient=t41_tmo_div(magnitude,(long long)interval<0 ? 0ULL-interval : interval);
		return ((long long)numerator<0)^((long long)interval<0) ?
			(int)((unsigned int)a-(unsigned int)quotient) : (int)((unsigned int)a+(unsigned int)quotient);
	}
	return (short)t41_tmo_le16(values+(count-1)*2);
}

static inline int t41_adr_ev(unsigned char *p,unsigned int bytes,
		unsigned char *work,unsigned int work_bytes,long long exposure,int changed)
{
	static const unsigned short special[8]={0x378,0x390,0x576,0x718,0x72e,0x868,0x87e,0x8fc};
	unsigned int i,j,mode;
	if(!p || !work || bytes<T41_ADR_PARAM_BYTES || work_bytes<T41_ADR_WORK_BYTES) return -1;
	if(!changed) return 0;
	for(i=1;i<11;++i) if((long long)t41_adr_le64(p+0x320+i*8)<
		(long long)t41_adr_le64(p+0x320+(i-1)*8)) return -1;
	for(i=0;i<16;++i) p[0x5a8+i]=0;
	mode=t41_tmo_le16(p+0x894);
	if(mode==2) for(i=0;i<8;++i) for(j=0;j<22;++j) work[0x18+i*22+j]=p[special[i]+j];
	for(i=0;i<4;++i) t41_adr_put16(p+0x3a8+i*2,mode && mode!=2 ?
		(int)t41_tmo_le16(p+0x896+i*2) : t41_adr_interpolate64(exposure,p+0x320,p+0x3d4+i*22,11));
	if(mode==2) for(i=0;i<8;++i) t41_adr_put16(p+0x5a8+i*2,
		t41_adr_interpolate64(exposure,p+0x320,work+0x18+i*22,11));
	for(i=0;i<2;++i) t41_adr_put16(p+0x3b0+i*2,t41_tmo_le16(p+0x9a4) ?
		(int)t41_tmo_le16(p+0x9a6+i*2) : t41_adr_interpolate64(exposure,p+0x320,p+0x6ec+i*22,11));
	for(i=0;i<13;++i) t41_adr_put16(p+(i<11 ? 0x3be + i*2 : 0x3b6+(i-11)*2),
		t41_tmo_le16(p+0x9d6) ? (int)t41_tmo_le16(p+0x9b8+i*2) :
		t41_adr_interpolate64(exposure,p+0x320,p+0x42c+i*22,11));
	t41_adr_put16(p+0x3ba,0); t41_adr_put16(p+0x3bc,0);
	for(i=0;i<14;++i) t41_adr_put16(p+0x58c+i*2,t41_tmo_le16(p+0x762) ?
		(int)t41_tmo_le16(p+0x746+i*2) : t41_adr_interpolate64(exposure,p+0x320,p+0x5b8+i*22,11));
	return 1;
}

static inline int t41_adr_interpolate16(int x,const unsigned char *knots,
		const unsigned char *values,unsigned int count)
{
	unsigned int i;
	for(i=0;i<count;++i) {
		int high=(short)t41_tmo_le16(knots+i*2),b=(short)t41_tmo_le16(values+i*2),low,a,span,numerator;
		if(x>high) continue;
		if(!i) return b;
		low=(short)t41_tmo_le16(knots+(i-1)*2);
		if(high==low) return b;
		a=(short)t41_tmo_le16(values+(i-1)*2); span=high-low;
		numerator=(int)((unsigned int)(b-a)*(unsigned int)(x-low)+(unsigned int)(span>>1));
		return (short)(numerator/span+a);
	}
	return (short)t41_tmo_le16(values+(count-1)*2);
}

/* The curve resampler differs from the scalar interpolator: differences
 * narrow to signed 16 bits and the output is capped at the right ordinate. */
static inline int t41_adr_resample16(const unsigned char *knots,const unsigned char *values,
		const unsigned char *queries,unsigned char *out,unsigned int count,unsigned int samples)
{
	unsigned int i,j;
	if(!knots || !values || !queries || !out || !count || count>512 || samples>512) return -1;
	for(j=0;j<samples;++j) {
		int x=(short)t41_tmo_le16(queries+j*2),value;
		if(x<=(short)t41_tmo_le16(knots)) value=(short)t41_tmo_le16(values);
		else {
			for(i=1;i<count && x>=(short)t41_tmo_le16(knots+i*2);++i) { }
			value=(short)t41_tmo_le16(values+(count-1)*2);
			if(i<count) {
				int a=(short)t41_tmo_le16(values+(i-1)*2),b=(short)t41_tmo_le16(values+i*2);
				int low=(short)t41_tmo_le16(knots+(i-1)*2);
				int span=(short)(t41_tmo_le16(knots+i*2)-low);
				int dx=(short)(x-low),dy=(short)(b-a);
				if(!span) return -1;
				value=(short)((dy*dx+span/2)/span+a);
				if(value>b) value=b;
			}
		}
		t41_adr_put16(out+j*2,value);
	}
	return 0;
}

static inline int t41_adr_gamma_lookup(const unsigned char *knots,const unsigned char *values,int x)
{
	unsigned int i;
	for(i=0;i<129;++i) {
		int high=(short)t41_tmo_le16(knots+i*2),span,b,a,n;
		if(x>=high) continue;
		if(!i) return 0;
		span=high-(short)t41_tmo_le16(knots+(i-1)*2);
		b=(short)t41_tmo_le16(values+i*2); a=(short)t41_tmo_le16(values+(i-1)*2);
		n=(int)((unsigned int)(b-a)*(unsigned int)(high-x)+(unsigned int)(span/2));
		return b-n/span;
	}
	return (short)t41_tmo_le16(values+256);
}

/* Some scalar ADR paths pass a wrapped negative exponent through the OEM
 * signed clamp. Preserve its MIPS variable-shift semantics locally without
 * weakening the checked exponent ABI used for sensor gain allocation. */
static inline unsigned int t41_adr_exp2(unsigned int value)
{ return tx_isp_exp2_u32(value&0xffff,16,30)>>((14-(value>>16))&31); }

static inline int t41_adr_local_strength(int strength,int pivot,int sigma_dark,int sigma_bright,int luma,int *out)
{
	unsigned int distance,denominator,exponent,value,gain=(unsigned int)strength<<16;
	int dark,sigma;
	if(!out || strength<0 || strength>10000 || pivot<0 || pivot>4095 ||
	   sigma_dark<=0 || sigma_dark>1448 || sigma_bright<=0 || sigma_bright>1448 || luma>65535) return -1;
	if(luma<1) luma=1;
	dark=pivot>=luma; sigma=dark ? sigma_dark : sigma_bright;
	distance=(unsigned int)(luma-pivot)*(unsigned int)(luma-pivot);
	if((int)distance>=0x3ffffd) distance=0x3ffffc;
	denominator=(unsigned int)(2*sigma*sigma)<<10;
	if(!denominator) return -1;
	exponent=t41_ae_fixed_mul(16,t41_ae_fixed_div(10,distance<<10,denominator)<<6,
		t41_ae_fixed_div(16,0x385b0000,0x27100000));
	if((int)exponent>=0xf0000) exponent=0xeffff;
	value=t41_adr_exp2(exponent); if(!value) return -1;
	value=t41_ae_fixed_div(16,gain,value);
	value=dark ? (unsigned int)(((int)(gain-value)>>16)+strength) : (unsigned int)((int)value>>16);
	if((int)value>=10001) value=10000;
	*out=(int)(t41_ae_fixed_div(10,value<<10,0x19000)+0x200)>>10;
	return 0;
}

static inline int t41_adr_gamma_fixed(const unsigned char *knots,const unsigned char *values,
		int x,unsigned int precision,unsigned int rounding)
{
	unsigned int i;
	for(i=0;i<129;++i) {
		int high=(short)t41_tmo_le16(knots+i*2),low,a,b;
		unsigned int slope,offset;
		if(x>=high) continue;
		if(!i) return 0;
		low=(short)t41_tmo_le16(knots+(i-1)*2);
		a=(short)t41_tmo_le16(values+(i-1)*2); b=(short)t41_tmo_le16(values+i*2);
		slope=t41_ae_fixed_div(precision,(unsigned int)(b-a)<<precision,(unsigned int)(high-low)<<precision);
		offset=t41_ae_fixed_mul(precision,slope,(unsigned int)(high-x)<<precision);
		return b-((int)(offset+rounding)>>precision);
	}
	return (short)t41_tmo_le16(values+256);
}

static inline int t41_adr_subsection_map(int target,int base,int strength,
		const unsigned char *gamma_x,const unsigned char *gamma_y,const int cdf[512],
		unsigned int scale,unsigned int blend_precision,unsigned int curve_precision,int allow_darkening)
{
	int best=10000,sum=0,matches=0,i,x,y,n;
	unsigned int rounding=1U<<(curve_precision-1),average;
	for(i=0;i<512;++i) {
		int difference=target>=cdf[i] ? target-cdf[i] : cdf[i]-target;
		if(difference<best) { best=difference; sum=0; matches=0; }
		if(difference==best) { sum+=i; ++matches; }
	}
	if(matches<1) matches=1;
	if(sum<1) sum=1;
	average=t41_ae_fixed_div(curve_precision,(unsigned int)sum<<curve_precision,(unsigned int)matches<<curve_precision);
	x=((int)(average+rounding)>>curve_precision)*scale-1;
	y=t41_adr_gamma_fixed(gamma_x,gamma_y,x,curve_precision,rounding);
	n=base*100;
	if(allow_darkening==1 || base>=y) n=(int)((unsigned int)n+(unsigned int)(y-base)*(unsigned int)strength);
	return (int)((1U<<(blend_precision-1))+
		t41_ae_fixed_div(blend_precision,(unsigned int)n<<blend_precision,100U<<blend_precision))>>blend_precision;
}

/* Adaptive dark-side quantiles: recursively split the global CDF below its
 * midpoint, then invert the gamma. The extended form adds two finer shadow
 * segments; the last three segments keep strict unit spacing near midpoint. */
static inline int t41_adr_subsections(int *out,unsigned int capacity,int strength,
		const unsigned char *gamma_x,const unsigned char *gamma_y,const int cdf[512],
		unsigned int scale,unsigned int precision,unsigned int curve_precision,int allow_darkening,int extended)
{
	static const unsigned char normal_dest[4]={4,2,1,3},extended_dest[6]={6,4,2,5,1,3};
	const unsigned char *dest=extended ? extended_dest : normal_dest;
	int x[6],y[6],i,count=extended ? 6 : 4,center=extended ? 6 : 4;
	unsigned int index,other;
	if(!out || !gamma_x || !gamma_y || !cdf || capacity<(extended ? 11U : 9U) ||
	   !scale || scale>4096 || precision<1 || precision>16 || curve_precision<1 || curve_precision>16) return -1;
	for(i=0;i<count;++i) {
		int target=5000,base=(int)(t41_ae_fixed_div(precision,4095U<<precision,2U<<precision)+512)>>precision;
		if(i) {
			int a=i==1 ? 0 : i==2 ? 1 : i==3 ? 0 : i==4 ? 2 : 1;
			int b=i==3 ? 1 : i==5 ? 2 : -1;
			index=(int)(t41_ae_fixed_div(precision,(unsigned int)x[a]<<precision,scale<<precision)+512)>>precision;
			if(index>=512) return -2;
			target=cdf[index]; base=y[a];
			if(b>=0) {
				other=(int)(t41_ae_fixed_div(precision,(unsigned int)x[b]<<precision,scale<<precision)+512)>>precision;
				if(other>=512) return -2;
				target+=cdf[other]; base+=y[b];
			}
			target/=2;
			base=(int)(t41_ae_fixed_div(precision,(unsigned int)base<<precision,2U<<precision)+512)>>precision;
		}
		y[i]=t41_adr_subsection_map(target,base,strength,gamma_x,gamma_y,cdf,scale,precision,curve_precision,allow_darkening);
		x[i]=t41_adr_gamma_fixed(gamma_y,gamma_x,y[i],precision,512);
		out[dest[i]]=x[i];
	}
	out[0]=0; out[center+4]=4095;
	if(out[center]+3>=4095) out[center]=4091;
	for(i=1;i<=3;++i) out[center+i]=out[center]+i;
	return 0;
}

/* Universal curvature transfer, not a sensor/bin output. The caller supplies
 * the ISP's existing signed 256-entry curvature coordinate LUT; this helper
 * evaluates the rational fourth-order curve and samples the hardware knots. */
static inline int t41_adr_map_curve(int out[14],int curvature,int knee,
		const unsigned char *coordinate_lut)
{
	unsigned int i; int s,c[33];
	static const unsigned char sample[8]={2,3,4,6,8,12,16,24};
	if(!out || !coordinate_lut) return -1;
	if(curvature<0) curvature=0; else if(curvature>255) curvature=255;
	if(knee<0) knee=0; else if(knee>255) knee=255;
	if(!knee) return -1; /* OEM has a zero denominator; reject unsafe tuning. */
	s=(int)t41_tmo_le32(coordinate_lut+curvature*4);
	for(i=0;i<33;++i) {
		int x=(s<0 ? 32-i : i)*8,shape=s<0 ? -s : s;
		long long u=256-(x+knee),cube=(long long)knee*knee*knee;
		unsigned long long numerator=(unsigned long long)(u*u*u*(256-knee)+cube*knee)*
			(unsigned int)((shape+256)*x);
		unsigned long long denominator=(unsigned long long)cube*(unsigned int)(x+shape);
		unsigned long long y;
		if(!denominator) return -1;
		y=t41_tmo_div(numerator,denominator); if(y>65536) y=65536;
		c[i]=s<0 ? 65536-(int)y : (int)y;
	}
	out[0]=(int)((unsigned int)(15*c[0]+c[1])*4095U)/0xffff0;
	out[1]=(int)((unsigned int)(7*c[0]+c[1])*4095U)/0x7fff8;
	out[2]=(int)((unsigned int)(3*c[0]+c[1])*4095U)/0x3fffc;
	out[3]=(int)((unsigned int)(c[0]+c[1])*4095U)/0x1fffe;
	out[4]=c[1]*4095/65535;
	out[5]=(int)((unsigned int)(c[1]+c[2])*4095U)/0x1fffe;
	for(i=0;i<8;++i) out[i+6]=c[sample[i]]*4095/65535;
	return 0;
}

static inline int t41_adr_smooth_slopes(const unsigned char *knots,int *curve,unsigned int count,
		int iterations,int normalized_segments,int narrow)
{
	int slopes[32],pass; unsigned int i;
	unsigned long long original_sum=0;
	if(!knots || !curve || count<2 || count>33 || iterations>16) return -1;
	for(i=0;i+1<count;++i) {
		int dx=(short)t41_tmo_le16(knots+(i+1)*2)-(short)t41_tmo_le16(knots+i*2);
		int dy=curve[i+1]-curve[i];
		if(dx<=0) return -1;
		slopes[i]=(int)((unsigned int)dy*10000U+(unsigned int)(dx/2))/dx;
		original_sum+=(long long)slopes[i];
	}
	for(pass=0;pass<iterations;++pass) {
		unsigned long long sum=(long long)slopes[0];
		for(i=1;i+2<count;++i) {
			slopes[i]=(int)((unsigned int)slopes[i-1]+(unsigned int)slopes[i]+(unsigned int)slopes[i+1]+1)/3;
			sum+=(long long)slopes[i];
		}
		sum+=(long long)slopes[count-2];
		if(!sum) return -1;
		for(i=0;i+1<count;++i) {
			int x=(short)t41_tmo_le16(knots+i*2),dx=(short)t41_tmo_le16(knots+(i+1)*2)-x,value;
			if((int)i<normalized_segments) slopes[i]=(int)t41_tmo_div(original_sum*(unsigned long long)(long long)slopes[i]+(sum>>1),sum);
			else {
				int remaining=4095-x;
				if(!remaining) return -1;
				slopes[i]=(int)((unsigned int)(4095-curve[i])*10000U+(unsigned int)(remaining/2))/remaining;
			}
			value=(int)((unsigned int)slopes[i]*(unsigned int)dx+5000U)/10000+curve[i];
			if(narrow) value=(short)value;
			curve[i+1]=value>=4096 ? 4095 : value;
		}
	}
	return 0;
}

static inline int t41_adr_filter(unsigned char knots[32],int curve[14],int iterations,int normalized_segments,int spatial)
{
	int values[33],result; unsigned char grid[66],input[32],output[66]; unsigned int i;
	if(!knots || !curve || iterations>16) return -1;
	t41_adr_put16(knots+30,4095);
	if(!spatial) {
		values[0]=0; values[15]=4095;
		for(i=0;i<14;++i) values[i+1]=curve[i];
		result=t41_adr_smooth_slopes(knots,values,16,iterations,normalized_segments,0);
		if(result) return result;
		for(i=0;i<14;++i) curve[i]=values[i+1];
		return 0;
	}
	for(i=0;i<33;++i) t41_adr_put16(grid+i*2,i==32 ? 4095 : i*128);
	t41_adr_put16(input,0); t41_adr_put16(input+30,4095);
	for(i=0;i<14;++i) t41_adr_put16(input+(i+1)*2,curve[i]);
	if(t41_adr_resample16(knots,input,grid,output,16,33)) return -1;
	for(i=0;i<33;++i) values[i]=(short)t41_tmo_le16(output+i*2);
	result=t41_adr_smooth_slopes(grid,values,33,iterations,32,1);
	if(result) return result;
	for(i=0;i<33;++i) t41_adr_put16(output+i*2,values[i]);
	if(t41_adr_resample16(grid,output,knots,input,33,16)) return -1;
	for(i=0;i<14;++i) curve[i]=(short)t41_tmo_le16(input+(i+1)*2);
	return 0;
}

struct t41_adr_context {
	unsigned char knees[32],gamma_x[258],face_flags[24],face_curve[672];
	unsigned char ctc_axis[46],ctc_grid[26],ctc_sample[3][26];
	unsigned int ctc_changed;
	unsigned int stage,tile;
	unsigned int dark_weights[128],dark_weight_sum,face_count,face_enabled;
	short gaussian_previous[4];
	int gaussian[24],cut[11];
	/* Reused frame scratch; the worker owns this context exclusively. */
	int strengths[24],a[14],b[14],c[14],rational[14],selected[14];
	unsigned char x[34],y[34],sample[32],queries[14];
};

static inline void t41_adr_context_init(struct t41_adr_context *ctx,unsigned char *state,unsigned char *work)
{
	static const unsigned short knees[16]={0,8,16,32,64,128,192,256,384,512,768,1024,1536,2048,3072,4096};
	static const unsigned short ctc_axis[23]={0,8,16,32,64,96,128,256,384,512,640,768,896,1024,1280,1536,1792,2048,2304,2560,3072,3584,4096};
	static const unsigned short ctc_grid[13]={0,32,64,128,256,512,1024,1536,2048,2560,3072,3584,4095};
	unsigned int i,j;
	ctx->dark_weight_sum=0; ctx->face_count=0; ctx->face_enabled=0; ctx->ctc_changed=1;
	for(i=0;i<23;++i) t41_adr_put16(ctx->ctc_axis+i*2,ctc_axis[i]);
	for(i=0;i<13;++i) t41_adr_put16(ctx->ctc_grid+i*2,ctc_grid[i]);
	for(i=0;i<4;++i) ctx->gaussian_previous[i]=-1;
	for(i=0;i<128;++i) { ctx->dark_weights[i]=t41_adr_dark_weight(i); ctx->dark_weight_sum+=ctx->dark_weights[i]; }
	for(i=0;i<129;++i) t41_adr_put16(ctx->gamma_x+i*2,i==128 ? 4095 : i*32);
	for(i=0;i<16;++i) { t41_adr_put16(ctx->knees+i*2,knees[i]); t41_adr_put16(state+0x44+i*2,0); }
	for(i=0;i<14;++i) t41_adr_put16(state+0x64+i*2,knees[i+1]);
	for(j=0;j<24;++j) {
		ctx->face_flags[j]=0;
		for(i=0;i<14;++i) {
			t41_adr_put16(state+0x80+j*28+i*2,knees[i+1]);
			t41_adr_put16(work+0xc8+j*28+i*2,knees[i+1]);
			t41_adr_put16(ctx->face_curve+j*28+i*2,knees[i+1]);
		}
	}
}

/* Cold initialization and the geometry cache used by day/night refresh.
 * The calibration carries its own dimensions; regenerate only on mismatch. */
static inline int t41_adr_initialize(struct t41_adr_context *ctx,unsigned char *p,
		unsigned int bytes,unsigned char *state,unsigned char *work,
		unsigned int width,unsigned int height,const unsigned char gamma[258],unsigned int scratch[8][32])
{
	unsigned int i;
	if(!ctx || !p || !state || !work || !gamma || !scratch || bytes<T41_ADR_PARAM_BYTES ||
	   !width || !height || width>8192 || height>8192) return -1;
	for(i=0;i<T41_ADR_STATE_BYTES;++i) state[i]=0;
	for(i=0;i<T41_ADR_WORK_BYTES;++i) work[i]=0;
	t41_adr_context_init(ctx,state,work);
	for(i=0;i<258;++i) p[0x766+i]=gamma[i];
	if((short)t41_tmo_le16(p+0x74)!=(int)width || (short)t41_tmo_le16(p+0x76)!=(int)height) {
		t41_adr_put16(p+0x74,width); t41_adr_put16(p+0x76,height);
		if(t41_adr_geometry(p,bytes,width,height,scratch)) return -1;
	}
	for(i=0;i<160;++i) work[0x3d4+i]=p[0x27f+i];
	for(i=0;i<24;++i) work[i]=p[0x13c+i];
	for(i=0;i<62;++i) work[0x396+i]=p[0x23a+i];
	return 0;
}

/* Both day/night replacement and linear/WDR switching retain temporal
 * history, reset the visible tile curves and restore cached geometry only
 * when the replacement calibration has different dimensions. */
static inline int t41_adr_refresh(struct t41_adr_context *ctx,unsigned char *p,
		unsigned int bytes,unsigned char *state,const unsigned char *work,
		unsigned int width,unsigned int height,const unsigned char gamma[258])
{
	unsigned int i,j;
	if(!ctx || !p || !state || !work || !gamma || bytes<T41_ADR_PARAM_BYTES ||
	   !width || !height || width>8192 || height>8192) return -1;
	for(i=0;i<258;++i) p[0x766+i]=gamma[i];
	for(i=0;i<14;++i) {
		unsigned int identity=t41_tmo_le16(ctx->knees+2+i*2);
		t41_adr_put16(state+0x64+i*2,identity);
		for(j=0;j<24;++j) t41_adr_put16(state+0x80+j*28+i*2,identity);
	}
	if((short)t41_tmo_le16(p+0x74)!=(int)width || (short)t41_tmo_le16(p+0x76)!=(int)height) {
		t41_adr_put16(p+0x74,width); t41_adr_put16(p+0x76,height);
		for(i=0;i<160;++i) p[0x27f+i]=work[0x3d4+i];
		for(i=0;i<24;++i) p[0x13c+i]=work[i];
		for(i=0;i<62;++i) p[0x23a+i]=work[0x396+i];
	}
	return 0;
}

/* Complete frame arithmetic. The caller snapshots statistics and runs this
 * on private candidate state before committing a successful RAM transaction. */
static inline int t41_adr_frame(struct t41_adr_context *ctx,unsigned char *p,unsigned int bytes,
		unsigned char *state,unsigned int state_bytes,unsigned char *work,unsigned int work_bytes,
		const unsigned char *stats,unsigned int stats_bytes,const unsigned char *coordinate_lut)
{
	static const unsigned short local_x[17]={16,64,128,192,256,384,512,640,768,1024,1280,1536,2048,2560,3072,3584,4095};
	static const unsigned short ctc_x[9]={8,16,32,64,128,256,512,1024,2048};
	unsigned int i,j,sum=0; int low=4095,high=0,mode;
	const int *cdf;
	if(!ctx || !p || !state || !work || !stats || !coordinate_lut || bytes<T41_ADR_PARAM_BYTES ||
	   state_bytes<T41_ADR_STATE_BYTES || work_bytes<T41_ADR_WORK_BYTES || stats_bytes<T41_ADR_STATS_BYTES ||
	   ((unsigned long)stats&3) || !ctx->dark_weight_sum) return -1;
	cdf=(const int *)(const void *)(stats+0x1640);
	ctx->stage=1; ctx->tile=0;
#define ADR_P(off) ((short)t41_tmo_le16(p+(off)))
	/* Gamma-derived segment widths are refreshed each frame, exactly before
	 * the adaptive curve. The uniform 129-entry abscissa is generated once. */
	for(i=0;i<7;++i) t41_adr_put16(ctx->queries+i*2,i<5 ? i*256 : i==5 ? 1536 : 2048);
	if(t41_adr_resample16(p+0x766,ctx->gamma_x,ctx->queries,ctx->sample,129,7)) return -1;
	for(i=0;i<6;++i) t41_adr_put16(p+0x560+i*2,t41_tmo_le16(ctx->sample+(i+1)*2)-t41_tmo_le16(ctx->sample+i*2));
	t41_adr_put16(p+0x56e,ADR_P(0x560)+ADR_P(0x562));
	t41_adr_put16(p+0x570,ADR_P(0x564)+ADR_P(0x566));
	t41_adr_put16(p+0x572,ADR_P(0x568)); t41_adr_put16(p+0x574,ADR_P(0x56a));
	for(i=0;i<128;++i) {
		sum+=(unsigned int)(cdf[i]-(i ? cdf[i-1] : 0))*ctx->dark_weights[i];
		if(i==16 || i==32 || i==64 || i==96) {
			unsigned int slot=i==16 ? 0x32 : i==32 ? 0x34 : i==64 ? 0x36 : 0x38;
			t41_adr_put16(state+slot,(int)sum/(int)ctx->dark_weight_sum);
		}
	}
	t41_adr_put16(state+0x30,(int)sum/(int)ctx->dark_weight_sum);
	mode=ADR_P(0x894);
	ctx->stage=2;
	for(i=0;i<24;++i) {
		ctx->tile=i;
		int mean=(int)t41_tmo_le32(stats+0x25c0+i*4),second=(int)t41_tmo_le32(stats+0x2620+i*4),strength,debug;
		if(mean<low) low=mean;
		if(mean>high) high=mean;
		if(mode==3) strength=(int)((unsigned int)ADR_P(0x89e + ((i%4)*6+i/4)*2)*(unsigned int)ADR_P(0x3a8))/51200;
		else if(mode==2) {
			for(j=0;j<17;++j) {
				int value=j<3 ? ADR_P(0x3aa+j*2) : j<11 ? ADR_P(0x5a8+(j-3)*2) : 0;
				if(value<0) value=0; else if(value>1024) value=1024;
				t41_adr_put16(ctx->x+j*2,local_x[j]); t41_adr_put16(ctx->y+j*2,value);
			}
			strength=(int)((unsigned int)t41_adr_interpolate16((short)mean,ctx->x,ctx->y,17)*(unsigned int)ADR_P(0x3a8))/51200;
		} else if(t41_adr_local_strength(ADR_P(0x3a8),ADR_P(0x3aa),ADR_P(0x3ac),ADR_P(0x3ae),mean,&strength)) return -1;
		ctx->strengths[i]=ADR_P(0x764)==1 ? (int)((unsigned int)strength*(unsigned int)ctx->gaussian[i])/10000 : strength;
		debug=ADR_P(0x60)==2 ? strength : ADR_P(0x60)==3 ? mean : ADR_P(0x60)==4 ? second : ADR_P(0x60)==5 ? ctx->gaussian[i] : 0;
		t41_adr_put16(state+i*2,debug);
	}
	if(ADR_P(0x60)==0) { t41_adr_put16(state,t41_tmo_le32(stats+0x2740)); t41_adr_put16(state+2,t41_tmo_le32(stats+0x2744)); }
	if(ADR_P(0x60)==1) { t41_adr_put16(state,low); t41_adr_put16(state+2,high); }
	if(ADR_P(0x9e0)==1) {
		int knee=ADR_P(0x9e4)+155,strength=ADR_P(0x9e2);
		if(knee<155) knee=155; else if(knee>255) knee=255;
		if(strength<0) strength=0; else if(strength>100) strength=100;
		if(t41_adr_map_curve(ctx->rational,30,knee,coordinate_lut)) return -1;
		for(i=0;i<14;++i) { int identity=t41_tmo_le16(ctx->knees+(i+1)*2); t41_adr_put16(state+0x64+i*2,(identity*100+(ctx->rational[i]-identity)*strength+50)/100); }
	} else for(i=0;i<14;++i) t41_adr_put16(state+0x64+i*2,t41_tmo_le16(ctx->knees+(i+1)*2));
	if(ADR_P(0x9a2)==1) {
		for(i=0;i<9;++i) {
			int value=ADR_P(0x98e + i*2); if(value<0) value=0; else if(value>800) value=800;
			t41_adr_put16(ctx->x+i*2,ctc_x[i]); t41_adr_put16(ctx->y+i*2,value*1024/100);
		}
		for(i=0;i<16;++i) t41_adr_put16(state+0x44+i*2,t41_adr_interpolate16((short)t41_tmo_le16(ctx->knees+i*2),ctx->x,ctx->y,9));
	} else for(i=0;i<16;++i) t41_adr_put16(state+0x44+i*2,1024);
	if(ctx->ctc_changed) {
		ctx->stage=3;
		if(ADR_P(0x98c)==1) {
			t41_adr_put16(ctx->y,0); t41_adr_put16(ctx->y+24,4095);
			for(i=0;i<11;++i) t41_adr_put16(ctx->y+2+i*2,ADR_P(0x966+i*2));
			if(t41_adr_resample16(ctx->gamma_x,p+0x766,ctx->ctc_grid,ctx->ctc_sample[0],129,13) ||
			   t41_adr_resample16(ctx->ctc_grid,ctx->y,ctx->ctc_sample[0],ctx->ctc_sample[1],13,13) ||
			   t41_adr_resample16(p+0x766,ctx->gamma_x,ctx->ctc_sample[1],ctx->ctc_sample[2],129,13) ||
			   t41_adr_resample16(ctx->ctc_grid,ctx->ctc_sample[2],ctx->ctc_axis,work+0x368,13,23)) return -1;
			for(i=0;i<21;++i) t41_adr_put16(p+0x174+i*2,t41_tmo_le16(work+0x36a+i*2));
		} else for(i=0;i<21;++i) t41_adr_put16(p+0x174+i*2,t41_tmo_le16(ctx->ctc_axis+2+i*2));
		ctx->ctc_changed=0;
	}
	/* This base curve is invariant across tiles; only its strength blend is
	 * local. OEM recalculates the same 33 rational samples 24 times. */
	if(t41_adr_map_curve(ctx->rational,ADR_P(0x3b0),ADR_P(0x3b2),coordinate_lut)) return -1;
	for(i=0;i<24;++i) {
		int strength=ctx->strengths[i],last=0,mixture=ADR_P(0x9ec)+ADR_P(0x9ee)+ADR_P(0x9f0),result;
		ctx->stage=4; ctx->tile=i;
		result=t41_adr_subsections(ctx->cut,11,strength,ctx->gamma_x,p+0x766,cdf,8,10,16,1,1);
		if(result) return result;
		if(ctx->cut[0]<1) ctx->cut[0]=1;
		for(j=0;j<10;++j) if(ctx->cut[j+1]<=ctx->cut[j]) ctx->cut[j+1]=ctx->cut[j]+1;
		for(j=0;j<7;++j) t41_adr_put16(ctx->x+j*2,ctx->cut[j]);
		t41_adr_put16(ctx->x+14,ctx->cut[10]); t41_adr_put16(ctx->y,0); t41_adr_put16(ctx->y+14,4095);
		for(j=0;j<6;++j) {
			int scale=ADR_P(0x3be + j*2),width,difference=(short)(ctx->cut[j+1]-ctx->cut[j]);
			if(scale>250) scale=250;
			t41_adr_put16(p+0x3be + j*2,scale);
			width=(short)((ADR_P(0x560+j*2)*scale+50)/100);
			t41_adr_put16(p+0x54a+j*2,width);
			t41_adr_put16(ctx->y+(j+1)*2,t41_tmo_le16(ctx->y+j*2)+(width<difference ? difference : width));
		}
		t41_adr_put16(p+0x558,ADR_P(0x54a)+ADR_P(0x54c)); t41_adr_put16(p+0x55a,ADR_P(0x54e)+ADR_P(0x550));
		t41_adr_put16(p+0x55c,ADR_P(0x552)); t41_adr_put16(p+0x55e,ADR_P(0x554));
		if(t41_adr_resample16(ctx->x,ctx->y,ctx->knees,ctx->sample,8,16)) return -1;
		for(j=0;j<14;++j) {
			int identity=t41_tmo_le16(ctx->knees+(j+1)*2),x=t41_tmo_le16(ctx->knees+j*2),dx=identity-x;
			int mass=(int)(t41_tmo_le32(stats+0xf80+i*56+j*4)*4095U+5000U)/10000;
			int value=(ADR_P(0x58c+j*2)*dx+32)/64;
			ctx->a[j]=(short)t41_tmo_le16(ctx->sample+(j+1)*2);
			if(value>=mass) { value=mass; if(mass<dx) value=(4095-last)*dx/(4095-x); }
			last+=value; if(last>=4096) last=4095;
			value=last<identity ? identity : last;
			ctx->b[j]=(int)((unsigned int)(identity*100)+(unsigned int)(value-identity)*(unsigned int)strength+50U)/100;
			value=ctx->rational[j]<identity ? identity : ctx->rational[j];
			ctx->c[j]=(int)((unsigned int)(identity*100)+(unsigned int)(value-identity)*(unsigned int)strength+50U)/100;
			mode=ADR_P(0x9ea);
			ctx->selected[j]=mode==0 ? ctx->a[j] : mode==1 ? ctx->b[j] : mode==2 ? ctx->c[j] : mode==3 ?
				(int)((unsigned int)ADR_P(0x9ec)*(unsigned int)ctx->a[j]+(unsigned int)ADR_P(0x9ee)*(unsigned int)ctx->b[j]+
				(unsigned int)ADR_P(0x9f0)*(unsigned int)ctx->c[j])/(mixture>0 ? mixture : 1) : identity;
		}
		mode=ADR_P(0x9b6);
		ctx->stage=5;
		if(mode>=1 && mode<=3 && t41_adr_filter(ctx->knees,ctx->selected,mode==1 ? 3 : ADR_P(0x9b4),ADR_P(0x9b2),mode!=3)) return -1;
		for(j=0;j<14;++j) t41_adr_put16(state+0x80+i*28+j*2,ctx->selected[j]);
	}
	for(i=0;i<24;++i) {
		int step=ADR_P(0x9f6),distance=0;
		ctx->stage=6; ctx->tile=i;
		if(step>512) step=512;
		if(ADR_P(0x9f8)!=9 && step<1) step=1;
		for(j=0;j<14;++j) {
			int old=(short)t41_tmo_le16(work+0xc8+i*28+j*2),value=(short)t41_tmo_le16(state+0x80+i*28+j*2);
			int delta=old>value ? old-value : value-old;
			if(delta>distance) distance=delta;
		}
		for(j=0;j<14;++j) {
			int value=(short)t41_tmo_le16(state+0x80+i*28+j*2);
			if(ADR_P(0x9f4)==1) {
				if(step<distance) {
					int old=(short)t41_tmo_le16(work+0xc8+i*28+j*2),ratio=(int)((unsigned int)step<<21)/distance;
					int delta=old>=value ? old-value : value-old;
					int change=(int)((unsigned int)delta*(unsigned int)ratio+0x100000)>>21;
					value=(short)(old>=value ? old-change : old+change);
				}
				if(value>4082+(int)j) value=4082+j;
				t41_adr_put16(state+0x80+i*28+j*2,value);
			}
			t41_adr_put16(work+0xc8+i*28+j*2,value);
		}
	}
	if(ADR_P(0x8d4)==1) for(i=0;i<24;++i) {
		int last=0;
		for(j=0;j<14;++j) {
			int dx=t41_tmo_le16(ctx->knees+(j+1)*2)-t41_tmo_le16(ctx->knees+j*2),value=(short)t41_tmo_le16(state+0x80+i*28+j*2);
			int limit=(ADR_P(0x8d6+j*2)*(dx ? dx : 1)+50)/100+last;
			last=(short)(value<limit ? value : limit); t41_adr_put16(state+0x80+i*28+j*2,last);
		}
	}
	if(ADR_P(0x9fc)==1) {
		int index=ADR_P(0xa02),value,lower=ADR_P(0x9fe),upper=ADR_P(0xa00);
		if(index<0) index=0; else if(index>511) index=511;
		value=(short)cdf[index];
		if(value>=lower) for(i=0;i<24;++i) for(j=0;j<14;++j) {
			int current=(short)t41_tmo_le16(state+0x80+i*28+j*2),identity=t41_tmo_le16(ctx->knees+(j+1)*2);
			if(value<=upper) { if(upper==lower) return -1; current-=(int)((unsigned int)(current-identity)*(unsigned int)(value-lower))/(upper-lower); }
			else current=identity;
			t41_adr_put16(state+0x80+i*28+j*2,current);
		}
	}
	if(ctx->face_enabled==1) {
		if(++ctx->face_count>100000) ctx->face_count=0;
		for(i=0;i<24;++i) if(ctx->face_flags[i]==1) for(j=0;j<14;++j) {
			int value=(short)t41_tmo_le16(state+0x80+i*28+j*2),face=(short)t41_tmo_le16(ctx->face_curve+i*28+j*2);
			t41_adr_put16(state+0x80+i*28+j*2,value>face ? value : face);
		}
	}
#undef ADR_P
	ctx->stage=0;
	return 0;
}
#endif
