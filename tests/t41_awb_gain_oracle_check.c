#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_gain.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_gain(unsigned int);
extern unsigned int oracle_words[2], oracle_bad_write, oracle_writes, oracle_triggers;
static unsigned char p[T41_AWB_GAIN_PARAM_BYTES], q[sizeof(p)];
static unsigned char s[0xf54c], t[sizeof(s)], report[0x41a], expected[sizeof(report)];
static uint32_t seed = 1981, info[7];
static unsigned int rng(void) { seed ^= seed<<13; seed ^= seed>>17; seed ^= seed<<5; return seed; }
int main(void)
{
	unsigned int f, i, fail = 0;
	*(uint32_t *)(void *)(oracle_bss+0x4114) = (uintptr_t)info;
	info[0] = (uintptr_t)q; info[1] = (uintptr_t)t; info[3] = (uintptr_t)expected;
	for(f=0;f<10000;++f) {
		unsigned int words[2], enable;
		for(i=0;i<sizeof(p);++i) p[i] = rng();
		for(i=0;i<sizeof(s);++i) s[i] = rng();
		for(i=0;i<sizeof(report);++i) report[i] = rng();
		for(i=0;i<2;++i) {
			t41_ae_put32(p+0x14+i*4,64+rng()%2048);
			t41_ae_put32(p+0x30+i*4,256+rng()%3072);
		}
		t41_awb_gain_put16(p+0xcd2,10+rng()%7);
		t41_awb_gain_put16(p+0xcd4,1+rng()%12);
		t41_ae_put32(p+0x28,1000+rng()%10000);
		t41_ae_put32(p+0xc0c,3000); t41_ae_put32(p+0xc10,6500);
		for(i=0;i<6;++i) t41_ae_put32(p+0xbf4+i*4,512+rng()%2048);
		t41_ae_put32(s+0x351c,t41_tmo_le32(p+0x28)+(f&1 ? rng()%200 : rng()%1000));
		s[0xeaa0] = f&2 ? 1 : 0; s[0xeaa1] = f&4 ? 1 : 0; s[0xeaa2] = f%10;
		memcpy(q,p,sizeof(p)); memcpy(t,s,sizeof(s)); memcpy(expected,report,sizeof(report));
		oracle_writes = 0; oracle_triggers = 0;
		if(t41_awb_gain_prepare(p,sizeof(p),s,sizeof(s),report,sizeof(report),words,&enable)) return 2;
		oracle_gain(0);
		if(memcmp(p,q,sizeof(p)) || memcmp(s,t,sizeof(s)) || memcmp(report,expected,sizeof(report)) ||
		   (enable && memcmp(words,oracle_words,sizeof(words))) ||
		   oracle_writes != (enable ? 9U : 0U) || oracle_triggers != enable) {
			if(fail++ < 10) {
				printf("AWB gain %u mode=%u: words=%x/%x OEM=%x/%x enable=%u writes=%u\n",
					f,f%10,words[0],words[1],oracle_words[0],oracle_words[1],enable,oracle_writes);
				for(i=0;i<sizeof(s);++i) if(s[i]!=t[i]) { printf("state+%x: %x/%x\n",i,s[i],t[i]); break; }
			}
		}
	}
	printf("10000 synthetic AWB gain/history cases: %u mismatches, %u unexpected writes\n",fail,oracle_bad_write);
	return fail || oracle_bad_write ? 1 : 0;
}
