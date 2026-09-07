#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_ydns.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_interpolate(unsigned int, unsigned int), oracle_pack(unsigned int);
extern unsigned int oracle_addresses[128], oracle_values[128], oracle_count, oracle_bad_write;
static unsigned char p[T41_YDNS_PARAM_BYTES], actual[T41_YDNS_STATE_BYTES], expected[sizeof(actual)];
static uint32_t seed=9127,info[4];
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
int main(void)
{
	unsigned int f,i,failures=0;
	*(uint32_t *)(void *)(oracle_bss+0x46d0)=(uintptr_t)info;
	*(uint32_t *)(void *)(oracle_bss+0x46d4)=(uintptr_t)info;
	info[0]=(uintptr_t)p; info[1]=(uintptr_t)expected;
	for(f=0;f<10000;++f) {
		struct t41_dpc_word words[T41_YDNS_WRITES];
		unsigned int gain=rng()%((16U<<16)+1),channel=f&1;
		int count;
		if(f%7==0) gain &= 0xffff0000;
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		for(i=0;i<sizeof(actual);++i) actual[i]=rng();
		memcpy(expected,actual,sizeof(actual));
		if(t41_ydns_interpolate(p,sizeof(p),actual,sizeof(actual),gain)) return 2;
		oracle_interpolate(channel,gain);
		for(i=0;i<sizeof(actual);++i) if(actual[i]!=expected[i]) {
			if(failures++<12) printf("YDNS interp frame=%u +%x got=%x OEM=%x\n",f,i,actual[i],expected[i]);
			break;
		}
		if(f&2) { for(i=0;i<sizeof(actual);++i) actual[i]=rng(); memcpy(expected,actual,sizeof(actual)); }
		count=t41_ydns_pack(p,sizeof(p),actual,sizeof(actual),channel,words,T41_YDNS_WRITES);
		oracle_count=0; oracle_pack(channel);
		if(count<0 || (unsigned int)count!=oracle_count) return 3;
		for(i=0;i<oracle_count;++i) if(words[i].address!=oracle_addresses[i] || words[i].value!=oracle_values[i]) {
			if(failures++<12) printf("YDNS write frame=%u word=%u %x=%x OEM=%x=%x\n",
				f,i,words[i].address,words[i].value,oracle_addresses[i],oracle_values[i]);
		}
	}
	printf("10000 synthetic YDNS interpolations and register transactions: %u mismatches, %u unexpected writes\n",failures,oracle_bad_write);
	return failures || oracle_bad_write ? 1 : 0;
}
