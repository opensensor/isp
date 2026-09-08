/* SPDX-License-Identifier: MIT */
#ifdef REGTRACE_KERNEL_TREE_BUILD
#include "tx_isp_t41_lsc.h"
struct t41_lsc_frame_state {
    uint8_t params[T41_LSC_PARAM_BYTES], state[T41_LSC_STATE_BYTES];
};
struct t41_lsc_runtime {
    struct t41_lsc_frame_state saved, candidate;
    uint8_t pending_params[T41_LSC_PARAM_BYTES];
    struct t41_dpc_word words[10];
    uint32_t wdr, observed_ct, observed_gain;
    int reapply, staging;
};
static struct t41_lsc_runtime *t41_lsc_runtime = (void *)1;
static DEFINE_MUTEX(t41_lsc_lock);
static int t41_lsc_error = -ENODEV;
static unsigned int t41_lsc_updates = ~0U, t41_lsc_applied_ct = ~0U, t41_lsc_applied_gain = ~0U;
module_param(t41_lsc_error, int, 0444);
module_param(t41_lsc_updates, uint, 0444);
module_param_named(t41_lsc_ct, t41_lsc_applied_ct, uint, 0444);
module_param_named(t41_lsc_gain, t41_lsc_applied_gain, uint, 0444);

static struct t41_lsc_runtime *t41_lsc_object(unsigned int channel)
{
    return !channel && t41_kernel_data_ptr(t41_lsc_runtime) ? t41_lsc_runtime : NULL;
}
static const uint8_t *t41_lsc_calibration(void)
{
    const uint8_t *p = (void *)(uintptr_t)((uint32_t *)(void *)tparamsP_storage)[0];
    return t41_kernel_data_ptr(p) ? p + 0x4178 : NULL;
}
static void t41_native_lsc_hold(void)
{
    uint32_t *top = (uint32_t *)(void *)top_bypass_global;
    *top |= BIT(1); system_reg_write(0x40, *top);
}
static int t41_native_lsc_emit(struct t41_lsc_runtime *s, const struct t41_lsc_frame_state *f,
        unsigned int mode, int upload)
{
    unsigned int i, count = f->params[26] ? (f->state[0x6c28] ? 1152 : 576) : t41_tmo_le16(f->params + 24);
    int n;
    if (!count || count > 1152) return -EINVAL;
    n = t41_lsc_registers(f->params, T41_LSC_PARAM_BYTES, f->state, T41_LSC_STATE_BYTES,
        mode, s->words, ARRAY_SIZE(s->words));
    if (n < 0) return -EINVAL;
    for (i = 0; i < (unsigned int)n; ++i) system_reg_write(s->words[i].address, s->words[i].value);
    if (upload) {
        /* All LSC writers are serialized by the mutex. The safe ISR never
         * writes this RAM port; the final command commits the complete bank. */
        system_reg_write(0x50020, 0x101);
        for (i = 0; i < count; ++i) {
            const uint8_t *entry = f->state + 32 + i * 12;
            system_reg_write(0x50024, t41_tmo_le32(entry));
            system_reg_write(0x50024, t41_tmo_le32(entry + 4));
            system_reg_write(0x50024, entry[8]);
        }
        system_reg_write(0x50020, ((count - 1) << 16) | 0x102);
    }
    return 0;
}
static void t41_native_lsc_save(struct t41_lsc_runtime *s)
{
    memcpy(&s->saved, &s->candidate, sizeof(s->saved));
    t41_lsc_put32(s->saved.state, (uintptr_t)s->saved.params);
    t41_lsc_put32(s->saved.state + 0x3620, (uintptr_t)(s->saved.params + (s->wdr ? 49 : 27)));
    t41_lsc_put32(s->saved.state + 0x3624, (uintptr_t)(s->saved.params + (s->wdr ? 60 : 38)));
    s->saved.state[0x6c2b] = 0;
    s->observed_ct = s->observed_gain = ~0U;
    t41_lsc_applied_ct = t41_tmo_le32(s->saved.state + 12);
    t41_lsc_applied_gain = t41_tmo_le32(s->saved.state + 24);
    ++t41_lsc_updates;
}
static int t41_native_lsc_init(unsigned int channel, const uint32_t *config)
{
    struct t41_lsc_runtime *s;
    const uint8_t *p;
    int ret = -EINVAL;
    if (channel || !t41_kernel_data_ptr(config)) return -EINVAL;
    mutex_lock(&t41_lsc_lock);
    if (t41_lsc_object(0)) { ret = -EBUSY; goto done; }
    p = t41_lsc_calibration();
    if (!p) goto done;
    s = vzalloc(sizeof(*s));
    if (!s) { ret = -ENOMEM; goto done; }
    s->wdr = config[30] == 1;
    memcpy(s->candidate.params, p, T41_LSC_PARAM_BYTES);
    ret = t41_lsc_initialize(s->candidate.params, T41_LSC_PARAM_BYTES, s->candidate.state,
        T41_LSC_STATE_BYTES, config[0], config[1], config[2], s->wdr);
    if (ret) { ret = -EINVAL; goto free_state; }
    t41_native_lsc_hold();
    ret = t41_native_lsc_emit(s, &s->candidate, 0, 1);
    if (ret) goto free_state;
    ret = t41_top_restore_bypass(BIT(1));
    if (ret) goto free_state;
    t41_lsc_updates = 0;
    t41_native_lsc_save(s);
    t41_lsc_runtime = s;
    lsc_info[0] = (uintptr_t)s->saved.state;
    system_irq_func_set(0, 36, 0);
    system_reg_write(0x314, 0x20230531);
    ((uint32_t *)(void *)tpm_cb_storage)[12] = (uintptr_t)tisp_lsc_pm_get_regsize;
    ((uint32_t *)(void *)tpm_cb_storage)[13] = (uintptr_t)tisp_lsc_pm_suspend;
    ((uint32_t *)(void *)tpm_cb_storage)[14] = (uintptr_t)tisp_lsc_pm_resume;
    goto done;
free_state:
    vfree(s);
done:
    t41_lsc_error = ret;
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static int t41_native_lsc_deinit(unsigned int channel)
{
    struct t41_lsc_runtime *s;
    uint32_t *callbacks = (uint32_t *)(void *)tpm_cb_storage;
    if (channel) return -EINVAL;
    t41_tmo_stream_stop();
    mutex_lock(&t41_lsc_lock);
    s = t41_lsc_object(0);
    if (s) t41_native_lsc_hold();
    system_irq_func_set(0, 36, 0);
    lsc_info[0] = 0; t41_lsc_runtime = (void *)1;
    callbacks[12] = callbacks[13] = callbacks[14] = 0;
    vfree(s);
    t41_lsc_error = -ENODEV;
    mutex_unlock(&t41_lsc_lock);
    return 0;
}
static int t41_native_lsc_frame(unsigned int ct, unsigned int gain)
{
    struct t41_lsc_runtime *s;
    int c, g, ret = -ENODEV;
    mutex_lock(&t41_lsc_lock);
    s = t41_lsc_object(0);
    if (!s) goto done;
    ret = 0;
    if (!s->reapply && ct == s->observed_ct && gain == s->observed_gain) goto done;
    memcpy(&s->candidate, &s->saved, sizeof(s->candidate));
    c = t41_lsc_ct(s->candidate.params, T41_LSC_PARAM_BYTES, s->candidate.state,
        T41_LSC_STATE_BYTES, ct, s->reapply);
    g = t41_lsc_gain(s->candidate.params, T41_LSC_PARAM_BYTES, s->candidate.state,
        T41_LSC_STATE_BYTES, gain, s->reapply, s->wdr);
    if (c < 0 || g < 0) { ret = -EINVAL; goto done; }
    if (c && g && !s->reapply) goto observed;
    ret = t41_native_lsc_emit(s, &s->candidate, s->reapply || (!c && !g) ? 0 : !c ? 2 : 1, !c || s->reapply);
    if (!ret) {
        t41_native_lsc_save(s);
        if (s->reapply) ret = t41_top_restore_bypass(BIT(1));
        s->reapply = 0;
    }
observed:
    if (!ret) { s->observed_ct = ct; s->observed_gain = gain; }
done:
    t41_lsc_error = ret;
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static void t41_native_lsc_reapply(void)
{
    mutex_lock(&t41_lsc_lock);
    if (t41_lsc_object(0)) t41_lsc_runtime->reapply = 1;
    mutex_unlock(&t41_lsc_lock);
}
static int t41_native_lsc_prepare(unsigned int channel)
{
    int ret = -ENODEV;
    mutex_lock(&t41_lsc_lock);
    if (t41_lsc_object(channel)) { t41_lsc_runtime->reapply = 1; ret = 0; }
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static int t41_native_lsc_refresh(unsigned int channel, int wdr)
{
    struct t41_lsc_runtime *s;
    const uint8_t *p;
    unsigned int mode;
    int ret = -ENODEV;
    mutex_lock(&t41_lsc_lock);
    s = t41_lsc_object(channel); p = t41_lsc_calibration();
    if (!s || !p) goto done;
    mode = wdr < 0 ? s->wdr : wdr == 1;
    memcpy(&s->candidate, &s->saved, sizeof(s->candidate));
    if (wdr < 0) {
        memcpy(s->candidate.params, p, T41_LSC_PARAM_BYTES);
        ret = t41_lsc_refresh(s->candidate.params, T41_LSC_PARAM_BYTES, s->candidate.state,
            T41_LSC_STATE_BYTES, mode, ((uint32_t *)(void *)top_bypass_global)[0] & BIT(1));
    } else {
        ret = t41_lsc_gain(s->candidate.params, T41_LSC_PARAM_BYTES, s->candidate.state,
            T41_LSC_STATE_BYTES, t41_lsc_applied_gain, 1, mode);
        if (!ret) ret = t41_lsc_ct(s->candidate.params, T41_LSC_PARAM_BYTES, s->candidate.state,
            T41_LSC_STATE_BYTES, t41_lsc_applied_ct, 1);
    }
    if (ret) { ret = -EINVAL; goto done; }
    ret = t41_native_lsc_emit(s, &s->candidate, 0, 1);
    if (!ret) { s->wdr = mode; s->staging = 0; t41_native_lsc_save(s); }
done:
    t41_lsc_error = ret;
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static int t41_native_lsc_flip(unsigned int width, unsigned int height, unsigned int v, unsigned int h)
{
    struct t41_lsc_runtime *s;
    int ret = -ENODEV;
    mutex_lock(&t41_lsc_lock);
    s = t41_lsc_object(0);
    if (!s) goto done;
    memcpy(&s->candidate, &s->saved, sizeof(s->candidate));
    ret = t41_lsc_flip(s->candidate.params, T41_LSC_PARAM_BYTES, s->candidate.state,
        T41_LSC_STATE_BYTES, width, height, v, h, ((uint32_t *)(void *)top_bypass_global)[0] & BIT(1));
    if (ret) { ret = -EINVAL; goto done; }
    ret = t41_native_lsc_emit(s, &s->candidate, 0, 1);
    if (!ret) { s->staging = 0; t41_native_lsc_save(s); }
done:
    t41_lsc_error = ret;
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static int t41_native_lsc_writer(unsigned int channel, unsigned int mode, int upload)
{
    struct t41_lsc_runtime *s;
    int ret = -ENODEV;
    mutex_lock(&t41_lsc_lock);
    s = t41_lsc_object(channel);
    if (s) ret = t41_native_lsc_emit(s, &s->saved, mode, upload);
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static int t41_native_lsc_interpolate(unsigned int channel, unsigned int value, unsigned int force, int gain)
{
    struct t41_lsc_runtime *s;
    int ret = -ENODEV;
    mutex_lock(&t41_lsc_lock);
    s = t41_lsc_object(channel);
    if (!s) goto done;
    memcpy(&s->candidate, &s->saved, sizeof(s->candidate));
    ret = gain ? t41_lsc_gain(s->candidate.params, T41_LSC_PARAM_BYTES, s->candidate.state,
        T41_LSC_STATE_BYTES, value, force, s->wdr) :
        t41_lsc_ct(s->candidate.params, T41_LSC_PARAM_BYTES, s->candidate.state,
        T41_LSC_STATE_BYTES, value, force);
    if (!ret) { t41_native_lsc_save(s); s->reapply = 1; }
done:
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static int t41_native_lsc_get(unsigned int channel, void *out, uint32_t *bytes)
{
    struct t41_lsc_runtime *s;
    int ret = -ENODEV;
    if (!t41_kernel_data_ptr(out) || !t41_kernel_data_ptr(bytes)) return -EINVAL;
    mutex_lock(&t41_lsc_lock);
    s = t41_lsc_object(channel);
    if (s) { memcpy(out, s->saved.params, 0x68); *bytes = 0x68; ret = 0; }
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static int t41_native_lsc_set(unsigned int channel, unsigned int part, const void *source)
{
    static const unsigned int offsets[5] = {0,0x68,0x3680,0x6c80,0xa280};
    static const unsigned int sizes[5] = {0x68,0x3618,0x3600,0x3600,0x3600};
    struct t41_lsc_runtime *s;
    int ret = -ENODEV;
    if (part >= 5) return -EINVAL;
    mutex_lock(&t41_lsc_lock);
    s = t41_lsc_object(channel);
    if (!s) goto done;
    memcpy(&s->candidate, &s->saved, sizeof(s->candidate));
    if (!s->staging) memcpy(s->pending_params, s->saved.params, T41_LSC_PARAM_BYTES);
    ret = probe_kernel_read(s->pending_params + offsets[part], source, sizes[part]);
    if (ret) { s->staging = 0; goto done; }
    if (part && part < 4) { s->staging = 1; goto done; }
    if (part == 4) memcpy(s->candidate.params, s->pending_params, T41_LSC_PARAM_BYTES);
    ret = t41_lsc_replace(s->candidate.params, T41_LSC_PARAM_BYTES, s->candidate.state,
        T41_LSC_STATE_BYTES, part, s->pending_params + offsets[part], sizes[part], s->wdr,
        ((uint32_t *)(void *)top_bypass_global)[0] & BIT(1));
    if (ret) { ret = -EINVAL; goto done; }
    ret = t41_native_lsc_emit(s, &s->candidate, 0, 1);
    if (!ret) { if (part == 4) s->staging = 0; t41_native_lsc_save(s); }
done:
    t41_lsc_error = ret;
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static int t41_native_lsc_geometry(unsigned int channel, unsigned int height, unsigned int width,
        unsigned int sy, unsigned int sx, unsigned int pairs)
{
    struct t41_lsc_runtime *s;
    int ret = -ENODEV;
    mutex_lock(&t41_lsc_lock);
    s = t41_lsc_object(channel);
    if (s) ret = t41_lsc_geometry(s->saved.state, T41_LSC_STATE_BYTES, height, width, sy, sx, pairs);
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static int t41_native_lsc_diagnostic(unsigned int channel, uint8_t *out)
{
    struct t41_lsc_runtime *s;
    int ret = -ENODEV;
    if (!t41_kernel_data_ptr(out)) return -EINVAL;
    mutex_lock(&t41_lsc_lock);
    s = t41_lsc_object(channel);
    if (s) { *out = s->saved.state[0x6c35]; ret = 0; }
    mutex_unlock(&t41_lsc_lock);
    return ret;
}
static int t41_native_lsc_suspend(unsigned int channel, uint32_t **cursor)
{
    unsigned int address;
    uint32_t *p;
    if (channel || !t41_kernel_data_ptr(cursor) || !t41_kernel_data_ptr(*cursor)) return -EINVAL;
    mutex_lock(&t41_lsc_lock);
    p = *cursor;
    for (address = 0x3000; address < 0x3100; address += 4) {
        *p++ = address; *p++ = system_reg_read(address);
    }
    *cursor = p;
    mutex_unlock(&t41_lsc_lock);
    return 0;
}
#endif
