#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_prior.h"
#include "../driver/t41/tx_isp_t41_awb_special.h"
#include "../driver/t41/tx_isp_t41_awb_ct.h"

static struct { unsigned char head[16], p[0x1400], tail[16]; } cal;
static struct { unsigned char head[16], s[T41_AWB_STATE_BYTES], tail[16]; } state;
static unsigned char dma[32768], saved[T41_AWB_STATE_BYTES], saved_p[sizeof(cal.p)];
static unsigned int planes[4][225], weights[225], before[225], gains[2];
static uint32_t seed=5541;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static void put16(unsigned int off,unsigned int v) { t41_awb_gain_put16(cal.p+off,v); }

int main(void)
{
	unsigned int f,i;
	unsigned short cr[6]={0}, cb[6]={0}; void *slots[45];
	struct t41_awb_register reg[18]; unsigned int nr, rgb[3]={128,256,384};
	unsigned char *p=cal.p, *s=state.s;
	memset(&cal,0xa5,sizeof(cal)); memset(&state,0xa5,sizeof(state));
	memset(p,0,sizeof(cal.p)); memset(s,0,sizeof(state.s));
	put16(0xc6e,1); put16(0xc72,1);
	/* First full record in each selection. Global and neutral remain distinct. */
	t41_ae_put32(dma,3); t41_ae_put32(dma+16,7);
	assert(!t41_awb_statistics(p,sizeof(cal.p),dma,sizeof(dma),s,sizeof(state.s)));
	assert(t41_tmo_le32(s+0x7f94)==3 && t41_tmo_le32(s+0x3944)==7);
	/* Selected-class mode must not destroy the other three class histories. */
	memcpy(saved,s,sizeof(saved)); p[0xcca]=1; p[0xccc]=3;
	t41_ae_put32(dma,11);
	assert(!t41_awb_statistics(p,sizeof(cal.p),dma,32,s,sizeof(state.s)));
	assert(t41_tmo_le32(s+0xb450)==11);
	assert(!memcmp(s+0x7f94,saved+0x7f94,3*0x1194));
	memcpy(saved,s,sizeof(saved));
	assert(t41_awb_statistics(p,sizeof(cal.p),dma,31,s,sizeof(state.s))<0);
	assert(!memcmp(s,saved,sizeof(saved)));
	put16(0xc6e,16);
	assert(t41_awb_statistics(p,sizeof(cal.p),dma,sizeof(dma),s,sizeof(state.s))<0);
	assert(!memcmp(s,saved,sizeof(saved))); put16(0xc6e,1);
	put16(0xcd2,10); put16(0xd56,1); gains[0]=gains[1]=1024;
	planes[0][0]=100; planes[1][0]=200; planes[2][0]=400; planes[3][0]=1;
	assert(!t41_awb_saturation_weights(p,sizeof(cal.p),planes[0],planes[1],planes[2],planes[3],gains,weights));
	assert(weights[0]==256);
	put16(0xd56,2);
	assert(!t41_awb_saturation_weights(p,sizeof(cal.p),planes[0],planes[1],planes[2],planes[3],gains,weights));
	assert(weights[0]==102400/233);
	planes[0][0]=planes[2][0]=1; planes[1][0]=~0U;
	memcpy(before,weights,sizeof(weights));
	assert(t41_awb_saturation_weights(p,sizeof(cal.p),planes[0],planes[1],planes[2],planes[3],gains,weights)<0);
	assert(!memcmp(before,weights,sizeof(weights)));

	for(f=0;f<2000;++f) {
		for(i=0;i<sizeof(cal.p);++i) p[i]=rng();
		for(i=0;i<sizeof(state.s);++i) s[i]=rng();
		for(i=0;i<sizeof(dma);++i) dma[i]=rng();
		put16(0xc6e,1+rng()%15); put16(0xc72,1+rng()%15);
		p[0xcca]=f&1; p[0xccc]=(f/2)%4;
		assert(!t41_awb_statistics(p,sizeof(cal.p),dma,sizeof(dma),s,sizeof(state.s)));
		put16(0xcd2,10+rng()%7); put16(0xd56,f%4);
		for(i=0;i<225;++i) {
			unsigned int j;
			for(j=0;j<4;++j) planes[j][i]=rng()%4000000;
		}
		assert(!t41_awb_saturation_weights(p,sizeof(cal.p),planes[0],planes[1],planes[2],planes[3],gains,weights));
		t41_ae_put32(p+0x1c,1000); t41_ae_put32(p+0x20,4000);
		t41_ae_put32(p+0xe0,f&1); t41_ae_put32(p+0xe8,(f/2)&1);
		t41_ae_put32(s+0xea98,rng()%6000);
		assert(!t41_awb_prior_prepare(p,sizeof(cal.p),s,sizeof(state.s),slots,cr,cb));
		assert(slots[0]==p+0xce2 && slots[44]==s+0xe714 && slots[22]==cr && slots[23]==cb);
		assert(t41_tmo_le32(p+0x2c)==t41_tmo_le32(p+0x28));
		for(i=0;i<10;++i) p[0x11f6+i]=rng()%4;
		s[0x34de]=f&1; s[0x34df]=f%3;
		for(i=0;i<3;++i) rgb[i]=rng();
		if(f%11==0) rgb[1]=0;
		assert(!t41_awb_special_prepare(p,sizeof(cal.p),s,sizeof(state.s),rgb,reg,&nr));
		assert(nr>=6 && nr<=18);
		for(i=0;i<nr;++i) assert(reg[i].address>=0x18054 && reg[i].address<=0x180a0);
		{
			unsigned int ct, fraction=1+f%10;
			put16(0xcd4,fraction);
			for(i=0;i<15;++i) {
				t41_ae_put32(p+0x38+i*4,20+i*20);
				t41_ae_put32(p+0x74+i*4,30+i*20);
			}
			for(i=0;i<225;++i) t41_ae_put32(p+0x870+i*4,f%7 ? 250 : 0);
			assert(!t41_awb_ct_calculate(p,sizeof(cal.p),rng()%(340U<<fraction),rng()%(340U<<fraction),&ct));
			assert(ct==(f%7 ? 4000U : 5000U));
			assert(!t41_awb_ct_calculate(p,sizeof(cal.p),~0U,~0U,&ct));
			assert(ct==(f%7 ? 4000U : 5000U));
			t41_ae_put32(p+0x38,40); ct=123;
			assert(t41_awb_ct_calculate(p,sizeof(cal.p),0,0,&ct)<0 && ct==123);
		}
		for(i=0;i<16;++i) assert(cal.head[i]==0xa5 && cal.tail[i]==0xa5 && state.head[i]==0xa5 && state.tail[i]==0xa5);
	}
	memcpy(saved,s,sizeof(saved)); memcpy(saved_p,p,sizeof(saved_p));
	assert(t41_awb_prior_prepare(p,0x1200,s,sizeof(state.s),slots,cr,cb)<0);
	assert(t41_awb_special_prepare(p,sizeof(cal.p),s,sizeof(state.s)-1,rgb,reg,&nr)<0);
	assert(!memcmp(saved,s,sizeof(saved)) && !memcmp(saved_p,p,sizeof(saved_p)));
	puts("AWB statistics/prior/special/CT: 2000 randomized frames, bounds, padding, rejected divisors and canaries passed");
	return 0;
}
