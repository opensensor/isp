#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_dpc.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_interpolate(unsigned int, unsigned int);
extern int oracle_long(unsigned int), oracle_short(unsigned int);
extern int oracle_other(unsigned int);
extern unsigned int oracle_addresses[128], oracle_values[128], oracle_count, oracle_bad_write;
static unsigned char p[T41_DPC_PARAM_BYTES], actual[T41_DPC_STATE_BYTES], expected[sizeof(actual)];
static uint32_t seed = 4513, info[6];
static unsigned int rng(void) { seed ^= seed<<13; seed ^= seed>>17; seed ^= seed<<5; return seed; }
int main(void)
{
	unsigned int f, i, bank, failures=0;
	*(uint32_t *)(void *)(oracle_bss+16912)=(uintptr_t)info;
	info[0]=(uintptr_t)p; info[1]=(uintptr_t)expected;
	for(f=0;f<10000;++f) {
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		for(i=0;i<sizeof(actual);++i) actual[i]=rng();
		memcpy(expected,actual,sizeof(actual));
		for(bank=0;bank<2;++bank) {
			struct t41_dpc_word words[T41_DPC_LONG_WRITES];
			unsigned int gain = rng()%(16U<<16);
			int count;
			if(f%7==0) gain &= 0xffff0000;
			info[bank+3]=gain;
			if(t41_dpc_interpolate_bank(p,sizeof(p),actual,sizeof(actual),gain,bank)) return 2;
			oracle_interpolate(0,bank);
			if(memcmp(actual,expected,sizeof(actual))) {
				if(failures++<10) {
					for(i=0;i<sizeof(actual);++i) if(actual[i]!=expected[i]) {
						printf("DPC interp frame=%u bank=%u gain=%u +%x got=%x OEM=%x\n",
							f,bank,gain,i,actual[i],expected[i]); break;
					}
				}
			}
			/* Independently fuzz the writer, not only interpolated states. */
			if(f&1) { for(i=0;i<sizeof(actual);++i) actual[i]=rng(); memcpy(expected,actual,sizeof(actual)); }
			count=t41_dpc_pack_bank(p,sizeof(p),actual,sizeof(actual),bank,words,T41_DPC_LONG_WRITES);
			oracle_count=0;
			if(bank) oracle_short(0); else oracle_long(0);
			if(count<0 || (unsigned int)count!=oracle_count) return 3;
			for(i=0;i<oracle_count;++i)
				if(words[i].address!=oracle_addresses[i] || words[i].value!=oracle_values[i]) {
					if(failures++<10) printf("DPC write frame=%u bank=%u word=%u %x=%x OEM=%x=%x\n",
						f,bank,i,words[i].address,words[i].value,oracle_addresses[i],oracle_values[i]);
				}
		}
		{
			struct t41_dpc_word words[T41_DPC_OTHER_WRITES];
			int count=t41_dpc_pack_other(p,sizeof(p),f&1,words,T41_DPC_OTHER_WRITES);
			((unsigned char *)info)[9]=f&1;
			oracle_count=0; oracle_other(0);
			if(count<0 || (unsigned int)count!=oracle_count) {
				printf("other count=%d OEM=%u\n",count,oracle_count); return 3;
			}
			for(i=0;i<oracle_count;++i)
				if(words[i].address!=oracle_addresses[i] || words[i].value!=oracle_values[i]) {
					if(failures++<10) printf("DPC other frame=%u word=%u %x=%x OEM=%x=%x\n",
						f,i,words[i].address,words[i].value,oracle_addresses[i],oracle_values[i]);
				}
		}
	}
	printf("10000 synthetic DPC long/short gain interpolations and register transactions: %u mismatches, %u unexpected writes\n",
		failures,oracle_bad_write);
	return failures || oracle_bad_write ? 1 : 0;
}
