#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_lsc.h"
unsigned char oracle_bss[0x5000] __attribute__((aligned(65536)));
unsigned char oracle_rodata[0x4000] __attribute__((aligned(65536)));
char oracle_message[256];
unsigned int oracle_addresses[7000],oracle_values[7000],oracle_count,oracle_bad;
unsigned int oracle_params[2],oracle_callbacks[32];
static unsigned char oracle_parameter_buffer[0x4178+T41_LSC_PARAM_BYTES];
int oracle_noop(void) { return 0; }
unsigned int oracle_read(unsigned int address) { if(address!=0x40) ++oracle_bad; return 0; }
unsigned int oracle_div64(unsigned long long *v,unsigned int divisor)
{ unsigned int r=*v%divisor; *v/=divisor; return r; }
void oracle_write(unsigned int address,unsigned int value)
{
    if(oracle_count>=7000) { ++oracle_bad; return; }
    oracle_addresses[oracle_count]=address; oracle_values[oracle_count++]=value;
}
int oracle_unexpected(void) { ++oracle_bad; return -1; }
extern int oracle_tisp_lsc_ct_interp(unsigned int,unsigned int,unsigned int);
extern int oracle_tisp_lsc_gain_interp(unsigned int,unsigned int,unsigned int);
extern int oracle_tisp_lsc_write_reg(unsigned int,unsigned int);
extern int oracle_tisp_lsc_mirror_flip(unsigned int,unsigned int,unsigned int,unsigned int);
extern int oracle_tisp_lsc_init(unsigned int,const unsigned int *);
extern int oracle_tisp_lsc_wdr_en(unsigned int,unsigned int);
extern int oracle_tisp_lsc_dn_params_refresh(unsigned int);
extern int oracle_tisp_lsc_lut_valid_judge(unsigned int,unsigned int,unsigned int,unsigned int,unsigned int,unsigned int);
static unsigned char p[T41_LSC_PARAM_BYTES],actual[T41_LSC_STATE_BYTES],expected[T41_LSC_STATE_BYTES];
static unsigned char reference_p[T41_LSC_PARAM_BYTES],original_p[T41_LSC_PARAM_BYTES];
void *oracle_kmalloc(unsigned int bytes,unsigned int flags)
{ (void)flags; if(bytes!=sizeof(expected)) { ++oracle_bad; return 0; } return expected; }
void *oracle_copy(void *d,const void *s,unsigned int n) { return memcpy(d,s,n); }
void *oracle_fill(void *d,int value,unsigned int n) { return memset(d,value,n); }
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
    unsigned int f,i,rejected=0;
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
    for(f=0;f<2000;++f) {
        unsigned int rows=2+rng()%40,columns=2+rng()%40,stride=(columns+1)&~1U;
        unsigned int width=(columns-1)*32,height=(rows-1)*32,v=f&1,h=(f>>1)&1;
        int a,b;
        if(f%8==0) {
            for(i=0;i<sizeof(p);++i) p[i]=rng();
            for(i=0;i<sizeof(actual);++i) actual[i]=rng();
        }
        p[78]=32; p[79]=32; p[80]=stride/2; p[26]=(f/8)&1;
        t41_dpc_put16(p+24,rows*stride/2);
        t41_dpc_put16(p+16,2000); t41_dpc_put16(p+18,3000);
        t41_dpc_put16(p+20,5000); t41_dpc_put16(p+22,6000);
        t41_lsc_put32(actual+12,2000+rng()%4001);
        actual[0x6c33]=(f>>2)&1; actual[0x6c34]=(f>>3)&1; actual[0x6c28]=(f/16)&1;
        memcpy(expected,actual,sizeof(actual)); memcpy(reference_p,p,sizeof(p));
        t41_lsc_put32(expected,(uintptr_t)reference_p);
        a=t41_lsc_flip(p,sizeof(p),actual,sizeof(actual),width,height,v,h,0);
        oracle_count=0; b=oracle_tisp_lsc_mirror_flip(width,height,v,h);
        compare(f,"flip",a,b);
        for(i=0;i<sizeof(p);++i) if(p[i]!=reference_p[i]) {
            if(failures++<20) printf("LSC flip params f=%u +%x native=%x OEM=%x grid=%ux%u flip=%u/%u\n",
                f,i,p[i],reference_p[i],columns,rows,v,h);
            break;
        }
    }
    printf("2000 LSC geometry/mesh flips with ring and IR packing: %u cumulative mismatches, %u unexpected writes\n",failures,oracle_bad);
    for(f=0;f<10000;++f) {
        unsigned int width=1+rng()%8192,height=1+rng()%8192,sx=1+rng()%128,sy=1+rng()%128;
        unsigned int pairs=f&1 ? (((width+sx-1)/sx+2)&~1U)/2 : 1+rng()%128;
        int a,b;
        actual[0x6c35]=rng(); expected[0x6c35]=actual[0x6c35];
        a=t41_lsc_geometry(actual,sizeof(actual),height,width,sy,sx,pairs);
        b=oracle_tisp_lsc_lut_valid_judge(0,height,width,sy,sx,pairs);
        if(a==-2) { ++rejected; continue; }
        if(a!=b || actual[0x6c35]!=expected[0x6c35]) {
            if(failures++<20) printf("LSC geometry f=%u native=%d/%x OEM=%d/%x\n",f,a,actual[0x6c35],b,expected[0x6c35]);
        }
    }
    printf("10000 LSC geometry diagnostics: %u unsafe product wraps rejected, %u cumulative valid-input mismatches\n",rejected,failures);
    oracle_params[0]=(uintptr_t)oracle_parameter_buffer;
    for(f=0;f<40;++f) {
        unsigned int config[33]={0},j,width=256+rng()%500,height=256+rng()%500;
        unsigned int pairs=(((width+31)/32+2)&~1U)/2,wdr=f&1;
        int a,b;
        for(i=0;i<sizeof(p);++i) p[i]=rng();
        p[78]=32; p[79]=32; p[80]=pairs; p[26]=(f/2)&1;
        t41_dpc_put16(p+24,((height+31)/32+1)*pairs);
        t41_dpc_put16(p+16,2000); t41_dpc_put16(p+18,3000);
        t41_dpc_put16(p+20,5000); t41_dpc_put16(p+22,6000);
        memcpy(original_p,p,sizeof(p));
        memcpy(oracle_parameter_buffer+0x4178,p,sizeof(p));
        config[0]=width; config[1]=height; config[2]=f%8; config[30]=wdr;
        a=t41_lsc_initialize(p,sizeof(p),actual,sizeof(actual),width,height,config[2],wdr);
        oracle_count=0; b=oracle_tisp_lsc_init(0,config); compare(f,"init",a,b);
        for(j=0;j<8;++j) {
            wdr^=1;
            a=t41_lsc_gain(p,sizeof(p),actual,sizeof(actual),t41_tmo_le32(actual+24),1,wdr);
            if(a>=0) a=t41_lsc_ct(p,sizeof(p),actual,sizeof(actual),t41_tmo_le32(actual+12),1);
            oracle_count=0; b=oracle_tisp_lsc_wdr_en(0,wdr); compare(f*8+j,"mode",a,b);
            a=t41_lsc_flip(p,sizeof(p),actual,sizeof(actual),width,height,j&1,(j>>1)&1,0);
            oracle_count=0; b=oracle_tisp_lsc_mirror_flip(width,height,j&1,(j>>1)&1); compare(f*8+j,"mode-flip",a,b);
            memcpy(p,original_p,sizeof(p)); memcpy(oracle_parameter_buffer+0x4178,p,sizeof(p));
            a=t41_lsc_refresh(p,sizeof(p),actual,sizeof(actual),wdr,0);
            oracle_count=0; b=oracle_tisp_lsc_dn_params_refresh(0); compare(f*8+j,"refresh",a,b);
            if(memcmp(p,oracle_parameter_buffer+0x4178,sizeof(p))) {
                if(failures++<20) printf("LSC refresh parameter mismatch f=%u j=%u\n",f,j);
            }
        }
    }
    printf("40 LSC cold starts and 320 linear/WDR, flip and day/night replacements: %u cumulative mismatches, %u unexpected writes\n",failures,oracle_bad);
    return failures || oracle_bad ? 1:0;
}
