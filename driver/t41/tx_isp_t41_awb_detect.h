/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_DETECT_H
#define TX_ISP_T41_AWB_DETECT_H
#include "tx_isp_t41_awb_gray.h"
#include "tx_isp_t41_awb_gain.h"

static inline unsigned int t41_awb_distance(unsigned int r,unsigned int b,
		unsigned int cr,unsigned int cb)
{
	unsigned int dr=r>cr ? r-cr : cr-r, db=b>cb ? b-cb : cb-b;
	return dr*dr+db*db;
}
#include "tx_isp_t41_awb_cluster.h"

/* Temperature prior: two outer limits surrounding a unity-weight plateau.
 * Knots are [inner low, outer low, inner high, outer high], not sorted. */
static inline int t41_awb_zone_ct_weight(unsigned int ct,const unsigned char *knots,
		unsigned int floor,unsigned int precision,unsigned int weight,unsigned int *out)
{
	unsigned int low=t41_tmo_le32(knots), outer_low=t41_tmo_le32(knots+4);
	unsigned int high=t41_tmo_le32(knots+8), outer_high=t41_tmo_le32(knots+12);
	unsigned int factor=floor, unity=1U<<precision, span;
	if (ct>outer_low && ct<outer_high) {
		if (ct>high) {
			span=outer_high-high;
			if (!span) return -1;
			factor=(floor*span+(outer_high-ct)*(unity-floor)+span/2)/span;
		} else if (ct<low) {
			span=low-outer_low;
			if (!span) return -1;
			factor=(floor*span+(ct-outer_low)*(unity-floor)+span/2)/span;
		} else factor=unity;
	}
	*out=t41_ae_fixed_mul(precision,weight,factor);
	return 0;
}

static inline int t41_awb_detect_fallback(unsigned char *p,unsigned int pb,
		unsigned char *s,unsigned int sb,unsigned int count,unsigned int out[2],unsigned int failed[2])
{
	unsigned int mode;
	failed[0]=failed[1]=1; out[0]=out[1]=1U<<t41_tmo_le16(p+0xcd2);
	t41_ae_put32(p+0x28,5000);
	for (mode=0; mode<2; ++mode)
		if (t41_awb_grayworld_mode(p,pb,s,sb,(const unsigned int *)(const void *)(s+0x1c20),
			4*count,mode,out,failed)) return -1;
	return 0;
}

static inline int t41_awb_detect_refine(const unsigned char *p,unsigned char *s,
		unsigned int rows,unsigned int cols,unsigned int precision,unsigned int fraction,
		unsigned int total_weight,unsigned int out[2])
{
	unsigned int r,c,count=rows*cols,sum=0,variance,lower=t41_tmo_le32(p+0xf8)<<6;
	unsigned int upper=t41_tmo_le32(p+0xf4)<<6,den=(t41_tmo_le32(p+0xf4)-t41_tmo_le32(p+0xf8))<<6;
	unsigned int sums[3]={0,0,0};
	if (!den) return -1;
	for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
		unsigned int pos=r*cols+c,at=(r*15+c)*4;
		unsigned int red=t41_tmo_le32(s+0x1c20+pos*4),blue=t41_tmo_le32(s+0x1c20+(count+pos)*4);
		unsigned int dr=red>out[0] ? red-out[0] : out[0]-red,db=blue>out[1] ? blue-out[1] : out[1]-blue;
		unsigned int dist=t41_ae_fixed_mul(precision,dr,dr)+t41_ae_fixed_mul(precision,db,db);
		unsigned int w=t41_tmo_le32(s+0xd904+at),cw=t41_tmo_le32(s+0xe390+at),spatial=p[0x1200+r*15+c];
		t41_ae_put32(s+0xdc88+at,dist);
		if (w && cw && spatial)
			sum+=t41_ae_fixed_mul(precision,t41_awb_mul3(precision,dist,w,cw),spatial<<precision);
	}
	variance=t41_ae_fixed_div(precision,sum,total_weight);
	if (!variance) return 0;
	for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
		unsigned int pos=r*cols+c,at=(r*15+c)*4;
		unsigned int norm=t41_ae_fixed_div(precision,t41_tmo_le32(s+0xdc88+at),variance);
		unsigned int weight=t41_tmo_le32(s+0xd904+at),combined;
		if (norm>upper) weight=0;
		else if (norm>=lower)
			weight=t41_ae_fixed_mul(precision,weight,(1U<<precision)-t41_ae_fixed_div(precision,norm-lower,den));
		t41_ae_put32(s+0xe00c+at,weight);
		combined=t41_awb_mul3(fraction,weight>>8,t41_tmo_le32(s+0xe390+at)>>8,
			(unsigned int)p[0x1200+r*15+c]<<fraction);
		sums[0]+=t41_ae_fixed_mul(precision,t41_tmo_le32(s+0x1c20+pos*4),combined<<8);
		sums[1]+=t41_ae_fixed_mul(precision,t41_tmo_le32(s+0x1c20+(count+pos)*4),combined<<8);
		sums[2]+=combined;
	}
	if (sums[2]) {
		out[0]=t41_ae_fixed_div(fraction,sums[0],sums[2]);
		out[1]=t41_ae_fixed_div(fraction,sums[1],sums[2]);
	}
	return 0;
}

/* Full calibrated detector; no live driver calls this boundary until its
 * runtime ownership and calibration-replacement lifecycle are checked. */
static inline int t41_awb_detect(unsigned char *p,unsigned int pb,
		unsigned char *s,unsigned int sb,unsigned short cluster_red[6],unsigned short cluster_blue[6],
		unsigned int out[2],unsigned int failed[2])
{
	unsigned int precision,fraction,rows,cols,count,i,r,c,round,max_count=0,max_sat=0,sum=0;
	unsigned int den,min_r,max_r,min_b,max_b,sums[3]={0,0,0},ct,cluster_mode,clusters=0;
	if (!p || pb<0x12e1 || !s || sb<T41_AWB_STATE_BYTES || ((unsigned long)s&3) ||
	    !cluster_red || !cluster_blue || !out || !failed) return -1;
	precision=t41_tmo_le16(p+0xcd2); fraction=t41_tmo_le16(p+0xcd4);
	rows=t41_tmo_le16(p+0xc6e); cols=t41_tmo_le16(p+0xc72); count=rows*cols;
	cluster_mode=t41_tmo_le32(p+0x120);
	if (!rows || rows>15 || !cols || cols>15 || precision>31 || !fraction || fraction>30 ||
	    cluster_mode>2 || s[0x34cc]>10 || s[0x34cd]>10 ||
	    (cluster_mode==1 && t41_tmo_le32(p+0x134)>65534) ||
	    t41_awb_ct_calculate(p,pb,0,0,&ct)) return -1;
	round=1U<<(fraction-1);
	for (i=0;i<6;++i) cluster_red[i]=cluster_blue[i]=0;
	if (t41_tmo_le16(p+0xd66)==1) return t41_awb_detect_fallback(p,pb,s,sb,count,out,failed);
	for (i=0;i<count;++i) {
		unsigned int v=t41_tmo_le32(s+0x2a30+i*4);
		if (v>max_count) max_count=v;
	}
	if (!max_count || t41_tmo_le16(p+0xd62)==2) {
		t41_ae_put32(s+0xe390,0);
		return t41_awb_detect_fallback(p,pb,s,sb,count,out,failed);
	}
	den=t41_tmo_le16(p+0xc74)*t41_tmo_le16(p+0xc92);
	if (!den) return -1;
	min_r=t41_tmo_le32(p+0x38)<<fraction; max_r=t41_tmo_le32(p+0x70)<<fraction;
	min_b=t41_tmo_le32(p+0x74)<<fraction; max_b=t41_tmo_le32(p+0xac)<<fraction;
	for (i=0;i<count;++i) {
		unsigned int red=t41_tmo_le32(s+0x1c20+i*4),blue=t41_tmo_le32(s+0x1c20+(count+i)*4);
		if (red<min_r) red=min_r;
		if (red>max_r) red=max_r;
		if (blue<min_b) blue=min_b;
		if (blue>max_b) blue=max_b;
		t41_ae_put32(s+0x1c20+i*4,red); t41_ae_put32(s+0x1c20+(count+i)*4,blue);
	}
	for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
		unsigned int pos=r*cols+c,at=(r*15+c)*4,ix=0,iy=0,bin,weight;
		unsigned int red=t41_tmo_le32(s+0x1c20+pos*4),blue=t41_tmo_le32(s+0x1c20+(count+pos)*4);
		t41_ae_put32(s+0xe390+at,(t41_tmo_le32(s+0x2a30+pos*4)<<precision)/den);
		while (ix<13 && red>=(t41_tmo_le32(p+0x3c+ix*4)<<fraction)) ++ix;
		while (iy<13 && blue>=(t41_tmo_le32(p+0x78+iy*4)<<fraction)) ++iy;
		bin=iy*14+ix;
		t41_awb_gain_put16(s+0xccd8+bin*2,t41_tmo_le16(s+0xccd8+bin*2)+1);
		t41_ae_put32(s+0xc6b8+bin*4,t41_tmo_le32(s+0xc6b8+bin*4)+red);
		t41_ae_put32(s+0xc9c8+bin*4,t41_tmo_le32(s+0xc9c8+bin*4)+blue);
		if (t41_awb_surface(p,pb,0x4ec,red,blue,&weight)) return -1;
		if (t41_tmo_le32(s+0x393c)==1) {
			/* Unlike final CT publication, the upper/upper zone prior uses
			 * an unrounded integer reciprocal at this exact endpoint. */
			if (red==max_r && blue==max_b) {
				unsigned int mired=t41_tmo_le32(p+0xbf0);
				if (!mired) return -1;
				ct=1000000U/mired;
			} else if (t41_awb_ct_calculate(p,pb,red,blue,&ct)) return -1;
			if (t41_awb_zone_ct_weight(ct,s+0x392c,t41_tmo_le32(s+0x3940),precision,weight,&weight)) return -1;
		}
		for (i=0;i<s[0x34cc]+s[0x34cd];++i) {
			unsigned int exclude=i>=s[0x34cc],j=exclude ? i-s[0x34cc] : i;
			const unsigned char *center=p+(exclude ? 0xd0a : 0xce2)+j*4;
			unsigned int dist=t41_awb_distance((red+round)>>fraction,(blue+round)>>fraction,
				t41_tmo_le16(center),t41_tmo_le16(center+2));
			unsigned int w=t41_awb_distance_weight(s+0x3520,dist);
			w=(exclude ? 256-w : w)<<fraction;
			if (exclude ? w<weight : w>weight) weight=w;
		}
		t41_ae_put32(s+0xd904+at,weight);
		if (t41_tmo_le32(s+0x3138+pos*4)>max_sat) max_sat=t41_tmo_le32(s+0x3138+pos*4);
	}
	for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
		unsigned int pos=r*cols+c,at=(r*15+c)*4,weight=t41_tmo_le32(s+0xd904+at);
		if ((t41_tmo_le16(p+0xd56)==1 || t41_tmo_le16(p+0xd56)==2) && max_sat>=2)
			weight=t41_ae_fixed_div(precision,t41_ae_fixed_mul(precision,weight,
				t41_tmo_le32(s+0x3138+pos*4)),max_sat);
		t41_ae_put32(s+0xd904+at,weight);
	}
	if (cluster_mode==1 && t41_awb_clusters(p,s,rows,cols,fraction,1)) return -1;
	for (i=0;i<60;++i) clusters+=t41_tmo_le32(s+0xd1c0+480+i*4)>=5;
	for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
		unsigned int pos=r*cols+c,at=(r*15+c)*4,weight=t41_tmo_le32(s+0xd904+at);
		unsigned int red=t41_tmo_le32(s+0x1c20+pos*4),blue=t41_tmo_le32(s+0x1c20+(count+pos)*4);
		for (i=0;i<10;++i) if (s[0x34e0+i]==2) {
			unsigned int radius=t41_tmo_le32(p+0x140+i*4);
			unsigned int dist=t41_awb_distance((red+round)>>fraction,(blue+round)>>fraction,
				t41_tmo_le16(s+0x34ea+i*2),t41_tmo_le16(s+0x34fe + i*2));
			unsigned int w=dist>=615 ? 0 : t41_awb_distance_weight(s+0x3520,dist);
			if (dist<514 && dist<radius*radius && (!cluster_mode || (cluster_mode==1 && clusters>=2))) w=256;
			w=(256-w)<<fraction;
			if (w<weight) weight=w;
		}
		t41_ae_put32(s+0xd904+at,weight); sum+=weight;
	}
	if (!sum) return t41_awb_detect_fallback(p,pb,s,sb,count,out,failed);
	for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
		unsigned int pos=r*cols+c,at=(r*15+c)*4;
		unsigned int weight=t41_awb_mul3(fraction,t41_tmo_le32(s+0xd904+at)>>8,
			t41_tmo_le32(s+0xe390+at)>>8,(unsigned int)p[0x1200+pos]<<fraction);
		sums[0]+=t41_ae_fixed_mul(precision,t41_tmo_le32(s+0x1c20+pos*4),weight<<8);
		sums[1]+=t41_ae_fixed_mul(precision,t41_tmo_le32(s+0x1c20+(count+pos)*4),weight<<8);
		sums[2]+=weight;
	}
	if (!sums[2]) return t41_awb_detect_fallback(p,pb,s,sb,count,out,failed);
	out[0]=t41_ae_fixed_div(fraction,sums[0],sums[2]); out[1]=t41_ae_fixed_div(fraction,sums[1],sums[2]);
	if (p[0xf0]==1 && t41_awb_detect_refine(p,s,rows,cols,precision,fraction,sums[2],out)) return -1;
	if (cluster_mode==2 && (t41_awb_clusters(p,s,rows,cols,fraction,2) ||
	    t41_awb_cluster_select(p,pb,s,rows,cols,fraction,cluster_red,cluster_blue,out))) return -1;
	if (t41_awb_ct_calculate(p,pb,out[0],out[1],&ct)) return -1;
	t41_ae_put32(p+0x28,ct); failed[0]=failed[1]=0;
	return 0;
}
#endif
