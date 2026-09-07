#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_dpc.h"
static unsigned int seed=901;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
int main(void)
{
	unsigned char storage[T41_DPC_PARAM_BYTES+1], *p=storage+1;
	unsigned char state[T41_DPC_STATE_BYTES], saved[sizeof(state)];
	struct t41_dpc_word words[T41_DPC_OTHER_WRITES], original[T41_DPC_OTHER_WRITES];
	unsigned int i, f, bank;
	for(f=0;f<1000;++f) {
		for(i=0;i<T41_DPC_PARAM_BYTES;++i) p[i]=rng();
		memset(state,0xa5,sizeof(state));
		for(bank=0;bank<2;++bank) {
			assert(!t41_dpc_interpolate_bank(p,T41_DPC_PARAM_BYTES,state,sizeof(state),0,bank));
			assert(t41_tmo_le16(state+(bank ? 0x26 : 0))==t41_tmo_le16(p+(bank ? 8 : 0x31e)));
			assert(!t41_dpc_interpolate_bank(p,T41_DPC_PARAM_BYTES,state,sizeof(state),~0U,bank));
			assert(t41_tmo_le16(state+(bank ? 0x26 : 0))==t41_tmo_le16(p+(bank ? 8 : 0x31e)+20));
			assert(t41_dpc_pack_bank(p,T41_DPC_PARAM_BYTES,state,sizeof(state),bank,words,T41_DPC_OTHER_WRITES)==(bank ? 19 : 28));
		}
		assert(state[5]==0xa5 && state[0x2b]==0xa5 && state[0x4c]==0xa5 && state[0x4d]==0xa5);
		for(bank=0;bank<2;++bank) {
			int count=t41_dpc_pack_other(p,T41_DPC_PARAM_BYTES,bank,words,T41_DPC_OTHER_WRITES);
			assert(count==(bank ? 71 : 62));
			for(i=0;i<(unsigned int)count;++i) {
				assert(words[i].address!=0x7098);
				assert(words[i].address<0x7088 || words[i].address>0x7090);
			}
		}
	}
	memcpy(saved,state,sizeof(state)); memset(words,0xa5,sizeof(words)); memcpy(original,words,sizeof(words));
	assert(t41_dpc_interpolate_bank(p,T41_DPC_PARAM_BYTES-1,state,sizeof(state),0,0));
	assert(t41_dpc_interpolate_bank(p,T41_DPC_PARAM_BYTES,state,sizeof(state)-1,0,0));
	assert(t41_dpc_interpolate_bank(p,T41_DPC_PARAM_BYTES,state,sizeof(state),0,2));
	assert(!memcmp(saved,state,sizeof(state)));
	assert(t41_dpc_pack_bank(p,T41_DPC_PARAM_BYTES,state,sizeof(state),0,words,27)<0);
	assert(t41_dpc_pack_other(p,T41_DPC_PARAM_BYTES,0,words,70)<0);
	assert(t41_dpc_pack_other(p,T41_DPC_PARAM_BYTES,2,words,T41_DPC_OTHER_WRITES)<0);
	assert(!memcmp(original,words,sizeof(words)));
	assert(t41_dpc_neighbors(0)==0 && t41_dpc_neighbors(1)==0x10001);
	assert(t41_dpc_neighbors(4)==t41_dpc_neighbors(255));
	puts("T41 DPC: unaligned calibration, gain endpoints, bank isolation, register ownership and rejection passed");
	return 0;
}
