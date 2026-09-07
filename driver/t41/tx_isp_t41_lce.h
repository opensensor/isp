/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_LCE_H
#define TX_ISP_T41_LCE_H
#include "tx_isp_t41_dpc.h"
#define T41_LCE_PARAM_BYTES 0x15cU
#define T41_LCE_STATE_BYTES 0x5934U
#define T41_LCE_BINS 32U
#define T41_LCE_TOTAL 0x100000U

static inline void t41_lce_put32(unsigned char *p, unsigned int v)
{ p[0]=v; p[1]=v>>8; p[2]=v>>16; p[3]=v>>24; }
/* MIPS arithmetic wraps at 32 bits before signed shifts/division. */
static inline int t41_lce_mul(int a, int b)
{ return (int)((unsigned int)a*(unsigned int)b); }

/* The OEM's 1920x1080 shortcut is exactly this resolution-independent
 * construction. Four cell sizes and nine overlapping-neighborhood sizes. */
static inline int t41_lce_geometry(unsigned char *s, unsigned int bytes,
		unsigned int width, unsigned int height, struct t41_dpc_word out[4])
{
	unsigned int w=width/18*2,h=height/5,a[4],n[9],r[4],i,precision;
	if(!s || !out || bytes<T41_LCE_STATE_BYTES || width<18 || height<5 ||
	   width>65535 || height>65535) return -1;
	a[0]=(h+1)*(w+2); a[1]=(h+1)*w; a[2]=h*(w+2); a[3]=h*w;
	n[0]=2*a[0]; n[1]=a[0]+a[1]; n[2]=2*a[1]; n[3]=a[0]+a[2];
	n[4]=(a[0]+a[2]+a[1]+a[3])>>1; n[5]=a[1]+a[3];
	n[6]=2*a[2]; n[7]=a[2]+a[3]; n[8]=2*a[3];
	precision=a[3]<=0x8000 ? 3 : a[3]<=0x10000 ? 2 : a[3]<=0x20000 ? 1 : 0;
	for(i=0;i<4;++i) {
		t41_lce_put32(s+0x5784+4*i,a[i]);
		r[i]=((a[i]>>1)+(1U<<(30-precision)))/a[i];
	}
	for(i=0;i<9;++i) {
		t41_lce_put32(s+0x5794+4*i,n[i]);
		t41_lce_put32(s+0x57b8+4*i,0x40000000U/n[i]);
	}
	t41_lce_put32(s+0x5770,width*height);
	t41_lce_put32(s+0x5774,0x80000000U/((width*height)>>2));
	t41_lce_put32(s+0x5778,12);
	t41_lce_put32(s+0x577c,height%5); t41_lce_put32(s+0x5780,(width/2)%9);
	for(i=0;i<5;++i) t41_dpc_put16(s+0x590c+2*i,height*i/5);
	t41_dpc_put16(s+0x5916,height-1);
	for(i=0;i<9;++i) t41_dpc_put16(s+0x5918+2*i,width*i/9);
	t41_dpc_put16(s+0x592a,width-1);
	for(i=0;i<4;++i) out[i].address=0xe000+4*i;
	out[0].value=(h<<16 & 0x1ff0000)|(w&0x1ff);
	out[1].value=((width/2)%9 & 15)|(precision<<20)|0x10000|(height%5<<8 & 0x700);
	out[2].value=(r[1]<<16 & 0x1fff0000)|(r[0]&0x1fff);
	out[3].value=(r[3]<<16 & 0x1fff0000)|(r[2]&0x1fff);
	return 0;
}

static inline int t41_lce_calibrate(unsigned char *p, unsigned int bytes,
		unsigned char *s, unsigned int state_bytes, unsigned int gain)
{
	static const struct { unsigned short src,dst,n; } copies[]={
		{0x40,0x57dc,20},{0x54,0x5840,5},{0x90,0x584a,3},{0x93,0x584d,5},
		{0x98,0x5852,32},{0xb8,0x5872,32},{0xd8,0x5892,5},{0x114,0x589c,3},
		{0x117,0x589f,5},{0x11c,0x58a4,32},{0x13c,0x58c4,32},
		{0x30,0x58e4,5},{0x35,0x58e9,5},{0,0x57f0,16},{0x3a,0x58ee,6}};
	unsigned int i,j;
	if(!p || !s || bytes<T41_LCE_PARAM_BYTES || state_bytes<T41_LCE_STATE_BYTES) return -1;
	for(i=0;i<sizeof(copies)/sizeof(copies[0]);++i)
		for(j=0;j<copies[i].n;++j) s[copies[i].dst+j]=p[copies[i].src+j];
	if(!s[0x592c]) t41_dpc_put16(s+0x57f0,0);
	else if(!t41_tmo_le16(s+0x57f2)) {
		unsigned int v=t41_tmo_le32(s+0x58f4)>>7;
		t41_dpc_put16(s+0x57f4,v); t41_dpc_put16(p+4,v);
	}
	for(i=0;i<5;++i) {
		s[0x5845+i]=t41_dpc_interpolate(p+0x59+11*i,gain,1);
		s[0x5897+i]=t41_dpc_interpolate(p+0xdd+11*i,gain,1);
	}
	return 0;
}

static inline void t41_lce_identity(unsigned char *s)
{
	unsigned int i;
	for(i=0;i<512;++i) s[0x550c+i]=(i%16)*16;
	for(i=0;i<32;++i) t41_dpc_put16(s+0x444c+2*i,i*32);
	for(i=0;i<1024;++i) t41_dpc_put16(s+0x44cc+2*i,(i%32)*32);
}

/* Positive normalized histograms sum to 2^20. Correct smoothing's rounding
 * residue without changing zero bins when an excess must be removed. */
static inline int t41_lce_filter(const unsigned int in[32], unsigned int out[32],
		const unsigned char weights[5], unsigned int result[2])
{
	unsigned int v[32],sum=0,i,peak=0,median=0,cumulative=0,found=0;
	for(i=0;i<32;++i) {
		int j; unsigned int numerator=0,denominator=0;
		for(j=-2;j<=2;++j) if((int)i+j>=0 && (int)i+j<32) {
			numerator+=in[i+j]*weights[j+2]; denominator+=weights[j+2];
		}
		v[i]=denominator ? (numerator+(denominator>>1))/denominator : 0;
		sum+=v[i];
	}
	if(sum<T41_LCE_TOTAL) {
		unsigned int missing=T41_LCE_TOTAL-sum;
		for(i=0;i<32;++i) v[i]+=missing/32+(i<missing%32);
	} else if(sum>T41_LCE_TOTAL) {
		unsigned int excess=sum-T41_LCE_TOTAL,nonzero,step;
		for(;;) {
			nonzero=0; for(i=0;i<32;++i) nonzero+=v[i]!=0;
			if(!nonzero) return -1;
			if(excess<nonzero) break;
			step=excess/nonzero;
			for(i=0;i<32;++i) { unsigned int sub=v[i]<step ? v[i] : step; v[i]-=sub; excess-=sub; }
		}
		for(i=0;i<32 && excess;++i) if(v[i]) { --v[i]; --excess; }
	}
	for(i=0;i<32;++i) {
		out[i]=v[i]; if(v[i]>v[peak]) peak=i;
		if(!found) {
			cumulative+=v[i];
			if(cumulative>=0x80001) {
				median=i; found=1;
				if(v[i]+T41_LCE_TOTAL < cumulative*2) median=(i-1)&255;
			}
		}
	}
	result[0]=peak; result[1]=median; return 0;
}

static inline void t41_lce_head_tail(const unsigned int hist[32], unsigned int head_cut,
		unsigned int tail_cut, unsigned int result[2])
{
	unsigned int sum=0,head=0,tail=31,i;
	for(i=0;i<32;++i) { sum+=hist[i]; if(sum>(head_cut<<11)) { head=i; break; } }
	sum=0; for(i=32;i>head;) { --i; sum+=hist[i]; if(sum>(tail_cut<<11)) { tail=i; break; } }
	result[0]=head; result[1]=tail;
}

static inline int t41_lce_hist_method(const unsigned int in[32], unsigned int out[32],
		const unsigned int p[8])
{
	int weights[32],v[32],i,total_weights=0,sum=0,total=0;
	int head=p[0],center=p[1],tail=p[2],base=p[3],height=p[4],wb=p[5],wh=p[6];
	int range=tail-head+1,residue;
	if(head<0 || head>center || center>tail || tail>31) return -1;
	for(i=0;i<32;++i) {
		int ceiling,weight,limit;
		if(i<head || i>tail) { ceiling=0; weight=0; }
		else if(i==center) { ceiling=(base+height)*256; weight=wb+wh; }
		else if(i<center) { ceiling=(i*height*256)/center+base*256; weight=i*wh/center+wb; }
		else { ceiling=((31-i)*height*256)/(31-center)+base*256; weight=(31-i)*wh/(31-center)+wb; }
		weights[i]=weight; total_weights+=weight;
		limit=(t41_lce_mul((int)(in[i]-0x8000),p[7])>>7)+0x8000;
		v[i]=limit<ceiling ? limit : ceiling; total+=v[i];
	}
	residue=range*0x8000-total;
	if(residue>=0 && !total_weights) return -1;
	for(i=0;i<32;++i) {
		if(i<head || i>tail) v[i]=0x8000;
		else if(residue>=0) v[i]+=t41_lce_mul(residue,weights[i])/total_weights;
		else v[i]=(int)((unsigned int)v[i]<<11)/((total/range>>4)+1);
		sum+=v[i];
	}
	residue=0x100000-sum;
	for(i=head;i<=tail;++i) v[i]+=residue/range+(i-head<residue%range);
	for(i=0;i<32;++i) out[i]=v[i];
	return 0;
}

static inline void t41_lce_cdf(const unsigned int pdf[32], unsigned short cdf[32])
{
	unsigned int i,sum=0;
	for(i=0;i<32;++i) { if(sum>>20) sum=0xfffff; cdf[i]=sum>>10; sum+=pdf[i]; }
}

static inline void t41_lce_converge(unsigned int ratio, unsigned int minimum,
		unsigned int maximum, unsigned int count, const unsigned short *old,
		const unsigned short *target, unsigned short *out)
{
	unsigned int i;
	for(i=0;i<count;++i) {
		int a=old[i],b=target[i],distance=a>b ? a-b : b-a,step;
		if(distance<=(int)minimum) { out[i]=b; continue; }
		step=(t41_lce_mul(distance,ratio)+64)>>7;
		if(step<(int)minimum) step=minimum;
		else if(step>(int)maximum) step=maximum;
		out[i]=a+(b>a ? step : -step);
	}
}
/* Adjust the cumulative distribution toward identity independently on its
 * bright and dark sides, then smooth its PDF and enforce the Q20 sum. */
static inline int t41_lce_light_correct(const unsigned int in[32], unsigned int out[32],
		const unsigned int p[4], const unsigned char upper[32], const unsigned char lower[32])
{
	static const unsigned char weights[5]={1,2,2,2,1};
	int v[32],cdf[33],filtered[32],i,j,total=0,minimum=0;
	if((p[2]&255) || (p[3]&255)) {
		cdf[0]=0;
		for(i=0;i<32;++i) cdf[i+1]=(int)((unsigned int)cdf[i]+in[i]);
		for(i=1;i<=32;++i) {
			int value=cdf[i],identity=i*0x8000;
			if(value>identity ? (p[2]&255)!=0 : (p[3]&255)!=0) {
				unsigned int ratio=value>identity ? upper[i-1] : lower[i-1];
				cdf[i]=(int)((unsigned int)t41_lce_mul(value,ratio)+
					((128-ratio)*(unsigned int)identity))>>7;
			}
			v[i-1]=(int)((unsigned int)cdf[i]-(unsigned int)cdf[i-1]);
		}
		for(i=0;i<32;++i) {
			unsigned int numerator=0,denominator=0;
			for(j=-2;j<=2;++j) if(i+j>=0 && i+j<32) {
				numerator+=(unsigned int)v[i+j]*weights[j+2]; denominator+=weights[j+2];
			}
			filtered[i]=(int)numerator/(int)denominator;
			total+=filtered[i]; if(filtered[i]<minimum) minimum=filtered[i];
		}
		if(minimum) {
			total-=minimum*32;
			for(i=0;i<32;++i) filtered[i]-=minimum;
		}
		if(total!=0x100000) {
			int denominator=(total>>10)+1,sum=0,residue;
			if(!denominator) return -1;
			for(i=0;i<32;++i) {
				filtered[i]=(int)((unsigned int)filtered[i]<<10)/denominator;
				sum+=filtered[i];
			}
			residue=0x100000-sum;
			for(i=0;i<32;++i) filtered[i]+=(residue>>5)+(i<(residue&31));
		}
		for(i=0;i<32;++i) v[i]=filtered[i];
	} else for(i=0;i<32;++i) v[i]=in[i];
	if((p[0]&255)==1) {
		int difference=0; unsigned int maximum=0;
		for(i=0;i<32;++i) {
			unsigned int distance;
			difference+=v[i]-0x8000;
			distance=difference<0 ? -(unsigned int)difference : (unsigned int)difference;
			if(distance>maximum) maximum=distance;
		}
		if(maximum>(p[1]<<12)) {
			unsigned int ratio=((p[1]<<19)+(maximum>>1))/maximum;
			int sum=0,residue;
			for(i=0;i<32;++i) { v[i]=(t41_lce_mul(v[i]-0x8000,ratio)>>7)+0x8000; sum+=v[i]; }
			residue=0x100000-sum;
			for(i=0;i<32;++i) v[i]+=(residue>>5)+(i<(residue&31));
		}
	}
	for(i=0;i<32;++i) out[i]=v[i];
	return 0;
}

static inline int t41_lce_light_lock(const unsigned int hist[45*32],
		unsigned int remainder_y,unsigned int remainder_x,const unsigned int areas[4],
		const unsigned short p[8],unsigned short out[32])
{
	int values[45]; unsigned int y,x,i;
	for(y=0;y<5;++y) for(x=0;x<9;++x) {
		int half=areas[(y<remainder_y ? 0 : 2)+(x<remainder_x ? 0 : 1)]>>1;
		int sum=0,low=t41_lce_mul(p[3],half)>>10,middle=t41_lce_mul(p[4],half)>>10;
		int high=t41_lce_mul(p[5],half)>>10,value;
		for(i=p[2];i<32;++i) sum+=(int)hist[(y*9+x)*32+i];
		if(sum<low) value=0;
		else if(sum<middle) {
			int denominator=t41_lce_mul((int)p[4]-p[3],half)>>10;
			if(!denominator) return -1;
			value=(int)((unsigned int)(sum-low)<<10)/denominator;
		} else if(sum<high) {
			int denominator=t41_lce_mul((int)p[5]-p[4],half)>>10;
			if(!denominator) return -1;
			value=t41_lce_mul(sum-middle,p[6])/denominator+1024;
		} else value=p[6]+1024;
		values[y*9+x]=value;
	}
	for(y=0;y<4;++y) for(x=0;x<8;++x) {
		int a=values[y*9+x],b=values[y*9+x+1],c=values[(y+1)*9+x],d=values[(y+1)*9+x+1];
		if(b>a) a=b;
		if(c>a) a=c;
		if(d>a) a=d;
		out[y*8+x]=a;
	}
	return 0;
}

static inline int t41_lce_lock_hist(const unsigned int in[32], unsigned int out[32],
		unsigned int threshold,int strength)
{
	int v[32],i,tail_sum=0,new_sum=0,head_sum=0,residue;
	if(threshold>=31 || strength<=0) { for(i=0;i<32;++i) out[i]=in[i]; return 0; }
	for(i=threshold+1;i<32;++i) tail_sum+=(int)in[i];
	if((31-(int)threshold)*0x8000<tail_sum) { for(i=0;i<32;++i) out[i]=in[i]; return 0; }
	for(i=0;i<32;++i) {
		v[i]=in[i];
		if(i<=(int)threshold) continue;
		if(strength>=1024) v[i]=(t41_lce_mul(strength-1024,v[i])>>10)+0x8000;
		else if(v[i]<0x8000) v[i]=(int)((unsigned int)t41_lce_mul(1024-strength,v[i])+
			((unsigned int)strength<<15)+1023)>>10;
		new_sum+=v[i];
	}
	if(tail_sum==0x100000) return -1;
	for(i=0;i<=(int)threshold;++i) {
		int ratio=(int)((0x100000U-(unsigned int)new_sum)<<10)/(0x100000-tail_sum);
		v[i]=t41_lce_mul(ratio,v[i])>>10; head_sum+=v[i];
	}
	residue=0x100000-new_sum-head_sum;
	for(i=0;i<=(int)threshold;++i) v[i]+=residue/((int)threshold+1)+(i<residue%((int)threshold+1));
	for(i=0;i<32;++i) out[i]=v[i];
	return 0;
}

static inline int t41_lce_normalize(unsigned int hist[32],unsigned int reciprocal,unsigned int shift)
{
	unsigned int v[32],sum=0,i;
	if(shift>63) return -1;
	for(i=0;i<32;++i) { v[i]=((unsigned long long)hist[i]*reciprocal)>>shift; sum+=v[i]; }
	if(sum>T41_LCE_TOTAL) return -1; /* OEM diagnoses this and leaves input unchanged. */
	for(i=0;i<32;++i) hist[i]=v[i]+((T41_LCE_TOTAL-sum)>>5)+(i<((T41_LCE_TOTAL-sum)&31));
	return 0;
}
static inline unsigned int t41_lce_distance(const unsigned int *a,const unsigned int *b)
{
	unsigned int sum=0,i;
	for(i=0;i<32;++i) sum+=a[i]>b[i] ? a[i]-b[i] : b[i]-a[i];
	return sum>>9;
}

static inline unsigned int t41_lce_step_byte(unsigned int old,unsigned int target)
{ return (old<target ? old+1 : old>target ? old-1 : old)&255; }

/* One global or local histogram's shared contrast/light pipeline. Settings
 * come from the gain-interpolated calibration, never a captured output LUT. */
static inline int t41_lce_hist_curve(unsigned int raw[32],unsigned int filtered[32],
		const unsigned char *p,const unsigned char weights[5],unsigned int mean,
		unsigned int reset,unsigned char *head_history,unsigned char *center_history,
		unsigned char *tail_history,unsigned int *method_debug,unsigned int *light_debug,
		unsigned int *lock_debug,unsigned int lock_threshold,int lock_strength,
		unsigned short curve[32],unsigned int global)
{
	unsigned int judge[2],head=0,tail=31,center,method[8],light[4],i;
	if(t41_lce_filter(raw,filtered,weights,judge)) return -1;
	center=p[0]==0 ? judge[0] : p[0]==1 ? judge[1] : p[0]==3 ? p[1] : mean;
	if(reset<2) center=t41_lce_step_byte(*center_history,center);
	*center_history=center;
	if(p[2]) {
		t41_lce_head_tail(filtered,p[3],p[4],judge); head=judge[0]; tail=judge[1];
		if(reset<2) { head=t41_lce_step_byte(*head_history,head); tail=t41_lce_step_byte(*tail_history,tail); }
		if(head>center) head=center;
		if(tail<center) tail=center;
		*head_history=head; *tail_history=tail;
	}
	method[0]=head; method[1]=center; method[2]=tail;
	for(i=0;i<5;++i) method[i+3]=p[i+5];
	if(t41_lce_hist_method(raw,filtered,method)) return -1;
	if(method_debug) for(i=0;i<32;++i) method_debug[i]=filtered[i];
	if(global ? p[13]==1 : p[13]!=0) {
		for(i=0;i<4;++i) light[i]=p[14+i];
		if(t41_lce_light_correct(filtered,raw,light,p+18,p+50)) return -1;
		for(i=0;i<32;++i) filtered[i]=raw[i];
	}
	if(light_debug) for(i=0;i<32;++i) light_debug[i]=filtered[i];
	if(lock_strength>0) {
		if(t41_lce_lock_hist(filtered,raw,lock_threshold,lock_strength)) return -1;
		for(i=0;i<32;++i) filtered[i]=raw[i];
	}
	if(lock_debug) for(i=0;i<32;++i) lock_debug[i]=filtered[i];
	t41_lce_cdf(filtered,curve); return 0;
}

/* Aligned little-endian workspace, laid out like H20250310a for the existing
 * statistics ABI. It is caller-owned and allocated once, not on each frame.
 * 45 input cells form 32 overlapping 2x2 output neighborhoods. */
static inline int t41_lce_process(unsigned char *s,unsigned int bytes,unsigned int reset)
{
	unsigned char *work;
	unsigned int *hist,*sums,*raw,*filtered,*history;
	unsigned short global[32],local[32];
	unsigned int i,j,y,x,pixels,ry,rx,changed,sum=0,blend,wdr;
	if(!s || bytes<T41_LCE_STATE_BYTES || ((unsigned long)s&3)) return -1;
	work=s+0x1898; hist=(unsigned int *)(void *)work;
	sums=(unsigned int *)(void *)(work+0x1680); raw=(unsigned int *)(void *)(work+0x2ab4);
	filtered=(unsigned int *)(void *)(work+0x2b34); history=(unsigned int *)(void *)(work+0x2734);
	pixels=t41_tmo_le32(s+0x5770); ry=t41_tmo_le32(s+0x577c); rx=t41_tmo_le32(s+0x5780);
	if(pixels<4 || ry>4 || rx>8) return -1;
	blend=t41_tmo_le32(s+0x57dc); wdr=t41_tmo_le16(s+0x57f0)==1;
	if(wdr) {
		unsigned short *lock=(unsigned short *)(void *)(s+0x5800);
		unsigned short *old=(unsigned short *)(void *)(work+0x3c34);
		if(t41_lce_light_lock(hist,ry,rx,(unsigned int *)(void *)(s+0x5784),
			(unsigned short *)(void *)(s+0x57f0),lock)) return -1;
		for(i=0;i<32;++i) {
			int a=(short)old[i],b=(short)lock[i];
			old[i]=reset==4 ? lock[i] : a<b ? (a+4<b ? a+4 : b) : a>b ? (b+4<a ? a-4 : b) : a;
		}
	}
	for(i=0;i<32;++i) { raw[i]=0; filtered[i]=0; }
	for(i=0;i<45;++i) {
		for(j=0;j<32;++j) raw[j]+=hist[i*32+j];
		sum+=sums[i];
	}
	if(t41_lce_normalize(raw,t41_tmo_le32(s+0x5774),t41_tmo_le32(s+0x5778))) return -1;
	for(i=0;i<32;++i) ((unsigned int *)(void *)(work+0x27b4))[i]=raw[i];
	changed=!s[0x58e9] || reset || t41_lce_distance(history,raw)>s[0x58ea];
	if(changed) {
		unsigned int mean=(((pixels>>2)+sum)/(pixels>>1)>>3)&255;
		if(s[0x58e9]) for(i=0;i<32;++i) history[i]=raw[i];
		if(t41_lce_hist_curve(raw,filtered,s+0x5892,s+0x58e4,mean,reset,
			work+0x3ed4,work+0x3ed5,work+0x3ed6,
			(unsigned int *)(void *)(work+0x2834),(unsigned int *)(void *)(work+0x28b4),
			0,0,0,global,1)) return -1;
		for(i=0;i<32;++i) ((unsigned short *)(void *)(work+0x2bf4))[i]=global[i];
	}
	if(reset!=4) t41_lce_converge(s[0x589c],s[0x589d],s[0x589e],32,
		(unsigned short *)(void *)(work+0x2bb4),(unsigned short *)(void *)(work+0x2bf4),global);
	for(i=0;i<32;++i) ((unsigned short *)(void *)(work+0x2bb4))[i]=global[i];
	for(y=0;y<4;++y) for(x=0;x<8;++x) {
		unsigned int tile=y*8+x,cell=y*9+x,reciprocal;
		unsigned int ry_index=y+1<ry ? 0 : y+1==ry ? 3 : 6;
		unsigned int rx_index=x+1<rx ? 0 : x+1==rx ? 1 : 2;
		unsigned int debug=s[0x58f2]==y && s[0x58f3]==x,ratio=blend;
		int lock=wdr ? (short)((unsigned short *)(void *)(work+0x3c34))[tile] : 0;
		history=(unsigned int *)(void *)(work+0x1734+tile*128);
		reciprocal=t41_tmo_le32(s+0x57b8+4*(ry_index+rx_index));
		for(i=0;i<32;++i) raw[i]=hist[cell*32+i]+hist[(cell+1)*32+i]+hist[(cell+9)*32+i]+hist[(cell+10)*32+i];
		if(t41_lce_normalize(raw,reciprocal,10)) return -1;
		if(changed || t41_lce_distance(history,raw)>s[0x58eb]) {
			unsigned long long product=(unsigned long long)(sums[cell]+sums[cell+1]+sums[cell+9]+sums[cell+10])*reciprocal;
			unsigned int mean=(((unsigned int)(product>>30)+((unsigned int)(product>>29)&1))>>3)&255;
			for(i=0;i<32;++i) history[i]=raw[i];
			if(t41_lce_hist_curve(raw,filtered,s+0x5840,s+0x58e4,mean,reset,
				work+0x3e74+tile,work+0x3e94+tile,work+0x3eb4+tile,
				debug ? (unsigned int *)(void *)(work+0x2934) : 0,
				debug ? (unsigned int *)(void *)(work+0x29b4) : 0,
				debug ? (unsigned int *)(void *)(work+0x2a34) : 0,
				t41_tmo_le16(s+0x57f4),lock,local,0)) return -1;
			for(i=0;i<32;++i) ((unsigned short *)(void *)(work+0x3434+tile*64))[i]=local[i];
		}
		if(reset!=4) t41_lce_converge(s[0x584a],s[0x584b],s[0x584c],32,
			(unsigned short *)(void *)(work+0x2c34+tile*64),
			(unsigned short *)(void *)(work+0x3434+tile*64),local);
		for(i=0;i<32;++i) ((unsigned short *)(void *)(work+0x2c34+tile*64))[i]=local[i];
		if(lock>0) ratio=lock>1024 ? 0 : (unsigned int)t41_lce_mul(1024-lock,blend)>>10;
		for(i=0;i<16;++i) {
			unsigned int value=local[i*2]*(128-ratio)+global[i*2]*ratio;
			value=(value>>9)+((value>>8)&1);
			work[0x3c74+tile*16+i]=value>255 ? 255 : value;
		}
	}
	return 0;
}

static inline int t41_lce_pack_curve(const unsigned char *s,unsigned int bytes,
		struct t41_dpc_word *out,unsigned int capacity)
{
	unsigned int i;
	if(!s || !out || bytes<T41_LCE_STATE_BYTES || capacity<132) return -1;
	out[0].address=0xe080; out[0].value=1;
	out[1].address=0x501e0; out[1].value=0x101;
	for(i=0;i<128;++i) { out[i+2].address=0x501e4; out[i+2].value=t41_tmo_le32(s+0x550c+4*i); }
	out[130].address=0x501e0; out[130].value=0x7f0102;
	out[131].address=0xe084; out[131].value=1;
	return 132;
}
#endif
