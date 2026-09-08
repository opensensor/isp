/* SPDX-License-Identifier: MIT */
#ifdef REGTRACE_KERNEL_TREE_BUILD
#include "tx_isp_t41_awb_frame.h"
struct t41_awb_runtime {
    struct t41_awb_owned saved, candidate;
    uint32_t info[7];
    uint32_t day_start[2], has_day_start;
    uint8_t *dma;
};
static struct t41_awb_runtime *t41_awb_runtime = (void *)1;
static DEFINE_MUTEX(t41_native_awb_lock);
static DEFINE_SPINLOCK(t41_native_awb_irq_lock);
static unsigned int t41_native_awb_live, t41_native_awb_pending, t41_native_awb_bank;
static unsigned int t41_native_awb_frames = ~0U, t41_native_awb_dropped = ~0U;
static int t41_native_awb_error = -ENODEV;
module_param(t41_native_awb_frames, uint, 0444);
module_param(t41_native_awb_dropped, uint, 0444);
module_param(t41_native_awb_error, int, 0444);
static void t41_native_awb_workfn(struct work_struct *work);
static DECLARE_WORK(t41_native_awb_work, t41_native_awb_workfn);

static struct t41_awb_runtime *t41_native_awb_object(unsigned int channel)
{
    return !channel && t41_kernel_data_ptr(t41_awb_runtime) ? t41_awb_runtime : NULL;
}
static void t41_native_awb_emit(const struct t41_awb_register *words, unsigned int n)
{
    unsigned int i;
    for (i=0;i<n;++i) {
        system_reg_write(words[i].address,words[i].value);
        if (words[i].address==0x4004) WRITE_ONCE(t41_awb_last_rgain,words[i].value&0x3fff);
        if (words[i].address==0x4008) WRITE_ONCE(t41_awb_last_bgain,words[i].value&0x3fff);
    }
}
static void t41_native_awb_save(struct t41_awb_runtime *s)
{
    memcpy(&s->saved,&s->candidate,sizeof(s->saved));
    WRITE_ONCE(t41_ccm_ct,t41_tmo_le32(s->saved.p+0x28));
}
static void t41_native_awb_stop(void)
{
    unsigned long flags;
    /* Gate and schedule share a lock; draining alone permits late requeue. */
    spin_lock_irqsave(&t41_native_awb_irq_lock,flags);
    t41_native_awb_live=0; t41_native_awb_pending=0;
    spin_unlock_irqrestore(&t41_native_awb_irq_lock,flags);
    cancel_work_sync(&t41_native_awb_work);
}
static void t41_native_awb_start(void)
{
    unsigned long flags;
    mutex_lock(&t41_native_awb_lock);
    if (t41_native_awb_object(0)) {
        spin_lock_irqsave(&t41_native_awb_irq_lock,flags);
        t41_native_awb_pending=0; t41_native_awb_live=1;
        spin_unlock_irqrestore(&t41_native_awb_irq_lock,flags);
        system_reg_write(0x18000,1);
        system_reg_write(0x4000,1); system_reg_write(0x5000,1);
    }
    mutex_unlock(&t41_native_awb_lock);
}
static void t41_native_awb_queue(void)
{
    unsigned long flags;
    spin_lock_irqsave(&t41_native_awb_irq_lock,flags);
    if (t41_native_awb_live) {
        if (t41_native_awb_pending) ++t41_native_awb_dropped;
        t41_native_awb_bank=system_reg_read(0x18050)&3U;
        t41_native_awb_pending=1;
        schedule_work(&t41_native_awb_work);
    }
    spin_unlock_irqrestore(&t41_native_awb_irq_lock,flags);
}
static void t41_native_awb_workfn(struct work_struct *work)
{
    struct t41_awb_runtime *s;
    struct t41_awb_frame_buffers buffers;
    struct t41_awb_register words[T41_AWB_FRAME_WORDS];
    unsigned long flags;
    unsigned int bank,bytes,rgb[3],i,n;
    int pending,ret=-EAGAIN;
    (void)work;
    mutex_lock(&t41_native_awb_lock);
    s=t41_native_awb_object(0);
    spin_lock_irqsave(&t41_native_awb_irq_lock,flags);
    pending=t41_native_awb_pending && t41_native_awb_live;
    bank=t41_native_awb_bank; t41_native_awb_pending=0;
    spin_unlock_irqrestore(&t41_native_awb_irq_lock,flags);
    if (!s || !pending) goto done;
    memcpy(&s->candidate,&s->saved,sizeof(s->candidate));
    if (s->candidate.p[0xcca]>1) { ret=-EINVAL; goto failed; }
    bytes=s->candidate.p[0xcca] ? 8192U : 32768U;
    dma_cache_sync(NULL,s->dma+bank*bytes,bytes,DMA_FROM_DEVICE);
    for (i=0;i<3;++i) rgb[i]=system_reg_read(0x180ac+i*4);
    buffers=t41_awb_buffers(&s->candidate);
    ret=t41_awb_frame(&buffers,s->candidate.red,s->candidate.blue,
        s->dma+bank*bytes,bytes,rgb,words,&n) ? -ERANGE : 0;
    if (!ret) {
        /* Exactly one publication after the entire candidate has validated. */
        t41_native_awb_emit(words,n);
        t41_native_awb_save(s);
        ++t41_native_awb_frames;
    }
failed:
    if (ret) ++t41_native_awb_dropped;
    /* A malformed frame never publishes gains, but must not stall capture. */
    if (READ_ONCE(t41_native_awb_live)) system_reg_write(0x18000,1);
    t41_native_awb_error=ret;
done:
    mutex_unlock(&t41_native_awb_lock);
}
static int t41_native_awb_init(unsigned int channel, const uint32_t *config)
{
    struct t41_awb_runtime *s;
    struct t41_awb_frame_buffers buffers;
    struct t41_awb_register words[T41_AWB_HARDWARE_WORDS+10];
    const uint8_t *p;
    unsigned int n,m,i;
    int ret=-EINVAL;
    if (channel || !t41_kernel_data_ptr(config)) return -EINVAL;
    mutex_lock(&t41_native_awb_lock);
    if (t41_native_awb_object(0)) { ret=-EBUSY; goto done; }
    p=(void *)(uintptr_t)((uint32_t *)(void *)tparamsP_storage)[0];
    if (!t41_kernel_data_ptr(p)) goto done;
    s=vzalloc(sizeof(*s));
    if (!s) { ret=-ENOMEM; goto done; }
    if (t41_awb_cold(&s->candidate,p+0x978,T41_AWB_PARAM_BYTES,config[0],config[1]) ||
        t41_awb_hardware(&s->candidate,1,words,&n)) goto free_state;
    buffers=t41_awb_buffers(&s->candidate);
    if (t41_awb_gain_words(&buffers,words+n,&m)) goto free_state;
    s->dma=private_kmalloc(131072,0x024000c0);
    if (!s->dma) { ret=-ENOMEM; goto free_state; }
    memset(s->dma,0,131072);
    dma_cache_sync(NULL,s->dma,131072,DMA_BIDIRECTIONAL);
    s->info[0]=(uintptr_t)s->saved.p; s->info[1]=(uintptr_t)s->saved.s;
    s->info[2]=(uintptr_t)s->saved.control; s->info[3]=(uintptr_t)s->saved.report;
    s->info[4]=4; s->info[5]=(uintptr_t)s->dma; s->info[6]=virt_to_phys(s->dma);
    /* Allocation and all math validation precede the first DMA/MMIO write. */
    for (i=0;i<4;++i) system_reg_write(0x1803c+i*4,s->info[6]+i*32768);
    system_reg_write(0x1804c,3);
    t41_native_awb_emit(words,n+m); t41_native_awb_save(s);
    t41_awb_runtime=s; awb_info[0]=(uintptr_t)s->info;
    system_irq_func_set(0,3,0); tisp_event_set_cb(0,12,0);
    system_reg_write(0x304,0x20220702);
    ((uint32_t *)(void *)tpm_cb_storage)[3]=(uintptr_t)tisp_awb_pm_get_regsize;
    ((uint32_t *)(void *)tpm_cb_storage)[4]=(uintptr_t)tisp_awb_pm_suspend;
    ((uint32_t *)(void *)tpm_cb_storage)[5]=(uintptr_t)tisp_awb_pm_resume;
    t41_native_awb_frames=t41_native_awb_dropped=0;
    smp_store_release(&t41_awb_ready[0],1);
    ret=0; goto done;
free_state:
    vfree(s);
done:
    t41_native_awb_error=ret;
    mutex_unlock(&t41_native_awb_lock);
    return ret;
}
static int t41_native_awb_deinit(unsigned int channel)
{
    struct t41_awb_runtime *s;
    if (channel) return -EINVAL;
    /* The parent must stop the ISP before releasing its DMA allocation.
     * Draining software alone cannot stop the hardware DMA master. */
    if (READ_ONCE(t41_native_awb_live)) return -EBUSY;
    t41_tmo_stream_stop(); t41_native_awb_stop();
    mutex_lock(&t41_native_awb_lock);
    s=t41_native_awb_object(0);
    smp_store_release(&t41_awb_ready[0],0);
    system_irq_func_set(0,3,0); tisp_event_set_cb(0,12,0);
    awb_info[0]=0; t41_awb_runtime=(void *)1;
    ((uint32_t *)(void *)tpm_cb_storage)[3]=0;
    ((uint32_t *)(void *)tpm_cb_storage)[4]=0;
    ((uint32_t *)(void *)tpm_cb_storage)[5]=0;
    if (s) { private_kfree(s->dma); vfree(s); }
    t41_native_awb_error=-ENODEV;
    mutex_unlock(&t41_native_awb_lock);
    return 0;
}

/* A geometry/format change while statistics are in flight needs a stopped
 * stream. Same-geometry calibration and software controls commit atomically
 * under the frame mutex. Do not reinterpret an old bank with a new layout. */
static int t41_native_awb_geometry_changed(const struct t41_awb_owned *a,const struct t41_awb_owned *b)
{
    return memcmp(a->p+0xc6c,b->p+0xc6c,0x44) ||
        memcmp(a->p+0xcc8,b->p+0xcc8,6);
}
static int t41_native_awb_control(unsigned int channel,enum t41_awb_control_op op,
        void *data,unsigned int bytes,int get)
{
    struct t41_awb_runtime *s;
    struct t41_awb_register words[T41_AWB_HARDWARE_WORDS];
    unsigned int n=0;
    int ret=-ENODEV;
    if (!t41_kernel_data_ptr(data)) return -EINVAL;
    mutex_lock(&t41_native_awb_lock);
    s=t41_native_awb_object(channel);
    if (!s) goto done;
    if (get) { ret=t41_awb_control_get(&s->saved,op,data,bytes) ? -EINVAL : 0; goto done; }
    memcpy(&s->candidate,&s->saved,sizeof(s->candidate));
    ret=t41_awb_control_set(&s->candidate,op,data,bytes) ? -EINVAL : 0;
    if (ret) goto done;
    if (READ_ONCE(t41_native_awb_live) && t41_native_awb_geometry_changed(&s->saved,&s->candidate)) {
        ret=-EBUSY; goto done;
    }
    if (op==T41_AWB_PARAMS && !s->candidate.s[0xeaa1]) {
        if (t41_awb_hardware(&s->candidate,1,words,&n)) { ret=-EINVAL; goto done; }
    }
    t41_native_awb_emit(words,n); t41_native_awb_save(s);
done:
    mutex_unlock(&t41_native_awb_lock);
    return ret;
}
static int t41_native_awb_refresh(unsigned int channel,int daynight)
{
    struct t41_awb_runtime *s;
    const uint8_t *p;
    struct t41_awb_register words[T41_AWB_HARDWARE_WORDS];
    unsigned int n=0;
    int ret=-ENODEV;
    mutex_lock(&t41_native_awb_lock);
    s=t41_native_awb_object(channel);
    if (!s) goto done;
    memcpy(&s->candidate,&s->saved,sizeof(s->candidate));
    if (daynight) {
        p=(void *)(uintptr_t)((uint32_t *)(void *)tparamsP_storage)[0];
        if (!t41_kernel_data_ptr(p)) { ret=-EINVAL; goto done; }
        memcpy(s->candidate.p,p+0x978,T41_AWB_PARAM_BYTES);
        if (s->has_day_start && !READ_ONCE(*(uint32_t *)(void *)day_night_storage)) {
            unsigned int i;
            for (i=0;i<4;++i) t41_ae_put32(s->candidate.p+0xc+i*4,s->day_start[i&1]);
        }
        ret=t41_awb_daynight_owned(&s->candidate);
    } else ret=t41_awb_params_refresh_owned(&s->candidate);
    if (ret) { ret=-EINVAL; goto done; }
    if (READ_ONCE(t41_native_awb_live) && t41_native_awb_geometry_changed(&s->saved,&s->candidate)) {
        ret=-EBUSY; goto done;
    }
    if (daynight && !s->candidate.s[0xeaa1] && t41_awb_hardware(&s->candidate,1,words,&n)) {
        ret=-EINVAL; goto done;
    }
    t41_native_awb_emit(words,n); t41_native_awb_save(s);
done:
    mutex_unlock(&t41_native_awb_lock);
    return ret;
}
static int t41_native_awb_exposure_update(unsigned int gain,unsigned int ev,unsigned int mask)
{
    struct t41_awb_runtime *s;
    struct t41_awb_register words[T41_AWB_HARDWARE_WORDS];
    unsigned int n=0;
    int ret=-ENODEV;
    mutex_lock(&t41_native_awb_lock);
    s=t41_native_awb_object(0);
    if (!s) goto done;
    if (!(mask&1)) gain=t41_tmo_le32(s->saved.s+0xea9c);
    if (!(mask&2)) ev=t41_tmo_le32(s->saved.s+0xea98);
    if (t41_tmo_le32(s->saved.s+0xea98)==ev && t41_tmo_le32(s->saved.s+0xea9c)==gain) { ret=0; goto done; }
    memcpy(&s->candidate,&s->saved,sizeof(s->candidate));
    t41_ae_put32(s->candidate.s+0xea98,ev); t41_ae_put32(s->candidate.s+0xea9c,gain);
    ret=t41_awb_yweight_owned(&s->candidate);
    if (!ret && gain!=t41_tmo_le32(s->saved.s+0xea9c)) ret=t41_awb_hardware(&s->candidate,0,words,&n);
    if (ret) { ret=-EINVAL; goto done; }
    t41_native_awb_emit(words,n); t41_native_awb_save(s);
done:
    mutex_unlock(&t41_native_awb_lock);
    return ret;
}
static int t41_native_awb_exposure(unsigned int gain,unsigned int ev)
{
    return t41_native_awb_exposure_update(gain,ev,3);
}
static int t41_native_awb_suspend(unsigned int channel,uint32_t **cursor)
{
    unsigned int i,address;
    if (channel || !t41_kernel_data_ptr(cursor) || !t41_kernel_data_ptr(*cursor)) return -EINVAL;
    t41_native_awb_stop();
    mutex_lock(&t41_native_awb_lock);
    for (i=0;i<55;++i) {
        address=i<47 ? 0x18004+i*4 : i<51 ? 0x4004+(i-47)*4 : 0x5004+(i-51)*4;
        *(*cursor)++=address; *(*cursor)++=system_reg_read(address);
    }
    mutex_unlock(&t41_native_awb_lock);
    return 0;
}

/* Legacy writer entry points share the same owner as frame publication.
 * kind: 0 gains, 1 full setup, 2 regional thresholds, 3 luminance thresholds. */
static int t41_native_awb_writer(unsigned int channel,unsigned int kind)
{
    struct t41_awb_runtime *s;
    struct t41_awb_frame_buffers b;
    struct t41_awb_register words[T41_AWB_HARDWARE_WORDS];
    unsigned int n,i,m=0;
    int ret=-ENODEV;
    if (kind>3) return -EINVAL;
    mutex_lock(&t41_native_awb_lock);
    s=t41_native_awb_object(channel);
    if (!s) goto done;
    memcpy(&s->candidate,&s->saved,sizeof(s->candidate));
    b=t41_awb_buffers(&s->candidate);
    ret=kind ? t41_awb_hardware(&s->candidate,kind==1,words,&n) : t41_awb_gain_words(&b,words,&n);
    if (ret) { ret=-EINVAL; goto done; }
    if (kind>=2) {
        if (kind==2) memcpy(s->candidate.p+0xcc0,s->saved.p+0xcc0,6);
        else memcpy(s->candidate.p+0xcb0,s->saved.p+0xcb0,16);
        for (i=0;i<n;++i) {
            if (kind==2 && words[i].address==0x18038) break;
            if (kind==3 && words[i].address!=0x18038 && i!=n-1) continue;
            words[m++]=words[i];
        }
        n=m;
    }
    t41_native_awb_emit(words,n); t41_native_awb_save(s);
done:
    mutex_unlock(&t41_native_awb_lock);
    return ret;
}

/* Open's eight-byte Q10 manual/auto extension predates OEM attribute support.
 * Manual means exact hardware gains; freeze only WB publication while the
 * generic estimator keeps history. Auto relinquishes that explicit override. */
static int t41_native_awb_compat(unsigned int mode,unsigned int red,unsigned int blue,int get,
        unsigned int result[3])
{
    struct t41_awb_runtime *s;
    struct t41_awb_register words[10];
    unsigned int request[3],wb[2],bank,i,n=0;
    int ret=-ENODEV;
    if (!get && (mode>1 || red<512 || red>6144 || blue<512 || blue>6144)) return -EINVAL;
    mutex_lock(&t41_native_awb_lock);
    s=t41_native_awb_object(0);
    if (!s) goto done;
    if (get) {
        if (!result) { ret=-EINVAL; goto done; }
        result[0]=s->saved.s[0xeaa2]==0 || s->saved.s[0xeaa2]==9;
        result[1]=t41_awb_last_rgain; result[2]=t41_awb_last_bgain;
        ret=0; goto done;
    }
    memcpy(&s->candidate,&s->saved,sizeof(s->candidate));
    request[0]=mode ? 0 : 1; request[1]=(red+2)>>2; request[2]=(blue+2)>>2;
    ret=t41_awb_control_set(&s->candidate,T41_AWB_MODE,(void *)request,sizeof(request));
    if (ret) { ret=-EINVAL; goto done; }
    s->candidate.s[0xeaa1]=!mode;
    t41_ae_put32(s->candidate.s+0xeaa4,t41_tmo_le32(s->candidate.p+0x28));
    if (!mode) {
        request[0]=red; request[1]=blue; t41_awb_gain_pack(request,wb);
        for (bank=0;bank<2;++bank) {
            for (i=0;i<4;++i) {
                words[n].address=0x4004+bank*0x1000+i*4; words[n++].value=wb[i&1];
            }
            words[n].address=0x4000+bank*0x1000; words[n++].value=1;
        }
    }
    t41_native_awb_emit(words,n); t41_native_awb_save(s);
done:
    mutex_unlock(&t41_native_awb_lock);
    return ret;
}
static int t41_native_awb_start_gain(unsigned int channel,const unsigned int gains[2])
{
    struct t41_awb_runtime *s;
    struct t41_awb_frame_buffers b;
    struct t41_awb_register words[10];
    unsigned int i,n;
    int ret=-ENODEV;
    if (!t41_kernel_data_ptr(gains)) return -EINVAL;
    mutex_lock(&t41_native_awb_lock);
    s=t41_native_awb_object(channel);
    if (!s) goto done;
    memcpy(&s->candidate,&s->saved,sizeof(s->candidate));
    for (i=0;i<4;++i) t41_ae_put32(s->candidate.p+0xc+i*4,gains[i&1]);
    b=t41_awb_buffers(&s->candidate);
    ret=t41_awb_gain_words(&b,words,&n);
    if (ret) { ret=-EINVAL; goto done; }
    /* OEM also updates day-bank +0xd24..0xd30, subsequently copied to
     * active +0x984..0x990. Keep that RAM-only override here; never edit the
     * installed calibration or let another algorithm observe our scratch. */
    s->day_start[0]=gains[0]; s->day_start[1]=gains[1]; s->has_day_start=1;
    t41_native_awb_emit(words,n); t41_native_awb_save(s);
done:
    mutex_unlock(&t41_native_awb_lock);
    return ret;
}
static int t41_native_awb_observe(int target,unsigned int result[2])
{
    struct t41_awb_runtime *s;
    struct t41_awb_frame_buffers b;
    struct t41_awb_register words[10];
    unsigned int i,n,precision,fraction,mean;
    int ret=-ENODEV;
    if (!result) return -EINVAL;
    mutex_lock(&t41_native_awb_lock);
    s=t41_native_awb_object(0);
    if (!s) goto done;
    if (!t41_native_awb_frames) { ret=-EAGAIN; goto done; }
    if (target) {
        memcpy(&s->candidate,&s->saved,sizeof(s->candidate));
        s->candidate.s[0xeaa1]=s->candidate.s[0xeaa2]=0;
        b=t41_awb_buffers(&s->candidate);
        ret=t41_awb_gain_words(&b,words,&n);
        if (ret || n!=10) { ret=-ERANGE; goto done; }
        result[0]=words[0].value&0x3fff; result[1]=words[1].value&0x3fff;
    } else {
        precision=t41_tmo_le16(s->saved.p+0xcd2); fraction=t41_tmo_le16(s->saved.p+0xcd4);
        if (precision<fraction || precision>31) { ret=-ERANGE; goto done; }
        for (i=0;i<2;++i) {
            mean=t41_tmo_le16(s->saved.s+0xeaac+i*2);
            if (!mean) { ret=-ENODATA; goto done; }
            result[i]=t41_tmo_div(1ULL<<(precision-fraction+10),mean);
        }
    }
    ret=0;
done:
    mutex_unlock(&t41_native_awb_lock);
    return ret;
}
#endif
