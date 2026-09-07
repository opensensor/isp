#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_adr.h"
static struct { unsigned char before,p[T41_ADR_PARAM_BYTES],after; } params;
static struct { unsigned char before,s[T41_ADR_STATS_BYTES],after; } stats;
static unsigned char dma[0xaa0],state[T41_ADR_STATE_BYTES],work[T41_ADR_WORK_BYTES];
static unsigned int scratch[8][32],seed=98765;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
int main(void)
{
	static const unsigned short knee[16]={0,8,16,32,64,128,192,256,384,512,768,1024,1536,2048,3072,4096};
	struct t41_dpc_word words[199]; unsigned int frame,i;
	unsigned char gx[258],gy[258],knots[32],query[66],sample[66],lut[1024];
	short previous[4]={-1,-1,-1,-1}; int weights[24],cdf[512],curve[14],sections[11];
	memset(&params,0xa5,sizeof(params)); memset(&stats,0xa5,sizeof(stats)); memset(words,0xa5,sizeof(words));
	assert(t41_adr_unpack(dma,sizeof(dma)-1,stats.s,sizeof(stats.s))<0);
	assert(t41_adr_unpack(dma,sizeof(dma),stats.s,sizeof(stats.s)-1)<0);
	assert(t41_adr_statistics(stats.s,sizeof(stats.s),0,1440)<0);
	assert(t41_adr_statistics(stats.s,sizeof(stats.s),65535,65535)<0);
	assert(t41_adr_geometry(params.p,sizeof(params.p),8193,1440,scratch)<0);
	assert(t41_adr_pack_controls(params.p,sizeof(params.p),2,words,199)<0);
	assert(t41_adr_pack_curve(params.p,sizeof(params.p),state,sizeof(state),0,words,198)<0);
	assert(t41_adr_pack_curve(params.p,sizeof(params.p),state,sizeof(state),1,words,199)==0);
	assert(words[0].address==0xa5a5a5a5);
	for(i=0;i<256;++i) t41_adr_put32(lut+i*4,2*(i+1));
	for(frame=0;frame<2000;++frame) {
		unsigned long long exposure=frame*1234567ULL;
		unsigned int total=0; int result;
		for(i=0;i<sizeof(dma);++i) dma[i]=rng();
		for(i=0;i<sizeof(params.p);++i) params.p[i]=rng();
		assert(!t41_adr_unpack(dma,sizeof(dma),stats.s,sizeof(stats.s)));
		assert(!t41_adr_statistics(stats.s,sizeof(stats.s),1+rng()%8192,1+rng()%8192));
		assert(t41_adr_pack_geometry(params.p,sizeof(params.p),frame&1,words,199)==7);
		assert(t41_adr_pack_spatial(params.p,sizeof(params.p),frame&1,words,199)==56);
		assert(t41_adr_pack_controls(params.p,sizeof(params.p),frame&1,words,199)==61);
		assert(t41_adr_pack_curve(params.p,sizeof(params.p),state,sizeof(state),0,words,199)==199);
		if(frame<20) assert(!t41_adr_geometry(params.p,sizeof(params.p),32+rng()%700,32+rng()%700,scratch));
		t41_adr_put16(params.p+0x9d8,rng()%1025); t41_adr_put16(params.p+0x9da,rng()%300);
		t41_adr_put16(params.p+0x9dc,rng()%500); t41_adr_put16(params.p+0x9de,frame%8);
		assert(t41_adr_gaussian(params.p,sizeof(params.p),previous,weights)>=0);
		assert(t41_adr_gaussian(params.p,sizeof(params.p),previous,weights)==0);
		assert(!t41_adr_local_strength(rng()%10001,rng()%4096,80+rng()%1025,80+rng()%1025,rng()%4096,&result));
		for(i=0;i<11;++i) { unsigned long long v=i*5000000000ULL; t41_adr_put32(params.p+0x320+i*8,v); t41_adr_put32(params.p+0x324+i*8,v>>32); }
		t41_adr_put16(params.p+0x894,frame%4); t41_adr_put16(params.p+0x9a4,frame%2);
		t41_adr_put16(params.p+0x9d6,frame%3==0); t41_adr_put16(params.p+0x762,frame%2);
		assert(t41_adr_ev(params.p,sizeof(params.p),work,sizeof(work),exposure,1)==1);
		for(i=0;i<129;++i) { t41_adr_put16(gx+i*2,i*32); t41_adr_put16(gy+i*2,i*32); }
		for(i=0;i<33;++i) t41_adr_put16(query+i*2,rng()%4097);
		assert(!t41_adr_resample16(gx,gy,query,sample,129,33));
		for(i=0;i<33;++i) assert(t41_tmo_le16(sample+i*2)==t41_tmo_le16(query+i*2));
		for(i=0;i<512;++i) { total+=1+rng()%100; cdf[i]=total; }
		for(i=0;i<512;++i) cdf[i]=cdf[i]*10000/total;
		assert(!t41_adr_subsections(sections,11,rng()%101,gx,gy,cdf,8,10,16,frame&1,frame&1));
		assert(!t41_adr_map_curve(curve,rng()%256,155+rng()%100,lut));
		for(i=0;i<16;++i) t41_adr_put16(knots+i*2,knee[i]);
		for(i=0;i<14;++i) curve[i]=knee[i+1];
		assert(!t41_adr_filter(knots,curve,frame%5,frame%16,frame&1));
		assert(params.before==0xa5 && params.after==0xa5 && stats.before==0xa5 && stats.after==0xa5);
	}
	puts("T41 ADR scalar, geometry, DMA, histogram, Gaussian, EV and curve bounds/canaries: PASS");
	return 0;
}
