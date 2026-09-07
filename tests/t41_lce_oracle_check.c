#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_lce.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
unsigned int oracle_params[2];
unsigned int oracle_callbacks[128];
static unsigned char p[T41_LCE_PARAM_BYTES],q[sizeof(p)];
static unsigned char actual[T41_LCE_STATE_BYTES] __attribute__((aligned(4)));
static unsigned char expected[T41_LCE_STATE_BYTES] __attribute__((aligned(4)));
extern unsigned int oracle_addresses[4096],oracle_values[4096],oracle_count,oracle_bad_write;
extern int oracle_tisp_lce_init(unsigned int,const unsigned int *);
extern int oracle_tisp_lce_awdr_to_used(unsigned int);
extern int oracle_tisp_lce_curve_init_default(unsigned int);
extern int oracle_tisp_lce_write_all_reg(unsigned int);
extern int oracle_lce_hist_filter_and_judge(const unsigned int *,unsigned int *,const unsigned char *,unsigned int *);
extern int oracle_lce_head_tail_search(const unsigned int *,unsigned int,unsigned int,unsigned int *);
extern int oracle_lce_hist_method(const unsigned int *,unsigned int *,const unsigned int *);
extern int oracle_lce_pdf_to_cdf(const unsigned int *,unsigned short *);
extern int oracle_lce_16bit_data_converge(unsigned int,unsigned int,unsigned int,unsigned int,const unsigned short *,const unsigned short *,unsigned short *);
extern int oracle_lce_self_light_correct(const unsigned int *,unsigned int *,const unsigned int *,const unsigned char *,const unsigned char *);
extern int oracle_lce_wdr_light_lock(const unsigned int *,unsigned int,unsigned int,const unsigned int *,const unsigned short *,unsigned short *);
extern int oracle_lce_light_lock_adjust_hist(const unsigned int *,unsigned int *,unsigned int,int);
extern int oracle_lce_std_hist_transform(unsigned int,unsigned int *,unsigned int,unsigned int);
extern int oracle_Tisp_lce_soft(unsigned int,void *,void *,unsigned int);
static unsigned int seed=345123,failures,frame;
void *oracle_alloc(unsigned int bytes) { return bytes==sizeof(expected) ? expected : 0; }
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static void compare(const char *name,const void *a,const void *b,unsigned int bytes)
{
	if(memcmp(a,b,bytes)) {
		unsigned int i; const unsigned char *x=a,*y=b;
		for(i=0;i<bytes;++i) if(x[i]!=y[i]) break;
		if(failures++<24) printf("LCE %s case=%u +%x got=%x OEM=%x\n",name,frame,i,x[i],y[i]);
	}
}
static void histogram(unsigned int h[32])
{
	unsigned int i,left=0x100000;
	for(i=0;i<32;++i) h[i]=0;
	for(i=0;i<128;++i) { unsigned int v=rng()%(left+1); h[rng()%32]+=v; left-=v; }
	h[rng()%32]+=left;
}
static void full_pipeline(void)
{
	unsigned int i,j,sequence;
	for(sequence=0;sequence<100;++sequence) {
		unsigned int dimensions[]={640+rng()%3200,360+rng()%1800};
		struct t41_dpc_word words[4];
		memset(p,0,sizeof(p)); memset(actual,0,sizeof(actual));
		t41_dpc_put16(p,sequence&1); t41_dpc_put16(p+2,1); t41_dpc_put16(p+4,rng()%32);
		t41_dpc_put16(p+6,128); t41_dpc_put16(p+8,256); t41_dpc_put16(p+10,512);
		t41_dpc_put16(p+12,rng()%1025); t41_lce_put32(p+0x40,rng()%129);
		for(i=0;i<2;++i) {
			unsigned int base=i ? 0xd8 : 0x54,converge=i ? 0x114 : 0x90;
			p[base]=sequence%4; p[base+1]=rng()%32; p[base+2]=(sequence>>2)&1;
			p[base+3]=rng()%64; p[base+4]=rng()%64;
			for(j=0;j<55;++j) p[base+5+j]=1+rng()%128;
			p[converge]=rng()%129; p[converge+1]=rng()%5; p[converge+2]=4+rng()%61;
			p[converge+3]=(sequence>>3)&1; p[converge+4]=(sequence>>4)&1;
			p[converge+5]=rng()%256; p[converge+6]=(sequence>>5)&1; p[converge+7]=(sequence>>6)&1;
			for(j=0;j<64;++j) p[converge+8+j]=rng()%129;
		}
		for(i=0;i<5;++i) p[0x30+i]=1+rng()%8;
		p[0x35]=(sequence>>1)&1; p[0x36]=rng()%256; p[0x37]=rng()%256;
		actual[0x592c]=sequence&1;
		t41_lce_geometry(actual,sizeof(actual),dimensions[0],dimensions[1],words);
		t41_lce_calibrate(p,sizeof(p),actual,sizeof(actual),rng()%(10U<<16));
		t41_lce_identity(actual);
		memcpy(expected,actual,sizeof(actual));
		for(frame=0;frame<100;++frame) {
			unsigned int ry=dimensions[1]%5,rx=(dimensions[0]/2)%9,reset=frame ? frame%5 : 4;
			unsigned int *hist=(unsigned int *)(void *)(actual+0x1898),*sums=hist+45*32;
			for(i=0;i<45;++i) {
				unsigned int area=t41_tmo_le32(actual+0x5784+4*((i/9<ry ? 0 : 2)+(i%9<rx ? 0 : 1)));
				unsigned int left=area/2;
				/* Hold every other frame to exercise the history-distance skip. */
				if(frame&1) break;
				for(j=0;j<32;++j) hist[i*32+j]=0;
				for(j=0;j<64;++j) { unsigned int value=rng()%(left+1); hist[i*32+rng()%32]+=value; left-=value; }
				hist[i*32+rng()%32]+=left; sums[i]=0;
				for(j=0;j<32;++j) sums[i]+=hist[i*32+j]*j*8;
			}
			memcpy(expected+0x1898,actual+0x1898,0x1734);
			if(t41_lce_process(actual,sizeof(actual),reset)) {
				printf("LCE process invalid sequence=%u frame=%u\n",sequence,frame); ++failures; return;
			}
			oracle_Tisp_lce_soft(0,expected+0x5770,expected+0x1898,reset);
			compare("full pipeline",actual,expected,sizeof(actual));
			if(failures) return;
		}
	}
}
int main(void)
{
	unsigned int i;
	*(unsigned int *)(void *)(oracle_bss+0x4240)=(uintptr_t)expected;
	*(unsigned int *)(void *)(oracle_bss+0x4244)=(uintptr_t)expected;
	oracle_params[0]=oracle_params[1]=(uintptr_t)q-0x13da4;
	for(frame=0;frame<10000;++frame) {
		struct t41_dpc_word words[4];
		unsigned int dimensions[]={18+rng()%8175,5+rng()%4092},gain=rng()%(16U<<16);
		unsigned int hist[32],filtered[32],reference[32],r[2],s[2],method[8];
		unsigned char weights[5]; unsigned short old[32],target[32],curve[32],refcurve[32];
		if(!frame) { dimensions[0]=1920; dimensions[1]=1080; }
		for(i=0;i<sizeof(p);++i) p[i]=rng();
		if(frame%3==0) t41_dpc_put16(p+2,0);
		memcpy(q,p,sizeof(p)); memset(actual,0,sizeof(actual));
		oracle_count=0; oracle_tisp_lce_init(0,dimensions);
		t41_lce_geometry(actual,sizeof(actual),dimensions[0],dimensions[1],words);
		compare("geometry",actual+0x5770,expected+0x5770,0x6c);
		compare("boundaries",actual+0x590c,expected+0x590c,32);
		for(i=0;i<4;++i) if(words[i].address!=oracle_addresses[i] || words[i].value!=oracle_values[i]) {
			if(failures++<24) printf("LCE geometry write case=%u i=%u got=%x=%x OEM=%x=%x\n",frame,i,words[i].address,words[i].value,oracle_addresses[i],oracle_values[i]);
		}
		for(i=0;i<sizeof(actual);++i) actual[i]=rng();
		memcpy(expected,actual,sizeof(actual));
		*(unsigned int *)(void *)expected=(uintptr_t)q;
		*(unsigned int *)(void *)(expected+0x5900)=gain;
		memcpy(actual,expected,4); t41_lce_put32(actual+0x5900,gain);
		t41_lce_calibrate(p,sizeof(p),actual,sizeof(actual),gain); oracle_tisp_lce_awdr_to_used(0);
		compare("calibration",actual,expected,sizeof(actual)); compare("calibration mutation",p,q,sizeof(p));
		t41_lce_identity(actual); oracle_count=0; oracle_tisp_lce_curve_init_default(0);
		compare("identity",actual,expected,sizeof(actual));
		{ struct t41_dpc_word curve_words[132];
		for(i=0;i<512;++i) actual[0x550c+i]=rng();
		memcpy(expected+0x550c,actual+0x550c,512);
		if(t41_lce_pack_curve(actual,sizeof(actual),curve_words,132)!=132) return 2;
		oracle_count=0; oracle_tisp_lce_write_all_reg(0);
		if(oracle_count!=132) return 2;
		for(i=0;i<132;++i) if(curve_words[i].address!=oracle_addresses[i] || curve_words[i].value!=oracle_values[i]) {
			if(failures++<24) printf("LCE curve write case=%u i=%u\n",frame,i);
		} }
		histogram(hist); for(i=0;i<5;++i) weights[i]=rng();
		t41_lce_filter(hist,filtered,weights,r); oracle_lce_hist_filter_and_judge(hist,reference,weights,s);
		compare("filter",filtered,reference,sizeof(filtered)); compare("judge",r,s,sizeof(r));
		{ unsigned int a=rng()%513,b=rng()%513;
		t41_lce_head_tail(hist,a,b,r); oracle_lce_head_tail_search(hist,a,b,s); compare("head tail",r,s,sizeof(r)); }
		method[0]=rng()%32; method[2]=method[0]+rng()%(32-method[0]);
		method[1]=method[0]+rng()%(method[2]-method[0]+1);
		for(i=3;i<8;++i) method[i]=1+rng()%255;
		if(t41_lce_hist_method(hist,filtered,method)) return 2;
		oracle_lce_hist_method(hist,reference,method); compare("method",filtered,reference,sizeof(filtered));
		t41_lce_cdf(hist,curve); oracle_lce_pdf_to_cdf(hist,refcurve); compare("cdf",curve,refcurve,sizeof(curve));
		for(i=0;i<32;++i) { old[i]=rng()%1024; target[i]=rng()%1024; }
		{ unsigned int ratio=rng()%256,minimum=rng()%256,maximum=rng()%256;
		t41_lce_converge(ratio,minimum,maximum,32,old,target,curve);
		oracle_lce_16bit_data_converge(ratio,minimum,maximum,32,old,target,refcurve);
		compare("converge",curve,refcurve,sizeof(curve)); }
		{ unsigned int light[4]={frame&1,rng()%256,(frame>>1)&1,(frame>>2)&1};
		unsigned char upper[32],lower[32];
		for(i=0;i<32;++i) { upper[i]=rng()%129; lower[i]=rng()%129; }
		if(t41_lce_light_correct(hist,filtered,light,upper,lower)) return 2;
		oracle_lce_self_light_correct(hist,reference,light,upper,lower); compare("light correct",filtered,reference,sizeof(filtered)); }
		{ unsigned int threshold=rng()%33; int strength=rng()%2049;
		if(t41_lce_lock_hist(hist,filtered,threshold,strength)) return 2;
		oracle_lce_light_lock_adjust_hist(hist,reference,threshold,strength); compare("lock hist",filtered,reference,sizeof(filtered)); }
		{ unsigned int cells[45*32],areas[4],ry=rng()%5,rx=rng()%9;
		unsigned short params[8]={1,0,rng()%32,128,256,512,rng()%1025,0};
		for(i=0;i<4;++i) areas[i]=10000+rng()%200000;
		for(i=0;i<45*32;++i) cells[i]=rng()%2048;
		if(t41_lce_light_lock(cells,ry,rx,areas,params,curve)) return 2;
		oracle_lce_wdr_light_lock(cells,ry,rx,areas,params,refcurve); compare("light lock",curve,refcurve,sizeof(curve)); }
		{ unsigned int inverse=rng()%16385,shift=10+rng()%20;
		memcpy(filtered,hist,sizeof(hist)); memcpy(reference,hist,sizeof(hist));
		t41_lce_normalize(filtered,inverse,shift); oracle_lce_std_hist_transform(0,reference,inverse,shift);
		compare("normalize",filtered,reference,sizeof(filtered)); }
	}
	full_pipeline();
	printf("10000 LCE geometry/calibration/histogram/curve cases and 100x100 temporal sequences: %u mismatches; %u reference diagnostics\n",failures,oracle_bad_write);
	return failures ? 1 : 0;
}
