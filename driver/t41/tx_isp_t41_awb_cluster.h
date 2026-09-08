/* SPDX-License-Identifier: MIT */
#ifndef TX_ISP_T41_AWB_CLUSTER_H
#define TX_ISP_T41_AWB_CLUSTER_H

/* Included after AWB distance/math helpers. Scratch arrays are caller-owned;
 * cold initialization is zero and subsequent contents are explicit input. All
 * sums/products retain u32 wrapping; ties retain the first scanned item. */
static inline int t41_awb_clusters(const unsigned char *p,unsigned char *s,
		unsigned int rows,unsigned int cols,unsigned int fraction,unsigned int mode)
{
	unsigned char *peaks=s+0xce60,*seeds=s+0xcef0,*clusters=s+0xd1c0;
	unsigned int i,j,k,r,c,at,count=rows*cols,round=1U<<(fraction-1),capacity=mode==1 ? 60 : 12;
	unsigned int radius=t41_tmo_le32(p+0x124),merge_radius=t41_tmo_le32(p+0x128),support=t41_tmo_le32(p+0x12c);
	for (i=0;i<12;++i) {
		unsigned int x,y;
		for (j=0;j<196;++j) {
			unsigned int n=t41_tmo_le16(s+0xccd8+j*2);
			if (n && n>t41_tmo_le32(peaks+96+i*4)) {
				t41_ae_put32(peaks+i*4,j%14); t41_ae_put32(peaks+48+i*4,j/14);
				t41_ae_put32(peaks+96+i*4,n);
			}
		}
		x=t41_tmo_le32(peaks+i*4); y=t41_tmo_le32(peaks+48+i*4);
		if (x>13 || y>13) return -1;
		t41_awb_gain_put16(s+0xccd8+(y*14+x)*2,0);
	}
	for (i=0;i<12;++i) {
		unsigned int x=t41_tmo_le32(peaks+i*4),y=t41_tmo_le32(peaks+48+i*4);
		if (mode==1) {
			unsigned int low_r=t41_tmo_le32(p+0x38+x*4),high_r=t41_tmo_le32(p+0x3c+x*4);
			unsigned int low_b=t41_tmo_le32(p+0x74+y*4),high_b=t41_tmo_le32(p+0x78+y*4);
			for (j=0;j<5;++j) {
				t41_ae_put32(seeds+(i*5+j)*4,j==2 ? (low_r+1+high_r)>>1 : j<2 ? low_r : high_r);
				t41_ae_put32(seeds+240+(i*5+j)*4,j==2 ? (low_b+1+high_b)>>1 : (j==1 || j==4) ? high_b : low_b);
			}
		} else {
			unsigned int n=t41_tmo_le32(peaks+96+i*4);
			if (n) {
				t41_ae_put32(seeds+i*4,(t41_tmo_le32(s+0xc6b8+(y*14+x)*4)/n)>>fraction);
				t41_ae_put32(seeds+240+i*4,(t41_tmo_le32(s+0xc9c8+(y*14+x)*4)/n)>>fraction);
			}
		}
	}
	if (mode==1) for (i=0;i<60;++i)
		if (t41_tmo_le32(seeds+i*4) && t41_tmo_le32(seeds+240+i*4))
			for (j=i+1;j<60;++j)
				if (t41_tmo_le32(seeds+i*4)==t41_tmo_le32(seeds+j*4) &&
				    t41_tmo_le32(seeds+240+i*4)==t41_tmo_le32(seeds+240+j*4)) {
					t41_ae_put32(seeds+j*4,0); t41_ae_put32(seeds+240+j*4,0);
				}
	for (i=0;i<capacity;++i) {
		unsigned int cr=t41_tmo_le32(seeds+i*4),cb=t41_tmo_le32(seeds+240+i*4),iteration=0;
		if (!cr || !cb) continue;
		do {
			unsigned int sum_r=0,sum_b=0,n=0,next_r,next_b,dr,db;
			++iteration;
			for (j=0;j<count;++j) {
				unsigned int red=t41_tmo_le32(s+0x1c20+j*4),blue=t41_tmo_le32(s+0x1c20+(count+j)*4);
				if (t41_awb_distance((red+round)>>fraction,(blue+round)>>fraction,cr,cb)<=radius) {
					sum_r+=red; sum_b+=blue; ++n;
				}
			}
			if (!n) break;
			next_r=((sum_r+n/2)/n)>>fraction; next_b=((sum_b+n/2)/n)>>fraction;
			dr=cr>next_r ? cr-next_r : next_r-cr; db=cb>next_b ? cb-next_b : next_b-cb;
			if (mode==1 ? dr<=t41_tmo_le32(p+0x130) && db<=t41_tmo_le32(p+0x130) : !dr && !db) break;
			if (iteration>(mode==1 ? t41_tmo_le32(p+0x134) : count)) break;
			cr=next_r; cb=next_b;
			t41_ae_put32(seeds+i*4,cr); t41_ae_put32(seeds+240+i*4,cb); t41_ae_put32(seeds+480+i*4,n);
		} while (iteration<65535);
	}
	for (i=0;i<capacity;++i) {
		unsigned int cr,cb,n=0,sum_r=0,sum_b=0;
		for (j=0;j<capacity;++j) {
			unsigned int candidate=t41_tmo_le32(seeds+480+j*4);
			if (candidate && candidate>t41_tmo_le32(clusters+480+i*4))
				for (k=0;k<3;++k) t41_ae_put32(clusters+k*240+i*4,t41_tmo_le32(seeds+k*240+j*4));
		}
		if (!t41_tmo_le32(clusters+480+i*4)) break;
		cr=t41_tmo_le32(clusters+i*4); cb=t41_tmo_le32(clusters+240+i*4);
		for (j=0;j<60;++j) {
			unsigned int candidate=t41_tmo_le32(seeds+480+j*4),red=t41_tmo_le32(seeds+j*4),blue=t41_tmo_le32(seeds+240+j*4);
			if (candidate && t41_awb_distance(red,blue,cr,cb)<=merge_radius) {
				n+=candidate; sum_r+=candidate*red; sum_b+=candidate*blue; t41_ae_put32(seeds+480+j*4,0);
			}
		}
		if (n) {
			t41_ae_put32(clusters+480+i*4,0);
			t41_ae_put32(clusters+i*4,(sum_r+n/2)/n); t41_ae_put32(clusters+240+i*4,(sum_b+n/2)/n);
		}
	}
	for (i=0;i<capacity;++i) {
		unsigned int cr=t41_tmo_le32(clusters+i*4),cb=t41_tmo_le32(clusters+240+i*4),n=0,sum=0;
		if (!cr || !cb) continue;
		for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
			unsigned int pos=r*cols+c,weight=t41_tmo_le32(s+0xd904+(r*15+c)*4);
			unsigned int red=t41_tmo_le32(s+0x1c20+pos*4),blue=t41_tmo_le32(s+0x1c20+(count+pos)*4);
			if ((mode==2 || weight) && t41_awb_distance((red+round)>>fraction,(blue+round)>>fraction,cr,cb)<=support) {
				++n; sum+=weight;
			}
		}
		t41_ae_put32(clusters+480+i*4,n); t41_ae_put32(clusters+720+i*4,sum);
	}
	if (mode==1) {
		unsigned int max_weight=0,max_count=0,blend=t41_tmo_le16(p+0xd08);
		for (i=0;i<60;++i) {
			unsigned int n=t41_tmo_le32(clusters+480+i*4),sum=t41_tmo_le32(clusters+720+i*4);
			if (n) { sum=(sum+n/2)/n; t41_ae_put32(clusters+720+i*4,sum); }
			if (sum>max_weight) max_weight=sum;
		}
		for (i=0;i<60;++i) {
			unsigned int n=t41_tmo_le32(clusters+480+i*4),weighted=n;
			if (max_weight && n) weighted=(n*t41_tmo_le32(clusters+720+i*4)+max_weight/2)/max_weight;
			weighted=(blend*n+(256-blend)*weighted)>>8;
			t41_ae_put32(clusters+480+i*4,weighted);
			if (weighted>max_count) max_count=weighted;
		}
		if (max_count) for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
			unsigned int pos=r*cols+c,chosen=60,red=t41_tmo_le32(s+0x1c20+pos*4),blue=t41_tmo_le32(s+0x1c20+(count+pos)*4);
			unsigned int weight=t41_tmo_le32(s+0xd904+(r*15+c)*4);
			for (i=0;i<60;++i) {
				unsigned int cr=t41_tmo_le32(clusters+i*4),cb=t41_tmo_le32(clusters+240+i*4);
				if (cr && cb && t41_awb_distance((red+round)>>fraction,(blue+round)>>fraction,cr,cb)<=support &&
				    (chosen==60 || t41_tmo_le32(clusters+480+chosen*4)<t41_tmo_le32(clusters+480+i*4))) chosen=i;
			}
			if (chosen!=60) weight*=t41_tmo_le32(clusters+480+chosen*4);
			t41_ae_put32(s+0xd904+(r*15+c)*4,(weight+max_count/2)/max_count);
		}
	} else {
		for (j=11;j>0;--j) for (i=0;i<j;++i)
			if (t41_tmo_le32(clusters+480+i*4)<t41_tmo_le32(clusters+484+i*4))
				for (k=0;k<4;++k) {
					unsigned int value=t41_tmo_le32(clusters+k*240+i*4);
					t41_ae_put32(clusters+k*240+i*4,t41_tmo_le32(clusters+k*240+(i+1)*4));
					t41_ae_put32(clusters+k*240+(i+1)*4,value);
				}
		for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
			unsigned int pos=r*cols+c,chosen=0,red=t41_tmo_le32(s+0x1c20+pos*4),blue=t41_tmo_le32(s+0x1c20+(count+pos)*4);
			for (i=0;i<12;++i) {
				unsigned int cr=t41_tmo_le32(clusters+i*4),cb=t41_tmo_le32(clusters+240+i*4);
				if (cr && cb && t41_awb_distance((red+round)>>fraction,(blue+round)>>fraction,cr,cb)<=support &&
				    (!chosen || t41_tmo_le32(clusters+476+chosen*4)<t41_tmo_le32(clusters+480+i*4))) chosen=i+1;
			}
			at=(r*15+c)*4; t41_ae_put32(s+0xe714+at,chosen);
		}
	}
	return 0;
}

static inline int t41_awb_cluster_curve(unsigned int x,const unsigned char *knots,
		const unsigned char *values,unsigned int count,unsigned int *out)
{
	unsigned int i;
	for (i=1;i<count;++i) if (t41_tmo_le16(knots+i*2)<=t41_tmo_le16(knots+(i-1)*2)) return -1;
	if (x<t41_tmo_le16(knots)) { *out=t41_tmo_le16(values); return 0; }
	for (i=1;i<count;++i) if (x<t41_tmo_le16(knots+i*2)) {
		unsigned int low=t41_tmo_le16(knots+(i-1)*2),span=t41_tmo_le16(knots+i*2)-low;
		unsigned int a=t41_tmo_le16(values+(i-1)*2),b=t41_tmo_le16(values+i*2);
		*out=(a*span+(b-a)*(x-low))/span; return 0;
	}
	*out=t41_tmo_le16(values+(count-1)*2); return 0;
}

struct t41_awb_cluster_result { unsigned int red,blue,ct,share,count; };

/* Mode 2 blends up to six ranked illuminants into the initial estimate.
 * Original support counts intentionally retain their pre-compaction index,
 * matching the reference's separate count and compacted centroid arrays. */
static inline int t41_awb_cluster_select(const unsigned char *p,unsigned int pb,unsigned char *s,
		unsigned int rows,unsigned int cols,unsigned int fraction,unsigned short red[6],unsigned short blue[6],unsigned int out[2])
{
	unsigned int counts[6]={0},weight[6]={0},sum_r[6]={0},sum_b[6]={0},mean[6],total=0;
	struct t41_awb_cluster_result result[6]={{0}};
	unsigned int r,c,i,j,n=0,count=rows*cols,policy=t41_tmo_le32(p+0x138),pair=t41_tmo_le32(p+0x13c);
	if (policy>3 || pair>1) return -1;
	for (r=0;r<rows;++r) for (c=0;c<cols;++c) {
		unsigned int at=(r*15+c)*4,pos=r*cols+c,which=t41_tmo_le32(s+0xe714+at);
		if (which && which<=6) {
			--which; ++counts[which];
			sum_r[which]+=t41_tmo_le32(s+0x1c20+pos*4);
			sum_b[which]+=t41_tmo_le32(s+0x1c20+(count+pos)*4);
			weight[which]+=t41_awb_mul3(fraction,t41_tmo_le32(s+0xd904+at)>>8,
				(unsigned int)p[0x1200+r*15+c]<<fraction,t41_tmo_le32(s+0xe390+at)>>8);
		}
	}
	for (i=0;i<6;++i) {
		if (!counts[i]) counts[i]=1;
		mean[i]=weight[i]/counts[i]; total+=mean[i]; result[i].count=counts[i];
	}
	if (!total) return 0;
	for (i=0;i<6;++i) if (weight[i]) {
		result[n].red=sum_r[i]/counts[i]; result[n].blue=sum_b[i]/counts[i];
		result[n].share=(mean[i]<<7)/total; ++n;
	}
	for (i=0;i<6;++i) {
		/* The reference report uses fixed Q6 rounding, independently of the
		 * estimator's parameter-selected fractional precision. */
		red[i]=(result[i].red+32)>>6; blue[i]=(result[i].blue+32)>>6;
		if (t41_awb_ct_calculate(p,pb,result[i].red,result[i].blue,&result[i].ct)) return -1;
	}
	for (j=n-1;j>0;--j) for (i=0;i<j;++i) if (result[i].ct<result[i+1].ct) {
		struct t41_awb_cluster_result temp=result[i]; result[i]=result[i+1]; result[i+1]=temp;
	}
	if (pair) {
		unsigned int first=0,second=0,largest=0,next=0,factor,chosen[2]={0,0};
		for (i=0;i<n;++i) if (result[i].count>largest) { first=i; largest=result[i].count; }
		for (i=0;i<n;++i) if (i!=first && result[i].count>next) { second=i; next=result[i].count; }
		if (t41_awb_cluster_curve(largest+next,p+0x11ec,p+0x11f0,2,&factor)) return -1;
		if (policy==2) {
			if (!largest && !next) return -1;
			chosen[0]=(largest*result[first].red+next*result[second].red)/(largest+next);
			chosen[1]=(largest*result[first].blue+next*result[second].blue)/(largest+next);
		} else {
			unsigned int index=policy==1 ? first : policy==0 ? (first<second ? first : second) : (first>second ? first : second);
			chosen[0]=result[index].red; chosen[1]=result[index].blue;
		}
		for (i=0;i<2;++i) out[i]=(chosen[i]*factor+(128-factor)*out[i])>>7;
	} else {
		unsigned int sums[3]={0},count_weight[6],chosen=6,max=0;
		for (i=0;i<n;++i) {
			unsigned int ct_weight,w;
			if (t41_awb_cluster_curve(result[i].ct,p+0x11d4,p+0x11dc,4,&ct_weight) ||
			    t41_awb_cluster_curve(result[i].count,p+0x11ec,p+0x11f0,2,&count_weight[i])) return -1;
			w=(result[i].share*ct_weight*count_weight[i])>>11;
			sums[0]+=w*result[i].red; sums[1]+=w*result[i].blue; sums[2]+=w;
			if (result[i].share>=11) {
				if (!policy && chosen==6 && count_weight[i]>=11) chosen=i;
				if (policy==1 && count_weight[i]>max) { chosen=i; max=count_weight[i]; }
			}
		}
		if (chosen!=6) { out[0]=result[chosen].red; out[1]=result[chosen].blue; }
		else if ((!policy || policy==2) && sums[2]) { out[0]=sums[0]/sums[2]; out[1]=sums[1]/sums[2]; }
	}
	return 0;
}
#endif
