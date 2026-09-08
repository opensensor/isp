/* Exercise the actual kernel adapter with private memory and recorded MMIO. */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../driver/t41/tx_isp_t41_lsc.h"
#define REGTRACE_KERNEL_TREE_BUILD 1
#define BIT(n) (1U << (n))
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
#define DEFINE_MUTEX(n) int n
#define module_param(...)
#define module_param_named(...)
static unsigned int locks,allocations,drained,fail_alloc,fail_restore,fail_probe;
static unsigned int writes,ram_words,ram_commits,ram_open;
static uint32_t tparamsP_storage[2],top_bypass_global[2],tpm_cb_storage[32],lsc_info[2];
static uint8_t calibration[0x4178+T41_LSC_PARAM_BYTES],saved_calibration[sizeof(calibration)];
static int t41_kernel_data_ptr(const void *p) { return (uintptr_t)p>4096; }
static void mutex_lock(int *p) { assert(!*p && !locks); *p=1; ++locks; }
static void mutex_unlock(int *p) { assert(*p && locks==1); *p=0; --locks; }
static void *vzalloc(size_t n) { void *p; if(fail_alloc) return NULL; p=calloc(1,n); if(p) ++allocations; return p; }
static void vfree(void *p) { if(p) { assert(allocations); --allocations; free(p); } }
static void t41_tmo_stream_stop(void) { assert(!locks); ++drained; }
static void system_reg_write(unsigned int a,unsigned int v)
{
    assert(locks==1); ++writes;
    if(a==0x50020) {
        if(v==0x101) { assert(!ram_open); ram_open=1; ram_words=0; }
        else { assert(ram_open && (v&65535)==0x102 && ram_words==((v>>16)+1)*3); ram_open=0; ++ram_commits; }
    } else if(a==0x50024) { assert(ram_open); ++ram_words; }
    else assert(a==0x40 || a==0x314 || a==0x3100 || (a>=0x3000 && a<=0x3020));
}
static uint32_t system_reg_read(unsigned int a) { assert(a>=0x3000 && a<0x3100); return a^0x1234; }
static int t41_top_restore_bypass(unsigned int mask)
{ if(fail_restore) return -EINVAL; top_bypass_global[0]&=~mask; system_reg_write(0x40,top_bypass_global[0]); return 0; }
static void system_irq_func_set(unsigned int c,unsigned int n,unsigned int callback)
{ assert(!c && n==36 && !callback); }
static int probe_kernel_read(void *d,const void *s,unsigned int n)
{ if(fail_probe || !t41_kernel_data_ptr(s)) return -EFAULT; memcpy(d,s,n); return 0; }
static void tisp_lsc_pm_get_regsize(void) {}
static void tisp_lsc_pm_suspend(void) {}
static void tisp_lsc_pm_resume(void) {}
#include "../driver/t41/tx_isp_t41_lsc_runtime.inc"
int main(void)
{
    unsigned int config[33]={640,480,0},i,n,commits;
    uint8_t *p=calibration+0x4178,controls[0x68],diagnostic;
    uint32_t bytes,pm[128],*cursor=pm;
    struct t41_lsc_runtime *s;
    assert((uintptr_t)calibration<=UINT32_MAX); /* Build this harness -no-pie. */
    tparamsP_storage[0]=(uintptr_t)calibration;
    p[78]=32; p[79]=32; p[80]=11; t41_dpc_put16(p+24,176);
    t41_dpc_put16(p+16,2000); t41_dpc_put16(p+18,3000);
    t41_dpc_put16(p+20,5000); t41_dpc_put16(p+22,6000);
    for(i=27;i<71;++i) p[i]=i*3;
    for(i=0x68;i<T41_LSC_PARAM_BYTES;++i) p[i]=i*7;
    memcpy(saved_calibration,calibration,sizeof(calibration));
    assert(t41_native_lsc_frame(5000,0)==-ENODEV);
    fail_alloc=1; assert(t41_native_lsc_init(0,config)==-ENOMEM); fail_alloc=0;
    fail_restore=1; assert(t41_native_lsc_init(0,config)==-EINVAL); fail_restore=0;
    assert(!allocations && !lsc_info[0] && !t41_lsc_object(0));
    assert(!t41_native_lsc_init(0,config)); s=t41_lsc_object(0); assert(s && allocations==1);
    assert(t41_native_lsc_init(0,config)==-EBUSY);
    assert(t41_lsc_applied_ct==5000 && !t41_lsc_applied_gain && !top_bypass_global[0]);
    assert(!t41_native_lsc_frame(4500,0)); n=writes;
    assert(!t41_native_lsc_frame(4510,0) && writes==n);
    assert(!t41_native_lsc_frame(4510,0) && writes==n);
    assert(!t41_native_lsc_frame(2500,65536) && writes>n);
    n=writes; commits=ram_commits;
    assert(!t41_native_lsc_frame(2500,131072) && writes==n+2 && ram_commits==commits);
    t41_native_lsc_reapply(); assert(!t41_native_lsc_frame(2500,131072) && ram_commits==commits+1);
    assert(!t41_native_lsc_get(0,controls,&bytes) && bytes==sizeof(controls));
    n=writes; controls[78]=0;
    assert(t41_native_lsc_set(0,0,controls)==-EINVAL && writes==n && s->saved.params[78]==32);
    controls[78]=32; assert(!t41_native_lsc_set(0,0,controls));
    n=writes; assert(!t41_native_lsc_set(0,1,p+0x68) && s->staging && writes==n);
    assert(!t41_native_lsc_frame(5500,196608) && s->staging);
    assert(!t41_native_lsc_set(0,2,p+0x3680));
    assert(!t41_native_lsc_set(0,3,p+0x6c80));
    assert(!t41_native_lsc_set(0,4,p+0xa280) && !s->staging);
    assert(!t41_native_lsc_flip(640,480,1,1));
    assert(!t41_native_lsc_refresh(0,1) && s->wdr==1 && s->saved.state[0x6c33]==1);
    assert(!t41_native_lsc_refresh(0,-1) && s->saved.state[0x6c34]==1);
    assert(!memcmp(saved_calibration,calibration,sizeof(calibration)));
    assert(!t41_native_lsc_interpolate(0,3500,1,0) && s->reapply);
    assert(!t41_native_lsc_interpolate(0,262144,1,1));
    assert(!t41_native_lsc_prepare(0));
    assert(!t41_native_lsc_writer(0,0,1));
    assert(t41_native_lsc_geometry(0,480,640,32,32,11)==1);
    assert(!t41_native_lsc_diagnostic(0,&diagnostic));
    assert(!t41_native_lsc_suspend(0,&cursor) && cursor==pm+128);
    for(i=0;i<64;++i) assert(pm[i*2]==0x3000+i*4 && pm[i*2+1]==((0x3000+i*4)^0x1234));
    assert(!t41_native_lsc_set(0,1,p+0x68)); fail_probe=1;
    assert(t41_native_lsc_set(0,2,p+0x3680)==-EFAULT && !s->staging); fail_probe=0;
    assert(!t41_native_lsc_deinit(0) && drained==1 && !allocations && !lsc_info[0]);
    assert(!tpm_cb_storage[12] && !tpm_cb_storage[13] && !tpm_cb_storage[14]);
    assert(!t41_native_lsc_deinit(0) && drained==2);
    assert(!t41_native_lsc_init(0,config) && !t41_native_lsc_deinit(0));
    assert(!allocations && !locks && !ram_open);
    puts("T41 LSC runtime ownership, staged tuning, hysteresis, RAM commit, PM and failure unwinding: PASS");
    return 0;
}
