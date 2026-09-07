#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_gamma.h"
static void put32(unsigned char *p, unsigned int v)
{ p[0]=v; p[1]=v>>8; p[2]=v>>16; p[3]=v>>24; }
int main(void)
{
	unsigned char storage[T41_GAMMA_PARAM_BYTES+1]={0}, *p=storage+1;
	unsigned short curve[129], saved[129];
	unsigned int i, strength=77;
	for(i=0;i<10;++i) { put32(p+0x104+i*4,100+i*100); p[0x22e + i]=i*20; }
	for(i=0;i<129;++i) { unsigned int v=(i*29)%4096; p[0x12c+i*2]=v; p[0x12d+i*2]=v>>8; }
	assert(!t41_gamma_strength(p,T41_GAMMA_PARAM_BYTES,250,0,&strength) && strength==30);
	assert(!t41_gamma_strength(p,T41_GAMMA_PARAM_BYTES,0,0,&strength) && !strength);
	assert(!t41_gamma_strength(p,T41_GAMMA_PARAM_BYTES,~0U,0,&strength) && strength==180);
	assert(!t41_gamma_strength(p,T41_GAMMA_PARAM_BYTES,0,1,&strength) && strength==180);
	assert(!t41_gamma_curve(p,T41_GAMMA_PARAM_BYTES,0,curve));
	for(i=0;i<128;++i) assert(curve[i]==i*32);
	assert(curve[128]==4095);
	assert(!t41_gamma_curve(p,T41_GAMMA_PARAM_BYTES,255,curve));
	for(i=0;i<128;++i) assert(curve[i]==t41_tmo_le16(p+0x12c+i*2));
	memcpy(saved,curve,sizeof(curve));
	p[0x12d]=0x10;
	assert(t41_gamma_curve(p,T41_GAMMA_PARAM_BYTES,1,curve));
	assert(t41_gamma_curve(p,T41_GAMMA_PARAM_BYTES,256,curve));
	assert(!memcmp(saved,curve,sizeof(curve)));
	put32(p+0x108,100); strength=77;
	assert(t41_gamma_strength(p,T41_GAMMA_PARAM_BYTES,0,0,&strength) && strength==77);
	assert(t41_gamma_strength(p,T41_GAMMA_PARAM_BYTES-1,0,0,&strength) && strength==77);
	puts("T41 gamma: unaligned calibration, EV endpoints, interpolation and fail-closed validation passed");
	return 0;
}
