#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_dmsc.h"
int main(void)
{
	unsigned char p[T41_DMSC_PARAM_BYTES+2], s[T41_DMSC_STATE_BYTES+2], before[sizeof(s)];
	struct t41_dpc_word words[T41_DMSC_WRITES+2], saved[sizeof(words)/sizeof(words[0])];
	unsigned int i, gain;
	memset(p,0,sizeof(p)); memset(s,0xa5,sizeof(s)); memset(words,0xa7,sizeof(words));
	memcpy(before,s,sizeof(s)); memcpy(saved,words,sizeof(words));
	assert(t41_dmsc_interpolate(p+1,T41_DMSC_PARAM_BYTES-1,s+1,T41_DMSC_STATE_BYTES,0,128)<0);
	assert(t41_dmsc_interpolate(p+1,T41_DMSC_PARAM_BYTES,s+1,T41_DMSC_STATE_BYTES-1,0,128)<0);
	assert(t41_dmsc_interpolate(p+1,T41_DMSC_PARAM_BYTES,s+1,T41_DMSC_STATE_BYTES,~0U,128)<0);
	assert(t41_dmsc_interpolate(p+1,T41_DMSC_PARAM_BYTES,s+1,T41_DMSC_STATE_BYTES,0,256)<0);
	assert(!memcmp(s,before,sizeof(s)));
	assert(t41_dmsc_pack_static(p+1,T41_DMSC_PARAM_BYTES,words+1,T41_DMSC_WRITES-1)<0);
	assert(t41_dmsc_pack_dynamic(p+1,T41_DMSC_PARAM_BYTES,s+1,T41_DMSC_STATE_BYTES,words+1,T41_DMSC_WRITES-1)<0);
	assert(!memcmp(words,saved,sizeof(words)));
	for(i=0;i<11;++i) { p[1+0x8b+i]=i*20; t41_dpc_put16(p+1+0xf0+i*2,i*40); }
	for(gain=0;gain<=16U<<16;gain+=1024) {
		unsigned int clamped=gain>10U<<16 ? 10U<<16 : gain;
		assert(!t41_dmsc_interpolate(p+1,T41_DMSC_PARAM_BYTES,s+1,T41_DMSC_STATE_BYTES,gain,128));
		assert(s[1] == ((clamped*20U+32768)>>16));
		assert(t41_tmo_le16(s+1+0xe) == ((clamped*40U+32768)>>16));
		assert(s[0]==0xa5 && s[sizeof(s)-1]==0xa5 && s[1+0xd]==0xa5);
		assert(t41_dmsc_pack_static(p+1,T41_DMSC_PARAM_BYTES,words+1,T41_DMSC_WRITES)==36);
		assert(t41_dmsc_pack_dynamic(p+1,T41_DMSC_PARAM_BYTES,s+1,T41_DMSC_STATE_BYTES,words+1,T41_DMSC_WRITES)==125);
		assert(!memcmp(words,saved,sizeof(words[0])));
		assert(!memcmp(words+T41_DMSC_WRITES+1,saved+T41_DMSC_WRITES+1,sizeof(words[0])));
	}
	assert(t41_dmsc_ratio(0,100)==0 && t41_dmsc_ratio(128,100)==100);
	assert(t41_dmsc_ratio(255,100)==596);
	puts("t41 DMSC interpolation, packing, unaligned buffers and bounds: ok");
	return 0;
}
