/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_ae_alloc.h"
static unsigned char p[T41_AE_PARAM_BYTES], s[T41_AE_STATE_BYTES], saved[T41_AE_STATE_BYTES];
static void put16(unsigned char *at, unsigned int v) { at[0] = v; at[1] = v>>8; }
int main(void)
{
	unsigned char dma[4096];
	unsigned int i, target = 999;
	unsigned long long knots[15], adjusted[15];
	unsigned short targets[15], values[15];
	struct t41_ae_meter meter, before;
	assert(t41_ae_fixed_mul(0, 3, 7) == 84);
	assert(t41_ae_fixed_mul64(10, 3ULL << 40, 1ULL << 10) == 3ULL << 40);
	assert(t41_ae_fixed_div64(10, 3, 2) == 1536);
	assert(t41_ae_fixed_div64(10, ~0ULL, 1ULL << 63) == 2047);
	assert(t41_ae_fixed_div64(10, ~0ULL - 1, ~0ULL) == 0);
	{
		unsigned char cache[0x688] = {0};
		struct t41_ae_allocation allocation = {0}, original;
		unsigned int masks[] = {0x4f8, 0x4f9, 0x4fb, 0x500};
		put16(p + 0x6c0, 10);
		t41_ae_put32(cache + 0x260, 2); t41_ae_put32(cache + 0x270, 1000);
		t41_ae_put32(cache + 0x264, 1024); t41_ae_put32(cache + 0x274, 8192);
		t41_ae_put32(cache + 0x26c, 1024); t41_ae_put32(cache + 0x27c, 4096);
		assert(!t41_ae_auto_allocate(p,sizeof(p),s,sizeof(s),cache,sizeof(cache),0,25,&allocation));
		assert(allocation.integration == 2 && allocation.again == 1024 && allocation.dgain == 1024);
		assert(!t41_ae_auto_allocate(p,sizeof(p),s,sizeof(s),cache,sizeof(cache),1500ULL<<10,25,&allocation));
		assert(allocation.integration == 1000 && allocation.again == 1536 && allocation.dgain == 1024);
		assert(!t41_ae_auto_allocate(p,sizeof(p),s,sizeof(s),cache,sizeof(cache),16000ULL<<10,25,&allocation));
		assert(allocation.integration == 1000 && allocation.again == 8192 && allocation.dgain == 2048);
		assert(!t41_ae_auto_allocate(p,sizeof(p),s,sizeof(s),cache,sizeof(cache),33000ULL<<10,25,&allocation));
		assert(allocation.integration == 1000 && allocation.again == 8192 && allocation.dgain == 4096);
		assert(allocation.saturated_frames == 1 && allocation.settled == 1);
		original = allocation;
		for (i = 0; i < sizeof(masks)/sizeof(masks[0]); ++i) {
			cache[masks[i]] = 1;
			assert(t41_ae_auto_allocate(p,sizeof(p),s,sizeof(s),cache,sizeof(cache),0,25,&allocation));
			assert(!memcmp(&original,&allocation,sizeof(allocation)));
			cache[masks[i]] = 0;
		}
		put16(p + 0x7a0, 1); s[0x2612] = 0;
		put16(s + 0x2438, 3000); put16(s + 0x243a, 3000);
		assert(t41_ae_auto_allocate(p,sizeof(p),s,sizeof(s),cache,sizeof(cache),4000ULL<<10,25,&allocation));
		assert(!memcmp(&original,&allocation,sizeof(allocation)));
		s[0x2612] = 120;
		assert(t41_ae_auto_allocate(p,sizeof(p),s,sizeof(s),cache,sizeof(cache),0,25,&allocation));
		assert(t41_ae_auto_allocate(p,sizeof(p)-1,s,sizeof(s),cache,sizeof(cache),0,25,&allocation));
		assert(!memcmp(&original,&allocation,sizeof(allocation)));
		memset(p, 0, sizeof(p)); memset(s, 0, sizeof(s));
	}
	{
		unsigned short nodes[120], previous[120];
		unsigned int last = 999;
		assert(!t41_ae_deflicker(50, 10, 25 << 10, 5 << 10, 1800, nodes, &last));
		assert(last == 19 && nodes[0] == 450 && nodes[19] == 9000 && nodes[119] == 9000);
		memcpy(previous, nodes, sizeof(nodes));
		assert(t41_ae_deflicker(0, 10, 25 << 10, 5 << 10, 1800, nodes, &last));
		assert(t41_ae_deflicker(50, 0, 25 << 10, 5 << 10, 1800, nodes, &last));
		assert(t41_ae_deflicker(50, 10, 25 << 10, 0, 1800, nodes, &last));
		assert(t41_ae_deflicker(~0U, 10, 25 << 10, 5 << 10, 1800, nodes, &last));
		assert(last == 19 && !memcmp(previous, nodes, sizeof(nodes)));
	}
	{
		unsigned char cache[0x688] = {0};
		memset(s, 0xa5, sizeof(s));
		put16(p + 0x6c0, 10); t41_ae_put32(p + 0x674, 50);
		put16(s + 0x216a, 1800);
		t41_ae_put32(cache + 0x4ec, 25 << 10);
		t41_ae_put32(cache + 0x4f0, 5 << 10);
		memcpy(saved, s, sizeof(s));
		for (i = 0; i < 120; ++i) put16(saved + 0x2438 + i * 2, (i < 20 ? i + 1 : 20) * 450);
		saved[0x2612] = 19;
		assert(!t41_ae_deflicker_refresh(p,sizeof(p),s,sizeof(s),cache,sizeof(cache)));
		assert(!memcmp(saved,s,sizeof(s)));
		t41_ae_put32(cache + 0x4f0, 0);
		assert(t41_ae_deflicker_refresh(p,sizeof(p),s,sizeof(s),cache,sizeof(cache)));
		assert(t41_ae_deflicker_refresh(p,sizeof(p)-1,s,sizeof(s),cache,sizeof(cache)));
		assert(t41_ae_deflicker_refresh(p,sizeof(p),s,sizeof(s)-1,cache,sizeof(cache)));
		assert(t41_ae_deflicker_refresh(p,sizeof(p),s,sizeof(s),cache,sizeof(cache)-1));
		assert(!memcmp(saved,s,sizeof(s)));
		memset(p, 0, sizeof(p)); memset(s, 0, sizeof(s));
	}
	{
		unsigned int delta[2] = {1024, 2048}, sum = 999;
		unsigned short down = 64, up = 128;
		assert(!t41_ae_convergence_speed(delta, 128, 10, 65535, &down, &up, &sum));
		assert(down == 1088 && up == 4224 && sum == 4224);
		assert(t41_ae_convergence_speed(delta, 128, 25, 65535, &down, &up, &sum));
		assert(t41_ae_convergence_speed(0, 128, 10, 65535, &down, &up, &sum));
		assert(t41_ae_convergence_speed(delta, 128, 10, 65535, &down, &down, &sum));
		assert(down == 1088 && up == 4224 && sum == 4224);
	}
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
