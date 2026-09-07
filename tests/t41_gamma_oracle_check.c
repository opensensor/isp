#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_gamma.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_select(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
extern int oracle_curve(unsigned int), oracle_pack(unsigned int);
extern unsigned int oracle_words[3][130], oracle_counts[3], oracle_bad_write;
static unsigned char p[T41_GAMMA_PARAM_BYTES], state[0x424];
static uint32_t seed = 9769;
static unsigned int rng(void) { seed ^= seed<<13; seed ^= seed>>17; seed ^= seed<<5; return seed; }
static void put32(unsigned char *p, unsigned int v) { memcpy(p,&v,4); }
static void put16(unsigned char *p, unsigned short v) { memcpy(p,&v,2); }
int main(void)
{
	unsigned int f, i, bank, failures=0;
	put32(oracle_bss+16944,(uintptr_t)state);
	put32(state,(uintptr_t)p);
	for(f=0;f<10000;++f) {
		unsigned short curve[129];
		unsigned int ev=rng(), high=f%13==0, strength, threshold=rng()%200;
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		for(i=0;i<10;++i) {
			threshold += 1+rng()%(f&1 ? 1000 : 100000000);
			put32(p+0x104+i*4,threshold);
		}
		if (f&1) ev=rng()%(threshold+1);
		if (f%7==0) ev=t41_tmo_le32(p+0x104+(f%10)*4);
		for(i=0;i<129;++i) put16(p+0x12c+i*2,rng()%4096);
		if (t41_gamma_strength(p,sizeof(p),ev,high,&strength) ||
		    t41_gamma_curve(p,sizeof(p),strength,curve)) return 2;
		state[0x420]=0;
		if (oracle_select(0,0,ev,high,1) ||
		    t41_tmo_le32(state+0x218)!=strength ||
		    t41_tmo_le32(state+0x214)!=ev || oracle_curve(0) ||
		    memcmp(state+0x21c,curve,sizeof(curve))) {
			if(failures++<10) printf("gamma mismatch %u ev=%u strength=%u OEM=%u\n",
				f,ev,strength,t41_tmo_le32(state+0x218));
		}
		if (oracle_select(0,0,ev,high,0)!=1) ++failures;
		memset(oracle_counts,0,sizeof(oracle_counts));
		oracle_pack(0);
		for(bank=0;bank<3;++bank) {
			if(oracle_counts[bank]!=130 || oracle_words[bank][0]!=0x101 ||
			   oracle_words[bank][129]!=0x7f0102) ++failures;
			for(i=0;i<128;++i)
				if(oracle_words[bank][i+1] != ((unsigned int)curve[i+1]<<12 | curve[i])) ++failures;
		}
		state[0x420]=1;
		if(oracle_curve(0)!=1 || memcmp(state+0x21c,curve,sizeof(curve))) ++failures;
	}
	printf("10000 synthetic gamma selections, curves and three-bank writes: %u mismatches, %u unexpected writes\n",
		failures,oracle_bad_write);
	return failures || oracle_bad_write ? 1 : 0;
}
