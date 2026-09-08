#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_detect.h"
#include "../driver/t41/tx_isp_t41_awb_prior.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
unsigned short oracle_cluster_red[6],oracle_cluster_blue[6];
#ifdef T41_AWB_DETECT_HOST
unsigned int oracle_bad;
#else
extern int oracle_tisp_awb_ct_detect(void **,unsigned int *,unsigned int *);
extern int oracle_tisp_awb_long_par_update(unsigned int,void **);
extern unsigned int oracle_bad;
#endif
static unsigned char p[0x1400],q[sizeof(p)],s[T41_AWB_STATE_BYTES] __attribute__((aligned(4))),t[sizeof(s)];
static unsigned short red[6],blue[6];
#ifndef T41_AWB_DETECT_HOST
static unsigned int info[7];
#endif
static uint32_t seed=253431;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static void difference(const char *kind,unsigned int frame,const unsigned char *a,const unsigned char *b,unsigned int n)
{
	unsigned int i,j=0;
	for(i=0;i<n && j<5;++i) if(a[i]!=b[i]) {
		printf("%s frame=%u +%x: %x/%x\n",kind,frame,i,a[i],b[i]); ++j;
	}
}
int main(void)
{
	unsigned int f,i,fail=0,classified=0,success[3]={0},failures=0;
	void *view[45];
#ifndef T41_AWB_DETECT_HOST
	void *oview[45];
	*(uint32_t *)(void *)(oracle_bss+0x4114)=(uintptr_t)info;
	info[0]=(uintptr_t)q; info[1]=(uintptr_t)t;
#endif
	for(f=0;f<10000;++f) {
		unsigned int fraction=1+rng()%10,precision=10+rng()%7,rows=1+rng()%15,cols=1+rng()%15,count=rows*cols;
		unsigned int output[2],expected[2],failed[2],ofailed[2],x=64,y=64;
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		for(i=0;i<sizeof(s);++i) s[i]=rng();
		if(f%20) memcpy(s+0xc6b8,t+0xc6b8,0xea98-0xc6b8);
		else memset(s+0xc6b8,0,0xea98-0xc6b8);
		for(i=0;i<2;++i) { output[i]=expected[i]=rng(); failed[i]=ofailed[i]=rng(); }
		t41_awb_gain_put16(p+0xc6e,rows); t41_awb_gain_put16(p+0xc72,cols);
		t41_awb_gain_put16(p+0xc74,16+rng()%256); t41_awb_gain_put16(p+0xc92,16+rng()%256);
		t41_awb_gain_put16(p+0xcd2,precision); t41_awb_gain_put16(p+0xcd4,fraction);
		t41_ae_put32(p+0x120,f%3); p[0xf0]=(f/3)&1;
		t41_ae_put32(p+0x124,100+rng()%2000); t41_ae_put32(p+0x128,100+rng()%2000);
		t41_ae_put32(p+0x12c,100+rng()%2000); t41_ae_put32(p+0x130,rng()%4); t41_ae_put32(p+0x134,1+rng()%10);
		t41_ae_put32(p+0x138,(f/6)%4); t41_ae_put32(p+0x13c,(f/24)%2);
		for(i=0;i<4;++i) {
			t41_awb_gain_put16(p+0x11d4+i*2,2000+i*2000); t41_awb_gain_put16(p+0x11dc+i*2,rng()%129);
		}
		t41_awb_gain_put16(p+0x11ec,10); t41_awb_gain_put16(p+0x11ee,50);
		t41_awb_gain_put16(p+0x11f0,rng()%129); t41_awb_gain_put16(p+0x11f2,rng()%129);
		t41_ae_put32(p+0xf8,rng()%100); t41_ae_put32(p+0xf4,200+rng()%2000);
		t41_ae_put32(p+0x1c,1000); t41_ae_put32(p+0x20,10000);
		t41_ae_put32(p+0xe0,f&1); t41_ae_put32(p+0xe8,(f/2)&1);
		t41_ae_put32(p+0xe4,rng()%257); t41_ae_put32(p+0xec,rng()%257);
		t41_ae_put32(s+0xea98,rng()%15000);
		for(i=0;i<3;++i) {
			t41_ae_put32(p+0xb0+i*16,3000); t41_ae_put32(p+0xb4+i*16,2000);
			t41_ae_put32(p+0xb8+i*16,7000); t41_ae_put32(p+0xbc+i*16,10000);
		}
		p[0x11f4]=rng()%11; p[0x11f5]=rng()%11;
		for(i=0;i<10;++i) {
			t41_awb_gain_put16(p+0xce2+i*4,128+rng()%256); t41_awb_gain_put16(p+0xce4+i*4,128+rng()%256);
			t41_awb_gain_put16(p+0xd0a+i*4,128+rng()%256); t41_awb_gain_put16(p+0xd0c+i*4,128+rng()%256);
			s[0x34e0+i]=rng()%4;
			t41_awb_gain_put16(s+0x34ea+i*2,128+rng()%256); t41_awb_gain_put16(s+0x34fe + i*2,128+rng()%256);
			t41_ae_put32(p+0x140+i*4,rng()%64);
		}
		for(i=0;i<15;++i) {
			x+=1+rng()%32; y+=1+rng()%32;
			t41_ae_put32(p+0x38+i*4,x); t41_ae_put32(p+0x74+i*4,y);
		}
		for(i=0;i<225;++i) {
			t41_ae_put32(p+0x4ec+i*4,f%29 ? rng()%257 : 0);
			t41_ae_put32(p+0x870+i*4,100+rng()%400);
		}
		for(i=0;i<4*count;++i) t41_ae_put32(s+0x1c20+i*4,(32+rng()%500)<<fraction);
		for(i=0;i<count;++i) {
			t41_ae_put32(s+0x2a30+i*4,f%19 ? rng()%16000 : 0);
			t41_ae_put32(s+0x3138+i*4,rng()%10000);
			p[0x1200+i]=f%23 ? rng()%8 : 0;
		}
		for(i=0;i<514;++i) t41_awb_gain_put16(s+0x3520+i*2,rng()%257);
		t41_awb_gain_put16(p+0xd56,f%4); t41_awb_gain_put16(p+0xd60,f&1);
		t41_awb_gain_put16(p+0xd62,(f/2)%3); t41_awb_gain_put16(p+0xd64,(f/6)&1);
		t41_awb_gain_put16(p+0xd66,f%13==0);
		memcpy(q,p,sizeof(p)); memcpy(t,s,sizeof(s));
		if(t41_awb_prior_prepare(p,sizeof(p),s,sizeof(s),view,red,blue)) return 2;
#ifndef T41_AWB_DETECT_HOST
		oracle_tisp_awb_long_par_update(0,oview);
#else
		memcpy(q,p,sizeof(p));
#endif
		if(t41_awb_detect(p,sizeof(p),s,sizeof(s),red,blue,output,failed)) { printf("rejected frame=%u\n",f); return 3; }
#ifndef T41_AWB_DETECT_HOST
		oracle_tisp_awb_ct_detect(oview,expected,ofailed);
#else
		/* The sanitizer build has no OEM instructions. Check unchanged
		 * calibration outside published CT and preserve native warm state. */
		if(memcmp(p,q,0x28) || memcmp(p+0x2c,q+0x2c,sizeof(p)-0x2c)) {
			difference("unexpected calibration write",f,p,q,sizeof(p)); return 4;
		}
		memcpy(q,p,sizeof(p)); memcpy(t,s,sizeof(s)); memcpy(expected,output,sizeof(output));
		memcpy(ofailed,failed,sizeof(failed)); memcpy(oracle_cluster_red,red,sizeof(red)); memcpy(oracle_cluster_blue,blue,sizeof(blue));
#endif
		classified+=red[0]!=0; failures+=failed[0]!=0; success[f%3]+=failed[0]==0;
		if(memcmp(p,q,sizeof(p)) || memcmp(s,t,sizeof(s)) || memcmp(output,expected,sizeof(output)) ||
		   memcmp(failed,ofailed,sizeof(failed)) || memcmp(red,oracle_cluster_red,sizeof(red)) || memcmp(blue,oracle_cluster_blue,sizeof(blue))) {
			if(fail++<10) {
				difference("parameters",f,p,q,sizeof(p)); difference("state",f,s,t,sizeof(s));
				printf("frame=%u result=%u/%u OEM=%u/%u failed=%u/%u OEM=%u/%u\n",f,output[0],output[1],expected[0],expected[1],failed[0],failed[1],ofailed[0],ofailed[1]);
			}
		}
	}
#ifdef T41_AWB_DETECT_HOST
	{
		unsigned int out[2]={123,456},failed[2]={789,1011},value;
		unsigned char lut[1030];
		memset(lut,0xb7,sizeof(lut));
		assert(t41_awb_distance_lut_init(lut,1027)==-1 && lut[0]==0xb7);
		assert(!t41_awb_distance_lut_init(lut,1028) && lut[1028]==0xb7 && lut[1029]==0xb7);
		assert(t41_tmo_le16(lut)==256 && t41_tmo_le16(lut+28*2)==256);
		assert(t41_tmo_le16(lut+29*2)==254 && t41_tmo_le16(lut+513*2)==6);
		assert(t41_awb_distance_weight(lut,514)==5 && t41_awb_distance_weight(lut,614)==3);
		assert(t41_awb_distance_weight(lut,615)==2 && !t41_awb_distance_weight(lut,818));
		memcpy(q,p,sizeof(p)); memcpy(t,s,sizeof(s));
		assert(t41_awb_detect(p,0x12e0,s,sizeof(s),red,blue,out,failed)==-1);
		assert(t41_awb_detect(p,sizeof(p),s,sizeof(s)-1,red,blue,out,failed)==-1);
		assert(t41_awb_detect(p,sizeof(p),s+1,sizeof(s),red,blue,out,failed)==-1);
		assert(out[0]==123 && out[1]==456 && failed[0]==789 && failed[1]==1011);
		assert(!memcmp(p,q,sizeof(p)) && !memcmp(s,t,sizeof(s)));
		value=t41_tmo_le32(p+0x3c); t41_ae_put32(p+0x3c,t41_tmo_le32(p+0x38));
		assert(t41_awb_detect(p,sizeof(p),s,sizeof(s),red,blue,out,failed)==-1);
		t41_ae_put32(p+0x3c,value);
		assert(!memcmp(p,q,sizeof(p)) && !memcmp(s,t,sizeof(s)));
		assert(t41_awb_distance(10,20,13,24)==25);
	}
#endif
#ifdef T41_AWB_DETECT_HOST
	printf("10000 native AWB detector frames: calibration-write and malformed-input checks PASS; success modes=%u/%u/%u failures=%u cluster reports=%u\n",success[0],success[1],success[2],failures,classified);
#else
	printf("10000 synthetic AWB detector cases: %u mismatches, %u unexpected accesses; success modes=%u/%u/%u failures=%u cluster reports=%u\n",fail,oracle_bad,success[0],success[1],success[2],failures,classified);
#endif
	return fail || oracle_bad ? 1 : 0;
}
