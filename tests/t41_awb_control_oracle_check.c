#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_control.h"
static struct t41_awb_owned native,saved;
static unsigned char calibration[0x978+T41_AWB_PARAM_BYTES];
static unsigned char input[T41_AWB_ARRAY_BYTES],out[T41_AWB_ARRAY_BYTES],expected[sizeof(out)];
static uint32_t seed=927331;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
#ifndef T41_AWB_CONTROL_HOST
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern unsigned int oracle_params[2],oracle_allocated,oracle_bad,oracle_writes,oracle_hw_calls,oracle_gain_calls;
extern int oracle_tisp_awb_init(unsigned int,void *);
extern int oracle_tisp_awb_params_refresh(unsigned int);
extern int oracle_tisp_awb_dn_params_refresh(unsigned int);
extern int oracle_tisp_awb_ev_update(unsigned int,unsigned int,unsigned int);
extern int oracle_tisp_awb_param_array_get(unsigned int,void *,unsigned int *);
#define API(name) extern int oracle_tisp_awb_##name(unsigned int,void *)
API(param_array_set); API(get_attr); API(get_global_statis); API(set_weight); API(get_weight);
API(get_zone); API(set_mode); API(get_mode); API(set_frz); API(get_frz); API(set_ct);
API(set_statis_localtion); API(api_set_ct_trend_offset); API(api_get_ct_trend_offset);
API(set_converge_step); API(get_converge_step);
static unsigned int *info;
static unsigned int mismatches;
static void compare(unsigned int sequence,unsigned int event,const char *kind,const void *a,const void *b,unsigned int n)
{
	unsigned int i;
	if (!memcmp(a,b,n)) return;
	if (mismatches++>=10) return;
	for (i=0;i<n;++i) if (((const unsigned char *)a)[i]!=((const unsigned char *)b)[i]) {
		printf("%s sequence=%u event=%u offset=%x native=%x OEM=%x\n",kind,sequence,event,i,
			((const unsigned char *)a)[i],((const unsigned char *)b)[i]); break;
	}
}
static void compare_state(unsigned int sequence,unsigned int event)
{
	compare(sequence,event,"parameters",native.p,(void *)(uintptr_t)info[0],sizeof(native.p));
	compare(sequence,event,"state",native.s,(void *)(uintptr_t)info[1],sizeof(native.s));
	compare(sequence,event,"report",native.report,(void *)(uintptr_t)info[3],sizeof(native.report));
	compare(sequence,event,"control",native.control,(void *)(uintptr_t)info[2],sizeof(native.control));
}
static void oracle_set(enum t41_awb_control_op op)
{
	switch (op) {
	case T41_AWB_MODE: oracle_tisp_awb_set_mode(0,input); break;
	case T41_AWB_FREEZE: oracle_tisp_awb_set_frz(0,(void *)(uintptr_t)input[0]); break;
	case T41_AWB_CT: oracle_tisp_awb_set_ct(0,input); break;
	case T41_AWB_WEIGHT: oracle_tisp_awb_set_weight(0,input); break;
	case T41_AWB_LOCATION: oracle_tisp_awb_set_statis_localtion(0,input); break;
	case T41_AWB_TREND: oracle_tisp_awb_api_set_ct_trend_offset(0,input); break;
	case T41_AWB_CONVERGE: oracle_tisp_awb_set_converge_step(0,input); break;
	case T41_AWB_EV: oracle_tisp_awb_ev_update(0,0,t41_tmo_le32(input)); break;
	case T41_AWB_PARAMS: oracle_tisp_awb_param_array_set(0,input); break;
	default: assert(0);
	}
}
static unsigned int oracle_get(enum t41_awb_control_op op)
{
	unsigned int n=t41_awb_control_bytes(op,1);
	switch (op) {
	case T41_AWB_MODE: oracle_tisp_awb_get_mode(0,expected); break;
	case T41_AWB_FREEZE: oracle_tisp_awb_get_frz(0,expected); break;
	case T41_AWB_WEIGHT: oracle_tisp_awb_get_weight(0,expected); break;
	case T41_AWB_TREND: oracle_tisp_awb_api_get_ct_trend_offset(0,expected); break;
	case T41_AWB_CONVERGE: oracle_tisp_awb_get_converge_step(0,expected); break;
	case T41_AWB_PARAMS: {
		unsigned int allocated=oracle_allocated,size=0;
		oracle_tisp_awb_param_array_get(0,expected,&size);
		assert(size==n); oracle_allocated=allocated;
		n-=2; break; /* OEM's uninitialized padding is deliberately not copied. */
	}
	case T41_AWB_ATTR: oracle_tisp_awb_get_attr(0,expected); break;
	case T41_AWB_GLOBAL: oracle_tisp_awb_get_global_statis(0,expected); break;
	case T41_AWB_ZONE: oracle_tisp_awb_get_zone(0,expected); break;
	default: assert(0);
	}
	return n;
}
#endif
int main(void)
{
	unsigned int seq,event,i,checks=0;
	for (seq=0;seq<100;++seq) {
		unsigned int rows=1+rng()%15,cols=1+rng()%15;
		unsigned int dimensions[2]={cols*(16+rng()%240)*2,rows*(16+rng()%240)*2};
		unsigned char *p=calibration+0x978;
		for (i=0;i<sizeof(calibration);++i) calibration[i]=rng();
		t41_awb_gain_put16(p+0xc6c,0); t41_awb_gain_put16(p+0xc70,0);
		t41_awb_gain_put16(p+0xc6e,rows); t41_awb_gain_put16(p+0xc72,cols);
		for (i=0;i<9;++i) t41_ae_put32(p+0xfc+i*4,100+i*1000);
		assert(!t41_awb_cold(&native,p,T41_AWB_PARAM_BYTES,dimensions[0],dimensions[1]));
#ifndef T41_AWB_CONTROL_HOST
		oracle_allocated=oracle_writes=oracle_hw_calls=oracle_gain_calls=0;
		memset(oracle_bss,0,sizeof(oracle_bss)); oracle_params[0]=(uintptr_t)calibration;
		assert(!oracle_tisp_awb_init(0,dimensions));
		info=(void *)(uintptr_t)t41_tmo_le32(oracle_bss+0x4114);
		assert(oracle_hw_calls==1 && oracle_gain_calls==1 && oracle_writes==6);
		compare_state(seq,0);
#endif
		for (event=0;event<100;++event) {
			enum t41_awb_control_op op=event%9;
			for (i=0;i<sizeof(input);++i) input[i]=rng();
			if (op==T41_AWB_MODE) t41_ae_put32(input,(seq+event/9)%10);
			if (op==T41_AWB_FREEZE) input[0]=(seq+event/9)&1;
			if (op==T41_AWB_LOCATION) {
				t41_ae_put32(input,0); t41_ae_put32(input+4,0); input[8]=cols; input[9]=rows;
			}
			if (op==T41_AWB_CONVERGE) t41_ae_put32(input,1+rng()%15);
			if (op==T41_AWB_EV) t41_ae_put32(input,(rng()%10000)<<10);
			if (op==T41_AWB_PARAMS) memcpy(input,p,T41_AWB_PARAM_BYTES);
			assert(!t41_awb_control_set(&native,op,input,sizeof(input)));
#ifndef T41_AWB_CONTROL_HOST
			oracle_set(op); compare_state(seq,event);
#endif
			++checks;
			if (event%13==0) {
				/* Each combination of the two distinct refresh flags. */
				native.s[0xc5f9]=(event/13)&1; native.s[0xeaa0]=(event/26)&1;
#ifndef T41_AWB_CONTROL_HOST
				((unsigned char *)(uintptr_t)info[1])[0xc5f9]=native.s[0xc5f9];
				((unsigned char *)(uintptr_t)info[1])[0xeaa0]=native.s[0xeaa0];
				oracle_tisp_awb_params_refresh(0);
#endif
				assert(!t41_awb_params_refresh_owned(&native));
#ifndef T41_AWB_CONTROL_HOST
				compare_state(seq,event);
#endif
			}
			if (event%17==0) {
				assert(!t41_awb_daynight_owned(&native));
#ifndef T41_AWB_CONTROL_HOST
				oracle_tisp_awb_dn_params_refresh(0); compare_state(seq,event);
#endif
			}
			for (i=0;i<=T41_AWB_ZONE;++i) {
				if (i==T41_AWB_CT || i==T41_AWB_LOCATION || i==T41_AWB_EV) continue;
				memset(out,0xa5,sizeof(out)); memset(expected,0xa5,sizeof(expected));
				assert(!t41_awb_control_get(&native,i,out,sizeof(out)));
#ifndef T41_AWB_CONTROL_HOST
				compare(seq,event,"getter",out,expected,oracle_get(i)); compare_state(seq,event);
#endif
				if (i==T41_AWB_PARAMS) assert(!out[sizeof(out)-1] && !out[sizeof(out)-2]);
			}
		}
	}
	/* Reject malformed requests before output/control mutation; EV/PARAMS
 * setters deliberately require candidate/discard semantics on late failure. */
	saved=native; t41_ae_put32(input,10);
	assert(t41_awb_control_set(&native,T41_AWB_MODE,input,12)==-1);
	assert(!memcmp(&native,&saved,sizeof(native)));
	for (i=0;i<=T41_AWB_ZONE;++i) {
		assert(t41_awb_control_set(&native,i,input,0)==-1);
		assert(t41_awb_control_get(&native,i,out,0)==-1);
	}
	assert(!memcmp(&native,&saved,sizeof(native)));
	t41_awb_gain_put16(native.p+0xc6e,0); saved=native;
	assert(t41_awb_params_refresh_owned(&native)==-1 && !memcmp(&native,&saved,sizeof(native)));
#ifdef T41_AWB_CONTROL_HOST
	printf("100 native cold starts, %u control transitions: guards and padding PASS\n",checks);
	return 0;
#else
	printf("100 AWB cold starts, %u control transitions: %u mismatches, %u unexpected accesses\n",checks,mismatches,oracle_bad);
	return mismatches || oracle_bad ? 1 : 0;
#endif
}
