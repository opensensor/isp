/* Runtime storage for the T23 IQ-bin AWB parameter block. */
#define REGTRACE_T23_AWB_HLIL_MAX_HISTORY 15U
#define REGTRACE_T23_AWB_HLIL_MAX_LIGHT_SOURCES 10U
#define REGTRACE_T23_AWB_PROFILE_BIAS_MAGIC 0x57415950U /* "PYAW" */
#define REGTRACE_T23_AWB_PROFILE_BIAS_UNITY 1024U
#define REGTRACE_T23_AWB_PROFILE_BIAS_MIN 512U
#define REGTRACE_T23_AWB_PROFILE_BIAS_MAX 2048U

static uint32_t regtrace_t23_awb_hlil_pixel_threshold;
static uint32_t regtrace_t23_awb_hlil_point_position;
static uint32_t regtrace_t23_awb_hlil_history_window;
static uint32_t regtrace_t23_awb_hlil_startup_ev;
static uint32_t regtrace_t23_awb_hlil_startup_ct;
static uint32_t regtrace_t23_awb_hlil_outdoor_ev;
static uint32_t regtrace_t23_awb_hlil_indoor_ev;
static uint32_t regtrace_t23_awb_hlil_light_source_count;

#define REGTRACE_T23_AWB_HLIL_PIXEL_THRESHOLD \
    regtrace_t23_awb_hlil_pixel_threshold
#define REGTRACE_T23_AWB_HLIL_POINT_POSITION \
    regtrace_t23_awb_hlil_point_position
#define REGTRACE_T23_AWB_HLIL_HISTORY_WINDOW \
    regtrace_t23_awb_hlil_history_window
#define REGTRACE_T23_AWB_HLIL_STARTUP_EV regtrace_t23_awb_hlil_startup_ev
#define REGTRACE_T23_AWB_HLIL_STARTUP_CT regtrace_t23_awb_hlil_startup_ct
#define REGTRACE_T23_AWB_HLIL_OUTDOOR_EV regtrace_t23_awb_hlil_outdoor_ev
#define REGTRACE_T23_AWB_HLIL_INDOOR_EV regtrace_t23_awb_hlil_indoor_ev
#define REGTRACE_T23_AWB_HLIL_LIGHT_SOURCE_COUNT \
    regtrace_t23_awb_hlil_light_source_count

static uint32_t regtrace_t23_awb_hlil_mf_parameters[6];
static uint32_t regtrace_t23_awb_hlil_wb_static[2];
static uint32_t regtrace_t23_awb_hlil_light_sources[20];
static uint32_t regtrace_t23_awb_hlil_rg_positions[15];
static uint32_t regtrace_t23_awb_hlil_bg_positions[15];
static uint32_t regtrace_t23_awb_hlil_distance_parameters[3];
static uint32_t regtrace_t23_awb_hlil_indoor_ct_weight_mesh[225];
static uint32_t regtrace_t23_awb_hlil_color_temperature_mesh[225];
static uint32_t regtrace_t23_awb_hlil_zone_weight_mesh[225];
static uint32_t regtrace_t23_awb_hlil_outdoor_ct_weight_mesh[225];
static uint32_t regtrace_t23_awb_hlil_light_source_weight_lut[514];

/*
 * The recovered runtime consumes mf_parameters[4:5], while the first two
 * words are zero in stock T23 banks and unused by this path.  A custom bank
 * may opt into a sensor-agnostic output calibration by storing "PYAW" in
 * word 0 and packed Q10 R/B multipliers in word 1.  Stock and older custom
 * banks retain exact unity behavior.
 */
static int regtrace_t23_awb_profile_bias_decode(const uint32_t *parameters)
{
    uint32_t rbias = REGTRACE_T23_AWB_PROFILE_BIAS_UNITY;
    uint32_t bbias = REGTRACE_T23_AWB_PROFILE_BIAS_UNITY;

    if (parameters[0] == REGTRACE_T23_AWB_PROFILE_BIAS_MAGIC) {
        rbias = parameters[1] & 0xffffU;
        bbias = parameters[1] >> 16;
        if (rbias < REGTRACE_T23_AWB_PROFILE_BIAS_MIN ||
            rbias > REGTRACE_T23_AWB_PROFILE_BIAS_MAX ||
            bbias < REGTRACE_T23_AWB_PROFILE_BIAS_MIN ||
            bbias > REGTRACE_T23_AWB_PROFILE_BIAS_MAX)
            return -ERANGE;
    }

    ACCESS_ONCE(regtrace_t23_source_awb_profile_rbias) = rbias;
    ACCESS_ONCE(regtrace_t23_source_awb_profile_bbias) = bbias;
    return 0;
}

static int regtrace_t23_source_awb_profile_bias_load(void)
{
    uint32_t parameters[2];
    int ret;

    ret = regtrace_t23_read_tuning_data(REGTRACE_T23_AWB_MF_OFFSET,
                                        parameters, sizeof(parameters));
    if (ret)
        return ret;
    return regtrace_t23_awb_profile_bias_decode(parameters);
}
