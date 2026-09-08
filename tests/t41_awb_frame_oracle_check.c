#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_detect.h"
#include "../driver/t41/tx_isp_t41_awb_long.h"
#include "../driver/t41/tx_isp_t41_awb_prior.h"
#include "../driver/t41/tx_isp_t41_awb_special.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
unsigned short oracle_cluster_red[6],oracle_cluster_blue[6];
#ifndef T41_AWB_FRAME_HOST
extern int oracle_tisp_awb_get_statistics(void *,unsigned int);
extern int oracle_tisp_awb_long_par_update(unsigned int,void **);
extern int oracle_tisp_awb_spec_calculate(unsigned int);
extern int oracle_tisp_awb_long_alogrithm(void **,unsigned int);
extern int oracle_tisp_awb_set_gain(unsigned int);
extern unsigned int oracle_rgb[3],oracle_words[28][2],oracle_writes,oracle_reads,oracle_bad;
#else
static unsigned int oracle_rgb[3],oracle_bad;
#endif
static unsigned char p[0x1400],q[sizeof(p)],s[T41_AWB_STATE_BYTES] __attribute__((aligned(4))),t[sizeof(s)],dma[32768];
static unsigned char report[1050],oreport[1050];
static unsigned short red[6],blue[6];
#ifndef T41_AWB_FRAME_HOST
static unsigned int info[7];
#endif
static uint32_t seed=612795;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static int detector(void *context,void **view,unsigned int *out,unsigned int *failed)
{
	(void)context;
	return t41_awb_detect(p,sizeof(p),s,sizeof(s),view[22],view[23],out,failed);
}
#ifndef T41_AWB_FRAME_HOST
static void difference(const char *kind,unsigned int sequence,unsigned int frame,const unsigned char *a,const unsigned char *b,unsigned int n)
{
	unsigned int i,j=0;
	for(i=0;i<n && j<4;++i) if(a[i]!=b[i]) {
		printf("%s sequence=%u frame=%u +%x: %x/%x\n",kind,sequence,frame,i,a[i],b[i]); ++j;
	}
}
#endif
int main(void)
{
	unsigned int sequence,f,i,fail=0,frames=0;
	void *view[45];
#ifndef T41_AWB_FRAME_HOST
	void *oview[45];
	*(uint32_t *)(void *)(oracle_bss+0x4114)=(uintptr_t)info;
	info[0]=(uintptr_t)q; info[1]=(uintptr_t)t; info[3]=(uintptr_t)oreport;
#endif
	for(sequence=0;sequence<100;++sequence) {
		unsigned int rows=1+rng()%15,cols=1+rng()%15,count=rows*cols;
		memset(p,0,sizeof(p)); memset(s,0,sizeof(s)); memset(report,0,sizeof(report));
		memset(red,0,sizeof(red)); memset(blue,0,sizeof(blue));
		memset(oracle_cluster_red,0,sizeof(red)); memset(oracle_cluster_blue,0,sizeof(blue));
		if(t41_awb_distance_lut_init(s+0x3520,1028)) return 2;
		t41_ae_put32(p,100); t41_ae_put32(p+4,1);
		for(i=0;i<4;++i) t41_ae_put32(p+0xc+i*4,1024);
		t41_ae_put32(p+0x30,768+rng()%512); t41_ae_put32(p+0x34,768+rng()%512);
		t41_awb_gain_put16(p+0xc6e,rows); t41_awb_gain_put16(p+0xc72,cols);
		t41_awb_gain_put16(p+0xc74,16+rng()%240); t41_awb_gain_put16(p+0xc92,16+rng()%240);
		t41_awb_gain_put16(p+0xcd2,16); t41_awb_gain_put16(p+0xcd4,6);
		t41_awb_gain_put16(p+0xcd6,1+sequence%15);
		t41_awb_gain_put16(p+0xcd8,10); t41_awb_gain_put16(p+0xcda,10); t41_awb_gain_put16(p+0xcdc,5);
		for(i=0;i<8;++i) t41_awb_gain_put16(p+0xd46+i*2,2);
		for(i=0;i<4;++i) t41_awb_gain_put16(s+0x3514+i*2,2);
		t41_awb_gain_put16(p+0xd44,sequence%5); t41_awb_gain_put16(p+0xd56,sequence%3);
		t41_awb_gain_put16(p+0xd60,1); t41_awb_gain_put16(p+0xd62,1); t41_awb_gain_put16(p+0xd64,sequence&1);
		t41_ae_put32(p+0x1c,1000); t41_ae_put32(p+0x20,10000);
		t41_ae_put32(p+0xe0,sequence&1); t41_ae_put32(p+0xe8,(sequence/2)&1);
		t41_ae_put32(p+0xe4,512); t41_ae_put32(p+0xec,512);
		for(i=0;i<3;++i) {
			t41_ae_put32(p+0xb0+i*16,3000); t41_ae_put32(p+0xb4+i*16,2000);
			t41_ae_put32(p+0xb8+i*16,7000); t41_ae_put32(p+0xbc+i*16,10000);
		}
		t41_ae_put32(p+0x120,sequence%3); p[0xf0]=(sequence/3)&1;
		t41_ae_put32(p+0xf8,100); t41_ae_put32(p+0xf4,2000);
		t41_ae_put32(p+0x124,2000); t41_ae_put32(p+0x128,1000); t41_ae_put32(p+0x12c,3000);
		t41_ae_put32(p+0x130,1); t41_ae_put32(p+0x134,5);
		t41_ae_put32(p+0x138,(sequence/6)%4); t41_ae_put32(p+0x13c,(sequence/24)&1);
		for(i=0;i<4;++i) { t41_awb_gain_put16(p+0x11d4+i*2,2000+i*2000); t41_awb_gain_put16(p+0x11dc+i*2,32+rng()%96); }
		t41_awb_gain_put16(p+0x11ec,5); t41_awb_gain_put16(p+0x11ee,50);
		t41_awb_gain_put16(p+0x11f0,32); t41_awb_gain_put16(p+0x11f2,128);
		p[0x11f4]=sequence%4; p[0x11f5]=(sequence/4)%4;
		for(i=0;i<10;++i) {
			t41_awb_gain_put16(p+0xce2+i*4,512+rng()%1024); t41_awb_gain_put16(p+0xce4+i*4,512+rng()%1024);
			t41_awb_gain_put16(p+0xd0a+i*4,512+rng()%1024); t41_awb_gain_put16(p+0xd0c+i*4,512+rng()%1024);
			t41_awb_gain_put16(p+0xd6c+i*2,512+rng()%1024); t41_awb_gain_put16(p+0xd80+i*2,512+rng()%1024);
			p[0x11f6+i]=rng()%3; t41_ae_put32(p+0x140+i*4,rng()%16);
		}
		t41_awb_gain_put16(p+0xd08,rng()%257);
		for(i=0;i<6;++i) t41_ae_put32(p+0xbf4+i*4,960+rng()%128);
		t41_ae_put32(p+0xc0c,3000); t41_ae_put32(p+0xc10,4500);
		s[0xeaa2]=sequence%10; t41_ae_put32(s+0xeaa4,2500+rng()%5000);
		t41_awb_gain_put16(s+0xeaa8,128+rng()%384); t41_awb_gain_put16(s+0xeaaa,128+rng()%384);
		for(i=0;i<15;++i) { t41_ae_put32(p+0x38+i*4,64+i*128); t41_ae_put32(p+0x74+i*4,64+i*128); }
		for(i=0;i<225;++i) {
			t41_ae_put32(p+0x4ec+i*4,32+rng()%225); t41_ae_put32(p+0x870+i*4,100+rng()%700);
			p[0x1200+i]=1+rng()%4;
		}
		for(f=0;f<100;++f) {
			struct t41_awb_register words[28]; unsigned int n,record,db,wb[2],enable;
			p[0xcca]=f%4==3; p[0xccc]=(f/4)%4; db=count*(p[0xcca] ? 32 : 128);
			for(record=0;record<db/16;++record) {
				unsigned int pixels=128+rng()%1000,g=pixels*(64+rng()%256),r=pixels*(64+rng()%256),b=pixels*(64+rng()%256),lum=(r+g+b)/3;
				t41_ae_put32(dma+record*16,r|(g<<22)); t41_ae_put32(dma+record*16+4,(g>>10)|(b<<12));
				t41_ae_put32(dma+record*16+8,(b>>20)|(lum<<2)|(pixels<<24)); t41_ae_put32(dma+record*16+12,pixels>>8);
			}
			s[0x34de]=(f/10)&1; t41_ae_put32(s+0xea98,500+f*150); t41_ae_put32(s+0xeaa0,f%17==0 ? 1 : 0);
			s[0xeaa1]=f%19==18;
			for(i=0;i<3;++i) oracle_rgb[i]=1000+rng()%10000;
			memcpy(q,p,sizeof(p)); memcpy(t,s,sizeof(s)); memcpy(oreport,report,sizeof(report));
			if(t41_awb_statistics(p,sizeof(p),dma,db,s,sizeof(s)) ||
			   t41_awb_prior_prepare(p,sizeof(p),s,sizeof(s),view,red,blue) ||
			   t41_awb_special_prepare(p,sizeof(p),s,sizeof(s),oracle_rgb,words,&n) ||
			   t41_awb_long(p,sizeof(p),s,sizeof(s),report,sizeof(report),view,detector,NULL) ||
			   t41_awb_gain_prepare(p,sizeof(p),s,sizeof(s),report,sizeof(report),wb,&enable)) {
				printf("native rejected sequence=%u frame=%u\n",sequence,f); return 3;
			}
			if(enable) {
				static const unsigned int addresses[10]={0x4004,0x4008,0x400c,0x4010,0x4000,0x5004,0x5008,0x500c,0x5010,0x5000};
				for(i=0;i<10;++i) {
					words[n].address=addresses[i];
					words[n++].value=i==4 || i==9 ? 1 : wb[i==1 || i==3 || i==6 || i==8];
				}
			}
			++frames;
#ifndef T41_AWB_FRAME_HOST
			oracle_tisp_awb_get_statistics(dma,0); oracle_tisp_awb_long_par_update(0,oview);
			oracle_writes=oracle_reads=0; oracle_tisp_awb_spec_calculate(0); oracle_tisp_awb_long_alogrithm(oview,0);
			oracle_tisp_awb_set_gain(0);
			if(memcmp(p,q,sizeof(p)) || memcmp(s,t,sizeof(s)) || memcmp(report,oreport,sizeof(report)) ||
			   n!=oracle_writes || memcmp(words,oracle_words,n*sizeof(words[0])) ||
			   memcmp(red,oracle_cluster_red,sizeof(red)) || memcmp(blue,oracle_cluster_blue,sizeof(blue))) {
				if(fail++<10) {
					difference("parameters",sequence,f,p,q,sizeof(p)); difference("state",sequence,f,s,t,sizeof(s));
					difference("report",sequence,f,report,oreport,sizeof(report));
				}
			}
#else
			assert(n<=28);
			assert(!memcmp(p,q,4) && !memcmp(p+0x1c,q+0x1c,12));
			assert(!memcmp(p+0x30,q+0x30,0xd6c-0x30));
			assert(!memcmp(p+0xd94,q+0xd94,sizeof(p)-0xd94));
#endif
		}
	}
#ifdef T41_AWB_FRAME_HOST
	printf("%u complete native AWB algorithm frames: calibration-write checks PASS\n",frames);
#else
	printf("%u complete AWB algorithm frames: %u mismatches, %u unexpected accesses\n",frames,fail,oracle_bad);
#endif
	return fail || oracle_bad ? 1 : 0;
}
