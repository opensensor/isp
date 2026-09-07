#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_ydns.h"
int main(void)
{
	unsigned char p[T41_YDNS_PARAM_BYTES+2],s[T41_YDNS_STATE_BYTES+2],before[sizeof(s)];
	struct t41_dpc_word words[10],saved[10];
	unsigned int i,gain;
	memset(p,0,sizeof(p)); memset(s,0xa5,sizeof(s)); memset(words,0xa7,sizeof(words));
	memcpy(before,s,sizeof(s)); memcpy(saved,words,sizeof(words));
	assert(t41_ydns_interpolate(p+1,T41_YDNS_PARAM_BYTES-1,s+1,T41_YDNS_STATE_BYTES,0)<0);
	assert(t41_ydns_interpolate(p+1,T41_YDNS_PARAM_BYTES,s+1,T41_YDNS_STATE_BYTES-1,0)<0);
	assert(t41_ydns_interpolate(p+1,T41_YDNS_PARAM_BYTES,s+1,T41_YDNS_STATE_BYTES,~0U)<0);
	assert(!memcmp(s,before,sizeof(s)));
	assert(t41_ydns_pack(p+1,T41_YDNS_PARAM_BYTES,s+1,T41_YDNS_STATE_BYTES,2,words+1,8)<0);
	assert(t41_ydns_pack(p+1,T41_YDNS_PARAM_BYTES,s+1,T41_YDNS_STATE_BYTES,0,words+1,7)<0);
	assert(!memcmp(words,saved,sizeof(words)));
	for(i=0;i<11;++i) p[1+0x19+i]=i*20;
	for(gain=0;gain<=16U<<16;gain+=1024) {
		unsigned int clamped=gain>10U<<16 ? 10U<<16 : gain;
		assert(!t41_ydns_interpolate(p+1,T41_YDNS_PARAM_BYTES,s+1,T41_YDNS_STATE_BYTES,gain));
		assert(s[1]==((clamped*20U+32768)>>16));
		assert(s[0]==0xa5 && s[sizeof(s)-1]==0xa5);
		assert(t41_ydns_pack(p+1,T41_YDNS_PARAM_BYTES,s+1,T41_YDNS_STATE_BYTES,0,words+1,8)==8);
		assert(!memcmp(words,saved,sizeof(words[0])));
		assert(!memcmp(words+9,saved+9,sizeof(words[0])));
	}
	puts("t41 YDNS interpolation, packing, unaligned buffers and bounds: ok");
	return 0;
}
