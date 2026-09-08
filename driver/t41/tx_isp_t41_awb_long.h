/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_LONG_H
#define TX_ISP_T41_AWB_LONG_H
#include "tx_isp_t41_awb_gain.h"
#include "tx_isp_t41_awb_stats.h"

/* H20250310a tisp_awb_long_alogrithm, ELF 0x31238. The estimator callback
 * separates the cluster detector from statistics preparation and temporal
 * policy. Buffers and the estimator view belong to the caller's private
 * working state; a nonzero return must not publish that working state. */
typedef int (*t41_awb_detector)(void *context, void **view,
		unsigned int ratios[2], unsigned int failed[2]);

static inline unsigned int t41_awb_absdiff(unsigned int a, unsigned int b)
{
	return a>b ? a-b : b-a;
}

/* MIPS subu followed by signed division: preserve subtraction wrapping,
 * but express the signed value without implementation-defined u32->s32. */
static inline unsigned int t41_awb_slew_step(unsigned int a, unsigned int b,
		unsigned int divisor)
{
	unsigned int delta=a-b;
	if (delta&0x80000000U) return 0U-((0U-delta)/divisor);
	return delta/divisor;
}

static inline int t41_awb_long_prepare(const unsigned char *p,
		unsigned char *s, unsigned char *report,
		unsigned int rows, unsigned int cols, unsigned int precision,
		unsigned int fraction)
{
	unsigned int weights[2][4], factors[2], sums[2]={0,0}, nonzero[2]={0,0};
	unsigned int selection, field, bin, r, c, override=t41_tmo_le16(p+0xd44);
	unsigned int gate=t41_tmo_le32(p), round=1U<<(fraction-1);
	for (selection=0; selection<2; ++selection) for (bin=0; bin<4; ++bin)
		weights[selection][bin]=override==bin+1 ? t41_tmo_le16(s+0x3514+bin*2) :
			t41_tmo_le16(p+0xd46+selection*8+bin*2);
	for (field=0; field<2; ++field)
		factors[field]=t41_tmo_le32(p+0x30+field*4)<<(precision-10);
	for (r=0; r<rows; ++r) for (c=0; c<cols; ++c) {
		unsigned int at=(r*15+c)*4;
		for (selection=0; selection<2; ++selection) for (field=0; field<4; ++field) {
			unsigned int sum=0, source=(selection ? 0x7f94 : 0x3944)+at;
			source+=(field==3 ? 4 : field)*T41_AWB_PLANE_BYTES;
			for (bin=0; bin<4; ++bin)
				sum+=weights[selection][bin]*t41_tmo_le32(s+source+bin*0x1194);
			t41_ae_put32(s+at+field*0x708+selection*0x384,sum>>3);
		}
		for (selection=0; selection<2; ++selection) {
			unsigned int base=at+selection*0x384;
			unsigned int green=t41_tmo_le32(s+base+0x708);
			for (field=0; field<2; ++field) {
				unsigned int raw=green ? (t41_tmo_le32(s+base+field*0xe10)<<10)/green : 0;
				unsigned int ratio=t41_ae_fixed_mul(precision,raw<<(precision-10),factors[field]);
				t41_ae_put32(s+at+0x1c20+selection*0x708+field*0x384,ratio);
				if (!selection) t41_awb_gain_put16(report+(r*15+c)*2+0x96+field*0x1c2,raw);
				else { sums[field]+=ratio; nonzero[field]+=ratio!=0; }
			}
			t41_ae_put32(s+at+0x2a30+selection*0x384,
				green ? t41_tmo_le32(s+base+0x1518) : 0);
		}
		for (field=0; field<3; ++field) {
			unsigned int count=t41_tmo_le32(s+at+0x189c);
			t41_ae_put32(s+at+0xeac0+field*0x384,
				count ? t41_tmo_le32(s+at+0x384+field*0x708)/count : 0);
		}
		/* Global mean is deliberately accumulated before the count gate. */
		for (selection=0; selection<2; ++selection)
			if (t41_tmo_le32(s+at+0x1518+selection*0x384)<=gate) {
				t41_ae_put32(s+at+0x1c20+selection*0x708,0);
				t41_ae_put32(s+at+0x1fa4+selection*0x708,0);
				t41_ae_put32(s+at+0x2a30+selection*0x384,0);
				if (!selection) {
					t41_awb_gain_put16(report+(r*15+c)*2+0x96,0);
					t41_awb_gain_put16(report+(r*15+c)*2+0x258,0);
				}
			}
	}
	for (field=0; field<2; ++field) {
		unsigned int mean=(unsigned int)((t41_tmo_div((unsigned long long)sums[field]<<precision,
			factors[field])+round)>>fraction);
		t41_awb_gain_put16(s+0xeab0+field*2,t41_tmo_le16(s+0xeaac+field*2));
		t41_awb_gain_put16(s+0xeaac+field*2,
			nonzero[field] ? (mean+nonzero[field]/2)/nonzero[field] : 256);
	}
	if (t41_tmo_le16(p+0xd56)==1 || t41_tmo_le16(p+0xd56)==2) {
		unsigned int gains[2]={1U<<precision,1U<<precision};
		/* s is explicitly required to be u32-aligned by the entry point. */
		if (t41_awb_saturation_weights(p,0xd58,(const unsigned int *)(const void *)s,
			(const unsigned int *)(const void *)(s+0x708),
			(const unsigned int *)(const void *)(s+0xe10),
			(const unsigned int *)(const void *)(s+0x1518),gains,
			(unsigned int *)(void *)(s+0x3138))) return -1;
	}
	return 0;
}

static inline int t41_awb_long_history(unsigned char *p, unsigned char *s,
		unsigned int ratios[2], unsigned int failed,
		unsigned int precision, unsigned int fraction)
{
	unsigned int ready=t41_tmo_le16(s+0xc5f8)==1, n=t41_tmo_le16(p+0xcd6);
	unsigned int field,i, target[2], old_target[2], current[2], delta;
	unsigned int active=t41_tmo_le32(p+4), counter=t41_tmo_le32(p+8);
	if (!n) n=1;
	if (n>15) n=15;
	for (field=0; field<3; ++field) {
		unsigned int value=field==2 ? t41_tmo_le32(p+0x28) : ratios[field];
		unsigned char *history=s+0xc604+field*60;
		for (i=0; i<14; ++i)
			t41_ae_put32(history+i*4,ready ? t41_tmo_le32(history+(i+1)*4) : value);
		t41_ae_put32(history+56,value);
		if (ready) {
			unsigned int sum=0, weights=0;
			for (i=15-n; i<15; ++i) {
				sum+=(i+1)*t41_tmo_le32(history+i*4); weights+=i+1;
			}
			value=(sum+weights/2)/weights;
			if (field==2) t41_ae_put32(p+0x28,value);
			else ratios[field]=value;
		}
	}
	if (!ready) {
		t41_awb_gain_put16(s+0xc5f8,1); s[0xc600]=0; n=1;
	} else if (t41_tmo_le32(s+0xc638)==t41_tmo_le32(s+0xc63c) &&
		   t41_tmo_le32(s+0xc674)==t41_tmo_le32(s+0xc678) &&
		   !(t41_tmo_le32(s+0xeaa0)&0x00ff00ffU) &&
		   t41_tmo_le32(p+0xc)==t41_tmo_le32(p+0x14) &&
		   t41_tmo_le32(p+0x10)==t41_tmo_le32(p+0x18)) {
		s[0xc600]=1; return 0;
	}
	for (field=0; field<2; ++field) {
		unsigned int inverse;
		if (!ratios[field]) return -1;
		inverse=(unsigned int)t41_tmo_div((unsigned long long)(1U<<precision)<<precision,ratios[field]);
		target[field]=(inverse+(1U<<(fraction-1)))>>fraction;
		old_target[field]=t41_tmo_le32(p+0xc+field*4);
		current[field]=t41_tmo_le32(p+0x14+field*4);
	}
	delta=t41_awb_absdiff(old_target[0],target[0])+t41_awb_absdiff(old_target[1],target[1]);
	if (delta>t41_tmo_le16(p+(active ? 0xcd8 : 0xcda)) && failed!=1) {
		for (field=0; field<2; ++field) {
			t41_ae_put32(p+0xc+field*4,target[field]);
			t41_ae_put32(p+0x14+field*4,current[field]+
				t41_awb_slew_step(target[field],active ? current[field] : old_target[field],n));
		}
		t41_ae_put32(p+4,1); t41_ae_put32(p+8,1);
	} else if (active) {
		unsigned int done=counter+1==n || n==1;
		if (!done && counter>=n) return -1;
		for (field=0; field<2; ++field)
			t41_ae_put32(p+0x14+field*4,done ? old_target[field] : current[field]+
				t41_awb_slew_step(target[field],current[field],n-counter));
		t41_ae_put32(p+4,!done); t41_ae_put32(p+8,counter+1);
	} else {
		t41_ae_put32(p+4,0); t41_ae_put32(p+8,0);
	}
	return 0;
}

static inline int t41_awb_long(unsigned char *p, unsigned int pb,
		unsigned char *s, unsigned int sb, unsigned char *report, unsigned int rb,
		void **view, t41_awb_detector detect, void *context)
{
	unsigned int rows,cols,precision,fraction,i,change;
	unsigned int ratios[2],failed[2]={0,0};
	if (!p || pb<0xd58 || !s || sb<T41_AWB_STATE_BYTES || ((unsigned long)s&3) ||
	    !report || rb<1050 || !view || !view[0x16] || !view[0x17] || !detect) return -1;
	rows=t41_tmo_le16(p+0xc6e); cols=t41_tmo_le16(p+0xc72);
	precision=t41_tmo_le16(p+0xcd2); fraction=t41_tmo_le16(p+0xcd4);
	if (!rows || rows>15 || !cols || cols>15 || precision<10 || precision>31 ||
	    !fraction || fraction>30 ||
	    !(t41_tmo_le32(p+0x30)<<(precision-10)) ||
	    !(t41_tmo_le32(p+0x34)<<(precision-10))) return -1;
	if (t41_awb_long_prepare(p,s,report,rows,cols,precision,fraction)) return -1;
	change=(t41_awb_absdiff(t41_tmo_le16(s+0xeab4),t41_tmo_le16(s+0xeaac))+
		t41_awb_absdiff(t41_tmo_le16(s+0xeab6),t41_tmo_le16(s+0xeaae)))&0xffff;
	if (t41_tmo_le16(s+0xc5f8)==1 && s[0xc600]==1 &&
	    change<t41_tmo_le16(p+0xcdc) && !(t41_tmo_le32(s+0xeaa0)&0x00ff00ffU)) return 0;
	t41_awb_gain_put16(s+0xeab4,t41_tmo_le16(s+0xeaac));
	t41_awb_gain_put16(s+0xeab6,t41_tmo_le16(s+0xeaae));
	ratios[0]=ratios[1]=1U<<precision;
	if (detect(context,view,ratios,failed)) return -1;
	for (i=0; i<6; ++i) {
		t41_awb_gain_put16(report+0x78+i*2,
			(t41_tmo_le16((const unsigned char *)view[0x16]+i*2)<<10)/t41_tmo_le32(p+0x30));
		t41_awb_gain_put16(report+0x84+i*2,
			(t41_tmo_le16((const unsigned char *)view[0x17]+i*2)<<10)/t41_tmo_le32(p+0x34));
	}
	return t41_awb_long_history(p,s,ratios,failed[0],precision,fraction);
}
#endif
