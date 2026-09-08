#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_lsc.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
unsigned char oracle_rodata[0x4000] __attribute__((aligned(65536)));
char oracle_message[256];
unsigned int oracle_addresses[16],oracle_values[16],oracle_count,oracle_bad;
int oracle_noop(void) { return 0; }
void oracle_write(unsigned int address,unsigned int value)
{
    if(oracle_count>=16) { ++oracle_bad; return; }
    oracle_addresses[oracle_count]=address; oracle_values[oracle_count++]=value;
}
int oracle_unexpected(void) { ++oracle_bad; return -1; }
extern int oracle_tisp_lsc_ct_interp(unsigned int,unsigned int,unsigned int);
extern int oracle_tisp_lsc_gain_interp(unsigned int,unsigned int,unsigned int);
extern int oracle_tisp_lsc_write_reg(unsigned int,unsigned int);
static unsigned char p[T41_LSC_PARAM_BYTES],actual[T41_LSC_STATE_BYTES],expected[T41_LSC_STATE_BYTES];
static unsigned int seed=8317,failures;
static unsigned int rng(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static void compare(unsigned int frame,const char *which,int a,int b)
{
    unsigned int i;
    if(a!=b && failures++<20) printf("LSC %s return f=%u native=%d OEM=%d\n",which,frame,a,b);
    for(i=4;i<sizeof(actual);++i) {
        if(i>=0x3620 && i<0x3628) continue; /* Private oracle's calibration pointers. */
        if(actual[i]!=expected[i]) {
            if(failures++<20) printf("LSC %s bytes f=%u +%x native=%x OEM=%x mode=%u count=%u ct=%u zone=%u\n",
                which,frame,i,actual[i],expected[i],p[26],t41_tmo_le16(p+24),t41_tmo_le32(actual+12),actual[16]);
            break;
        }
    }
}
int main(void)
{
    unsigned int f,i;
    t41_lsc_put32(oracle_bss+0x4140,(uintptr_t)expected);
    for(f=0;f<10000;++f) {
        unsigned int ct,force,wdr=f&1,gain,mode=f%4; int a,b,n;
        struct t41_dpc_word words[10];
        if(f%10==0) {
            for(i=0;i<sizeof(p);++i) p[i]=rng();
            for(i=0;i<sizeof(actual);++i) actual[i]=rng();
            t41_dpc_put16(p+16,2000); t41_dpc_put16(p+18,3000);
            t41_dpc_put16(p+20,5000); t41_dpc_put16(p+22,6000);
            p[26]=(f/10)%2; actual[0x6c28]=(f/20)%2;
            t41_dpc_put16(p+24,(f/10)%3==0?1152:1+rng()%1152);
            t41_lsc_put32(actual+12,5000); t41_lsc_put32(actual+16,5);
            t41_lsc_put32(actual+20,16); t41_lsc_put32(actual+24,0); t41_lsc_put32(actual+28,256);
            memcpy(expected,actual,sizeof(actual));
            t41_lsc_put32(expected,(uintptr_t)p);
        }
        ct=f%10==0?5000:f%10==1?5016:f%10==2?5017:f%10==3?2000:
           f%10==4?2001:f%10==5?2999:f%10==6?3000:f%10==7?5999:f%10==8?6000:rng()%10000;
        force=f%10==0 || f%3==0;
        a=t41_lsc_ct(p,sizeof(p),actual,sizeof(actual),ct,force);
        b=oracle_tisp_lsc_ct_interp(0,ct,force); compare(f,"CT",a,b);
        gain=f%5==0?t41_tmo_le32(actual+24)+256:rng()%((16U<<16)+1);
        t41_lsc_put32(expected+0x3620,(uintptr_t)(p+(wdr?49:27)));
        t41_lsc_put32(expected+0x3624,(uintptr_t)(p+(wdr?60:38)));
        a=t41_lsc_gain(p,sizeof(p),actual,sizeof(actual),gain,force,wdr);
        b=oracle_tisp_lsc_gain_interp(0,gain,force); compare(f,"gain",a,b);
        n=t41_lsc_registers(p,sizeof(p),actual,sizeof(actual),mode,words,10);
        oracle_count=0; oracle_tisp_lsc_write_reg(0,mode);
        if(n!=(int)oracle_count) { if(failures++<20) printf("LSC count f=%u native=%d OEM=%u\n",f,n,oracle_count); }
        else for(i=0;i<oracle_count;++i) if(words[i].address!=oracle_addresses[i] || words[i].value!=oracle_values[i]) {
            if(failures++<20) printf("LSC register f=%u word=%u %x=%x OEM=%x=%x\n",f,i,
                words[i].address,words[i].value,oracle_addresses[i],oracle_values[i]);
            break;
        }
    }
    printf("10000 LSC mesh/ring/IR CT history, gain and register cases: %u mismatches, %u unexpected writes\n",failures,oracle_bad);
    return failures || oracle_bad ? 1:0;
}
