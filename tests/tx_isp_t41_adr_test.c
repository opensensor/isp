#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_adr.h"
static struct { unsigned char before,p[T41_ADR_PARAM_BYTES],after; } params;
static struct { unsigned char before,s[T41_ADR_STATS_BYTES],after; } stats;
static unsigned char dma[0xaa0],state[T41_ADR_STATE_BYTES],work[T41_ADR_WORK_BYTES];
static unsigned int scratch[8][32],seed=98765;
static struct t41_adr_context ctx;
static unsigned char saved_work[T41_ADR_WORK_BYTES];
static struct { unsigned int before; unsigned char s[T41_ADR_STATS_BYTES]; unsigned int after; } full_stats;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
int main(void)
{
	static const unsigned short knee[16]={0,8,16,32,64,128,192,256,384,512,768,1024,1536,2048,3072,4096};
	struct t41_dpc_word words[199]; unsigned int frame,i,valid=0,rejected=0;
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
	for(frame=0;frame<20;++frame) {
		unsigned int width=64+rng()%700,height=64+rng()%700;
		for(i=0;i<129;++i) t41_adr_put16(gx+i*2,i==128 ? 4095 : i*32);
		memset(params.p,0,sizeof(params.p));
		assert(!t41_adr_initialize(&ctx,params.p,sizeof(params.p),state,work,width,height,gx,scratch));
		memcpy(saved_work,work,sizeof(work));
		memset(params.p,0,sizeof(params.p));
		assert(!t41_adr_refresh(&ctx,params.p,sizeof(params.p),state,work,width,height,gx));
		assert(!memcmp(saved_work,work,sizeof(work)));
		assert(t41_tmo_le16(params.p+0x74)==width && t41_tmo_le16(params.p+0x76)==height);
		assert(!memcmp(params.p+0x27f,work+0x3d4,160));
	}
	full_stats.before=full_stats.after=0xa5a5a5a5;
	for(frame=0;frame<10000;++frame) {
		unsigned char *p=params.p; unsigned int seq=frame/20,cell; int result;
		if(frame%20==0) {
			memset(p,0,sizeof(params.p)); memset(state,0,sizeof(state)); memset(work,0,sizeof(work));
			t41_adr_context_init(&ctx,state,work);
			for(i=0;i<129;++i) t41_adr_put16(p+0x766+i*2,seq%3==0 ? i*32 : seq%3==1 ? (i*i+2)/4 : 4096-((128-i)*(128-i)+2)/4);
			t41_adr_put16(p+0x894,seq%4); t41_adr_put16(p+0x60,seq%6);
			t41_adr_put16(p+0x3a8,1000+rng()%4001); t41_adr_put16(p+0x3aa,300+rng()%1000);
			t41_adr_put16(p+0x3ac,100+rng()%500); t41_adr_put16(p+0x3ae,100+rng()%500);
			t41_adr_put16(p+0x3b0,seq%256); t41_adr_put16(p+0x3b2,155+rng()%101);
			for(i=0;i<24;++i) t41_adr_put16(p+0x89e + i*2,rng()%1025);
			for(i=0;i<8;++i) t41_adr_put16(p+0x5a8+i*2,rng()%1025);
			for(i=0;i<13;++i) t41_adr_put16(p+0x3be + i*2,50+rng()%201);
			for(i=0;i<14;++i) { t41_adr_put16(p+0x58c+i*2,20+rng()%101); t41_adr_put16(p+0x8d6+i*2,100+rng()%301); }
			t41_adr_put16(p+0x8d4,seq%2); t41_adr_put16(p+0x764,seq%2);
			t41_adr_put16(p+0x9d8,200+rng()%825); t41_adr_put16(p+0x9da,rng()%201);
			t41_adr_put16(p+0x9dc,rng()%321); t41_adr_put16(p+0x9de,seq%6);
			t41_adr_put16(p+0x9e0,seq%2); t41_adr_put16(p+0x9e2,rng()%101); t41_adr_put16(p+0x9e4,rng()%101);
			t41_adr_put16(p+0x9a2,seq%2); for(i=0;i<9;++i) t41_adr_put16(p+0x98e + i*2,rng()%901);
			t41_adr_put16(p+0x98c,seq%2);
			for(i=0;i<11;++i) t41_adr_put16(p+0x966+i*2,t41_tmo_le16(ctx.ctc_grid+2+i*2)+rng()%25);
			t41_adr_put16(p+0x9ea,seq%5); t41_adr_put16(p+0x9ec,rng()%6); t41_adr_put16(p+0x9ee,rng()%6); t41_adr_put16(p+0x9f0,rng()%6);
			t41_adr_put16(p+0x9b6,seq%4); t41_adr_put16(p+0x9b4,seq%5); t41_adr_put16(p+0x9b2,seq%16);
			t41_adr_put16(p+0x9f4,seq%3!=0); t41_adr_put16(p+0x9f6,rng()%600); t41_adr_put16(p+0x9f8,seq%10);
			t41_adr_put16(p+0x9fc,seq%3==0); t41_adr_put16(p+0x9fe,4000); t41_adr_put16(p+0xa00,9000); t41_adr_put16(p+0xa02,seq%2 ? 256 : 500);
			ctx.face_enabled=seq%2; ctx.face_count=99999;
			for(i=0;i<24;++i) ctx.face_flags[i]=i%2;
		}
		t41_adr_put16(p+0xa36,seq%5-1); t41_adr_put16(p+0xa38,frame%3==0);
		t41_adr_put16(p+0xa2e,rng()); t41_adr_put16(p+0xa30,rng());
		for(i=0;i<4;++i) { t41_adr_put16(p+0xa3a+i*2,i*5); t41_adr_put16(p+0xa42+i*2,rng()%4096); }
		assert(!t41_adr_hardware_parameters(p,sizeof(params.p))); ctx.ctc_changed=1;
		memset(full_stats.s,0,sizeof(full_stats.s));
		for(i=0;i<512;++i) t41_adr_put32(full_stats.s+i*4,1000+rng()%1600);
		for(cell=0;cell<24;++cell) {
			unsigned int left=38400;
			for(i=0;i<19;++i) { unsigned int n=rng()%(left/3+1); left-=n; t41_adr_put32(full_stats.s+0x800+cell*80+i*4,n); }
			t41_adr_put32(full_stats.s+0x800+cell*80+19*4,left);
			t41_adr_put32(full_stats.s+0x14c0+cell*4,(100+rng()%3500)*38400);
			t41_adr_put32(full_stats.s+0x1520+cell*4,(100+rng()%3500)*38400);
		}
		assert(!t41_adr_statistics(full_stats.s,sizeof(full_stats.s),2560,1440));
		assert(t41_adr_gaussian(p,sizeof(params.p),ctx.gaussian_previous,ctx.gaussian)>=0);
		result=t41_adr_frame(&ctx,p,sizeof(params.p),state,sizeof(state),work,sizeof(work),full_stats.s,sizeof(full_stats.s),lut);
		if(result) { assert(result==-2 || ctx.stage==5); ++rejected; } else ++valid;
		assert(params.before==0xa5 && params.after==0xa5 && full_stats.before==0xa5a5a5a5 && full_stats.after==0xa5a5a5a5);
	}
	assert(valid>9500);
	printf("T41 ADR full frame/history: %u valid, %u invalid rejected; bounds/canaries PASS\n",valid,rejected);
	return 0;
}
