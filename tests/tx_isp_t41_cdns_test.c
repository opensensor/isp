#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_cdns.h"
int main(void)
{
	unsigned char p[T41_CDNS_PARAM_BYTES+2],s[T41_CDNS_STATE_BYTES+2],before[sizeof(s)];
	struct t41_dpc_word words[14],saved[14];
	unsigned int i,gain;
	memset(p,0,sizeof(p)); memset(s,0xa5,sizeof(s)); memset(words,0xa7,sizeof(words));
	memcpy(before,s,sizeof(s)); memcpy(saved,words,sizeof(words));
	assert(t41_cdns_interpolate(p+1,T41_CDNS_PARAM_BYTES-1,s+1,T41_CDNS_STATE_BYTES,0)<0);
	assert(t41_cdns_interpolate(p+1,T41_CDNS_PARAM_BYTES,s+1,T41_CDNS_STATE_BYTES-1,0)<0);
	assert(t41_cdns_interpolate(p+1,T41_CDNS_PARAM_BYTES,s+1,T41_CDNS_STATE_BYTES,~0U)<0);
	assert(!memcmp(s,before,sizeof(s)));
	assert(t41_cdns_pack(p+1,T41_CDNS_PARAM_BYTES,s+1,T41_CDNS_STATE_BYTES,2,words+1,12)<0);
	assert(t41_cdns_pack(p+1,T41_CDNS_PARAM_BYTES,s+1,T41_CDNS_STATE_BYTES,0,words+1,11)<0);
	assert(!memcmp(words,saved,sizeof(words)));
	for(i=0;i<11;++i) p[1+0x17+i]=i*20;
	for(gain=0;gain<=16U<<16;gain+=1024) {
		unsigned int clamped=gain>10U<<16 ? 10U<<16 : gain;
		assert(!t41_cdns_interpolate(p+1,T41_CDNS_PARAM_BYTES,s+1,T41_CDNS_STATE_BYTES,gain));
		assert(s[1]==((clamped*20U+32768)>>16));
		assert(s[0]==0xa5 && s[sizeof(s)-1]==0xa5);
		assert(t41_cdns_pack(p+1,T41_CDNS_PARAM_BYTES,s+1,T41_CDNS_STATE_BYTES,0,words+1,12)==12);
		assert(!memcmp(words,saved,sizeof(words[0])));
		assert(!memcmp(words+13,saved+13,sizeof(words[0])));
	}
	for(i=0;i<65536;++i) {
		int lower=(int)(i&255)-128,upper=(int)(i>>8)-128;
		unsigned int expected;
		s[31]=(unsigned char)lower; s[32]=(unsigned char)upper;
		if(upper<lower) upper=lower;
		expected=((unsigned int)lower&255) | ((unsigned int)upper&255)<<8;
		if(upper!=lower) expected|=(unsigned int)(4080/(upper-lower))<<16;
		assert(t41_cdns_pack(p+1,T41_CDNS_PARAM_BYTES,s+1,T41_CDNS_STATE_BYTES,0,words+1,12)==12);
		assert(words[12].value==expected);
	}
	puts("t41 CDNS interpolation, signed thresholds, packing, unaligned buffers and bounds: ok");
	return 0;
}
