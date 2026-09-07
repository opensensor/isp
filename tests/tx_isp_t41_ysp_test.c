#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_ysp.h"
int main(void)
{
	unsigned char p[T41_YSP_PARAM_BYTES+2],s[T41_YSP_STATE_BYTES+2],before[sizeof(s)];
	struct t41_dpc_word words[79],saved[79];
	unsigned int i,gain;
	memset(p,0,sizeof(p)); memset(s,0xa5,sizeof(s)); memset(words,0xa7,sizeof(words));
	memcpy(before,s,sizeof(s)); memcpy(saved,words,sizeof(words));
	assert(t41_ysp_interpolate(p+1,T41_YSP_PARAM_BYTES-1,s+1,T41_YSP_STATE_BYTES,0)<0);
	assert(t41_ysp_interpolate(p+1,T41_YSP_PARAM_BYTES,s+1,T41_YSP_STATE_BYTES-1,0)<0);
	assert(t41_ysp_interpolate(p+1,T41_YSP_PARAM_BYTES,s+1,T41_YSP_STATE_BYTES,~0U)<0);
	assert(!memcmp(s,before,sizeof(s)));
	assert(t41_ysp_pack_static(p+1,T41_YSP_PARAM_BYTES,words+1,12)<0);
	assert(t41_ysp_pack_dynamic(p+1,T41_YSP_PARAM_BYTES,s+1,T41_YSP_STATE_BYTES,words+1,76)<0);
	assert(!memcmp(words,saved,sizeof(words)));
	for(i=0;i<11;++i) p[1+0x70c+i]=i*20;
	for(gain=0;gain<=16U<<16;gain+=1024) {
		unsigned int clamped=gain>10U<<16 ? 10U<<16 : gain;
		assert(!t41_ysp_interpolate(p+1,T41_YSP_PARAM_BYTES,s+1,T41_YSP_STATE_BYTES,gain));
		assert(t41_tmo_le16(s+1+0x12)==((clamped*20U+32768)>>16));
		assert(s[0]==0xa5 && s[sizeof(s)-1]==0xa5 && s[2]==0xa5 && s[16]==0xa5);
		assert(t41_ysp_pack_static(p+1,T41_YSP_PARAM_BYTES,words+1,77)==13);
		assert(t41_ysp_pack_dynamic(p+1,T41_YSP_PARAM_BYTES,s+1,T41_YSP_STATE_BYTES,words+1,77)==77);
		assert(!memcmp(words,saved,sizeof(words[0])));
		assert(!memcmp(words+78,saved+78,sizeof(words[0])));
	}
	for(i=0;i<256;++i) {
		unsigned char ramp[]={250,i,7};
		unsigned int k;
		for(k=0;k<8;++k) {
			int expected=250+(int)k*((int)i-128);
			unsigned int word=t41_ysp_ramp_word(ramp,(k/4)*4);
			if(expected<7) expected=7;
			if(expected>250) expected=250;
			assert(((word>>((k%4)*8))&255)==(unsigned int)expected);
		}
	}
	puts("t41 YSP interpolation, ramps, packing, unaligned buffers and bounds: ok");
	return 0;
}
