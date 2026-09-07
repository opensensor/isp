#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_mdns.h"
static unsigned int seed=87139;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
int main(void)
{
	unsigned char p[T41_MDNS_PARAM_BYTES+2],s[T41_MDNS_STATE_BYTES+2],before[sizeof(s)],original[0x48e];
	struct t41_dpc_word words[130],saved[130];
	unsigned int i,f;
	memset(p,0,sizeof(p)); memset(s,0xa5,sizeof(s)); memset(words,0xa7,sizeof(words));
	memcpy(before,s,sizeof(s)); memcpy(saved,words,sizeof(words));
	assert(t41_mdns_interpolate(p+1,T41_MDNS_PARAM_BYTES-1,s+1,T41_MDNS_STATE_BYTES,0)<0);
	assert(t41_mdns_interpolate(p+1,T41_MDNS_PARAM_BYTES,s+1,T41_MDNS_STATE_BYTES-1,0)<0);
	assert(t41_mdns_interpolate(p+1,T41_MDNS_PARAM_BYTES,s+1,T41_MDNS_STATE_BYTES,~0U)<0);
	assert(!memcmp(s,before,sizeof(s)));
	assert(t41_mdns_pack(p+1,T41_MDNS_PARAM_BYTES,s+1,T41_MDNS_STATE_BYTES,2,640,480,words+1,128)<0);
	assert(t41_mdns_pack(p+1,T41_MDNS_PARAM_BYTES,s+1,T41_MDNS_STATE_BYTES,0,640,480,words+1,127)<0);
	s[5]=0;
	assert(t41_mdns_pack(p+1,T41_MDNS_PARAM_BYTES,s+1,T41_MDNS_STATE_BYTES,0,640,480,words+1,128)<0);
	s[5]=20; s[1+0x84]=100; s[1+0x85]=92;
	assert(t41_mdns_pack(p+1,T41_MDNS_PARAM_BYTES,s+1,T41_MDNS_STATE_BYTES,0,640,480,words+1,128)<0);
	assert(!memcmp(words,saved,sizeof(words)));
	for(f=0;f<5000;++f) {
		int count;
		for(i=1;i<sizeof(p)-1;++i) p[i]=rng();
		for(i=0;i<sizeof(original);++i) original[i]=rng();
		assert(!t41_mdns_strength(original,sizeof(original),p+1,T41_MDNS_PARAM_BYTES,f%256,f&1));
		assert(!t41_mdns_interpolate(p+1,T41_MDNS_PARAM_BYTES,s+1,T41_MDNS_STATE_BYTES,rng()%((16U<<16)+1)));
		assert(s[0]==0xa5 && s[sizeof(s)-1]==0xa5);
		count=t41_mdns_pack(p+1,T41_MDNS_PARAM_BYTES,s+1,T41_MDNS_STATE_BYTES,f&1,
			1+rng()%8192,1+rng()%8192,words+1,128);
		assert(count<0 || (count>0 && count<=128));
		assert(!memcmp(words,saved,sizeof(words[0])));
		assert(!memcmp(words+129,saved+129,sizeof(words[0])));
		assert(t41_mdns_pack_enable(p+1,T41_MDNS_PARAM_BYTES,f&1,f%16,words+1,128)==2);
	}
	puts("t41 MDNS ramps, strength, interpolation, geometry, unaligned buffers and zero-divisor guards: ok");
	return 0;
}
