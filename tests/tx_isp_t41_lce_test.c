#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_lce.h"
static struct { unsigned int before; unsigned char s[T41_LCE_STATE_BYTES]; unsigned int after; } state;
static unsigned int seed=871238;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
int main(void)
{
	unsigned char p[T41_LCE_PARAM_BYTES];
	struct t41_dpc_word words[132];
	unsigned int iteration,i,j;
	memset(&state,0xa5,sizeof(state)); memset(words,0xa5,sizeof(words));
	assert(t41_lce_geometry(state.s,sizeof(state.s),0,1080,words)<0);
	assert(t41_lce_geometry(state.s,sizeof(state.s),1920,4,words)<0);
	assert(t41_lce_geometry(state.s,sizeof(state.s),65536,1080,words)<0);
	assert(t41_lce_geometry(state.s,sizeof(state.s)-1,1920,1080,words)<0);
	assert(t41_lce_process(NULL,sizeof(state.s),4)<0);
	assert(t41_lce_process(state.s+1,sizeof(state.s),4)<0);
	assert(t41_lce_process(state.s,sizeof(state.s)-1,4)<0);
	assert(t41_lce_pack_curve(state.s,sizeof(state.s),words,131)<0);
	assert(words[0].address==0xa5a5a5a5);
	for(iteration=0;iteration<2000;++iteration) {
		unsigned int w=640+rng()%3457,h=360+rng()%1801,ry=h%5,rx=(w/2)%9;
		unsigned int *hist=(unsigned int *)(void *)(state.s+0x1898),*sums=hist+1440;
		memset(state.s,0,sizeof(state.s)); memset(p,0,sizeof(p));
		state.s[0x592c]=iteration&1;
		t41_dpc_put16(p,iteration&1); t41_dpc_put16(p+2,1); t41_dpc_put16(p+4,rng()%32);
		t41_dpc_put16(p+6,128); t41_dpc_put16(p+8,256); t41_dpc_put16(p+10,512);
		t41_dpc_put16(p+12,rng()%1025); t41_lce_put32(p+0x40,rng()%129);
		for(i=0;i<2;++i) {
			unsigned int b=i ? 0xd8 : 0x54,c=i ? 0x114 : 0x90;
			p[b]=iteration%4; p[b+1]=rng()%32; p[b+2]=(iteration>>2)&1;
			p[b+3]=rng()%64; p[b+4]=rng()%64;
			for(j=0;j<55;++j) p[b+5+j]=1+rng()%128;
			p[c]=rng()%129; p[c+1]=rng()%5; p[c+2]=4+rng()%61;
			p[c+3]=(iteration>>3)&1; p[c+4]=(iteration>>4)&1;
			p[c+5]=rng()%256; p[c+6]=(iteration>>5)&1; p[c+7]=(iteration>>6)&1;
			for(j=0;j<64;++j) p[c+8+j]=rng()%129;
		}
		for(i=0;i<5;++i) p[0x30+i]=1+rng()%8;
		p[0x35]=(iteration>>1)&1; p[0x36]=rng()%256; p[0x37]=rng()%256;
		assert(!t41_lce_geometry(state.s,sizeof(state.s),w,h,words));
		assert(!t41_lce_calibrate(p,sizeof(p),state.s,sizeof(state.s),rng()%(10U<<16)));
		t41_lce_identity(state.s);
		for(i=0;i<45;++i) {
			unsigned int left=t41_tmo_le32(state.s+0x5784+4*((i/9<ry ? 0 : 2)+(i%9<rx ? 0 : 1)))/2;
			for(j=0;j<64;++j) { unsigned int n=rng()%(left+1); hist[i*32+rng()%32]+=n; left-=n; }
			hist[i*32+rng()%32]+=left;
			for(j=0;j<32;++j) sums[i]+=hist[i*32+j]*j*8;
		}
		assert(!t41_lce_process(state.s,sizeof(state.s),4));
		for(i=0;i<5;++i) assert(!t41_lce_process(state.s,sizeof(state.s),i));
		assert(t41_lce_pack_curve(state.s,sizeof(state.s),words,132)==132);
		assert(words[0].address==0xe080 && words[1].value==0x101);
		assert(words[130].address==0x501e0 && words[130].value==0x7f0102);
		assert(words[131].address==0xe084 && words[131].value==1);
		assert(state.before==0xa5a5a5a5 && state.after==0xa5a5a5a5);
	}
	puts("T41 LCE geometry, histogram/temporal pipeline, bounds and canaries: PASS");
	return 0;
}
