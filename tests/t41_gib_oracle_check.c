#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_gib.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern unsigned int oracle_self_gain(unsigned int, unsigned int, unsigned int,
	unsigned int, unsigned int, unsigned int);
extern int oracle_dgain(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
extern unsigned int oracle_words[3], oracle_bad_write;
static uint32_t seed = 1979, info[11];
static unsigned int rng(void) { seed ^= seed<<13; seed ^= seed>>17; seed ^= seed<<5; return seed; }
int main(void)
{
	unsigned int f, i, fail = 0;
	/* Exact stock gib_info BSS location (validated generator's SHA). */
	*(uint32_t *)(void *)(oracle_bss+0x4120) = (uintptr_t)info;
	for(f=0;f<10000;++f) {
		unsigned short black[5]; unsigned int infrared=f&1, self, expected, gains[4], words[3];
		((unsigned char *)info)[41] = infrared;
		for(i=0;i<5;++i) black[i] = rng()%(f&2 ? 65536 : 2048);
		expected = oracle_self_gain(0,black[0],black[1],black[2],black[3],black[4]);
		if(t41_gib_self_gain(black,infrared,&self)) return 2;
		if(self != expected && fail++ < 20) printf("self %u: scalar=%u OEM=%u\n",f,self,expected);
		info[4] = self;
		for(i=0;i<4;++i) gains[i] = f&4 ? rng() : rng()%131072;
		oracle_dgain(0,gains[0],gains[1],gains[2],gains[3]);
		if(t41_gib_dgain(self,infrared,gains,words)) return 2;
		for(i=0;i<3;++i) if(words[i]!=oracle_words[i] && fail++<20)
			printf("gain %u word %u: scalar=%x OEM=%x\n",f,i,words[i],oracle_words[i]);
	}
	printf("10000 synthetic GIB black-level/gain cases: %u mismatches, %u unexpected writes\n",fail,oracle_bad_write);
	return fail || oracle_bad_write ? 1 : 0;
}
