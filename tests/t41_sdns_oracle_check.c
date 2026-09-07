#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_sdns.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_interpolate(unsigned int, unsigned int);
extern int oracle_static(unsigned int), oracle_dynamic(unsigned int);
extern unsigned int oracle_addresses[128], oracle_values[128], oracle_count, oracle_bad_write;
static unsigned char p[T41_SDNS_PARAM_BYTES], actual[T41_SDNS_STATE_BYTES], expected[sizeof(actual)];
static uint32_t seed=8239,info[5];
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
int main(void)
{
	unsigned int f,i,part,failures=0;
	*(uint32_t *)(void *)(oracle_bss+0x46f0)=(uintptr_t)info;
	*(uint32_t *)(void *)(oracle_bss+0x46f4)=(uintptr_t)info;
	info[0]=(uintptr_t)p; info[1]=(uintptr_t)expected;
	for(f=0;f<10000;++f) {
		unsigned int gain=rng()%((16U<<16)+1),strength=f%256,channel=f&1;
		if(f%7==0) gain &= 0xffff0000;
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		for(i=0;i<sizeof(actual);++i) actual[i]=rng();
		memcpy(expected,actual,sizeof(actual)); info[4]=strength;
		if(t41_sdns_interpolate(p,sizeof(p),actual,sizeof(actual),gain,strength)) return 2;
		oracle_interpolate(channel,gain);
		for(i=0;i<sizeof(actual);++i) if(actual[i]!=expected[i]) {
			if(failures++<12) printf("SDNS interp frame=%u +%x got=%x OEM=%x\n",f,i,actual[i],expected[i]);
			break;
		}
		if(f&2) { for(i=0;i<sizeof(actual);++i) actual[i]=rng(); memcpy(expected,actual,sizeof(actual)); }
		for(part=0;part<2;++part) {
			struct t41_dpc_word words[T41_SDNS_STATIC_WRITES];
			int count=part ? t41_sdns_pack_dynamic(p,sizeof(p),actual,sizeof(actual),channel,words,T41_SDNS_STATIC_WRITES) :
				t41_sdns_pack_static(p,sizeof(p),channel,words,T41_SDNS_STATIC_WRITES);
			oracle_count=0; if(part) oracle_dynamic(channel); else oracle_static(channel);
			if(count<0 || (unsigned int)count!=oracle_count) return 3;
			for(i=0;i<oracle_count;++i) if(words[i].address!=oracle_addresses[i] || words[i].value!=oracle_values[i]) {
				if(failures++<12) printf("SDNS write frame=%u part=%u word=%u %x=%x OEM=%x=%x\n",
					f,part,i,words[i].address,words[i].value,oracle_addresses[i],oracle_values[i]);
			}
		}
	}
	printf("10000 synthetic SDNS interpolations and static/dynamic transactions: %u mismatches, %u unexpected writes\n",failures,oracle_bad_write);
	return failures || oracle_bad_write ? 1 : 0;
}
