#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_sdns.h"
int main(void)
{
	unsigned char p[T41_SDNS_PARAM_BYTES+2], s[T41_SDNS_STATE_BYTES+2], before[sizeof(s)];
	struct t41_dpc_word words[31], saved[31];
	unsigned int i, gain;
	memset(p,0,sizeof(p)); memset(s,0xa5,sizeof(s)); memset(words,0xa7,sizeof(words));
	memcpy(before,s,sizeof(s)); memcpy(saved,words,sizeof(words));
	assert(t41_sdns_interpolate(p+1,T41_SDNS_PARAM_BYTES-1,s+1,T41_SDNS_STATE_BYTES,0,128)<0);
	assert(t41_sdns_interpolate(p+1,T41_SDNS_PARAM_BYTES,s+1,T41_SDNS_STATE_BYTES-1,0,128)<0);
	assert(t41_sdns_interpolate(p+1,T41_SDNS_PARAM_BYTES,s+1,T41_SDNS_STATE_BYTES,~0U,128)<0);
	assert(t41_sdns_interpolate(p+1,T41_SDNS_PARAM_BYTES,s+1,T41_SDNS_STATE_BYTES,0,256)<0);
	assert(!memcmp(s,before,sizeof(s)));
	assert(t41_sdns_pack_static(p+1,T41_SDNS_PARAM_BYTES,2,words+1,29)<0);
	assert(t41_sdns_pack_dynamic(p+1,T41_SDNS_PARAM_BYTES,s+1,T41_SDNS_STATE_BYTES,0,words+1,26)<0);
	assert(!memcmp(words,saved,sizeof(words)));
	for(i=0;i<11;++i) { p[1+0x6e + i]=i*20; t41_dpc_put16(p+1+0x58+i*2,i*40); }
	for(gain=0;gain<=16U<<16;gain+=1024) {
		unsigned int clamped=gain>10U<<16 ? 10U<<16 : gain;
		assert(!t41_sdns_interpolate(p+1,T41_SDNS_PARAM_BYTES,s+1,T41_SDNS_STATE_BYTES,gain,128));
		assert(s[1]==((clamped*20U+32768)>>16));
		assert(t41_tmo_le16(s+5)==((clamped*40U+32768)>>16));
		assert(s[0]==0xa5 && s[sizeof(s)-1]==0xa5 && s[1+75]==0xa5);
		assert(t41_sdns_pack_static(p+1,T41_SDNS_PARAM_BYTES,0,words+1,29)==29);
		assert(t41_sdns_pack_dynamic(p+1,T41_SDNS_PARAM_BYTES,s+1,T41_SDNS_STATE_BYTES,0,words+1,29)==27);
		assert(!memcmp(words,saved,sizeof(words[0])));
		assert(!memcmp(words+30,saved+30,sizeof(words[0])));
	}
	s[1+0x4a]=1;
	assert(t41_sdns_pack_dynamic(p+1,T41_SDNS_PARAM_BYTES,s+1,T41_SDNS_STATE_BYTES,0,words,29)==27);
	assert(words[25].address==0x1402c && words[25].value==1U<<18);
	puts("t41 SDNS interpolation, packing, unaligned buffers and bounds: ok");
	return 0;
}
