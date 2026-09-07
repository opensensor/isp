#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_ysp.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
unsigned char oracle_data[0x5000] __attribute__((aligned(65536)));
uint32_t oracle_original;
int oracle_noop(void) { return 0; }
extern int oracle_interpolate(unsigned int, unsigned int), oracle_strength(unsigned int, unsigned int);
extern int oracle_static(unsigned int), oracle_dynamic(unsigned int);
extern unsigned int oracle_addresses[128], oracle_values[128], oracle_count, oracle_bad_write;
static unsigned char p[T41_YSP_PARAM_BYTES], reference_p[sizeof(p)], original[sizeof(p)];
static unsigned char actual[T41_YSP_STATE_BYTES], expected[sizeof(actual)];
static uint32_t seed=7487,info[4];
static unsigned int failures;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static void compare_words(const struct t41_dpc_word *words,int count,unsigned int f,unsigned int part)
{
	unsigned int i;
	if (count<0 || (unsigned int)count!=oracle_count) {
		if (failures++<12) printf("YSP count frame=%u part=%u got=%d OEM=%u\n",f,part,count,oracle_count);
		return;
	}
	for(i=0;i<oracle_count;++i) if(words[i].address!=oracle_addresses[i] || words[i].value!=oracle_values[i]) {
		if(failures++<12) printf("YSP write frame=%u part=%u word=%u %x=%x OEM=%x=%x\n",
			f,part,i,words[i].address,words[i].value,oracle_addresses[i],oracle_values[i]);
	}
}
static void compare_bytes(const unsigned char *a,const unsigned char *b,unsigned int size,unsigned int f,const char *part)
{
	unsigned int i;
	for(i=0;i<size;++i) if(a[i]!=b[i]) {
		if(failures++<12) printf("YSP %s frame=%u +%x got=%x OEM=%x\n",part,f,i,a[i],b[i]);
		break;
	}
}
int main(void)
{
	unsigned int f,i;
	*(uint32_t *)(void *)(oracle_bss+0x4704)=(uintptr_t)info;
	info[0]=(uintptr_t)reference_p; info[1]=(uintptr_t)expected;
	oracle_original=(uintptr_t)original;
	for(f=0;f<10000;++f) {
		struct t41_dpc_word words[T41_YSP_DYNAMIC_WRITES];
		unsigned int gain=rng()%((16U<<16)+1),strength=f%256;
		int count;
		if(f%7==0) gain &= 0xffff0000;
		for(i=0;i<sizeof(p);++i) { p[i]=rng(); original[i]=rng(); }
		for(i=0;i<sizeof(actual);++i) actual[i]=rng();
		memcpy(reference_p,p,sizeof(p)); memcpy(expected,actual,sizeof(actual));
		if(t41_ysp_strength(original,sizeof(original),p,sizeof(p),strength)) return 2;
		oracle_strength(0,strength);
		compare_bytes(p,reference_p,sizeof(p),f,"strength");
		if(t41_ysp_interpolate(p,sizeof(p),actual,sizeof(actual),gain)) return 2;
		oracle_interpolate(0,gain);
		compare_bytes(actual,expected,sizeof(actual),f,"interpolate");
		count=t41_ysp_pack_static(p,sizeof(p),words,T41_YSP_DYNAMIC_WRITES);
		oracle_count=0; oracle_static(0); compare_words(words,count,f,0);
		if(f&2) { for(i=0;i<sizeof(actual);++i) actual[i]=rng(); memcpy(expected,actual,sizeof(actual)); }
		/* Exercise every nonlinear threshold and both sides of each knee. */
		t41_dpc_put16(actual+0x6c,f%4097); t41_dpc_put16(expected+0x6c,f%4097);
		if(f&4) { t41_dpc_put16(actual+0x6e,0); t41_dpc_put16(actual+0x70,0); memcpy(expected,actual,sizeof(actual)); }
		count=t41_ysp_pack_dynamic(p,sizeof(p),actual,sizeof(actual),words,T41_YSP_DYNAMIC_WRITES);
		oracle_count=0; oracle_dynamic(0); compare_words(words,count,f,1);
	}
	printf("10000 synthetic YSP strength/interpolation and static/dynamic register transactions: %u mismatches, %u unexpected writes\n",failures,oracle_bad_write);
	return failures || oracle_bad_write ? 1 : 0;
}
