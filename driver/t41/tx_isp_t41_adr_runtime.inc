/* SPDX-License-Identifier: MIT */
/* Included after the recovered globals/prototypes. No recovered ADR BSS
 * aliases or captured sensor maps participate in this implementation. */
#ifdef REGTRACE_KERNEL_TREE_BUILD
struct t41_adr_frame_state {
    struct t41_adr_context context;
    uint8_t params[T41_ADR_PARAM_BYTES];
    uint8_t output[T41_ADR_STATE_BYTES];
    uint8_t history[T41_ADR_WORK_BYTES];
};
struct t41_adr_runtime {
    struct t41_adr_frame_state saved, candidate;
    uint8_t statistics[T41_ADR_STATS_BYTES] __aligned(4);
    uint8_t pending_dma[0xaa0], working_dma[0xaa0];
    struct t41_dpc_word words[199];
    unsigned int geometry_scratch[8][32];
    uint8_t *dma;
    uint32_t physical, width, height;
    uint64_t exposure;
    int ready, armed, ev_changed, restore_top;
};
static struct t41_adr_runtime *t41_adr_runtime = (void *)1;
static DEFINE_MUTEX(t41_adr_lock);
static spinlock_t t41_adr_irq_lock __section(".data") =
    __SPIN_LOCK_UNLOCKED(t41_adr_irq_lock);
static int t41_adr_error = -ENODEV;
static unsigned int t41_adr_frames = ~0U, t41_adr_dropped = ~0U;
static unsigned int t41_adr_irqs = ~0U, t41_adr_bad_dma = ~0U;
static unsigned int t41_adr_stage = ~0U, t41_adr_tile = ~0U;
module_param(t41_adr_error, int, 0444);
module_param(t41_adr_frames, uint, 0444);
module_param(t41_adr_dropped, uint, 0444);
module_param(t41_adr_irqs, uint, 0444);
module_param(t41_adr_bad_dma, uint, 0444);
module_param(t41_adr_stage, uint, 0444);
module_param(t41_adr_tile, uint, 0444);

static struct t41_adr_runtime *t41_adr_object(void)
{
    return t41_kernel_data_ptr(t41_adr_runtime) ? t41_adr_runtime : NULL;
}

static void t41_adr_hold_locked(struct t41_adr_runtime *s)
{
    ((uint32_t *)(void *)top_bypass_global)[0] |= BIT(7);
    system_reg_write(0x40, ((uint32_t *)(void *)top_bypass_global)[0]);
    s->restore_top = 1;
}

static int t41_adr_emit_locked(struct t41_adr_runtime *s, int count)
{
    int i;
    if (count < 0) return -EINVAL;
    for (i = 0; i < count; ++i)
        system_reg_write(s->words[i].address, s->words[i].value);
    return 0;
}

static int t41_adr_configuration_locked(struct t41_adr_runtime *s, int full)
{
    const uint8_t *p = s->saved.params;
    int ret = 0, count;
    if (full) {
        ret = t41_adr_emit_locked(s, t41_adr_pack_geometry(p, T41_ADR_PARAM_BYTES, 0, s->words, ARRAY_SIZE(s->words)));
        if (!ret) ret = t41_adr_emit_locked(s, t41_adr_pack_spatial(p, T41_ADR_PARAM_BYTES, 0, s->words, ARRAY_SIZE(s->words)));
    }
    if (!ret) ret = t41_adr_emit_locked(s, t41_adr_pack_controls(p, T41_ADR_PARAM_BYTES, 0, s->words, ARRAY_SIZE(s->words)));
    if (ret) return ret;
    count = t41_adr_pack_curve(p, T41_ADR_PARAM_BYTES, s->saved.output,
        T41_ADR_STATE_BYTES, system_reg_read(0x5040c), s->words, ARRAY_SIZE(s->words));
    return count ? t41_adr_emit_locked(s, count) : -EBUSY;
}

/* Used at both initialization and the late stream-reset boundary. */
static void t41_apply_stock_adr_profile(void)
{
    struct t41_adr_runtime *s;
    unsigned long flags;
    unsigned int i;
    int ret;
    mutex_lock(&t41_adr_lock);
    s = t41_adr_object();
    if (!s) goto done;
    t41_adr_hold_locked(s);
    spin_lock_irqsave(&t41_adr_irq_lock, flags);
    s->armed = 0; s->ready = 0;
    system_reg_write(0x9510, 0);
    spin_unlock_irqrestore(&t41_adr_irq_lock, flags);
    if (t41_stock_adr_profile > 0) goto done;
    ret = t41_adr_configuration_locked(s, 1);
    if (ret && ret != -EBUSY) { t41_adr_error = ret; goto done; }
    dma_cache_sync(NULL, s->dma, 0x4000, DMA_FROM_DEVICE);
    spin_lock_irqsave(&t41_adr_irq_lock, flags);
    for (i = 0; i < 4; ++i) system_reg_write(0x9514 + i * 4, s->physical + i * 0x1000);
    s->armed = 1;
    wmb();
    system_reg_write(0x9510, 3);
    spin_unlock_irqrestore(&t41_adr_irq_lock, flags);
    /* Unlike TMO, bypassing ADR also stops its statistics DMA. Bootstrap
     * from the oracle-verified cold/last-good map, only after all addresses
     * and controls are valid, then accept the first histogram frame. */
    if (!ret) {
        ret = t41_top_restore_bypass(BIT(7));
        if (!ret) s->restore_top = 0;
    }
    t41_adr_error = ret;
done:
    mutex_unlock(&t41_adr_lock);
}

/* Only a completed, owned DMA page is copied in IRQ context. Expensive
 * histogram/curve arithmetic runs in the existing drained tone worker. */
static int t41_native_adr_interrupt(void)
{
    struct t41_adr_runtime *s;
    uint32_t address, offset;
    unsigned long flags;
    spin_lock_irqsave(&t41_adr_irq_lock, flags);
    s = t41_adr_object();
    if (!s || !s->armed) goto done;
    address = system_reg_read(0x9540);
    offset = address - s->physical;
    if (offset >= 0x4000 || (offset & 0xfff)) { ++t41_adr_bad_dma; goto done; }
    dma_cache_sync(NULL, s->dma + offset, 0x1000, DMA_FROM_DEVICE);
    memcpy(s->pending_dma, s->dma + offset, sizeof(s->pending_dma));
    s->ready = 1;
    ++t41_adr_irqs;
done:
    spin_unlock_irqrestore(&t41_adr_irq_lock, flags);
    return 1;
}

static int t41_native_adr_frame(uint64_t exposure)
{
    struct t41_adr_runtime *s;
    struct t41_adr_frame_state *c;
    unsigned long flags;
    int ret = 0, count;
    mutex_lock(&t41_adr_lock);
    s = t41_adr_object();
    if (!s || t41_stock_adr_profile > 0) goto done;
    if (s->restore_top && s->armed) {
        count = t41_adr_pack_curve(s->saved.params, T41_ADR_PARAM_BYTES, s->saved.output,
            T41_ADR_STATE_BYTES, system_reg_read(0x5040c), s->words, ARRAY_SIZE(s->words));
        if (count > 0 && !t41_adr_emit_locked(s, count) && !t41_top_restore_bypass(BIT(7)))
            s->restore_top = 0;
    }
    spin_lock_irqsave(&t41_adr_irq_lock, flags);
    if (!s->ready || !s->armed) { spin_unlock_irqrestore(&t41_adr_irq_lock, flags); goto done; }
    memcpy(s->working_dma, s->pending_dma, sizeof(s->working_dma)); s->ready = 0;
    spin_unlock_irqrestore(&t41_adr_irq_lock, flags);
    c = &s->candidate;
    memcpy(c, &s->saved, sizeof(*c));
    if (t41_adr_unpack(s->working_dma, sizeof(s->working_dma), s->statistics, sizeof(s->statistics)) ||
        t41_adr_statistics(s->statistics, sizeof(s->statistics), s->width, s->height) ||
        t41_adr_ev(c->params, sizeof(c->params), c->history, sizeof(c->history), exposure,
            s->ev_changed || exposure != s->exposure) < 0 ||
        t41_adr_gaussian(c->params, sizeof(c->params), c->context.gaussian_previous, c->context.gaussian) < 0) {
        ret = -EINVAL; goto failed;
    }
    ret = t41_adr_frame(&c->context, c->params, sizeof(c->params), c->output, sizeof(c->output),
        c->history, sizeof(c->history), s->statistics, sizeof(s->statistics), ai_curve1_y);
    if (ret) { ret = -ERANGE; goto failed; }
    count = t41_adr_pack_curve(c->params, sizeof(c->params), c->output, sizeof(c->output),
        system_reg_read(0x5040c), s->words, ARRAY_SIZE(s->words));
    if (count <= 0) { ret = count ? -EINVAL : -EBUSY; goto failed; }
    ret = t41_adr_emit_locked(s, count);
    if (ret) goto failed;
    /* A busy/invalid candidate never advances the temporal history. */
    memcpy(&s->saved, c, sizeof(*c));
    s->exposure = exposure; s->ev_changed = 0;
    if (s->restore_top) {
        ret = t41_top_restore_bypass(BIT(7));
        if (!ret) s->restore_top = 0;
    }
    ++t41_adr_frames;
    t41_adr_stage = 0; t41_adr_tile = 0; t41_adr_error = ret;
    goto done;
failed:
    t41_adr_stage = c->context.stage; t41_adr_tile = c->context.tile;
    ++t41_adr_dropped; t41_adr_error = ret;
done:
    mutex_unlock(&t41_adr_lock);
    return ret;
}

static int t41_native_adr_init(unsigned int channel, uintptr_t config)
{
    struct t41_adr_runtime *s;
    uint8_t *params;
    unsigned long physical, flags;
    int ret = -EINVAL;
    if (channel || !config) return -EINVAL;
    mutex_lock(&t41_adr_lock);
    if (t41_adr_object()) { ret = -EBUSY; goto done; }
    params = (uint8_t *)(uintptr_t)((uint32_t *)(void *)tparamsP_storage)[0];
    if (!t41_kernel_data_ptr(params)) goto done;
    s = vzalloc(sizeof(*s));
    if (!s) { ret = -ENOMEM; goto done; }
    s->dma = kzalloc(0x4000, GFP_KERNEL | GFP_DMA);
    if (!s->dma) { ret = -ENOMEM; goto free_state; }
    physical = virt_to_phys(s->dma);
    if ((physical & 0xfff) || physical > 0x1fffc000UL) goto free_dma;
    s->physical = physical;
    s->width = *(uint32_t *)config; s->height = *(uint32_t *)(config + 4);
    memcpy(s->saved.params, params + 0x11d28, T41_ADR_PARAM_BYTES);
    ret = t41_adr_initialize(&s->saved.context, s->saved.params, T41_ADR_PARAM_BYTES,
        s->saved.output, s->saved.history, s->width, s->height, params + 0x139c0, s->geometry_scratch);
    if (ret) { ret = -EINVAL; goto free_dma; }
    s->ev_changed = 1;
    t41_adr_hold_locked(s);
    spin_lock_irqsave(&t41_adr_irq_lock, flags);
    t41_adr_runtime = s;
    spin_unlock_irqrestore(&t41_adr_irq_lock, flags);
    t41_adr_frames = 0; t41_adr_dropped = 0; t41_adr_irqs = 0; t41_adr_bad_dma = 0;
    t41_adr_stage = 0; t41_adr_tile = 0; t41_adr_error = 0;
    system_irq_func_set(0, 9, (uintptr_t)tiziano_adr_interrupt_static);
    system_reg_write(0x320, 0x20220727);
    goto done;
free_dma:
    kfree(s->dma);
free_state:
    vfree(s);
done:
    mutex_unlock(&t41_adr_lock);
    return ret;
}

static int t41_native_adr_deinit(unsigned int channel)
{
    struct t41_adr_runtime *s;
    unsigned long flags;
    uint32_t *callbacks = (uint32_t *)(void *)tpm_cb_storage;
    if (channel) return -EINVAL;
    /* No ADR work may survive the allocation it references. */
    t41_tmo_stream_stop();
    mutex_lock(&t41_adr_lock);
    s = t41_adr_object();
    system_irq_func_set(0, 9, 0);
    if (s) t41_adr_hold_locked(s);
    spin_lock_irqsave(&t41_adr_irq_lock, flags);
    if (s) { s->armed = 0; s->ready = 0; }
    system_reg_write(0x9510, 0);
    (void)system_reg_read(0x9510);
    t41_adr_runtime = (void *)1;
    spin_unlock_irqrestore(&t41_adr_irq_lock, flags);
    if (s) { kfree(s->dma); vfree(s); }
    callbacks[84 / 4] = callbacks[88 / 4] = callbacks[92 / 4] = 0;
    t41_adr_error = -ENODEV;
    mutex_unlock(&t41_adr_lock);
    return 0;
}

static int t41_native_adr_refresh(unsigned int channel)
{
    struct t41_adr_runtime *s;
    uint8_t *params;
    int ret = -ENODEV;
    if (channel) return -EINVAL;
    mutex_lock(&t41_adr_lock);
    s = t41_adr_object();
    params = (uint8_t *)(uintptr_t)((uint32_t *)(void *)tparamsP_storage)[0];
    if (!s || !t41_kernel_data_ptr(params)) goto done;
    memcpy(&s->candidate, &s->saved, sizeof(s->candidate));
    memcpy(s->candidate.params, params + 0x11d28, T41_ADR_PARAM_BYTES);
    ret = t41_adr_refresh(&s->candidate.context, s->candidate.params, T41_ADR_PARAM_BYTES,
        s->candidate.output, s->candidate.history, s->width, s->height, params + 0x139c0);
    if (ret) { ret = -EINVAL; goto done; }
    if (t41_adr_gaussian(s->candidate.params, T41_ADR_PARAM_BYTES, s->candidate.context.gaussian_previous,
        s->candidate.context.gaussian) < 0) { ret = -EINVAL; goto done; }
    memcpy(&s->saved, &s->candidate, sizeof(s->saved));
    s->ev_changed = 1;
    ret = t41_adr_configuration_locked(s, 1);
done:
    t41_adr_error = ret;
    mutex_unlock(&t41_adr_lock);
    return ret;
}

static int t41_native_adr_gamma(unsigned int channel, const void *source)
{
    struct t41_adr_runtime *s;
    const uint8_t *params;
    int ret = -ENODEV;
    if (channel) return -EINVAL;
    mutex_lock(&t41_adr_lock);
    s = t41_adr_object();
    if (!s) goto done;
    if (!source) {
        params = (const uint8_t *)(uintptr_t)((uint32_t *)(void *)tparamsP_storage)[0];
        if (!t41_kernel_data_ptr(params)) goto done;
        source = params + 0x139c0;
    }
    ret = probe_kernel_read(s->candidate.params + 0x766, source, 258);
    if (!ret) memcpy(s->saved.params + 0x766, s->candidate.params + 0x766, 258);
done:
    mutex_unlock(&t41_adr_lock);
    return ret;
}

static int t41_native_adr_writer(unsigned int channel, unsigned int which)
{
    struct t41_adr_runtime *s;
    int ret = -ENODEV, count;
    if (channel) return -EINVAL;
    mutex_lock(&t41_adr_lock);
    s = t41_adr_object();
    if (!s) goto done;
    if (which == 0) count = t41_adr_pack_geometry(s->saved.params, T41_ADR_PARAM_BYTES, 0, s->words, ARRAY_SIZE(s->words));
    else if (which == 1) count = t41_adr_pack_spatial(s->saved.params, T41_ADR_PARAM_BYTES, 0, s->words, ARRAY_SIZE(s->words));
    else if (which == 2) count = t41_adr_pack_controls(s->saved.params, T41_ADR_PARAM_BYTES, 0, s->words, ARRAY_SIZE(s->words));
    else count = t41_adr_pack_curve(s->saved.params, T41_ADR_PARAM_BYTES, s->saved.output,
        T41_ADR_STATE_BYTES, system_reg_read(0x5040c), s->words, ARRAY_SIZE(s->words));
    ret = count ? t41_adr_emit_locked(s, count) : -EBUSY;
done:
    mutex_unlock(&t41_adr_lock);
    return ret;
}

static int t41_native_adr_set(unsigned int channel, const void *source)
{
    struct t41_adr_runtime *s;
    int ret = -ENODEV;
    if (channel) return -EINVAL;
    mutex_lock(&t41_adr_lock);
    s = t41_adr_object();
    if (!s) goto done;
    memcpy(&s->candidate, &s->saved, sizeof(s->candidate));
    if (source && probe_kernel_read(s->candidate.params, source, T41_ADR_PARAM_BYTES)) { ret = -EFAULT; goto done; }
    if (t41_adr_hardware_parameters(s->candidate.params, T41_ADR_PARAM_BYTES) ||
        t41_adr_gaussian(s->candidate.params, T41_ADR_PARAM_BYTES, s->candidate.context.gaussian_previous,
            s->candidate.context.gaussian) < 0) { ret = -EINVAL; goto done; }
    ret = t41_adr_emit_locked(s, t41_adr_pack_controls(s->candidate.params, T41_ADR_PARAM_BYTES,
        0, s->words, ARRAY_SIZE(s->words)));
    if (!ret) {
        s->candidate.context.ctc_changed = 1;
        memcpy(&s->saved, &s->candidate, sizeof(s->saved)); s->ev_changed = 1;
    }
done:
    t41_adr_error = ret;
    mutex_unlock(&t41_adr_lock);
    return ret;
}

static int t41_native_adr_get(unsigned int channel, void *output, uint32_t *bytes)
{
    struct t41_adr_runtime *s;
    int ret = -ENODEV;
    if (channel || !t41_kernel_data_ptr(output) || !t41_kernel_data_ptr(bytes)) return -EINVAL;
    mutex_lock(&t41_adr_lock);
    s = t41_adr_object();
    if (s) {
        memcpy(output, s->saved.params, T41_ADR_PARAM_BYTES);
        memcpy((uint8_t *)output + T41_ADR_PARAM_BYTES, s->saved.output, T41_ADR_STATE_BYTES);
        *bytes = T41_ADR_PARAM_BYTES + T41_ADR_STATE_BYTES; ret = 0;
    }
    mutex_unlock(&t41_adr_lock);
    return ret;
}

static int t41_native_adr_ev(unsigned int channel, uint64_t exposure)
{
    struct t41_adr_runtime *s;
    int ret = -ENODEV;
    if (channel) return -EINVAL;
    mutex_lock(&t41_adr_lock);
    s = t41_adr_object();
    if (s) {
        memcpy(&s->candidate, &s->saved, sizeof(s->candidate));
        ret = t41_adr_ev(s->candidate.params, T41_ADR_PARAM_BYTES, s->candidate.history,
            T41_ADR_WORK_BYTES, exposure, 1);
        if (ret >= 0) { memcpy(&s->saved, &s->candidate, sizeof(s->saved)); s->exposure = exposure; ret = 0; }
    }
    mutex_unlock(&t41_adr_lock);
    return ret;
}

static int t41_native_adr_face(unsigned int channel, unsigned int enabled, const void *flags, const void *curve)
{
    struct t41_adr_runtime *s;
    int ret = -ENODEV;
    if (channel || enabled > 1) return -EINVAL;
    mutex_lock(&t41_adr_lock);
    s = t41_adr_object();
    if (!s) goto done;
    if (probe_kernel_read(s->candidate.context.face_flags, flags, 24) ||
        probe_kernel_read(s->candidate.context.face_curve, curve, 672)) { ret = -EFAULT; goto done; }
    memcpy(s->saved.context.face_flags, s->candidate.context.face_flags, 24);
    memcpy(s->saved.context.face_curve, s->candidate.context.face_curve, 672);
    s->saved.context.face_enabled = enabled; ret = 0;
done:
    mutex_unlock(&t41_adr_lock);
    return ret;
}
#else
static void t41_apply_stock_adr_profile(void) { }
#endif
