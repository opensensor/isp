/*
 * Active T23 Tiziano AWB path reconstructed from tx-isp-t23-hlil.txt.
 *
 * The SC2336 tuning disables cluster selection and CT edge weighting. The live
 * EV selects outdoor or indoor CT weights; indoor mode also applies the OEM
 * light-source distance LUT before the final distance refinement.
 */

#define REGTRACE_T23_AWB_HLIL_COLS 15U
#define REGTRACE_T23_AWB_HLIL_ROWS 15U
#define REGTRACE_T23_AWB_HLIL_ZONES \
    (REGTRACE_T23_AWB_HLIL_COLS * REGTRACE_T23_AWB_HLIL_ROWS)

struct regtrace_t23_awb_hlil_snapshot {
    uint32_t r[REGTRACE_T23_AWB_HLIL_ZONES];
    uint32_t g[REGTRACE_T23_AWB_HLIL_ZONES];
    uint32_t b[REGTRACE_T23_AWB_HLIL_ZONES];
    uint32_t pixels[REGTRACE_T23_AWB_HLIL_ZONES];
    uint32_t bank;
    uint32_t snapshot;
};

struct regtrace_t23_awb_hlil_result {
    uint32_t rg;
    uint32_t bg;
    uint32_t ct;
    uint32_t active_zones;
    uint32_t light_source_zones;
};

static struct regtrace_t23_awb_hlil_snapshot
    regtrace_t23_awb_hlil_snapshot_data;
static uint32_t regtrace_t23_awb_hlil_ratio[REGTRACE_T23_AWB_HLIL_ZONES * 2U];
static uint32_t regtrace_t23_awb_hlil_pixel_weight[REGTRACE_T23_AWB_HLIL_ZONES];
static uint32_t regtrace_t23_awb_hlil_mesh_weight[REGTRACE_T23_AWB_HLIL_ZONES];
static uint32_t regtrace_t23_awb_hlil_distance[REGTRACE_T23_AWB_HLIL_ZONES];
static uint32_t regtrace_t23_awb_hlil_distance_weight[REGTRACE_T23_AWB_HLIL_ZONES];
static uint32_t regtrace_t23_awb_hlil_history_rg[15];
static uint32_t regtrace_t23_awb_hlil_history_bg[15];
static uint32_t regtrace_t23_awb_hlil_history_ct[15];
static uint32_t regtrace_t23_awb_hlil_history_count;
static bool regtrace_t23_awb_hlil_pending;
static DEFINE_SPINLOCK(regtrace_t23_awb_hlil_lock);

static void regtrace_t23_source_awb_hlil_work(struct work_struct *work);
static DECLARE_WORK(regtrace_t23_source_awb_hlil_work_item,
                    regtrace_t23_source_awb_hlil_work);

static uint32_t regtrace_t23_awb_hlil_mul_q(uint32_t left, uint32_t right)
{
    return (uint32_t)(((uint64_t)left * right) >>
                      REGTRACE_T23_AWB_HLIL_POINT_POSITION);
}

static uint32_t regtrace_t23_awb_hlil_div_q(uint64_t numerator,
                                            uint64_t denominator)
{
    if (!denominator)
        return 0;
    return (uint32_t)div64_u64(
        numerator << REGTRACE_T23_AWB_HLIL_POINT_POSITION, denominator);
}

static uint32_t regtrace_t23_awb_hlil_interpolate_q(uint32_t x_q,
                                                    uint32_t x0,
                                                    uint32_t x1,
                                                    uint32_t y0_q,
                                                    uint32_t y1_q)
{
    uint32_t q = REGTRACE_T23_AWB_HLIL_POINT_POSITION;
    uint32_t x0_q = x0 << q;
    uint32_t span_q = (x1 - x0) << q;
    uint32_t delta_x;
    uint64_t delta;

    if (!span_q || x_q <= x0_q)
        return y0_q;
    if (x_q >= (x1 << q))
        return y1_q;

    delta_x = x_q - x0_q;
    if (y1_q >= y0_q) {
        delta = (uint64_t)(y1_q - y0_q) * delta_x;
        return y0_q + (uint32_t)div64_u64(delta, span_q);
    }

    delta = (uint64_t)(y0_q - y1_q) * delta_x;
    return y0_q - (uint32_t)div64_u64(delta, span_q);
}

static uint32_t regtrace_t23_awb_hlil_mesh_q(const uint32_t *mesh,
                                             uint32_t rg_q,
                                             uint32_t bg_q)
{
    const uint32_t *rg_pos = regtrace_t23_awb_hlil_rg_positions;
    const uint32_t *bg_pos = regtrace_t23_awb_hlil_bg_positions;
    uint32_t q = REGTRACE_T23_AWB_HLIL_POINT_POSITION;
    uint32_t rg_index = 13U;
    uint32_t bg_index = 13U;
    uint32_t row0_q;
    uint32_t row1_q;
    uint32_t i;

    rg_q = max(rg_q, rg_pos[0] << q);
    rg_q = min(rg_q, rg_pos[14] << q);
    bg_q = max(bg_q, bg_pos[0] << q);
    bg_q = min(bg_q, bg_pos[14] << q);

    for (i = 0; i < 14U; ++i) {
        if (rg_q <= (rg_pos[i + 1U] << q)) {
            rg_index = i;
            break;
        }
    }
    for (i = 0; i < 14U; ++i) {
        if (bg_q <= (bg_pos[i + 1U] << q)) {
            bg_index = i;
            break;
        }
    }

    row0_q = regtrace_t23_awb_hlil_interpolate_q(
        rg_q, rg_pos[rg_index], rg_pos[rg_index + 1U],
        mesh[bg_index * 15U + rg_index] << q,
        mesh[bg_index * 15U + rg_index + 1U] << q);
    row1_q = regtrace_t23_awb_hlil_interpolate_q(
        rg_q, rg_pos[rg_index], rg_pos[rg_index + 1U],
        mesh[(bg_index + 1U) * 15U + rg_index] << q,
        mesh[(bg_index + 1U) * 15U + rg_index + 1U] << q);

    return regtrace_t23_awb_hlil_interpolate_q(
        bg_q, bg_pos[bg_index], bg_pos[bg_index + 1U], row0_q, row1_q);
}

static uint32_t regtrace_t23_awb_hlil_light_source_weight_q(uint32_t rg_q,
                                                            uint32_t bg_q)
{
    const uint32_t q = REGTRACE_T23_AWB_HLIL_POINT_POSITION;
    const uint32_t rounding = 1U << (q - 1U);
    uint32_t rg = (rg_q + rounding) >> q;
    uint32_t bg = (bg_q + rounding) >> q;
    uint32_t best_weight_q = 0;
    uint32_t source;

    for (source = 0; source < REGTRACE_T23_AWB_HLIL_LIGHT_SOURCE_COUNT;
         ++source) {
        uint32_t ref_rg = regtrace_t23_awb_hlil_light_sources[source * 2U];
        uint32_t ref_bg =
            regtrace_t23_awb_hlil_light_sources[source * 2U + 1U];
        uint32_t delta_rg;
        uint32_t delta_bg;
        uint32_t distance;
        uint32_t weight = 0;

        if (ref_rg)
            ref_rg--;
        if (ref_bg)
            ref_bg--;
        delta_rg = rg >= ref_rg ? rg - ref_rg : ref_rg - rg;
        delta_bg = bg >= ref_bg ? bg - ref_bg : ref_bg - bg;
        distance = delta_rg * delta_rg + delta_bg * delta_bg;

        if (distance < 0x332U) {
            weight = 1U;
            if (distance < 0x2a7U) {
                weight = 2U;
                if (distance < 0x267U) {
                    weight = 3U;
                    if (distance < 0x23cU) {
                        weight = 4U;
                        if (distance < 0x21cU) {
                            weight = 5U;
                            if (distance < 0x202U)
                                weight =
                                    regtrace_t23_awb_hlil_light_source_weight_lut[
                                        distance];
                        }
                    }
                }
            }
        }
        best_weight_q = max(best_weight_q, weight << q);
    }

    return best_weight_q;
}

static int regtrace_t23_awb_hlil_calculate(
    const struct regtrace_t23_awb_hlil_snapshot *snapshot,
    struct regtrace_t23_awb_hlil_result *result)
{
    const uint32_t q = REGTRACE_T23_AWB_HLIL_POINT_POSITION;
    const uint32_t rounding = 1U << (q - 1U);
    const uint32_t total = REGTRACE_T23_AWB_HLIL_ZONES;
    const uint32_t bg_offset = REGTRACE_T23_AWB_HLIL_ZONES;
    uint32_t max_pixels = 0;
    uint32_t coefficient_rg_q;
    uint32_t coefficient_bg_q;
    uint64_t total_mesh_weight = 0;
    uint64_t sum_rg = 0;
    uint64_t sum_bg = 0;
    uint64_t sum_weight = 0;
    uint32_t average_rg_q;
    uint32_t average_bg_q;
    uint32_t active_zones = 0;
    bool indoor = regtrace_t23_source_ae_hlil_ev >=
                  (REGTRACE_T23_AWB_HLIL_INDOOR_EV << q);
    const uint32_t *ct_weight_mesh = indoor ?
        regtrace_t23_awb_hlil_indoor_ct_weight_mesh :
        regtrace_t23_awb_hlil_outdoor_ct_weight_mesh;
    uint32_t i;

    coefficient_rg_q = regtrace_t23_awb_hlil_mul_q(
        0x100U << q, regtrace_t23_awb_hlil_wb_static[0]);
    coefficient_bg_q = regtrace_t23_awb_hlil_mul_q(
        0x100U << q, regtrace_t23_awb_hlil_wb_static[1]);

    for (i = 0; i < total; ++i)
        max_pixels = max(max_pixels, snapshot->pixels[i]);
    if (!max_pixels)
        return -ENODATA;

    for (i = 0; i < total; ++i) {
        uint32_t green = snapshot->g[i];
        uint32_t pixels = snapshot->pixels[i];
        uint32_t rg_q = 0;
        uint32_t bg_q = 0;

        if (green) {
            uint32_t raw_rg_q = (uint32_t)div64_u64(
                (uint64_t)snapshot->r[i] << q, green);
            uint32_t raw_bg_q = (uint32_t)div64_u64(
                (uint64_t)snapshot->b[i] << q, green);

            rg_q = regtrace_t23_awb_hlil_mul_q(raw_rg_q, coefficient_rg_q);
            bg_q = regtrace_t23_awb_hlil_mul_q(raw_bg_q, coefficient_bg_q);
        }

        if (pixels < regtrace_t23_source_awb_hlil_min_pixels ||
            !rg_q || !bg_q) {
            regtrace_t23_awb_hlil_ratio[i] = 0;
            regtrace_t23_awb_hlil_ratio[bg_offset + i] = 0;
            regtrace_t23_awb_hlil_pixel_weight[i] = 0;
            regtrace_t23_awb_hlil_mesh_weight[i] = 0;
            continue;
        }

        rg_q = max(rg_q, regtrace_t23_awb_hlil_rg_positions[0] << q);
        rg_q = min(rg_q, regtrace_t23_awb_hlil_rg_positions[14] << q);
        bg_q = max(bg_q, regtrace_t23_awb_hlil_bg_positions[0] << q);
        bg_q = min(bg_q, regtrace_t23_awb_hlil_bg_positions[14] << q);
        regtrace_t23_awb_hlil_ratio[i] = rg_q;
        regtrace_t23_awb_hlil_ratio[bg_offset + i] = bg_q;
        regtrace_t23_awb_hlil_pixel_weight[i] =
            regtrace_t23_awb_hlil_div_q(pixels, max_pixels);
        regtrace_t23_awb_hlil_mesh_weight[i] =
            regtrace_t23_awb_hlil_mesh_q(
                regtrace_t23_awb_hlil_color_temperature_mesh, rg_q, bg_q);
        active_zones++;
    }

    if (indoor) {
        for (i = 0; i < total; ++i) {
            uint32_t light_weight;

            if (!regtrace_t23_awb_hlil_ratio[i] ||
                !regtrace_t23_awb_hlil_ratio[bg_offset + i] ||
                !regtrace_t23_awb_hlil_pixel_weight[i])
                continue;
            light_weight = regtrace_t23_awb_hlil_light_source_weight_q(
                regtrace_t23_awb_hlil_ratio[i],
                regtrace_t23_awb_hlil_ratio[bg_offset + i]);
            if (light_weight > regtrace_t23_awb_hlil_mesh_weight[i]) {
                regtrace_t23_awb_hlil_mesh_weight[i] = light_weight;
                result->light_source_zones++;
            }
        }
    }
    for (i = 0; i < total; ++i)
        total_mesh_weight += regtrace_t23_awb_hlil_mesh_weight[i];

    if (!active_zones || !total_mesh_weight)
        return -ENODATA;

    for (i = 0; i < total; ++i) {
        uint32_t weight = regtrace_t23_awb_hlil_mul_q(
            regtrace_t23_awb_hlil_mesh_weight[i],
            regtrace_t23_awb_hlil_pixel_weight[i]);

        sum_rg += regtrace_t23_awb_hlil_mul_q(
            regtrace_t23_awb_hlil_ratio[i], weight);
        sum_bg += regtrace_t23_awb_hlil_mul_q(
            regtrace_t23_awb_hlil_ratio[bg_offset + i], weight);
        sum_weight += weight;
    }
    if (!sum_weight)
        return -ENODATA;

    average_rg_q = regtrace_t23_awb_hlil_div_q(sum_rg, sum_weight);
    average_bg_q = regtrace_t23_awb_hlil_div_q(sum_bg, sum_weight);

    if (regtrace_t23_awb_hlil_distance_parameters[0] == 1U) {
        uint64_t distance_sum = 0;
        uint64_t refined_rg = 0;
        uint64_t refined_bg = 0;
        uint64_t refined_weight = 0;
        uint32_t normalized_distance;

        for (i = 0; i < total; ++i) {
            uint32_t rg_q = regtrace_t23_awb_hlil_ratio[i];
            uint32_t bg_q = regtrace_t23_awb_hlil_ratio[bg_offset + i];
            uint32_t delta_rg;
            uint32_t delta_bg;
            uint32_t distance;

            if (!regtrace_t23_awb_hlil_mesh_weight[i] ||
                !regtrace_t23_awb_hlil_pixel_weight[i]) {
                regtrace_t23_awb_hlil_distance[i] = 0;
                continue;
            }
            delta_rg = average_rg_q >= rg_q ?
                average_rg_q - rg_q : rg_q - average_rg_q;
            delta_bg = average_bg_q >= bg_q ?
                average_bg_q - bg_q : bg_q - average_bg_q;
            distance = regtrace_t23_awb_hlil_mul_q(delta_rg, delta_rg) +
                       regtrace_t23_awb_hlil_mul_q(delta_bg, delta_bg);
            regtrace_t23_awb_hlil_distance[i] = distance;
            distance_sum += regtrace_t23_awb_hlil_mul_q(
                regtrace_t23_awb_hlil_mul_q(
                    distance, regtrace_t23_awb_hlil_mesh_weight[i]),
                regtrace_t23_awb_hlil_zone_weight_mesh[i] << q);
        }

        normalized_distance = regtrace_t23_awb_hlil_div_q(
            distance_sum, sum_weight);
        if (normalized_distance) {
            uint32_t distance_hi =
                regtrace_t23_awb_hlil_distance_parameters[1];
            uint32_t distance_lo =
                regtrace_t23_awb_hlil_distance_parameters[2];

            for (i = 0; i < total; ++i) {
                uint32_t relative_distance;
                uint32_t distance_weight;
                uint32_t weight;

                if (!regtrace_t23_awb_hlil_mesh_weight[i] ||
                    !regtrace_t23_awb_hlil_pixel_weight[i])
                    continue;
                relative_distance = regtrace_t23_awb_hlil_div_q(
                    regtrace_t23_awb_hlil_distance[i], normalized_distance);
                if (relative_distance > distance_hi) {
                    distance_weight = 0;
                } else if (relative_distance >= distance_lo) {
                    uint32_t ramp = regtrace_t23_awb_hlil_div_q(
                        relative_distance - distance_lo,
                        distance_hi - distance_lo);

                    distance_weight = regtrace_t23_awb_hlil_mul_q(
                        regtrace_t23_awb_hlil_mesh_weight[i],
                        (1U << q) - min(ramp, 1U << q));
                } else {
                    distance_weight = regtrace_t23_awb_hlil_mesh_weight[i];
                }
                regtrace_t23_awb_hlil_distance_weight[i] = distance_weight;
                weight = regtrace_t23_awb_hlil_mul_q(
                    distance_weight, regtrace_t23_awb_hlil_pixel_weight[i]);
                refined_rg += regtrace_t23_awb_hlil_mul_q(
                    regtrace_t23_awb_hlil_ratio[i], weight);
                refined_bg += regtrace_t23_awb_hlil_mul_q(
                    regtrace_t23_awb_hlil_ratio[bg_offset + i], weight);
                refined_weight += weight;
            }
            if (refined_weight) {
                average_rg_q = regtrace_t23_awb_hlil_div_q(
                    refined_rg, refined_weight);
                average_bg_q = regtrace_t23_awb_hlil_div_q(
                    refined_bg, refined_weight);
            }
        }
    }

    average_rg_q = max(average_rg_q,
                       regtrace_t23_awb_hlil_rg_positions[0] << q);
    average_rg_q = min(average_rg_q,
                       regtrace_t23_awb_hlil_rg_positions[14] << q);
    average_bg_q = max(average_bg_q,
                       regtrace_t23_awb_hlil_bg_positions[0] << q);
    average_bg_q = min(average_bg_q,
                       regtrace_t23_awb_hlil_bg_positions[14] << q);

    result->rg = (average_rg_q + rounding) >> q;
    result->bg = (average_bg_q + rounding) >> q;
    result->active_zones = active_zones;
    {
        uint32_t inverse_ct_q = regtrace_t23_awb_hlil_mesh_q(
            ct_weight_mesh, average_rg_q, average_bg_q);

        result->ct = inverse_ct_q ?
            (uint32_t)div64_u64(((uint64_t)1000000U << q) +
                                (inverse_ct_q >> 1), inverse_ct_q) :
            5000U;
    }
    return 0;
}

static void regtrace_t23_awb_hlil_history_update(uint32_t *rg,
                                                 uint32_t *bg,
                                                 uint32_t *ct)
{
    uint32_t window;
    uint32_t start;
    uint32_t i;
    uint32_t weight_sum = 0;
    uint64_t rg_sum = 0;
    uint64_t bg_sum = 0;
    uint64_t ct_sum = 0;

    if (!regtrace_t23_awb_hlil_history_count) {
        for (i = 0; i < 15U; ++i) {
            regtrace_t23_awb_hlil_history_rg[i] = *rg;
            regtrace_t23_awb_hlil_history_bg[i] = *bg;
            regtrace_t23_awb_hlil_history_ct[i] = *ct;
        }
        regtrace_t23_awb_hlil_history_count = 1U;
    } else {
        memmove(regtrace_t23_awb_hlil_history_rg,
                regtrace_t23_awb_hlil_history_rg + 1, 14U * sizeof(uint32_t));
        memmove(regtrace_t23_awb_hlil_history_bg,
                regtrace_t23_awb_hlil_history_bg + 1, 14U * sizeof(uint32_t));
        memmove(regtrace_t23_awb_hlil_history_ct,
                regtrace_t23_awb_hlil_history_ct + 1, 14U * sizeof(uint32_t));
        regtrace_t23_awb_hlil_history_rg[14] = *rg;
        regtrace_t23_awb_hlil_history_bg[14] = *bg;
        regtrace_t23_awb_hlil_history_ct[14] = *ct;
        regtrace_t23_awb_hlil_history_count = min(
            regtrace_t23_awb_hlil_history_count + 1U, 15U);
    }

    window = min((uint32_t)REGTRACE_T23_AWB_HLIL_HISTORY_WINDOW,
                 regtrace_t23_awb_hlil_history_count);
    window = max(window, 1U);
    start = 15U - window;
    for (i = start; i < 15U; ++i) {
        uint32_t weight = i + 1U;

        rg_sum += (uint64_t)weight * regtrace_t23_awb_hlil_history_rg[i];
        bg_sum += (uint64_t)weight * regtrace_t23_awb_hlil_history_bg[i];
        ct_sum += (uint64_t)weight * regtrace_t23_awb_hlil_history_ct[i];
        weight_sum += weight;
    }
    *rg = (uint32_t)div64_u64(rg_sum + (weight_sum >> 1), weight_sum);
    *bg = (uint32_t)div64_u64(bg_sum + (weight_sum >> 1), weight_sum);
    *ct = (uint32_t)div64_u64(ct_sum + (weight_sum >> 1), weight_sum);
}

static uint32_t regtrace_t23_awb_hlil_ratio_to_mf(uint32_t ratio)
{
    uint32_t q = REGTRACE_T23_AWB_HLIL_POINT_POSITION;
    uint32_t rounding = 1U << (q - 1U);
    uint64_t reciprocal;

    if (!ratio)
        return 0x100U;
    reciprocal = div64_u64(1ULL << (q + 16U), ratio);
    return (uint32_t)((reciprocal + rounding) >> q);
}

static void regtrace_t23_source_awb_hlil_work(struct work_struct *work)
{
    struct regtrace_t23_awb_hlil_result result;
    unsigned long flags;
    uint32_t mf_rg;
    uint32_t mf_bg;
    uint32_t gain_rg;
    uint32_t gain_bg;
    int ret;

    (void)work;
    memset(&result, 0, sizeof(result));
    ret = regtrace_t23_awb_hlil_calculate(
        &regtrace_t23_awb_hlil_snapshot_data, &result);
    regtrace_t23_source_awb_hlil_runs++;
    regtrace_t23_source_awb_hlil_status = ret ? 1U : 0U;

    if (!ret) {
        regtrace_t23_awb_hlil_history_update(
            &result.rg, &result.bg, &result.ct);
        mf_rg = regtrace_t23_awb_hlil_ratio_to_mf(result.rg);
        mf_bg = regtrace_t23_awb_hlil_ratio_to_mf(result.bg);
        gain_rg = (mf_rg * regtrace_t23_awb_hlil_wb_static[0] +
                   (1U << (REGTRACE_T23_AWB_HLIL_POINT_POSITION - 1U))) >>
                  REGTRACE_T23_AWB_HLIL_POINT_POSITION;
        gain_bg = (mf_bg * regtrace_t23_awb_hlil_wb_static[1] +
                   (1U << (REGTRACE_T23_AWB_HLIL_POINT_POSITION - 1U))) >>
                  REGTRACE_T23_AWB_HLIL_POINT_POSITION;
        gain_rg = min(gain_rg << 2, 0x3fffU);
        gain_bg = min(gain_bg << 2, 0x3fffU);

        regtrace_t23_source_awb_hlil_rg = result.rg;
        regtrace_t23_source_awb_hlil_bg = result.bg;
        regtrace_t23_source_awb_hlil_ct = result.ct;
        if (regtrace_t23_source_ccm_events &&
            regtrace_t23_source_ccm_tuning_init &&
            result.ct >= 1500U && result.ct <= 10000U) {
            uint32_t delta = regtrace_t23_source_ccm_last_ct >= result.ct ?
                regtrace_t23_source_ccm_last_ct - result.ct :
                result.ct - regtrace_t23_source_ccm_last_ct;

            if (delta > 0x20U)
                regtrace_t23_source_ccm_commit(
                    result.ct, regtrace_t23_source_ae_hlil_ev);
        }
        if (regtrace_t23_source_lsc_events &&
            result.ct >= regtrace_t23_lsc_sc2336_ct_points[0] &&
            result.ct <= 10000U) {
            uint32_t delta = regtrace_t23_source_lsc_last_ct >= result.ct ?
                regtrace_t23_source_lsc_last_ct - result.ct :
                result.ct - regtrace_t23_source_lsc_last_ct;

            if (delta > 0x20U) {
                tisp_lsc_ct_update(result.ct);
                if (!tisp_lsc_write_lut_datas()) {
                    regtrace_t23_source_lsc_last_ct = result.ct;
                    regtrace_t23_source_lsc_event_count++;
                }
            }
        }
        if (regtrace_t23_source_bcsh_events &&
            result.ct >= 2000U && result.ct <= 10000U) {
            uint32_t delta = regtrace_t23_source_bcsh_last_ct >= result.ct ?
                regtrace_t23_source_bcsh_last_ct - result.ct :
                result.ct - regtrace_t23_source_bcsh_last_ct;

            if (delta > 0x20U) {
                tisp_bcsh_ct_update(0, result.ct);
                regtrace_t23_source_bcsh_last_ct = result.ct;
                regtrace_t23_source_bcsh_ct_events++;
            }
        }
        if ((gain_rg > regtrace_t23_source_awb_last_rgain ?
             gain_rg - regtrace_t23_source_awb_last_rgain :
             regtrace_t23_source_awb_last_rgain - gain_rg) > 4U ||
            (gain_bg > regtrace_t23_source_awb_last_bgain ?
             gain_bg - regtrace_t23_source_awb_last_bgain :
             regtrace_t23_source_awb_last_bgain - gain_bg) > 4U) {
            regtrace_t23_source_awb_apply_gains(gain_rg, gain_bg);
            regtrace_t23_source_awb_hlil_updates++;
        }

        if (regtrace_t23_source_awb_hlil_runs <= 8U ||
            !(regtrace_t23_source_awb_hlil_runs &
              (regtrace_t23_source_awb_hlil_runs - 1U)))
            printk(KERN_INFO
                   "tx_isp_t23_recovered: source AWB HLIL run=%u snapshot=%u bank=%u zones=%u lights=%u ev=%u target=%u/%u ct=%u mf=%u/%u gain=0x%x/0x%x updates=%u\n",
                   regtrace_t23_source_awb_hlil_runs,
                   regtrace_t23_awb_hlil_snapshot_data.snapshot,
                   regtrace_t23_awb_hlil_snapshot_data.bank,
                   result.active_zones, result.light_source_zones,
                   regtrace_t23_source_ae_hlil_ev,
                   result.rg, result.bg, result.ct,
                   mf_rg, mf_bg, gain_rg, gain_bg,
                   regtrace_t23_source_awb_hlil_updates);
    } else if (regtrace_t23_source_awb_hlil_runs <= 8U) {
        printk(KERN_WARNING
               "tx_isp_t23_recovered: source AWB HLIL run=%u snapshot=%u failed ret=%d\n",
               regtrace_t23_source_awb_hlil_runs,
               regtrace_t23_awb_hlil_snapshot_data.snapshot, ret);
    }

    spin_lock_irqsave(&regtrace_t23_awb_hlil_lock, flags);
    regtrace_t23_awb_hlil_pending = false;
    spin_unlock_irqrestore(&regtrace_t23_awb_hlil_lock, flags);
}

static void regtrace_t23_source_awb_hlil_capture(const uint32_t *words,
                                                 uint32_t bank,
                                                 uint32_t snapshot)
{
    unsigned long flags;
    unsigned int zone;

    if (!regtrace_t23_source_awb_hlil)
        return;
    if (regtrace_t23_source_awb_hlil_interval &&
        snapshot % regtrace_t23_source_awb_hlil_interval)
        return;

    spin_lock_irqsave(&regtrace_t23_awb_hlil_lock, flags);
    if (regtrace_t23_awb_hlil_pending) {
        regtrace_t23_source_awb_hlil_drops++;
        spin_unlock_irqrestore(&regtrace_t23_awb_hlil_lock, flags);
        return;
    }

    for (zone = 0; zone < REGTRACE_T23_AWB_HLIL_ZONES;
         ++zone, words += 4) {
        uint32_t w0 = words[0];
        uint32_t w1 = words[1];
        uint32_t w2 = words[2];
        uint32_t w3 = words[3];

        regtrace_t23_awb_hlil_snapshot_data.r[zone] = w0 & 0x1fffffU;
        regtrace_t23_awb_hlil_snapshot_data.g[zone] =
            ((w1 & 0x3ffU) << 11) | (w0 >> 21);
        regtrace_t23_awb_hlil_snapshot_data.b[zone] =
            (w1 & 0x7ffffc00U) >> 10;
        regtrace_t23_awb_hlil_snapshot_data.pixels[zone] =
            ((w3 & 1U) << 12) | (w2 >> 20);
    }
    regtrace_t23_awb_hlil_snapshot_data.bank = bank;
    regtrace_t23_awb_hlil_snapshot_data.snapshot = snapshot;
    regtrace_t23_awb_hlil_pending = true;
    spin_unlock_irqrestore(&regtrace_t23_awb_hlil_lock, flags);
    schedule_work(&regtrace_t23_source_awb_hlil_work_item);
}

static void regtrace_t23_source_awb_hlil_reset(void)
{
    unsigned long flags;

    cancel_work_sync(&regtrace_t23_source_awb_hlil_work_item);
    spin_lock_irqsave(&regtrace_t23_awb_hlil_lock, flags);
    regtrace_t23_awb_hlil_pending = false;
    regtrace_t23_awb_hlil_history_count = 0;
    spin_unlock_irqrestore(&regtrace_t23_awb_hlil_lock, flags);
    regtrace_t23_source_awb_hlil_runs = 0;
    regtrace_t23_source_awb_hlil_updates = 0;
    regtrace_t23_source_awb_hlil_drops = 0;
    regtrace_t23_source_awb_hlil_status = 0;
    regtrace_t23_source_awb_hlil_rg = 0;
    regtrace_t23_source_awb_hlil_bg = 0;
    regtrace_t23_source_awb_hlil_ct = REGTRACE_T23_AWB_HLIL_STARTUP_CT;
    regtrace_t23_source_ccm_event_count = 0;
    regtrace_t23_source_ccm_last_ct = regtrace_t23_source_ccm_runtime_ct;
    regtrace_t23_source_lsc_event_count = 0;
    regtrace_t23_source_lsc_last_ct = regtrace_t23_source_lsc_runtime_ct;
    regtrace_t23_source_lsc_update_pending = false;
    regtrace_t23_source_bcsh_ct_events = 0;
    regtrace_t23_source_bcsh_last_ct = 3187U;
    printk(KERN_WARNING
           "tx_isp_t23_recovered: source AWB HLIL reset ev=%u ct=%u q=%u mf=%u/%u interval=%u enabled=%u\n",
           REGTRACE_T23_AWB_HLIL_STARTUP_EV,
           REGTRACE_T23_AWB_HLIL_STARTUP_CT,
           REGTRACE_T23_AWB_HLIL_POINT_POSITION,
           regtrace_t23_awb_hlil_mf_parameters[4],
           regtrace_t23_awb_hlil_mf_parameters[5],
           regtrace_t23_source_awb_hlil_interval,
           regtrace_t23_source_awb_hlil ? 1U : 0U);
}
