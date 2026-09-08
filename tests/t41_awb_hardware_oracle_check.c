#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_frame.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
extern int oracle_tisp_awb_set_hardware_param(unsigned int);
extern int oracle_tisp_awb_set_regional_threshold(unsigned int);
extern int oracle_tisp_awb_set_lum_th_freq(unsigned int);
extern unsigned int oracle_words[18][2],oracle_writes,oracle_bad;
static struct t41_awb_owned o,q;
static unsigned int info[7];
static uint32_t seed=622429;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
int main(void)
{
    unsigned int f,i,j,fail=0;
    info[0]=(uintptr_t)q.p; info[1]=(uintptr_t)q.s;
    t41_ae_put32(oracle_bss+0x4114,(uintptr_t)info);
    for (f=0;f<10000;++f) {
        struct t41_awb_register words[T41_AWB_HARDWARE_WORDS];
        unsigned int n,geometry=f&1;
        for (i=0;i<sizeof(o);++i) ((unsigned char *)&o)[i]=rng();
        t41_awb_gain_put16(o.p+0xc6c,rng()%4096); t41_awb_gain_put16(o.p+0xc70,rng()%4096);
        t41_awb_gain_put16(o.p+0xc6e,1+rng()%15); t41_awb_gain_put16(o.p+0xc72,1+rng()%15);
        for (i=0;i<30;++i) t41_awb_gain_put16(o.p+0xc74+i*2,rng()%256);
        t41_awb_gain_put16(o.p+0xcc8,rng()%256); t41_awb_gain_put16(o.p+0xcca,rng()%2);
        t41_awb_gain_put16(o.p+0xccc,rng()%4);
        o.s[0xc5f8]=(f>>1)&1; o.s[0xeaa1]=(f>>2)&1;
        t41_ae_put32(o.s+0xea9c,rng()%(13*65536));
        q=o; oracle_writes=0;
        if (t41_awb_hardware(&o,geometry,words,&n)) return 2;
        if (geometry) oracle_tisp_awb_set_hardware_param(0);
        else { oracle_tisp_awb_set_regional_threshold(0); oracle_tisp_awb_set_lum_th_freq(0); }
        if (memcmp(&o,&q,sizeof(o)) || n!=oracle_writes || memcmp(words,oracle_words,n*sizeof(words[0]))) {
            if (fail++<10) {
                printf("frame=%u writes=%u/%u\n",f,n,oracle_writes);
                for (i=0,j=0;i<sizeof(o) && j<4;++i) if (((unsigned char *)&o)[i]!=((unsigned char *)&q)[i]) {
                    printf(" +%x: %x/%x\n",i,((unsigned char *)&o)[i],((unsigned char *)&q)[i]); ++j;
                }
            }
        }
    }
    printf("10000 AWB hardware transactions: %u mismatches, %u unexpected accesses\n",fail,oracle_bad);
    return fail || oracle_bad ? 1 : 0;
}
