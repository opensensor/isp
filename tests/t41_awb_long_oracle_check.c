#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_long.h"
#include "../driver/t41/tx_isp_t41_awb_prior.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
unsigned short oracle_cluster_red[6],oracle_cluster_blue[6];
extern int oracle_tisp_awb_long_alogrithm(void **,unsigned int);
extern int oracle_tisp_awb_long_par_update(unsigned int,void **);
extern unsigned int oracle_ratios[2],oracle_failed,oracle_temperature,oracle_calls,oracle_bad;
static unsigned char p[0x1400],q[sizeof(p)],s[T41_AWB_STATE_BYTES] __attribute__((aligned(4))),t[sizeof(s)];
static unsigned char report[1050],oreport[1050];
static unsigned int info[7],calls;
static uint32_t seed=176501;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static int detector(void *context,void **view,unsigned int *ratios,unsigned int *failed)
{
	(void)context; ++calls;
	ratios[0]=oracle_ratios[0]; ratios[1]=oracle_ratios[1]; failed[0]=oracle_failed;
	t41_ae_put32(view[0xb],oracle_temperature);
	return 0;
}
static int difference(const char *kind,unsigned int sequence,unsigned int frame,
		const unsigned char *a,const unsigned char *b,unsigned int n)
{
	unsigned int i;
	for(i=0;i<n;++i) if(a[i]!=b[i]) {
		printf("%s sequence=%u frame=%u +%x: %x/%x\n",kind,sequence,frame,i,a[i],b[i]); return 1;
	}
	return 0;
}
int main(void)
{
	unsigned int sequence,f,i,fail=0,frames=0,skips=0;
	void *view[45],*oview[45];
	*(uint32_t *)(void *)(oracle_bss+0x4114)=(uintptr_t)info;
	info[0]=(uintptr_t)q; info[1]=(uintptr_t)t; info[3]=(uintptr_t)oreport;
	for(sequence=0;sequence<200;++sequence) {
		unsigned int precision=10+rng()%7,fraction=1+rng()%10;
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		for(i=0;i<sizeof(s);++i) s[i]=rng();
		for(i=0;i<sizeof(report);++i) report[i]=rng();
		t41_awb_gain_put16(p+0xc6e,1+rng()%15); t41_awb_gain_put16(p+0xc72,1+rng()%15);
		t41_awb_gain_put16(p+0xcd2,precision); t41_awb_gain_put16(p+0xcd4,fraction);
		t41_awb_gain_put16(p+0xcd6,sequence%19);
		t41_awb_gain_put16(p+0xcd8,rng()%100); t41_awb_gain_put16(p+0xcda,rng()%100);
		t41_awb_gain_put16(p+0xcdc,sequence%2 ? 65535 : 0);
		t41_awb_gain_put16(p+0xd44,sequence%6); t41_awb_gain_put16(p+0xd56,sequence%4);
		for(i=0;i<8;++i) t41_awb_gain_put16(p+0xd46+i*2,rng()%17);
		for(i=0;i<4;++i) t41_awb_gain_put16(s+0x3514+i*2,rng()%17);
		t41_awb_gain_put16(s+0xc5f8,0); s[0xc600]=0;
		t41_ae_put32(p,rng()%16384); t41_ae_put32(p+4,0); t41_ae_put32(p+8,0);
		for(i=0;i<4;++i) t41_ae_put32(p+0xc+i*4,1024);
		t41_ae_put32(p+0x30,512+rng()%1024); t41_ae_put32(p+0x34,512+rng()%1024);
		t41_ae_put32(p+0x1c,1000); t41_ae_put32(p+0x20,10000);
		t41_ae_put32(p+0xe0,1); t41_ae_put32(p+0xe8,1);
		t41_ae_put32(s+0xea98,5000); t41_ae_put32(s+0xeaa0,0);
		for(i=0;i<6;++i) { oracle_cluster_red[i]=rng(); oracle_cluster_blue[i]=rng(); }
		for(f=0;f<100;++f) {
			/* Stable stretches exercise the settled skip, punctuated by changes
			 * and explicit event flags. Random padding must never change. */
			if(f%20==0) {
				unsigned int selection,bin,zone,field;
				for(selection=0;selection<2;++selection) for(bin=0;bin<4;++bin)
					for(zone=0;zone<225;++zone) for(field=0;field<5;++field) {
						unsigned int value=field==4 ? rng()%16384 : rng()%4000000;
						if(sequence%11==0 || (field==1 && zone%7==0)) value=0;
						t41_ae_put32(s+(selection ? 0x7f94 : 0x3944)+bin*0x1194+field*0x384+zone*4,value);
					}
				oracle_ratios[0]=(64+rng()%700)<<fraction;
				oracle_ratios[1]=(64+rng()%700)<<fraction;
				oracle_temperature=2000+rng()%8000;
				oracle_failed=(sequence+f)%7==0;
			}
			t41_ae_put32(s+0xeaa0,f%31==30 ? 0x10001 : 0);
			memcpy(q,p,sizeof(p)); memcpy(t,s,sizeof(s)); memcpy(oreport,report,sizeof(report));
			if(t41_awb_prior_prepare(p,sizeof(p),s,sizeof(s),view,oracle_cluster_red,oracle_cluster_blue)) return 2;
			oracle_tisp_awb_long_par_update(0,oview);
			calls=oracle_calls=0;
			if(t41_awb_long(p,sizeof(p),s,sizeof(s),report,sizeof(report),view,detector,NULL)) {
				printf("native rejected sequence=%u frame=%u\n",sequence,f); return 3;
			}
			oracle_tisp_awb_long_alogrithm(oview,0);
			++frames; skips+=!calls;
			if(memcmp(p,q,sizeof(p)) || memcmp(s,t,sizeof(s)) || memcmp(report,oreport,sizeof(report)) || calls!=oracle_calls) {
				if(fail++<10) {
					difference("parameters",sequence,f,p,q,sizeof(p));
					difference("state",sequence,f,s,t,sizeof(s));
					difference("report",sequence,f,report,oreport,sizeof(report));
					printf("detector calls=%u/%u precision=%u fraction=%u\n",calls,oracle_calls,precision,fraction);
				}
			}
		}
	}
	printf("%u synthetic AWB long frames, %u skipped detectors: %u mismatches, %u unexpected accesses\n",frames,skips,fail,oracle_bad);
	return fail || oracle_bad ? 1 : 0;
}
