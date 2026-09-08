#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_prior.h"
#include "../driver/t41/tx_isp_t41_awb_special.h"
#include "../driver/t41/tx_isp_t41_awb_ct.h"
#include "../driver/t41/tx_isp_t41_awb_gray.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_tisp_awb_get_statistics(void *, unsigned int);
extern int oracle_tisp_awb_sat_weight(unsigned int, void *, void *, void *, void *, void *, void *);
extern int oracle_tisp_awb_long_par_update(unsigned int, void **);
extern int oracle_tisp_awb_spec_calculate(unsigned int);
extern unsigned int oracle_Tiziano_Awb_Ct_Cal(void **, unsigned int, unsigned int);
extern int oracle_Tiziano_Awb_Ct_Detect_GrayWorld(void *,unsigned int,void **,unsigned int *,unsigned int *);
extern unsigned int oracle_rgb[3], oracle_words[18][2], oracle_writes, oracle_reads, oracle_bad;
unsigned short oracle_cluster_red[6], oracle_cluster_blue[6];
static unsigned char p[0x1400], q[sizeof(p)], dma[32768], s[T41_AWB_STATE_BYTES], t[sizeof(s)];
static unsigned int info[7], input[4][225], gains[2], weights[225], expected[225];
static uint32_t seed=144831;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static void put16(unsigned int off,unsigned int v) { p[off]=v; p[off+1]=v>>8; }
int main(void)
{
	unsigned int f,i,fail=0;
	*(uint32_t *)(void *)(oracle_bss+0x4114)=(uintptr_t)info;
	info[0]=(uintptr_t)p; info[1]=(uintptr_t)t;
	for(f=0;f<10000;++f) {
		info[0]=(uintptr_t)p;
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		for(i=0;i<sizeof(dma);++i) dma[i]=rng();
		for(i=0;i<sizeof(s);++i) s[i]=rng();
		put16(0xc6e,1+rng()%15); put16(0xc72,1+rng()%15);
		p[0xcca]=f&1; p[0xccc]=(f/2)%4;
		memcpy(t,s,sizeof(s));
		if(t41_awb_statistics(p,sizeof(p),dma,sizeof(dma),s,sizeof(s))) return 2;
		oracle_tisp_awb_get_statistics(dma,0);
		if(memcmp(s,t,sizeof(s))) {
			if(fail++<10) for(i=0;i<sizeof(s);++i) if(s[i]!=t[i]) {
				printf("statistics case=%u mode=%u class=%u state+%x: %x/%x\n",f,p[0xcca],p[0xccc],i,s[i],t[i]); break;
			}
		}
		for(i=0;i<225;++i) {
			unsigned int j;
			for(j=0;j<4;++j) input[j][i]=rng()%4000000;
			if(i%9==0) input[1][i]=0;
			weights[i]=expected[i]=rng();
		}
		gains[0]=256+rng()%16000; gains[1]=256+rng()%16000;
		put16(0xcd2,10+rng()%7); put16(0xd56,f%4);
		t41_ae_put32(p,rng()%4000000);
		if(t41_awb_saturation_weights(p,sizeof(p),input[0],input[1],input[2],input[3],gains,weights)) return 3;
		oracle_tisp_awb_sat_weight(0,input[0],input[1],input[2],input[3],expected,gains);
		if(memcmp(weights,expected,sizeof(weights))) {
			if(fail++<10) for(i=0;i<225;++i) if(weights[i]!=expected[i]) {
				printf("saturation case=%u zone=%u: %x/%x\n",f,i,weights[i],expected[i]); break;
			}
		}
		{
			void *slots[45], *oslots[45];
			unsigned int low=rng()%100000, high=low+3+rng()%100000;
			t41_ae_put32(p+0x1c,low); t41_ae_put32(p+0x20,high);
			t41_ae_put32(p+0xe0,f&1); t41_ae_put32(p+0xe8,(f/2)&1);
			t41_ae_put32(p+0xe4,rng()%257); t41_ae_put32(p+0xec,rng()%257);
			put16(0xcd4,8);
			for(i=0;i<12;++i) t41_ae_put32(p+0xb0+i*4,1000+rng()%20000);
			t41_ae_put32(s+0xea98,rng()%(high+(high-low)/3+1));
			memcpy(q,p,sizeof(p)); memcpy(t,s,sizeof(s));
			info[0]=(uintptr_t)q;
			if(t41_awb_prior_prepare(p,sizeof(p),s,sizeof(s),slots,oracle_cluster_red,oracle_cluster_blue)) return 4;
			oracle_tisp_awb_long_par_update(0,oslots);
			if(memcmp(p,q,sizeof(p)) || memcmp(s,t,sizeof(s))) {
				if(fail++<10) for(i=0;i<sizeof(s);++i) if(s[i]!=t[i]) {
					printf("prior case=%u state+%x: %x/%x ev=%u low=%u high=%u flags=%u/%u\n",f,i,s[i],t[i],t41_tmo_le32(s+0xea98),low,high,f&1,(f/2)&1); break;
				}
			}
			for(i=0;i<45;++i) {
				uintptr_t address=(uintptr_t)slots[i], expected_address=address;
				if(address>=(uintptr_t)p && address<(uintptr_t)(p+sizeof(p))) expected_address=address-(uintptr_t)p+(uintptr_t)q;
				if(address>=(uintptr_t)s && address<(uintptr_t)(s+sizeof(s))) expected_address=address-(uintptr_t)s+(uintptr_t)t;
				if((uintptr_t)oslots[i]!=expected_address && fail++<10)
					printf("prior pointer case=%u slot=%u\n",f,i);
			}
		}
		{
			struct t41_awb_register words[18]; unsigned int n, reads;
			for(i=0;i<10;++i) p[0x11f6+i]=rng()%4;
			s[0x34de]=f&1; s[0x34df]=f%3;
			for(i=0;i<3;++i) oracle_rgb[i]=rng();
			if(f%11==0) oracle_rgb[1]=0;
			reads=p[0x11f6] && s[0x34de]==1 && s[0x34df]>=1 ? 3 : 0;
			memcpy(q,p,sizeof(p)); memcpy(t,s,sizeof(s));
			if(t41_awb_special_prepare(p,sizeof(p),s,sizeof(s),oracle_rgb,words,&n)) return 5;
			oracle_writes=oracle_reads=0;
			oracle_tisp_awb_spec_calculate(0);
			if(memcmp(p,q,sizeof(p)) || memcmp(s,t,sizeof(s)) || n!=oracle_writes ||
			   memcmp(words,oracle_words,n*sizeof(words[0])) || reads!=oracle_reads) {
				if(fail++<10) printf("special case=%u writes=%u/%u reads=%u/%u\n",f,n,oracle_writes,reads,oracle_reads);
			}
		}
		{
			void *oslots[45]; unsigned int red, blue, ct, oct, fraction=1+rng()%10;
			unsigned int ax=32, ay=32;
			put16(0xcd2,10+rng()%7); put16(0xcd4,fraction);
			for(i=0;i<15;++i) {
				ax+=1+rng()%50; ay+=1+rng()%50;
				t41_ae_put32(p+0x38+i*4,ax); t41_ae_put32(p+0x74+i*4,ay);
			}
			for(i=0;i<225;++i) t41_ae_put32(p+0x870+i*4,f%17 ? 100+rng()%800 : 0);
			red=rng()%((ax+50)<<fraction); blue=rng()%((ay+50)<<fraction);
			if(f%7==0) red=ax<<fraction;
			if(f%9==0) blue=ay<<fraction;
			memcpy(q,p,sizeof(p)); memcpy(t,s,sizeof(s));
			oracle_tisp_awb_long_par_update(0,oslots);
			if(t41_awb_ct_calculate(p,sizeof(p),red,blue,&ct)) return 6;
			oct=oracle_Tiziano_Awb_Ct_Cal(oslots,red,blue);
			if(ct!=oct || oct!=t41_tmo_le32(q+0x28)) {
				if(fail++<10) printf("CT case=%u frac=%u input=%u/%u: %u/%u\n",f,fraction,red,blue,ct,oct);
			}
		}
		{
			void *slots[45], *oslots[45]; unsigned int result[2], ores[2], bad=f%5 ? 1:0, obad=bad;
			unsigned int count=t41_tmo_le16(p+0xc6e)*t41_tmo_le16(p+0xc72), fraction=t41_tmo_le16(p+0xcd4);
			put16(0xd60,f&1); put16(0xd62,(f/2)&1); put16(0xd64,(f/4)&1);
			for(i=0;i<4*count;++i) t41_ae_put32(s+0x1c20+i*4,(f%23 ? 40+rng()%500 : 0)<<fraction);
			for(i=0;i<514;++i) t41_awb_gain_put16(s+0x3520+i*2,rng()%257);
			for(i=0;i<count;++i) p[0x1200+i]=f%29 ? rng()%10:0;
			result[0]=ores[0]=rng(); result[1]=ores[1]=rng();
			if(t41_awb_prior_prepare(p,sizeof(p),s,sizeof(s),slots,oracle_cluster_red,oracle_cluster_blue)) return 7;
			memcpy(q,p,sizeof(p)); memcpy(t,s,sizeof(s));
			oracle_tisp_awb_long_par_update(0,oslots);
			for(i=0;i<2;++i) if(t41_awb_grayworld_mode(p,sizeof(p),s,sizeof(s),(const unsigned int *)(const void *)(s+0x1c20),4*count,i,result,&bad)) return 8;
			oracle_Tiziano_Awb_Ct_Detect_GrayWorld(t+0x1c20,0,oslots,ores,&obad);
			if(memcmp(p,q,sizeof(p)) || memcmp(s,t,sizeof(s)) || memcmp(result,ores,sizeof(result)) || bad!=obad) {
				if(fail++<10) {
					printf("gray case=%u result=%u/%u OEM=%u/%u bad=%u/%u\n",f,result[0],result[1],ores[0],ores[1],bad,obad);
					for(i=0;i<sizeof(s);++i) if(s[i]!=t[i]) { printf("gray state+%x: %x/%x\n",i,s[i],t[i]); break; }
				}
			}
		}
	}
	printf("10000 synthetic AWB DMA/selection/saturation/prior/special/CT/gray cases: %u mismatches, %u unexpected accesses\n",fail,oracle_bad);
	return fail || oracle_bad ? 1 : 0;
}
