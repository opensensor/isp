#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_mdns.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_interpolate(unsigned int,unsigned int),oracle_pack(unsigned int),oracle_enable(unsigned int);
extern int oracle_smp(unsigned int,unsigned int,unsigned int,unsigned int);
extern int oracle_dif(unsigned int,unsigned int,unsigned int,unsigned int);
extern unsigned int oracle_addresses[128],oracle_values[128],oracle_count,oracle_bad_write;
static unsigned char p[T41_MDNS_PARAM_BYTES],reference_p[sizeof(p)];
static unsigned char actual[T41_MDNS_STATE_BYTES],expected[sizeof(actual)];
static unsigned int seed=71831,failures,memopt;
static uint32_t info[12];
uint32_t oracle_params[2],oracle_day[2],oracle_night[2];
static unsigned char day[0x18000],night[sizeof(day)],tuning[512];
extern int oracle_strength(unsigned int);
uintptr_t oracle_tuning(void) { return (uintptr_t)tuning; }
int oracle_noop(void) { return 0; }
unsigned int oracle_memopt(void) { return memopt; }
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static void compare_words(const struct t41_dpc_word *words,int count,unsigned int f)
{
	unsigned int i;
	if(count<0 || (unsigned int)count!=oracle_count) {
		if(failures++<12) printf("MDNS count frame=%u got=%d OEM=%u\n",f,count,oracle_count);
		return;
	}
	for(i=0;i<oracle_count;++i) if(words[i].address!=oracle_addresses[i] || words[i].value!=oracle_values[i]) {
		if(failures++<12) printf("MDNS write frame=%u word=%u %x=%x OEM=%x=%x\n",f,i,
			words[i].address,words[i].value,oracle_addresses[i],oracle_values[i]);
	}
}
int main(void)
{
	unsigned int f,i;
	*(uint32_t *)(void *)(oracle_bss+0x46c8)=(uintptr_t)info;
	*(uint32_t *)(void *)(oracle_bss+0x46cc)=(uintptr_t)info;
	info[0]=(uintptr_t)reference_p; info[1]=(uintptr_t)expected;
	for(i=0;i<2;++i) {
		oracle_params[i]=(uintptr_t)reference_p-0x13f00;
		oracle_day[i]=(uintptr_t)day; oracle_night[i]=(uintptr_t)night;
	}
	for(f=0;f<100000;++f) {
		unsigned int x0=rng()&255,x1=rng()&255,y0=rng()&255,y1=rng()&255,v[8],cut;
		t41_mdns_smp(x0,x1,y0,y1,v); oracle_smp(x0,x1,y0,y1);
		for(i=0;i<8;++i) if(v[i]!=*(uint32_t *)(void *)(oracle_bss+0x46c4-i*4)) {
			if(failures++<12) printf("MDNS smp frame=%u args=%u/%u/%u/%u i=%u got=%u OEM=%u\n",f,x0,x1,y0,y1,i,v[i],*(uint32_t *)(void *)(oracle_bss+0x46c4-i*4));
		}
		cut=t41_mdns_dif(x0,x1,y0,y1,v); oracle_dif(x0,x1,y0,y1);
		if(cut!=*(uint32_t *)(void *)(oracle_bss+0x46a0) || *(uint32_t *)(void *)(oracle_bss+0x46a4)) ++failures;
		for(i=0;i<8;++i) if(v[i]!=*(uint32_t *)(void *)(oracle_bss+0x469c-i*4)) {
			if(failures++<12) printf("MDNS dif frame=%u args=%u/%u/%u/%u i=%u got=%u OEM=%u\n",f,x0,x1,y0,y1,i,v[i],*(uint32_t *)(void *)(oracle_bss+0x469c-i*4));
		}
	}
	for(f=0;f<10000;++f) {
		struct t41_dpc_word words[T41_MDNS_WRITES];
		unsigned int gain=rng()%((16U<<16)+1),channel=f&1;
		int count;
		if(f%7==0) gain&=0xffff0000;
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		for(i=0;i<sizeof(actual);++i) actual[i]=rng();
		memcpy(reference_p,p,sizeof(p)); memcpy(expected,actual,sizeof(actual));
		for(i=0;i<0x48e;++i) { day[0x17406+i]=rng(); night[0x17406+i]=rng(); }
		info[4]=(f>>1)&1; info[7]=(f>>2)&1;
		*(uint32_t *)(void *)(tuning+channel*128+192)=1;
		tuning[channel*128+196]=f%256;
		if(t41_mdns_strength((info[7] ? night : day)+0x17406,0x48e,p,sizeof(p),f%256,info[4])) return 2;
		oracle_strength(channel);
		for(i=0;i<sizeof(p);++i) if(p[i]!=reference_p[i]) {
			if(failures++<12) printf("MDNS strength frame=%u +%x got=%x OEM=%x\n",f,i,p[i],reference_p[i]);
			break;
		}
		if(t41_mdns_interpolate(p,sizeof(p),actual,sizeof(actual),gain)) return 2;
		oracle_interpolate(channel,gain);
		for(i=0;i<sizeof(actual);++i) if(actual[i]!=expected[i]) {
			if(failures++<12) printf("MDNS interpolate frame=%u +%x got=%x OEM=%x\n",f,i,actual[i],expected[i]);
			break;
		}
		memopt=(f%16)<<(channel*4);
		count=t41_mdns_pack_enable(p,sizeof(p),channel,f%16,words,T41_MDNS_WRITES);
		oracle_count=0; oracle_enable(channel); compare_words(words,count,f);
		if(memcmp(p,reference_p,sizeof(p))) ++failures;
		if(f&2) for(i=0;i<sizeof(actual);++i) actual[i]=rng();
		if(!actual[4]) actual[4]=1;
		/* Exclude only the OEM's zero-divisor combinations. Negative geometry
		 * and negative nonzero reciprocal denominators remain covered. */
		for(i=0x84;i<=0x86;i+=2) {
			unsigned int low=actual[i]<129 ? actual[i] : 128;
			if(low==(unsigned int)actual[i+1]+8) ++actual[i+1];
		}
		memcpy(expected,actual,sizeof(actual));
		info[5]=1+rng()%8192; info[6]=1+rng()%8192;
		count=t41_mdns_pack(p,sizeof(p),actual,sizeof(actual),channel,info[5],info[6],words,T41_MDNS_WRITES);
		oracle_count=0; oracle_pack(channel); compare_words(words,count,f);
	}
	printf("100000 MDNS ramp pairs; 10000 synthetic interpolation, enable and register cases: %u mismatches, %u unexpected writes\n",failures,oracle_bad_write);
	return failures || oracle_bad_write ? 1 : 0;
}
