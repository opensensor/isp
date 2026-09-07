/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_ae.h"
static unsigned char p[T41_AE_PARAM_BYTES], s[T41_AE_STATE_BYTES], saved[T41_AE_STATE_BYTES];
static void put16(unsigned char *at, unsigned int v) { at[0] = v; at[1] = v>>8; }
int main(void)
{
	unsigned char dma[4096];
	unsigned int i, target = 999;
	unsigned long long knots[15], adjusted[15];
	unsigned short targets[15], values[15];
	struct t41_ae_meter meter, before;
	memset(dma,255,sizeof(dma)); memset(s,0xa5,sizeof(s)); memcpy(saved,s,sizeof(s));
	assert(t41_ae_statistics(dma,15,1,1,s,sizeof(s)));
	assert(t41_ae_statistics(dma,sizeof(dma),0,15,s,sizeof(s)));
	assert(t41_ae_statistics(dma,sizeof(dma),15,16,s,sizeof(s)));
	assert(t41_ae_statistics(dma,sizeof(dma),15,15,s,sizeof(s)-1));
	assert(!memcmp(s,saved,sizeof(s)));
	assert(!t41_ae_statistics(dma,16,1,1,s,sizeof(s)));
	assert(t41_tmo_le32(s+0x800)==0x3fffff);
	assert(t41_tmo_le32(s+0xb84)==0x3fffff);
	assert(t41_tmo_le32(s+0xf08)==0x3fffff);
	assert(t41_tmo_le32(s+0x128c)==0x3fff);
	assert(t41_tmo_le32(s+0x1610)==0x3fff);
	assert(t41_tmo_le32(s+0x1994)==0x3fffff && s[0x219e]==3);
	assert(t41_tmo_le32(s+0x804)==0xa5a5a5a5);
	for (i=0;i<15;++i) { knots[i]=10+i*10; targets[i]=150-i*3; }
	assert(!t41_ae_long_target(0,knots,targets,10,&target) && target==150);
	assert(!t41_ae_long_target((65ULL<<10)+1023,knots,targets,10,&target) && target==134);
	assert(!t41_ae_long_target(1000ULL<<10,knots,targets,10,&target) && target==108);
	knots[5]=knots[4]; target=999;
	assert(t41_ae_long_target(0,knots,targets,10,&target) && target==999);
	knots[5]=60;
	assert(t41_ae_long_target(0,knots,targets,32,&target) && target==999);
	knots[14]=1ULL<<40;
	assert(t41_ae_long_target(0,knots,targets,10,&target) && target==999);
	knots[14]=150;
	for(i=0;i<15;++i) {
		t41_ae_put32(p+0x5d0+i*8,knots[i]); put16(p+0x76e + i*2,targets[i]);
		put16(p+0x7d4+i*2,128);
	}
	put16(p+0x7c6,128); put16(p+0x7c8,128);
	for(i=0;i<2;++i) {
		put16(p+0x7c4,i);
		assert(!t41_ae_target_tables(p,sizeof(p),adjusted,values));
		assert(!memcmp(adjusted,knots,sizeof(knots)) && !memcmp(values,targets,sizeof(targets)));
	}
	put16(p+0x7c4,2);
	assert(t41_ae_target_tables(p,sizeof(p),adjusted,values));
	assert(!memcmp(adjusted,knots,sizeof(knots)) && !memcmp(values,targets,sizeof(targets)));
	memset(s,0,sizeof(s));
	put16(p+0x70a,1); put16(p+0x70e,1); put16(p+0x6c0,10); put16(p+0x7b8,2);
	p[0x4ee]=4; p[0x82e]=1;
	put16(s+0x21fc,10); put16(s+0x221a,10);
	t41_ae_put32(s+0x800,1000); t41_ae_put32(s+0xb84,2000); t41_ae_put32(s+0xf08,3000);
	t41_ae_put32(s+0x128c,10); t41_ae_put32(s+0x1610,5);
	assert(!t41_ae_weight_mean(p,sizeof(p),s,sizeof(s),&meter));
	assert(meter.mean==60 && meter.foreground==60 && meter.background==60);
	assert(meter.bright_q==51 && meter.dark_q==102);
	before=meter; put16(p+0x7b8,0);
	assert(t41_ae_weight_mean(p,sizeof(p),s,sizeof(s),&meter));
	assert(!memcmp(&before,&meter,sizeof(meter)));
	put16(p+0x7b8,2); p[0x82e]=0;
	assert(t41_ae_weight_mean(p,sizeof(p),s,sizeof(s),&meter));
	p[0x82e]=1; p[0x4ee]=9;
	assert(t41_ae_weight_mean(p,sizeof(p),s,sizeof(s),&meter));
	p[0x4ee]=4; put16(s+0x21fc,0);
	assert(t41_ae_weight_mean(p,sizeof(p),s,sizeof(s),&meter));
	puts("T41 calibrated AE scalar tests passed");
	return 0;
}
