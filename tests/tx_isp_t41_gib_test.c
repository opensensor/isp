#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_gib.h"
int main(void)
{
	unsigned short black[5] = {0};
	unsigned int gain, values[4] = {1024,1024,1024,1024}, words[3], before[3];
	assert(!t41_gib_self_gain(black,0,&gain) && gain==4096);
	assert(!t41_gib_dgain(gain,0,values,words));
	assert(words[0]==0 && words[1]==0x04000400 && words[2]==words[1]);
	black[0]=black[1]=black[2]=black[3]=256;
	assert(!t41_gib_self_gain(black,0,&gain) && gain==4369);
	assert(!t41_gib_dgain(gain,0,values,words));
	assert(words[0]==0 && words[1]==0x04440444 && words[2]==words[1]);
	black[4]=2048;
	assert(!t41_gib_self_gain(black,0,&gain) && gain==4369);
	assert(!t41_gib_self_gain(black,1,&gain) && gain==8192);
	values[0]=32768; values[1]=values[2]=values[3]=1024;
	assert(!t41_gib_dgain(4096,0,values,words) && words[0]==1 && words[1]==0x08004000);
	values[0]=65536;
	assert(!t41_gib_dgain(4096,0,values,words) && words[0]==2 && words[1]==0x10004000);
	values[0]=131072;
	assert(!t41_gib_dgain(4096,0,values,words) && words[0]==3 && words[1]==0x40004000);
	memcpy(before,words,sizeof(words));
	assert(t41_gib_dgain(0,0,values,words));
	assert(t41_gib_dgain(8193,0,values,words));
	assert(t41_gib_dgain(4096,2,values,words));
	assert(!memcmp(before,words,sizeof(words)));
	assert(t41_gib_self_gain(black,2,&gain));
	puts("T41 black-range compensation and digital-gain packing: passed");
	return 0;
}
