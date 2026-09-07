#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_dmsc.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_interpolate(unsigned int, unsigned int);
extern int oracle_static(unsigned int), oracle_dynamic(unsigned int);
extern unsigned int oracle_addresses[128], oracle_values[128], oracle_count, oracle_bad_write;
static unsigned char p[T41_DMSC_PARAM_BYTES], reference_p[sizeof(p)];
static unsigned char actual[T41_DMSC_STATE_BYTES], expected[sizeof(actual)];
static uint32_t seed = 7487, info[5];
static unsigned int rng(void) { seed ^= seed<<13; seed ^= seed>>17; seed ^= seed<<5; return seed; }
static unsigned int failures;
static void compare_words(const struct t41_dpc_word *words, int count, unsigned int f, unsigned int part)
{
	unsigned int i;
	if (count < 0 || (unsigned int)count != oracle_count) {
		if (failures++<12) printf("DMSC count frame=%u part=%u got=%d OEM=%u\n",f,part,count,oracle_count);
		return;
	}
	for (i=0;i<oracle_count;++i)
		if (words[i].address!=oracle_addresses[i] || words[i].value!=oracle_values[i]) {
			if (failures++<12) printf("DMSC write frame=%u part=%u word=%u %x=%x OEM=%x=%x\n",
				f,part,i,words[i].address,words[i].value,oracle_addresses[i],oracle_values[i]);
		}
}
static void compare_bytes(const unsigned char *got, const unsigned char *ref, unsigned int size,
		unsigned int f, const char *what)
{
	unsigned int i;
	for(i=0;i<size;++i) if(got[i]!=ref[i]) {
		if(failures++<12) printf("DMSC %s frame=%u +%x got=%x OEM=%x\n",what,f,i,got[i],ref[i]);
		break;
	}
}
int main(void)
{
	unsigned int f,i;
	/* dmsc_info .bss offset, independently resolved from the locked symbol table. */
	*(uint32_t *)(void *)(oracle_bss+0x4224)=(uintptr_t)info;
	info[0]=(uintptr_t)reference_p; info[1]=(uintptr_t)expected;
	for(f=0;f<10000;++f) {
		struct t41_dpc_word words[T41_DMSC_WRITES];
		unsigned int gain=rng()%((16U<<16)+1), sharpness=f%256;
		int count;
		if(f%7==0) gain &= 0xffff0000;
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		/* Explicitly exercise disabled branches, not only the rare random zero. */
		if(f&1) for(i=0x5fa;i<=0x607;++i) p[i]=rng()&1;
		for(i=0;i<sizeof(actual);++i) actual[i]=rng();
		memcpy(reference_p,p,sizeof(p)); memcpy(expected,actual,sizeof(actual));
		info[4]=sharpness;
		if(t41_dmsc_interpolate(p,sizeof(p),actual,sizeof(actual),gain,sharpness)) return 2;
		oracle_interpolate(0,gain);
		compare_bytes(actual,expected,sizeof(actual),f,"interpolate");
		count=t41_dmsc_pack_static(p,sizeof(p),words,T41_DMSC_WRITES);
		oracle_count=0; oracle_static(0);
		compare_words(words,count,f,0); compare_bytes(p,reference_p,sizeof(p),f,"static calibration");
		if(f&2) { for(i=0;i<sizeof(actual);++i) actual[i]=rng(); memcpy(expected,actual,sizeof(actual)); }
		count=t41_dmsc_pack_dynamic(p,sizeof(p),actual,sizeof(actual),words,T41_DMSC_WRITES);
		oracle_count=0; oracle_dynamic(0);
		compare_words(words,count,f,1); compare_bytes(actual,expected,sizeof(actual),f,"dynamic state");
	}
	printf("10000 synthetic DMSC interpolations, static/dynamic states and register transactions: %u mismatches, %u unexpected writes\n", failures,oracle_bad_write);
	return failures || oracle_bad_write ? 1 : 0;
}
