#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_long.h"
#include "../driver/t41/tx_isp_t41_awb_prior.h"
static struct { unsigned int before; unsigned char v[T41_AWB_STATE_BYTES]; unsigned int after; } state;
static unsigned char p[0x1400], saved_p[sizeof(p)], saved_s[sizeof(state.v)], report[1052];
static unsigned short red[6],blue[6];
static unsigned int seed=34681, calls, frames;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static int detector(void *context,void **view,unsigned int *ratios,unsigned int *failed)
{
	unsigned int fraction=t41_tmo_le16(p+0xcd4);
	(void)context;
	++calls;
	ratios[0]=(128+frames%128)<<fraction;
	ratios[1]=(256+frames%128)<<fraction;
	failed[0]=frames%17==0;
	t41_ae_put32(view[0xb],4000+frames%3000);
	return 0;
}
static void initialize(void)
{
	memset(p,0,sizeof(p)); memset(state.v,0,sizeof(state.v)); memset(report,0,sizeof(report));
	state.before=0x1234abcd; state.after=0x5678fedc; report[1050]=0xb7; report[1051]=0x4b;
	t41_awb_gain_put16(p+0xc6e,15); t41_awb_gain_put16(p+0xc72,15);
	t41_awb_gain_put16(p+0xcd2,16); t41_awb_gain_put16(p+0xcd4,8);
	t41_awb_gain_put16(p+0xcd6,5);
	t41_ae_put32(p+0x30,1024); t41_ae_put32(p+0x34,1024);
	t41_ae_put32(p+0x1c,1000); t41_ae_put32(p+0x20,10000);
	t41_ae_put32(p+0xe0,1); t41_ae_put32(p+0xe8,1);
}
static void history(void)
{
	unsigned int ratios[2],i,mean,inverse;
	initialize();
	for(i=0;i<4;++i) t41_ae_put32(p+0xc+i*4,256);
	t41_ae_put32(p+0x28,5000);
	ratios[0]=32768; ratios[1]=65536;
	assert(!t41_awb_long_history(p,state.v,ratios,0,16,8));
	assert(t41_tmo_le32(p+0xc)==512 && t41_tmo_le32(p+0x14)==512);
	assert(t41_tmo_le32(p+0x10)==256 && t41_tmo_le32(p+0x18)==256);
	for(i=0;i<15;++i) {
		assert(t41_tmo_le32(state.v+0xc604+i*4)==32768);
		assert(t41_tmo_le32(state.v+0xc640+i*4)==65536);
		assert(t41_tmo_le32(state.v+0xc67c+i*4)==5000);
	}
	ratios[0]=65536; ratios[1]=65536; t41_ae_put32(p+0x28,6000);
	assert(!t41_awb_long_history(p,state.v,ratios,0,16,8));
	mean=(50U*32768+15U*65536+32)/65;
	inverse=(unsigned int)(4294967296ULL/mean);
	assert(ratios[0]==mean);
	assert(t41_tmo_le32(p+0xc)==(inverse+128)/256);
	assert(t41_tmo_le32(p+0x14)==512-(512-(inverse+128)/256)/5);
	assert(t41_tmo_le32(p+0x28)==(50*5000+15*6000+32)/65);
	/* Complete a transition without restarting; malformed counter must not divide by zero. */
	t41_awb_gain_put16(p+0xcd8,65535); t41_ae_put32(p+4,1); t41_ae_put32(p+8,5);
	t41_ae_put32(state.v+0xeaa0,1);
	assert(t41_awb_long_history(p,state.v,ratios,1,16,8)==-1);
	t41_ae_put32(p+8,4);
	assert(!t41_awb_long_history(p,state.v,ratios,1,16,8));
	assert(!t41_tmo_le32(p+4) && t41_tmo_le32(p+8)==5);
	assert(t41_tmo_le32(p+0xc)==t41_tmo_le32(p+0x14));
	assert(t41_awb_slew_step(0,0x80000000U,1)==0x80000000U);
	assert(t41_awb_slew_step(0,7,3)==0U-2);
	initialize(); ratios[0]=0; ratios[1]=65536;
	assert(t41_awb_long_history(p,state.v,ratios,0,16,8)==-1);
}
int main(void)
{
	void *view[45]; unsigned int i,selection,bin,field,r,c;
	history(); initialize();
	for(frames=0;frames<2000;++frames) {
		if(frames%100==0) {
			initialize();
			t41_awb_gain_put16(p+0xc6e,1+rng()%15); t41_awb_gain_put16(p+0xc72,1+rng()%15);
			t41_awb_gain_put16(p+0xcd2,10+rng()%7); t41_awb_gain_put16(p+0xcd4,1+rng()%10);
			t41_awb_gain_put16(p+0xcd6,rng()%19);
		}
		for(i=0;i<8;++i) t41_awb_gain_put16(p+0xd46+i*2,rng()%17);
		t41_awb_gain_put16(p+0xd56,frames%4); t41_awb_gain_put16(p+0xd44,frames%6);
		for(selection=0;selection<2;++selection) for(bin=0;bin<4;++bin)
			for(field=0;field<5;++field) for(i=0;i<225;++i)
				t41_ae_put32(state.v+(selection ? 0x7f94 : 0x3944)+bin*0x1194+field*0x384+i*4,
					field==4 ? rng()%16384 : rng()%4000000);
		assert(!t41_awb_prior_prepare(p,sizeof(p),state.v,sizeof(state.v),view,red,blue));
		memcpy(saved_s,state.v,sizeof(saved_s));
		assert(!t41_awb_long(p,sizeof(p),state.v,sizeof(state.v),report,1050,view,detector,NULL));
		assert(state.before==0x1234abcd && state.after==0x5678fedc);
		assert(report[1050]==0xb7 && report[1051]==0x4b);
		assert(!memcmp(saved_s+0x3944,state.v+0x3944,0xc5f4-0x3944));
		for(r=0;r<15;++r) for(c=0;c<15;++c)
			if(r>=t41_tmo_le16(p+0xc6e) || c>=t41_tmo_le16(p+0xc72))
				for(i=0;i<15;++i)
					assert(!memcmp(saved_s+i*0x384+(r*15+c)*4,state.v+i*0x384+(r*15+c)*4,4));
	}
	memcpy(saved_p,p,sizeof(p)); memcpy(saved_s,state.v,sizeof(saved_s)); i=calls;
	assert(t41_awb_long(p,0xd57,state.v,sizeof(state.v),report,1050,view,detector,NULL)==-1);
	assert(t41_awb_long(p,sizeof(p),state.v+1,sizeof(state.v)-1,report,1050,view,detector,NULL)==-1);
	assert(t41_awb_long(p,sizeof(p),state.v,sizeof(state.v),report,1049,view,detector,NULL)==-1);
	assert(calls==i && !memcmp(p,saved_p,sizeof(p)) && !memcmp(state.v,saved_s,sizeof(saved_s)));
	printf("AWB long: 2000 frames, history/slew, padding/canaries and malformed-input checks PASS\n");
	return 0;
}
