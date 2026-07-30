/*
 * Bounded T23 AE path reconstructed from verified SC2336 exposure events.
 * IRQ context only captures luma; sensor I2C and gain-dependent ISP updates
 * run on the system workqueue.
 */

struct regtrace_t23_ae_hlil_rung {
    uint32_t packed;
    uint32_t gain_q16;
    uint32_t ev_q10;
};

static const struct regtrace_t23_ae_hlil_rung regtrace_t23_ae_hlil_rungs[] = {
    { 0x00800020U, 0x10000U,   32U << 10 },
    { 0x00800030U, 0x10000U,   48U << 10 },
    { 0x00800040U, 0x10000U,   64U << 10 },
    { 0x00800060U, 0x10000U,   96U << 10 },
    { 0x00800080U, 0x10000U,  128U << 10 },
    { 0x008000a0U, 0x10000U,  160U << 10 },
    { 0x008000c0U, 0x10000U,  192U << 10 },
    { 0x008000e0U, 0x10000U,  224U << 10 },
    { 0x00800100U, 0x10000U,  256U << 10 },
    { 0x00800140U, 0x10000U,  320U << 10 },
    { 0x00800190U, 0x10000U,  400U << 10 },
    { 0x008001f4U, 0x10000U,  500U << 10 },
    { 0x00800258U, 0x10000U,  600U << 10 },
    { 0x008002eeU, 0x10000U,  750U << 10 },
    { 0x00800384U, 0x10000U,  900U << 10 },
    { 0x0080041aU, 0x10000U, 1050U << 10 },
    /* SC2336 init registers 0x3e01/0x3e02 encode 0x4ab lines. */
    { 0x008004abU, 0x10000U, 1195U << 10 },
    { 0x00800514U, 0x10000U, 1300U << 10 },
    { 0x0080059cU, 0x10000U, 1436U << 10 },
    { 0x0090059cU, 0x12b80U, 1616U << 10 },
    { 0x00a0059cU, 0x15269U, 1795U << 10 },
    { 0x00b0059cU, 0x1759dU, 1975U << 10 },
    { 0x00c0059cU, 0x195c0U, 2154U << 10 },
    { 0x00d0059cU, 0x1b350U, 2334U << 10 },
    { 0x00e0059cU, 0x1ceaeU, 2513U << 10 },
    { 0x00f0059cU, 0x1e829U, 2693U << 10 },
    { 0x0880059cU, 0x20000U, 2872U << 10 },
};

#define REGTRACE_T23_AE_HLIL_STATE_COUNT \
    ARRAY_SIZE(regtrace_t23_ae_hlil_rungs)

static uint32_t regtrace_t23_ae_hlil_nearest_state(uint32_t ev)
{
    uint32_t best = 0;
    uint32_t best_distance = 0xffffffffU;
    uint32_t state;

    for (state = 0; state < REGTRACE_T23_AE_HLIL_STATE_COUNT; ++state) {
        uint32_t rung_ev = regtrace_t23_ae_hlil_rungs[state].ev_q10 >> 10;
        uint32_t distance = rung_ev > ev ? rung_ev - ev : ev - rung_ev;

        if (distance < best_distance) {
            best = state;
            best_distance = distance;
        }
    }
    return best;
}

static uint32_t regtrace_t23_ae_hlil_pending_luma;
static uint32_t regtrace_t23_ae_hlil_pending_snapshot;
static bool regtrace_t23_ae_hlil_pending;
static DEFINE_SPINLOCK(regtrace_t23_ae_hlil_lock);

static void regtrace_t23_source_ae_hlil_work(struct work_struct *work);
static DECLARE_WORK(regtrace_t23_source_ae_hlil_work_item,
                    regtrace_t23_source_ae_hlil_work);

static void regtrace_t23_source_ae_hlil_work(struct work_struct *work)
{
    const struct regtrace_t23_ae_hlil_rung *rung;
    unsigned long flags;
    uint32_t luma;
    uint32_t snapshot;
    uint32_t state;
    uint32_t next_state;
    uint32_t lower;
    uint32_t upper;
    uint32_t desired_ev;
    uint32_t desired_state;
    int ret = 0;

    (void)work;
    spin_lock_irqsave(&regtrace_t23_ae_hlil_lock, flags);
    luma = regtrace_t23_ae_hlil_pending_luma;
    snapshot = regtrace_t23_ae_hlil_pending_snapshot;
    state = min(regtrace_t23_source_ae_hlil_state,
                REGTRACE_T23_AE_HLIL_STATE_COUNT - 1U);
    spin_unlock_irqrestore(&regtrace_t23_ae_hlil_lock, flags);

    regtrace_t23_source_ae_hlil_runs++;
    regtrace_t23_source_ae_hlil_luma = luma;
    next_state = state;
    lower = regtrace_t23_source_ae_hlil_target >
            regtrace_t23_source_ae_hlil_deadband ?
            regtrace_t23_source_ae_hlil_target -
            regtrace_t23_source_ae_hlil_deadband : 0U;
    upper = regtrace_t23_source_ae_hlil_target +
            regtrace_t23_source_ae_hlil_deadband;

    if (!regtrace_t23_sensor_streaming || !regtrace_t23_core_started) {
        ret = -EAGAIN;
    } else if (luma && (luma < lower || luma > upper)) {
        uint32_t current_ev =
            regtrace_t23_ae_hlil_rungs[state].ev_q10 >> 10;

        desired_ev = (uint32_t)div64_u64(
            (uint64_t)current_ev * regtrace_t23_source_ae_hlil_target +
            luma / 2U, luma);
        desired_ev = clamp(desired_ev,
            regtrace_t23_ae_hlil_rungs[0].ev_q10 >> 10,
            regtrace_t23_ae_hlil_rungs[
                REGTRACE_T23_AE_HLIL_STATE_COUNT - 1U].ev_q10 >> 10);
        desired_state = regtrace_t23_ae_hlil_nearest_state(desired_ev);
        if (desired_state > state + 4U)
            next_state = state + 4U;
        else if (state > desired_state + 4U)
            next_state = state - 4U;
        else
            next_state = desired_state;
    }

    if (!ret && next_state != state) {
        rung = &regtrace_t23_ae_hlil_rungs[next_state];
        ret = regtrace_t23_call_sensor_exposure(rung->packed,
                                                "source-ae-hlil");
        if (!ret && rung->gain_q16 !=
            regtrace_t23_ae_hlil_rungs[state].gain_q16)
            ret = regtrace_t23_source_apply_total_gain_value(
                rung->gain_q16, rung->packed >> 16, false, true);
        if (!ret) {
            regtrace_t23_source_ae_hlil_state = next_state;
            regtrace_t23_source_ae_hlil_ev = rung->ev_q10;
            regtrace_t23_source_ae_hlil_updates++;
            if (regtrace_t23_source_ccm_events &&
                regtrace_t23_source_ccm_tuning_init &&
                regtrace_t23_ccm_saturation(rung->ev_q10) !=
                regtrace_t23_source_ccm_last_saturation)
                regtrace_t23_source_ccm_commit(
                    regtrace_t23_source_ccm_runtime_ct, rung->ev_q10);
            if (regtrace_t23_source_bcsh_events) {
                tisp_bcsh_ev_update(rung->ev_q10, 0);
                regtrace_t23_source_bcsh_ev_events++;
            }
        }
    }

    regtrace_t23_source_ae_hlil_status = ret ? (uint32_t)(-ret) : 0U;
    if (next_state != state || regtrace_t23_source_ae_hlil_runs <= 8U ||
        !(regtrace_t23_source_ae_hlil_runs &
          (regtrace_t23_source_ae_hlil_runs - 1U))) {
        if (ret)
            printk(KERN_WARNING
                   "tx_isp_t23_recovered: source AE HLIL run=%u snapshot=%u luma=%u target=%u+-%u state=%u->%u updates=%u ret=%d\n",
                   regtrace_t23_source_ae_hlil_runs, snapshot, luma,
                   regtrace_t23_source_ae_hlil_target,
                   regtrace_t23_source_ae_hlil_deadband, state, next_state,
                   regtrace_t23_source_ae_hlil_updates, ret);
        else
            printk(KERN_INFO
                   "tx_isp_t23_recovered: source AE HLIL run=%u snapshot=%u luma=%u target=%u+-%u state=%u->%u updates=%u ret=%d\n",
                   regtrace_t23_source_ae_hlil_runs, snapshot, luma,
                   regtrace_t23_source_ae_hlil_target,
                   regtrace_t23_source_ae_hlil_deadband, state, next_state,
                   regtrace_t23_source_ae_hlil_updates, ret);
    }

    spin_lock_irqsave(&regtrace_t23_ae_hlil_lock, flags);
    regtrace_t23_ae_hlil_pending = false;
    spin_unlock_irqrestore(&regtrace_t23_ae_hlil_lock, flags);
}

static void regtrace_t23_source_ae_hlil_capture(uint32_t luma,
                                               uint32_t snapshot)
{
    unsigned long flags;

    if (!regtrace_t23_source_ae_hlil)
        return;
    if (regtrace_t23_source_ae_hlil_interval &&
        snapshot % regtrace_t23_source_ae_hlil_interval)
        return;

    spin_lock_irqsave(&regtrace_t23_ae_hlil_lock, flags);
    if (regtrace_t23_ae_hlil_pending) {
        regtrace_t23_source_ae_hlil_drops++;
        spin_unlock_irqrestore(&regtrace_t23_ae_hlil_lock, flags);
        return;
    }
    regtrace_t23_ae_hlil_pending_luma = luma;
    regtrace_t23_ae_hlil_pending_snapshot = snapshot;
    regtrace_t23_ae_hlil_pending = true;
    spin_unlock_irqrestore(&regtrace_t23_ae_hlil_lock, flags);
    schedule_work(&regtrace_t23_source_ae_hlil_work_item);
}

static void regtrace_t23_source_ae_hlil_trigger(void)
{
    unsigned long flags;

    if (!regtrace_t23_source_ae_hlil || !regtrace_t23_sensor_streaming ||
        !regtrace_t23_core_started)
        return;

    spin_lock_irqsave(&regtrace_t23_ae_hlil_lock, flags);
    if (regtrace_t23_ae_hlil_pending) {
        regtrace_t23_source_ae_hlil_drops++;
        spin_unlock_irqrestore(&regtrace_t23_ae_hlil_lock, flags);
        return;
    }
    regtrace_t23_ae_hlil_pending_luma = regtrace_t23_source_ae_hlil_luma;
    regtrace_t23_ae_hlil_pending_snapshot =
        regtrace_t23_source_ae_stats_snapshots;
    regtrace_t23_ae_hlil_pending = true;
    spin_unlock_irqrestore(&regtrace_t23_ae_hlil_lock, flags);
    schedule_work(&regtrace_t23_source_ae_hlil_work_item);
}

static void regtrace_t23_source_ae_hlil_reset(void)
{
    unsigned long flags;
    uint32_t initial_packed = regtrace_t23_source_ae_force_packed ?
        regtrace_t23_source_ae_force_packed : 0x008004abU;
    uint32_t state;

    cancel_work_sync(&regtrace_t23_source_ae_hlil_work_item);
    spin_lock_irqsave(&regtrace_t23_ae_hlil_lock, flags);
    regtrace_t23_ae_hlil_pending = false;
    regtrace_t23_ae_hlil_pending_luma = 0;
    regtrace_t23_ae_hlil_pending_snapshot = 0;
    spin_unlock_irqrestore(&regtrace_t23_ae_hlil_lock, flags);

    regtrace_t23_source_ae_hlil_state = 0U;
    for (state = 0; state < REGTRACE_T23_AE_HLIL_STATE_COUNT; ++state) {
        if (regtrace_t23_ae_hlil_rungs[state].packed == initial_packed) {
            regtrace_t23_source_ae_hlil_state = state;
            break;
        }
    }
    if (state == REGTRACE_T23_AE_HLIL_STATE_COUNT)
        regtrace_t23_source_ae_hlil_state =
            regtrace_t23_ae_hlil_nearest_state(initial_packed & 0xffffU);
    regtrace_t23_source_ae_hlil_ev =
        regtrace_t23_ae_hlil_rungs[regtrace_t23_source_ae_hlil_state].ev_q10;
    regtrace_t23_source_ae_hlil_runs = 0;
    regtrace_t23_source_ae_hlil_updates = 0;
    regtrace_t23_source_ae_hlil_drops = 0;
    regtrace_t23_source_ae_hlil_luma = 0;
    regtrace_t23_source_ae_hlil_status = 0;
    memset(regtrace_t23_source_ae_scene_luma, 0,
           sizeof(regtrace_t23_source_ae_scene_luma));
    regtrace_t23_source_ae_scene_luma_valid = false;
    regtrace_t23_source_bcsh_ev_events = 0;

    printk(KERN_WARNING
           "tx_isp_t23_recovered: source AE HLIL reset target=%u+-%u interval=%u state=%u ev=%u enabled=%u\n",
           regtrace_t23_source_ae_hlil_target,
           regtrace_t23_source_ae_hlil_deadband,
           regtrace_t23_source_ae_hlil_interval,
           regtrace_t23_source_ae_hlil_state,
           regtrace_t23_source_ae_hlil_ev,
           regtrace_t23_source_ae_hlil ? 1U : 0U);
}
