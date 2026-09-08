#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_lsc.h"
static struct { unsigned int before; unsigned char data[T41_LSC_PARAM_BYTES]; unsigned int after; } params;
static struct { unsigned int before; unsigned char data[T41_LSC_STATE_BYTES]; unsigned int after; } state;
static unsigned int seed=47117;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
int main(void)
{
    unsigned char *p=params.data,*s=state.data;
    struct t41_dpc_word words[10]; unsigned int f,i;
    params.before=params.after=state.before=state.after=0xa5a5a5a5;
    memset(words,0xa5,sizeof(words));
    assert(t41_lsc_ct(p,sizeof(params.data)-1,s,sizeof(state.data),5000,1)<0);
    assert(t41_lsc_ct(p,sizeof(params.data),s,sizeof(state.data)-1,5000,1)<0);
    assert(t41_lsc_registers(p,sizeof(params.data),s,sizeof(state.data),0,words,9)<0);
    assert(words[0].address==0xa5a5a5a5);
    assert(t41_lsc_gain(p,sizeof(params.data),s,sizeof(state.data),0,1,2)<0);
    t41_dpc_put16(p+24,1153);
    assert(t41_lsc_ct(p,sizeof(params.data),s,sizeof(state.data),5000,1)<0);
    for(f=0;f<10000;++f) {
        if(f%10==0) {
            for(i=0;i<sizeof(params.data);++i) p[i]=rng();
            for(i=0;i<sizeof(state.data);++i) s[i]=rng();
            t41_dpc_put16(p+16,2000); t41_dpc_put16(p+18,3000);
            t41_dpc_put16(p+20,5000); t41_dpc_put16(p+22,6000);
            t41_dpc_put16(p+24,1+rng()%1152); p[26]=(f/10)&1; s[0x6c28]=(f/20)&1;
            t41_lsc_put32(s+12,5000); t41_lsc_put32(s+16,5);
            t41_lsc_put32(s+20,16); t41_lsc_put32(s+24,0); t41_lsc_put32(s+28,256);
        }
        assert(t41_lsc_ct(p,sizeof(params.data),s,sizeof(state.data),1000+rng()%6000,f%3==0)>=0);
        assert(t41_lsc_gain(p,sizeof(params.data),s,sizeof(state.data),rng()%((16U<<16)+1),f%3==0,f&1)>=0);
        assert(t41_lsc_registers(p,sizeof(params.data),s,sizeof(state.data),f%4,words,10)==
            (f%4==0?10:f%4==1?2:f%4==2?3:1));
        assert(params.before==0xa5a5a5a5 && params.after==0xa5a5a5a5);
        assert(state.before==0xa5a5a5a5 && state.after==0xa5a5a5a5);
    }
    puts("T41 LSC 10000 mesh/ring/IR CT/gain/register cases, bounds and canaries: PASS");
    return 0;
}
