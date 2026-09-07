#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ucontext.h>
#include <stdlib.h>
#include "../driver/t41/tx_isp_t41_adr.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
static unsigned char p[T41_ADR_PARAM_BYTES] __attribute__((aligned(4)));
static unsigned char state[T41_ADR_STATE_BYTES] __attribute__((aligned(4)));
static uint32_t info[9];
static unsigned char stats[T41_ADR_STATS_BYTES] __attribute__((aligned(4)));
static unsigned char expected[T41_ADR_STATS_BYTES] __attribute__((aligned(4)));
static unsigned char dma[0x1000] __attribute__((aligned(4)));
static unsigned char native_params[T41_ADR_PARAM_BYTES];
static unsigned char ev_work[T41_ADR_WORK_BYTES] __attribute__((aligned(4))),native_work[T41_ADR_WORK_BYTES];
extern int oracle_tiziano_adr_ev_func(unsigned int);
extern int oracle_ispint_adr_16(int,const void *,const void *,int);
extern int oracle_func_interp1_short(const void *,const void *,const void *,void *,short,short);
extern int oracle_func_gam_x2y(const void *,const void *,int);
extern int oracle_func_local_info(int,int,int,int,int);
extern int oracle_subsection_map(int,int,int,const void *,const void *,const int *,int,int,int,int);
extern int oracle_subsection(int *,int,const void *,const void *,const int *,int,int,int,int);
extern int oracle_subsection_up(int *,int,const void *,const void *,const int *,int,int,int,int);
extern int oracle_func_adr_map_curve1(int *,int,int);
extern unsigned char oracle_data[];
extern int oracle_func_map_y_filter(void *,int *,int,int),oracle_func_map_y_filter_sp(void *,int *,int);
extern int oracle_tiziano_adr_algorithm(unsigned int);
extern int oracle_tiziano_adr_hardpars_ctl(unsigned int);
extern int oracle_tisp_adr_init(unsigned int,const void *);
extern int oracle_tisp_adr_dn_params_refresh(unsigned int,unsigned int),oracle_tisp_adr_linear_switch(unsigned int,unsigned int);
extern unsigned int oracle_params[2],oracle_heap_used;
static unsigned char image_params[0x14000] __attribute__((aligned(4)));
static struct t41_adr_context ctx;
static unsigned char native_state[T41_ADR_STATE_BYTES];
static unsigned int scratch[8][32];
short oracle_gauss_old[10];
static short native_gauss_old[4];
static int gaussian[24],oem_gaussian[24];
extern int oracle_func_gauss_local(int *,unsigned int);
extern unsigned int oracle_allocations;
extern unsigned int oracle_unsafe_divisions;
extern int oracle_tiziano_adr_base_pars(unsigned int,unsigned int,unsigned int);
extern int oracle_tiziano_adr_read_data(const void *,unsigned int),oracle_tiziano_adr_stat_calc(unsigned int);
extern int oracle_func_adr_reg_write_one(unsigned int),oracle_func_adr_reg_write_5x5(unsigned int);
extern int oracle_func_adr_reg_write_sometimes(unsigned int),oracle_func_adr_reg_write_every(unsigned int);
extern unsigned int oracle_addresses[4096],oracle_values[4096],oracle_count,oracle_bad_write;
extern unsigned int oracle_radial_reference[31];
static unsigned int seed=145811,failures,frame,busy,rejected;
static void trap(int sig,siginfo_t *detail,void *context)
{
	ucontext_t *u=context; unsigned int i; (void)detail;
	printf("ADR signal=%d frame=%u pc=%llx\n",sig,frame,(unsigned long long)u->uc_mcontext.pc);
	for(i=0;i<32;++i) printf("r%u=%llx%c",i,(unsigned long long)u->uc_mcontext.gregs[i],i%4==3 ? '\n' : ' ');
	exit(1);
}
unsigned int oracle_read(unsigned int address)
{ if(address!=0x5040c) ++failures; return busy; }
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static void compare_bytes(const char *name,const void *a,const void *b,unsigned int size)
{
	unsigned int i; const unsigned char *x=a,*y=b;
	for(i=0;i<size;++i) if(x[i]!=y[i]) {
		if(failures++<20) printf("ADR %s frame=%u offset=%x native=%x OEM=%x\n",name,frame,i,x[i],y[i]);
	}
}
static void compare(const char *name,const struct t41_dpc_word *words,int n)
{
	unsigned int i;
	struct sigaction action;
	memset(&action,0,sizeof(action)); action.sa_sigaction=trap; action.sa_flags=SA_SIGINFO;
	sigaction(SIGTRAP,&action,NULL); sigaction(SIGFPE,&action,NULL);
	if(n<0 || (unsigned int)n!=oracle_count) { printf("ADR %s frame=%u count=%d OEM=%u\n",name,frame,n,oracle_count); ++failures; return; }
	for(i=0;i<oracle_count;++i) if(words[i].address!=oracle_addresses[i] || words[i].value!=oracle_values[i]) {
		if(failures++<20) printf("ADR %s frame=%u i=%u got=%x=%x OEM=%x=%x\n",name,frame,i,words[i].address,words[i].value,oracle_addresses[i],oracle_values[i]);
	}
}
int main(void)
{
	unsigned int i,full_valid=0,full_rejected=0,bounds_rejected=0,reset=0;
	setvbuf(stdout,NULL,_IONBF,0);
	*(unsigned int *)(void *)(oracle_bss+0x4658)=(uintptr_t)info;
	*(unsigned int *)(void *)(oracle_bss+0x465c)=(uintptr_t)info;
	info[0]=(uintptr_t)p; info[1]=(uintptr_t)state;
	for(i=0;i<128;++i) if(t41_adr_dark_weight(i)!=t41_tmo_le32(oracle_data+0x40e0+i*4)) {
		printf("ADR dark weight i=%u native=%u OEM=%u\n",i,t41_adr_dark_weight(i),t41_tmo_le32(oracle_data+0x40e0+i*4)); ++failures;
	}
	for(i=0;i<31;++i) if(t41_adr_radial_threshold(i)!=oracle_radial_reference[i]) {
		printf("ADR radial i=%u native=%u OEM=%u\n",i,t41_adr_radial_threshold(i),oracle_radial_reference[i]); ++failures;
	}
	for(frame=0;frame<10000;++frame) {
		struct t41_dpc_word words[256]; unsigned int channel=frame&1; int count;
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		for(i=0;i<sizeof(state);++i) state[i]=rng();
		count=t41_adr_pack_geometry(p,sizeof(p),channel,words,256);
		oracle_count=0; oracle_func_adr_reg_write_one(channel); compare("geometry",words,count);
		count=t41_adr_pack_spatial(p,sizeof(p),channel,words,256);
		oracle_count=0; oracle_func_adr_reg_write_5x5(channel); compare("spatial",words,count);
		count=t41_adr_pack_controls(p,sizeof(p),channel,words,256);
		oracle_count=0; oracle_func_adr_reg_write_sometimes(channel); compare("controls",words,count);
		busy=frame%7==0;
		count=t41_adr_pack_curve(p,sizeof(p),state,sizeof(state),busy,words,256);
		oracle_count=0; oracle_func_adr_reg_write_every(channel); compare("curve",words,count);
		/* Include cache hits, clamping, all modes and the entire signed ABI. */
		if(frame%3==0) {
			t41_adr_put16(p+0x9d8,rng()%1500);
			t41_adr_put16(p+0x9da,rng()%250);
			t41_adr_put16(p+0x9dc,rng()%400);
			t41_adr_put16(p+0x9de,frame%9-2);
		}
		oracle_func_gauss_local(oem_gaussian,channel);
		if(t41_adr_gaussian(p,sizeof(p),native_gauss_old,gaussian)<0) ++failures;
		compare_bytes("gaussian",gaussian,oem_gaussian,sizeof(gaussian));
		compare_bytes("gaussian cache",native_gauss_old,oracle_gauss_old,sizeof(native_gauss_old));
		oracle_func_gauss_local(oem_gaussian,channel);
		if(t41_adr_gaussian(p,sizeof(p),native_gauss_old,gaussian)!=0) ++failures;
		{
			unsigned char x[258] __attribute__((aligned(4))),y[258] __attribute__((aligned(4)));
			unsigned char queries[66] __attribute__((aligned(4))),a[66],b[66] __attribute__((aligned(4)));
			unsigned int value=0; int sample,result,oem,strength=rng()%10001,pivot=rng()%4096;
			int sd=1+rng()%1448,sb=1+rng()%1448,luma=(int)(rng()%4300)-100;
			for(i=0;i<129;++i) { value+=1+rng()%32; t41_adr_put16(x+i*2,value); t41_adr_put16(y+i*2,rng()); }
			for(i=0;i<33;++i) t41_adr_put16(queries+i*2,(int)(rng()%(value+100))-50);
			sample=(short)t41_tmo_le16(queries);
			result=t41_adr_interpolate16(sample,x,y,129); oem=oracle_ispint_adr_16(sample,x,y,129);
			if(result!=oem) { if(failures++<20) printf("ADR interp16 frame=%u native=%d OEM=%d\n",frame,result,oem); }
			result=t41_adr_gamma_lookup(x,y,sample); oem=oracle_func_gam_x2y(x,y,sample);
			if(result!=oem) { if(failures++<20) printf("ADR gamma lookup frame=%u native=%d OEM=%d\n",frame,result,oem); }
			oracle_func_interp1_short(x,y,queries,b,129,33);
			if(t41_adr_resample16(x,y,queries,a,129,33)) ++failures;
			compare_bytes("resample",a,b,66);
			if(t41_adr_local_strength(strength,pivot,sd,sb,luma,&result)) {
				int effective=luma<1 ? 1 : luma,sigma=pivot>=effective ? sd : sb;
				unsigned int d=(effective-pivot)*(effective-pivot),exponent;
				if(d>0x3ffffc) d=0x3ffffc;
				exponent=t41_ae_fixed_mul(16,t41_ae_fixed_div(10,d<<10,(unsigned int)(2*sigma*sigma)<<10)<<6,
					t41_ae_fixed_div(16,0x385b0000,0x27100000));
				if((int)exponent>=0xf0000) exponent=0xeffff;
				/* Do not reproduce the OEM's raw MIPS divide-by-zero. */
				if(t41_adr_exp2(exponent)!=0) ++failures;
				++rejected;
			} else {
				oem=oracle_func_local_info(strength,pivot,sd,sb,luma);
				if(result!=oem) { if(failures++<20) printf("ADR local strength frame=%u inputs=%d,%d,%d,%d,%d native=%d OEM=%d\n",frame,strength,pivot,sd,sb,luma,result,oem); }
			}
		}
		{
			unsigned char gx[258] __attribute__((aligned(4))),gy[258] __attribute__((aligned(4)));
			int cdf[512],native[11]={0},oem[11]={0},result,reference;
			int strength=rng()%101,mode=frame%2,target=rng()%10001,base=rng()%4096,total=0;
			for(i=0;i<129;++i) { t41_adr_put16(gx+i*2,i*32); t41_adr_put16(gy+i*2,i*32); }
			for(i=0;i<512;++i) { total+=rng()%100; cdf[i]=total; }
			for(i=0;i<512;++i) cdf[i]=cdf[i]*10000/total;
			result=t41_adr_subsection_map(target,base,strength,gx,gy,cdf,8,10,16,mode);
			reference=oracle_subsection_map(target,base,strength,gx,gy,cdf,8,10,16,mode);
			if(result!=reference) { if(failures++<20) printf("ADR subsection map frame=%u native=%d OEM=%d\n",frame,result,reference); }
			oracle_subsection(oem,strength,gx,gy,cdf,8,10,16,mode);
			if(t41_adr_subsections(native,11,strength,gx,gy,cdf,8,10,16,mode,0)) ++failures;
			compare_bytes("subsection",native,oem,36);
			oracle_subsection_up(oem,strength,gx,gy,cdf,8,10,16,mode);
			if(t41_adr_subsections(native,11,strength,gx,gy,cdf,8,10,16,mode,1)) ++failures;
			compare_bytes("subsection extended",native,oem,44);
		}
		{
			int native[14],oem[14],curvature=frame%256,knee=1+rng()%255;
			oracle_func_adr_map_curve1(oem,curvature,knee);
			if(t41_adr_map_curve(native,curvature,knee,oracle_data+0x42e4)) ++failures;
			compare_bytes("rational curve",native,oem,sizeof(native));
		}
		{
			unsigned char knots[32] __attribute__((aligned(4))),native_knots[32];
			int native[14],oem[14],original[14],total=0,iterations=frame%5,segments=frame%17,spatial;
			memcpy(knots,oracle_data+0x4894,32);
			for(i=0;i<14;++i) { total+=1+rng()%100; original[i]=total; }
			for(i=0;i<14;++i) original[i]=original[i]*4000/total;
			for(spatial=0;spatial<2;++spatial) {
				memcpy(native_knots,knots,32); memcpy(native,original,sizeof(native)); memcpy(oem,original,sizeof(oem));
				if(spatial) oracle_func_map_y_filter_sp(knots,oem,iterations);
				else oracle_func_map_y_filter(knots,oem,iterations,segments);
				if(t41_adr_filter(native_knots,native,iterations,segments,spatial)) ++failures;
				compare_bytes(spatial ? "spatial curve filter" : "curve filter",native,oem,sizeof(native));
				compare_bytes("filter knots",native_knots,knots,32);
			}
		}
		{
			unsigned long long knot=0,exposure;
			for(i=0;i<11;++i) {
				knot+=rng();
				t41_adr_put32(p+0x320+i*8,knot); t41_adr_put32(p+0x324+i*8,knot>>32);
			}
			exposure=((unsigned long long)rng()*10)%(knot+100);
			if(frame%4==0) exposure=0; else if(frame%4==1) exposure=knot;
			t41_adr_put16(p+0x894,frame%4); t41_adr_put16(p+0x9a4,frame%2);
			t41_adr_put16(p+0x9d6,frame%3==0); t41_adr_put16(p+0x762,frame%4==0);
			for(i=0;i<sizeof(ev_work);++i) ev_work[i]=rng();
			memcpy(native_params,p,sizeof(p)); memcpy(native_work,ev_work,sizeof(ev_work));
			info[2]=(uintptr_t)ev_work;
			t41_adr_put32(oracle_bss+0x4610+channel*8,exposure);
			t41_adr_put32(oracle_bss+0x4614+channel*8,exposure>>32);
			t41_adr_put16(oracle_bss+0x460c+channel*2,frame%5!=0);
			oracle_tiziano_adr_ev_func(channel);
			if(t41_adr_ev(native_params,sizeof(p),native_work,sizeof(native_work),exposure,frame%5!=0)<0) ++failures;
			compare_bytes("EV calibration",native_params,p,sizeof(p));
			compare_bytes("EV work",native_work,ev_work,sizeof(ev_work));
		}
		for(i=0;i<sizeof(dma);++i) dma[i]=rng();
		for(i=0;i<sizeof(stats);++i) stats[i]=rng();
		memcpy(expected,stats,sizeof(stats));
		info[3]=(uintptr_t)expected;
		oracle_tiziano_adr_read_data(dma,channel);
		if(t41_adr_unpack(dma,sizeof(dma),stats,sizeof(stats))) ++failures;
		compare_bytes("unpack",stats,expected,sizeof(stats));
		{
			unsigned int width=1+rng()%8192,height=1+rng()%8192;
			*(unsigned int *)(void *)(oracle_bss+0x4638+4*channel)=width;
			*(unsigned int *)(void *)(oracle_bss+0x4640+4*channel)=height;
			oracle_tiziano_adr_stat_calc(channel);
			if(t41_adr_statistics(stats,sizeof(stats),width,height)) ++failures;
			compare_bytes("statistics",stats,expected,sizeof(stats));
		}
	}
	for(frame=0;frame<105;++frame) {
		static const unsigned int dimensions[5][2]={{2560,1440},{1920,1080},{1280,720},{3840,2160},{1,1}};
		unsigned int width=frame<100 ? 1+rng()%768 : dimensions[frame-100][0];
		unsigned int height=frame<100 ? 1+rng()%768 : dimensions[frame-100][1];
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		memcpy(native_params,p,sizeof(p)); oracle_allocations=0;
		oracle_tiziano_adr_base_pars(width,height,frame&1);
		if(t41_adr_geometry(native_params,sizeof(p),width,height,scratch)) ++failures;
		compare_bytes("base geometry",native_params,p,sizeof(p));
	}
	printf("31 generated radial thresholds; 10000 ADR register/DMA/statistics/Gaussian/EV/curve/filter/quantile cases; 105 integrated spatial grids: %u mismatches; %u unsafe zero-divisors rejected; %u busy diagnostics\n",failures,rejected,oracle_bad_write);
	for(frame=0;frame<10000;++frame) {
		unsigned int cell,seq=frame/20;
		int frame_result;
		if(frame%100==0) printf("ADR full frame progress=%u\n",frame);
		if(frame%20==0 || reset) {
			reset=0;
			memset(p,0,sizeof(p)); memset(state,0,sizeof(state)); memset(ev_work,0,sizeof(ev_work));
			t41_adr_context_init(&ctx,state,ev_work);
			memcpy(native_state,state,sizeof(state)); memcpy(native_work,ev_work,sizeof(ev_work));
			memcpy(oracle_data+0x4894,ctx.knees,32); memset(oracle_gauss_old,0xff,sizeof(oracle_gauss_old));
			for(i=0;i<129;++i) t41_adr_put16(p+0x766+i*2,seq%3==0 ? i*32 : seq%3==1 ? (i*i+2)/4 : 4096-((128-i)*(128-i)+2)/4);
			t41_adr_put16(p+0x894,seq%4); t41_adr_put16(p+0x60,seq%6);
			t41_adr_put16(p+0x3a8,1000+rng()%9001); t41_adr_put16(p+0x3aa,300+rng()%1000);
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
			t41_adr_put16(p+0x9f4,seq%3!=0); t41_adr_put16(p+0x9f6,rng()%513); t41_adr_put16(p+0x9f8,seq%10);
			t41_adr_put16(p+0x9fc,seq%3==0); t41_adr_put16(p+0x9fe,4000); t41_adr_put16(p+0xa00,9000); t41_adr_put16(p+0xa02,seq%2 ? 256 : 500);
			ctx.face_enabled=seq%2; ctx.face_count=99999;
			for(i=0;i<24;++i) ctx.face_flags[i]=i%2;
			memcpy(oracle_bss+0x45e8,ctx.face_flags,24); memcpy(oracle_bss+0x4348,ctx.face_curve,672);
			t41_adr_put16(oracle_bss+0x4600,ctx.face_enabled); t41_adr_put32(oracle_bss+0x4344,ctx.face_count);
		}
		/* Parameter updates during a sequence must preserve history. */
		if(frame%7==0) {
			t41_adr_put16(p+0x9ea,rng()%5); t41_adr_put16(p+0x9f6,rng()%600);
			t41_adr_put16(p+0x9d8,200+rng()%825); t41_adr_put16(p+0x3a8,1000+rng()%9001);
		}
		t41_adr_put16(p+0xa36,seq%5-1); t41_adr_put16(p+0xa38,frame%3==0);
		t41_adr_put16(p+0xa2e,rng()); t41_adr_put16(p+0xa30,rng());
		for(i=0;i<4;++i) { t41_adr_put16(p+0xa3a+i*2,i*5); t41_adr_put16(p+0xa42+i*2,rng()%4096); }
		memcpy(native_params,p,sizeof(p));
		oracle_tiziano_adr_hardpars_ctl(0);
		if(t41_adr_hardware_parameters(native_params,sizeof(p))) ++failures;
		ctx.ctc_changed=1;
		compare_bytes("hardware parameters",native_params,p,sizeof(p));
		memset(stats,0,sizeof(stats));
		for(i=0;i<512;++i) t41_adr_put32(stats+i*4,1000+rng()%1600);
		for(cell=0;cell<24;++cell) {
			unsigned int left=38400;
			for(i=0;i<19;++i) { unsigned int n=rng()%(left/3+1); left-=n; t41_adr_put32(stats+0x800+cell*80+i*4,n); }
			t41_adr_put32(stats+0x800+cell*80+19*4,left);
			t41_adr_put32(stats+0x14c0+cell*4,(100+rng()%3500)*38400);
			t41_adr_put32(stats+0x1520+cell*4,(100+rng()%3500)*38400);
		}
		if(t41_adr_statistics(stats,sizeof(stats),2560,1440)) ++failures;
		info[0]=(uintptr_t)p; info[1]=(uintptr_t)state; info[2]=(uintptr_t)ev_work; info[3]=(uintptr_t)stats;
		memcpy(native_params,p,sizeof(p));
		oracle_func_gauss_local((int *)(void *)(oracle_bss+0x42b0),0);
		if(t41_adr_gaussian(native_params,sizeof(p),ctx.gaussian_previous,ctx.gaussian)<0) ++failures;
		oracle_unsafe_divisions=0;
		frame_result=t41_adr_frame(&ctx,native_params,sizeof(p),native_state,sizeof(native_state),native_work,sizeof(native_work),stats,sizeof(stats),oracle_data+0x42e4);
		if(frame_result==-2) { ++bounds_rejected; reset=1; continue; }
		if(frame_result) {
			oracle_tiziano_adr_algorithm(0);
			if(!oracle_unsafe_divisions) { printf("ADR full frame %u unexpectedly rejected stage=%u tile=%u strength=%d\n",frame,ctx.stage,ctx.tile,ctx.strengths[ctx.tile]); ++failures; break; }
			++full_rejected; reset=1; continue;
		}
		oracle_tiziano_adr_algorithm(0);
		if(oracle_unsafe_divisions) { printf("ADR full frame %u failed to reject invalid OEM division\n",frame); ++failures; break; }
		compare_bytes("full params",native_params,p,sizeof(p));
		compare_bytes("full output",native_state,state,sizeof(state));
		compare_bytes("full history",native_work,ev_work,sizeof(ev_work));
		if(failures) {
			unsigned char a[26] __attribute__((aligned(4))),b[26] __attribute__((aligned(4))),c[26] __attribute__((aligned(4))),y[26] __attribute__((aligned(4))),v[46] __attribute__((aligned(4)));
			t41_adr_put16(y,0); t41_adr_put16(y+24,4095); memcpy(y+2,p+0x966,22);
			oracle_func_interp1_short(oracle_data+0x4760,p+0x766,ctx.ctc_grid,a,129,13);
			oracle_func_interp1_short(ctx.ctc_grid,y,a,b,13,13);
			oracle_func_interp1_short(p+0x766,oracle_data+0x4760,b,c,129,13);
			oracle_func_interp1_short(ctx.ctc_grid,c,oracle_data+0x4864,v,13,23);
			compare_bytes("CTC gamma axis",ctx.gamma_x,oracle_data+0x4760,258);
			compare_bytes("CTC output axis",ctx.ctc_axis,oracle_data+0x4864,46);
			compare_bytes("CTC stage 0",ctx.ctc_sample[0],a,26);
			compare_bytes("CTC stage 1",ctx.ctc_sample[1],b,26);
			compare_bytes("CTC stage 2",ctx.ctc_sample[2],c,26);
			printf("CTC last: standalone=%u OEM=%u native=%u gamma=%u\n",t41_tmo_le16(v+44),t41_tmo_le16(ev_work+0x394),t41_tmo_le16(native_work+0x394),t41_tmo_le16(p+0x866));
		}
		compare_bytes("full knots",ctx.knees,oracle_data+0x4894,32);
		compare_bytes("full quantiles",ctx.cut,oracle_bss+0x4260,44);
		if(ctx.face_count!=t41_tmo_le32(oracle_bss+0x4344)) ++failures;
		if(failures) break;
		++full_valid;
	}
	printf("ADR full frame/history attempts=%u valid=%u unsafe divisions rejected=%u CDF bounds rejected=%u mismatches=%u\n",frame,full_valid,full_rejected,bounds_rejected,failures);
	if(failures) return 1;
	for(frame=0;frame<20;++frame) {
		unsigned int dimensions[2]={2560,1440},mode; unsigned char *q=image_params+0x11d28; uint32_t *oem_info;
		for(i=0;i<sizeof(image_params);++i) image_params[i]=rng();
		if(frame>=4) { dimensions[0]=32+rng()%700; dimensions[1]=32+rng()%700; }
		if(frame&1) { t41_adr_put16(q+0x74,dimensions[0]); t41_adr_put16(q+0x76,dimensions[1]); }
		for(i=0;i<129;++i) t41_adr_put16(image_params+0x139c0+i*2,i==128 ? 4095 : i*32);
		memcpy(native_params,q,sizeof(p));
		/* Restore the OEM's initial globals for independent cold starts. */
		t41_adr_context_init(&ctx,native_state,native_work);
		memcpy(oracle_data+0x4894,ctx.knees,32); memset(oracle_gauss_old,0xff,sizeof(oracle_gauss_old));
		oracle_params[0]=(uintptr_t)image_params; oracle_count=0; busy=0; oracle_heap_used=0; oracle_allocations=0;
		if(oracle_tisp_adr_init(0,dimensions)) ++failures;
		oem_info=(void *)(uintptr_t)t41_tmo_le32(oracle_bss+0x4658);
		if(t41_adr_initialize(&ctx,native_params,sizeof(p),native_state,native_work,dimensions[0],dimensions[1],image_params+0x139c0,scratch)) ++failures;
		compare_bytes("cold params",native_params,q,sizeof(p));
		compare_bytes("cold output",native_state,(void *)(uintptr_t)oem_info[1],sizeof(native_state));
		compare_bytes("cold work",native_work,(void *)(uintptr_t)oem_info[2],sizeof(native_work));
		if(failures) break;
		for(mode=0;mode<10;++mode) {
			for(i=0;i<sizeof(p);++i) q[i]=rng();
			if(mode&1) { t41_adr_put16(q+0x74,dimensions[0]); t41_adr_put16(q+0x76,dimensions[1]); }
			for(i=0;i<sizeof(state);++i) native_state[i]=rng();
			memcpy((void *)(uintptr_t)oem_info[1],native_state,sizeof(native_state));
			memcpy(native_params,q,sizeof(p)); oracle_count=0;
			if(mode&1) oracle_tisp_adr_dn_params_refresh(0,mode%2); else oracle_tisp_adr_linear_switch(0,mode%3);
			if(t41_adr_refresh(&ctx,native_params,sizeof(p),native_state,native_work,dimensions[0],dimensions[1],image_params+0x139c0)) ++failures;
			compare_bytes("refresh params",native_params,q,sizeof(p));
			compare_bytes("refresh output",native_state,(void *)(uintptr_t)oem_info[1],sizeof(native_state));
			compare_bytes("refresh history",native_work,(void *)(uintptr_t)oem_info[2],sizeof(native_work));
			if(failures) break;
		}
		if(failures) break;
	}
	printf("ADR cold initialization cases=%u (each with 10 day/night or linear/WDR replacements) mismatches=%u\n",frame,failures);
	return failures ? 1 : 0;
}
