/* Userspace-only, synthetic calibration and CSC inputs. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_bcsh.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_ct(unsigned int, unsigned int, unsigned int, unsigned int);
extern int oracle_ev(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
extern void oracle_hard(unsigned int), oracle_pack(unsigned int);
extern unsigned int oracle_words[29], oracle_bad_write;
static unsigned char p[360], reference[360], csc[92], info[336];
static uint32_t seed = 15;
static unsigned int random_u32(void) { seed ^= seed<<13; seed ^= seed>>17; seed ^= seed<<5; return seed; }
static void put16(unsigned char *at, unsigned int v) { at[0] = v; at[1] = v>>8; }
static void put32(unsigned char *at, unsigned int v) { put16(at,v); put16(at+2,v>>16); }
int main(void)
{
	static const unsigned int offsets[] = {0x42,0x54,0x6c,0x7e,0x90,0xa2,0xbe,0xd0,0xe2,0xf4,0x106};
	unsigned int f,i,j,words[29],fail = 0;
	*(uint32_t *)(void *)(oracle_bss+0x4710) = (uintptr_t)info;
	*(uint32_t *)(void *)info = (uintptr_t)reference;
	for (f = 0; f < 10000; ++f) {
		unsigned int ct = (f*17)%10000, ev = (f*11)%1000;
		unsigned int span = f & 2 ? 5000000 : 100;
		if (f & 2) ev = random_u32() % 50000000;
		for (i=0;i<9;++i) put32(p+i*4,10+i*span);
		for (i=0;i<6;++i) put16(p+0x24+i*2,1800+i*1000);
		for (i=0;i<11;++i) for(j=0;j<9;++j) put16(p+offsets[i]+j*2,random_u32()%1024);
		for (i=0;i<3;++i) {
			put16(p+0x30+i*6,1); put16(p+0x32+i*6,random_u32()%1024); put16(p+0x34+i*6,random_u32()%1024);
			put16(p+0x118+i*2,random_u32()%2049);
		}
		put16(p+0x66,1); put16(p+0x68,12); put16(p+0x6a,48); put16(p+0xb4,1);
		for(i=0;i<36;++i) put16(p + 0x11e + i*2,(random_u32()%4097)-2048);
		p[0x166] = f&1;
		for(i=0;i<9;++i) {
			put32(csc+4+i*4,(random_u32()%131073)-65536);
			put32(csc+48+i*4,(random_u32()%131073)-65536);
		}
		csc[40]=16; csc[42]=16; csc[43]=235; csc[44]=16; csc[45]=240;
		memcpy(info+212,csc,92); memcpy(reference,p,360);
		info[328]=255; info[329]=info[330]=info[331]=info[332]=128;
		info[172]=0; info[334]=0;
		oracle_ev(0,0,ev,0,1); oracle_ct(0,0,ct,1);
		oracle_hard(0); oracle_pack(0);
		if(t41_bcsh_compute(p,360,ct,ev,csc,92,words)) return 2;
		for(i=0;i<29;++i) if(words[i]!=oracle_words[i]) {
			if(fail++<20) printf("frame=%u word=%u scalar=%x OEM=%x\n",f,i,words[i],oracle_words[i]);
		}
	}
	printf("10000 synthetic neutral-API BCSH cases: %u mismatches, %u unexpected register destinations\n",fail,oracle_bad_write);
	return fail || oracle_bad_write ? 1 : 0;
}
