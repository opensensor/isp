/* Actual adapter, private allocations, mocked IRQ/workqueue and MMIO. */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_awb_frame.h"
#define REGTRACE_KERNEL_TREE_BUILD 1
#define DEFINE_MUTEX(n) int n
#define DEFINE_SPINLOCK(n) int n
#define READ_ONCE(v) (v)
#define WRITE_ONCE(v,x) ((v)=(x))
#define smp_store_release(p,v) (*(p)=(v))
#define module_param(...)
#define DMA_FROM_DEVICE 1
#define DMA_BIDIRECTIONAL 2
struct work_struct { void (*fn)(struct work_struct *); int queued; };
#define DECLARE_WORK(n,f) struct work_struct n={f,0}
static unsigned int locks,spins,allocations,drained,fail_alloc,allocation_calls,writes,cache_syncs;
static uint32_t tparamsP_storage[2],tpm_cb_storage[32],awb_info[2],t41_ccm_ct,t41_awb_last_rgain,t41_awb_last_bgain;
static int t41_awb_ready[2];
static uint32_t day_night_storage[2];
static uint32_t registers[0x180c0/4];
static unsigned char calibration[0x978+T41_AWB_PARAM_BYTES],saved_calibration[sizeof(calibration)];
static struct t41_awb_owned before;
static int t41_kernel_data_ptr(const void *p) { return (uintptr_t)p>4096; }
static void mutex_lock(int *p) { assert(!*p && !locks && !spins); *p=1; ++locks; }
static void mutex_unlock(int *p) { assert(*p && locks==1 && !spins); *p=0; --locks; }
#define spin_lock_irqsave(p,f) do { (f)=0; assert(!*(p) && !spins); *(p)=1; ++spins; } while(0)
#define spin_unlock_irqrestore(p,f) do { (void)(f); assert(*(p) && spins==1); *(p)=0; --spins; } while(0)
static void *vzalloc(size_t n)
{
    void *p;
    if (++allocation_calls==fail_alloc) return NULL;
    p=calloc(1,n); if (p) ++allocations; return p;
}
static void vfree(void *p) { if (p) { assert(allocations); --allocations; free(p); } }
static void *private_kmalloc(unsigned int n,unsigned int flags) { (void)flags; return vzalloc(n); }
static void private_kfree(void *p) { vfree(p); }
static uint32_t virt_to_phys(const void *p) { return (uintptr_t)p&0x1fffffff; }
static void t41_tmo_stream_stop(void) { assert(!locks && !spins); ++drained; }
static void system_reg_write(unsigned int a,unsigned int v)
{ assert(locks && !spins && !(a&3) && a<sizeof(registers)); registers[a/4]=v; ++writes; }
static uint32_t system_reg_read(unsigned int a)
{ assert((locks || spins) && !(a&3) && a<sizeof(registers)); return registers[a/4]; }
static void system_irq_func_set(unsigned int c,unsigned int n,unsigned int cb) { assert(!c && n==3 && !cb); }
static void tisp_event_set_cb(unsigned int c,unsigned int n,unsigned int cb) { assert(!c && n==12 && !cb); }
static void dma_cache_sync(void *dev,void *p,unsigned int n,int mode)
{ assert(!dev && p && locks && !spins && (n==8192 || n==32768 || n==131072)); assert(mode==1 || mode==2); ++cache_syncs; }
static int schedule_work(struct work_struct *w) { int old=w->queued; assert(spins); w->queued=1; return !old; }
static void t41_native_awb_queue(void);
static void cancel_work_sync(struct work_struct *w)
{
    assert(!locks && !spins);
    /* Simulate an IRQ arriving after the stop gate but before drain. */
    t41_native_awb_queue(); w->queued=0; ++drained;
}
static void tisp_awb_pm_get_regsize(void) {}
static void tisp_awb_pm_suspend(void) {}
static void tisp_awb_pm_resume(void) {}
#include "../driver/t41/tx_isp_t41_awb_runtime.inc"
static void pump(void)
{
    assert(t41_native_awb_work.queued);
    t41_native_awb_work.queued=0; t41_native_awb_work.fn(&t41_native_awb_work);
}
static void fixture(void)
{
    unsigned char *p=calibration+0x978;
    unsigned int i;
    memset(calibration,0,sizeof(calibration));
    t41_ae_put32(p,100);
    for (i=0;i<4;++i) t41_ae_put32(p+0xc+i*4,1024);
    t41_ae_put32(p+0x28,5000); t41_ae_put32(p+0x30,1024); t41_ae_put32(p+0x34,1024);
    t41_ae_put32(p+0x1c,1000); t41_ae_put32(p+0x20,10000);
    t41_awb_gain_put16(p+0xc6e,15); t41_awb_gain_put16(p+0xc72,15);
    t41_awb_gain_put16(p+0xcd2,16); t41_awb_gain_put16(p+0xcd4,6);
    t41_awb_gain_put16(p+0xcd6,5);
    t41_awb_gain_put16(p+0xd60,1); t41_awb_gain_put16(p+0xd62,1);
    for (i=0;i<9;++i) { t41_ae_put32(p+0xfc+i*4,100+i*1000); t41_awb_gain_put16(p+0xd32+i*2,2); }
    for (i=0;i<8;++i) t41_awb_gain_put16(p+0xd46+i*2,2);
    for (i=0;i<15;++i) { t41_ae_put32(p+0x38+i*4,64+i*128); t41_ae_put32(p+0x74+i*4,64+i*128); }
    for (i=0;i<6;++i) t41_ae_put32(p+0xbf4+i*4,1024);
    for (i=0;i<225;++i) { t41_ae_put32(p+0x4ec+i*4,128); t41_ae_put32(p+0x870+i*4,200); p[0x1200+i]=1; }
    memcpy(saved_calibration,calibration,sizeof(calibration));
}
static void populate(struct t41_awb_runtime *s)
{
    unsigned int bank,i;
    for (bank=0;bank<4;++bank) for (i=0;i<225*8;++i) {
        unsigned char *p=s->dma+bank*32768+i*16;
        unsigned int pixels=128,r=24000,g=32000,b=40000,lum=32000;
        t41_ae_put32(p,r|(g<<22)); t41_ae_put32(p+4,(g>>10)|(b<<12));
        t41_ae_put32(p+8,(b>>20)|(lum<<2)|(pixels<<24)); t41_ae_put32(p+12,pixels>>8);
    }
}
int main(void)
{
    unsigned int config[2]={480,480},i,n,values[7]={0},pm[110],*cursor=pm;
    struct t41_awb_runtime *s;
    unsigned char freeze=1,parameters[T41_AWB_PARAM_BYTES];
    assert((uintptr_t)calibration<=UINT32_MAX); /* Compile host harness -no-pie. */
    tparamsP_storage[0]=(uintptr_t)calibration; fixture();
    assert(t41_native_awb_exposure(0,400)==-ENODEV);
    for (i=1;i<=2;++i) {
        allocation_calls=0; fail_alloc=i;
        assert(t41_native_awb_init(0,config)==-ENOMEM && !allocations && !writes && !awb_info[0]);
    }
    fail_alloc=0; t41_awb_gain_put16(calibration+0x978+0xc6e,0);
    assert(t41_native_awb_init(0,config)==-EINVAL && !allocations && !writes);
    fixture(); assert(!t41_native_awb_init(0,config)); s=t41_native_awb_object(0);
    assert(s && allocations==2 && t41_awb_ready[0] && t41_native_awb_init(0,config)==-EBUSY);
    assert(s->info[0]==(uint32_t)(uintptr_t)s->saved.p && s->info[1]==(uint32_t)(uintptr_t)s->saved.s);
    assert(!memcmp(saved_calibration,calibration,sizeof(calibration)));
    populate(s); t41_native_awb_start(); assert(t41_native_awb_live);
    assert(t41_native_awb_deinit(0)==-EBUSY && allocations==2);
    for (i=0;i<100;++i) { registers[0x18050/4]=i&3; t41_native_awb_queue(); pump(); assert(!t41_native_awb_error); }
    assert(t41_native_awb_frames==100 && !t41_native_awb_dropped);
    before=s->saved; n=writes;
    assert(!t41_native_awb_observe(0,values) && values[0]==1365 && values[1]==819);
    assert(!t41_native_awb_observe(1,values));
    assert(!memcmp(&before,&s->saved,sizeof(before)) && writes==n);
    assert(!t41_native_awb_control(0,T41_AWB_FREEZE,&freeze,1,0)); n=writes;
    values[0]=registers[0x4004/4]; values[1]=registers[0x4008/4];
    t41_native_awb_queue(); pump(); assert(writes==n+7); /* six region controls and rearm, no WB */
    assert(values[0]==registers[0x4004/4] && values[1]==registers[0x4008/4]);
    assert(!t41_native_awb_control(0,T41_AWB_MODE,values,sizeof(values),1));
    values[0]=9; values[1]=40; values[2]=24;
    assert(!t41_native_awb_control(0,T41_AWB_MODE,values,sizeof(values),0));
    freeze=0; assert(!t41_native_awb_control(0,T41_AWB_FREEZE,&freeze,1,0));
    t41_native_awb_queue(); pump(); assert(!t41_native_awb_error);
    before=s->saved; n=writes; values[0]=10;
    assert(t41_native_awb_control(0,T41_AWB_MODE,values,sizeof(values),0)==-EINVAL);
    assert(!memcmp(&before,&s->saved,sizeof(before)) && writes==n);
    memcpy(parameters,s->saved.p,sizeof(parameters)); t41_awb_gain_put16(parameters+0xc6e,14);
    assert(t41_native_awb_control(0,T41_AWB_PARAMS,parameters,sizeof(parameters),0)==-EBUSY);
    assert(!memcmp(&before,&s->saved,sizeof(before)) && writes==n);
    assert(!t41_native_awb_exposure(65536,2000)); n=writes;
    assert(!t41_native_awb_exposure(65536,2000) && writes==n);
    for (i=0;i<4;++i) assert(!t41_native_awb_writer(0,i));
    assert(!t41_native_awb_compat(0,1501,2403,0,NULL));
    assert(!t41_native_awb_compat(0,0,0,1,values));
    assert(!values[0] && values[1]==1501 && values[2]==2403);
    t41_native_awb_queue(); pump();
    assert(registers[0x4004/4]==(0x04000000|1501) && registers[0x4008/4]==(0x04000000|2403));
    assert(!t41_native_awb_compat(1,1501,2403,0,NULL));
    t41_native_awb_queue(); pump(); assert(!t41_native_awb_error && !s->saved.s[0xeaa1]);
    assert(!t41_native_awb_refresh(0,1)); assert(!memcmp(saved_calibration,calibration,sizeof(calibration)));
    values[0]=1200; values[1]=1400;
    assert(!t41_native_awb_start_gain(0,values));
    assert(s->has_day_start && t41_tmo_le32(s->saved.p+0xc)==1200);
    day_night_storage[0]=1; assert(!t41_native_awb_refresh(0,1));
    assert(t41_tmo_le32(s->saved.p+0xc)==1024);
    day_night_storage[0]=0; assert(!t41_native_awb_refresh(0,1));
    assert(t41_tmo_le32(s->saved.p+0xc)==1200 && t41_tmo_le32(s->saved.p+0x18)==1400);
    assert(!memcmp(saved_calibration,calibration,sizeof(calibration)));
    t41_native_awb_queue(); t41_native_awb_queue(); pump(); assert(t41_native_awb_dropped==1);
    before=s->saved; s->saved.p[0xcd2]=0; n=writes;
    t41_native_awb_queue(); pump(); assert(t41_native_awb_error==-ERANGE && writes==n+1);
    before.p[0xcd2]=0; assert(!memcmp(&before,&s->saved,sizeof(before)));
    s->saved.p[0xcd2]=16;
    t41_native_awb_queue(); t41_native_awb_stop(); assert(!t41_native_awb_work.queued && !t41_native_awb_pending);
    n=writes; t41_native_awb_queue(); assert(!t41_native_awb_work.queued && writes==n);
    assert(!t41_native_awb_suspend(0,&cursor) && cursor==pm+110);
    for (i=0;i<55;++i) {
        unsigned int a=i<47 ? 0x18004+i*4 : i<51 ? 0x4004+(i-47)*4 : 0x5004+(i-51)*4;
        assert(pm[i*2]==a && pm[i*2+1]==registers[a/4]);
    }
    assert(!t41_native_awb_deinit(0) && !allocations && !awb_info[0] && !t41_awb_ready[0]);
    assert(!tpm_cb_storage[3] && !tpm_cb_storage[4] && !tpm_cb_storage[5]);
    assert(!t41_native_awb_deinit(0) && !allocations);
    assert(!t41_native_awb_init(0,config) && !t41_native_awb_deinit(0));
    assert(!locks && !spins && !allocations);
    puts("T41 AWB adapter: private calibration, frame publication, controls, IRQ drain, PM and failure unwinding PASS");
    return 0;
}
