#ifndef TX_ISP_TUNING_ABI_H
#define TX_ISP_TUNING_ABI_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#endif

/*
 * Proprietary libimp control IDs shared by the legacy T23/T31 generation.
 * T41 moved the startup frame-rate and running-mode controls, so those IDs
 * remain explicitly generation-qualified below.
 */
#define TX_ISP_TUNING_CMD_WB			0x08000004U
#define TX_ISP_TUNING_CMD_WB_STATS		0x08000005U
#define TX_ISP_TUNING_CMD_WB_GLOBAL_STATS	0x08000009U
#define TX_ISP_TUNING_CMD_AE_COMP		0x08000023U
#define TX_ISP_TUNING_CMD_AE_ROI		0x08000024U
#define TX_ISP_TUNING_CMD_EXPR			0x08000025U
#define TX_ISP_TUNING_CMD_EV_ATTR		0x08000026U
#define TX_ISP_TUNING_CMD_TOTAL_GAIN		0x08000027U
#define TX_ISP_TUNING_CMD_MAX_AGAIN		0x08000028U
#define TX_ISP_TUNING_CMD_MAX_DGAIN		0x08000029U
#define TX_ISP_TUNING_CMD_HIGHLIGHT		0x0800002aU
#define TX_ISP_TUNING_CMD_MOVE_STATE		0x0800002cU
#define TX_ISP_TUNING_CMD_AE_STATS		0x0800002dU
#define TX_ISP_TUNING_CMD_AE_ZONE		0x08000030U
#define TX_ISP_TUNING_CMD_AF_ZONE		0x08000031U
#define TX_ISP_TUNING_CMD_AE_LUMA		0x08000033U
#define TX_ISP_TUNING_CMD_AE_MANUAL		0x08000035U
#define TX_ISP_TUNING_CMD_BACKLIGHT		0x08000037U
#define TX_ISP_TUNING_CMD_DEFOG			0x08000039U
#define TX_ISP_TUNING_CMD_DPC			0x08000062U
#define TX_ISP_TUNING_CMD_TEMPER		0x08000085U
#define TX_ISP_TUNING_CMD_SINTER		0x08000086U
#define TX_ISP_TUNING_CMD_DRC			0x080000a2U
#define TX_ISP_TUNING_CMD_T31_SENSOR_FPS	0x080000e0U
#define TX_ISP_TUNING_CMD_T31_RUNNING_MODE	0x080000e1U
#define TX_ISP_TUNING_CMD_BCSH_HUE		0x08000101U

#define TX_ISP_TUNING_CMD_T41_SENSOR_FPS	0x08000070U
#define TX_ISP_TUNING_CMD_T41_RUNNING_MODE	0x08000071U
#define TX_ISP_TUNING_CMD_T41_AWB_ATTR		0x08000010U
#define TX_ISP_TUNING_CMD_T41_AWB_STATS		0x08000011U
#define TX_ISP_TUNING_CMD_T41_AWB_WEIGHT	0x08000012U
#define TX_ISP_TUNING_CMD_T41_AWB_GLOBAL_STATS	0x08000013U
#define TX_ISP_TUNING_CMD_T41_AE_WEIGHT		0x08000021U
#define TX_ISP_TUNING_CMD_T41_AE_STATS		0x08000022U
#define TX_ISP_TUNING_CMD_T41_AE_EXPR_INFO	0x08000023U
#define TX_ISP_TUNING_CMD_T41_BCSH_HUE		0x08000081U
#define TX_ISP_TUNING_CMD_T41_BRIGHTNESS	0x08000092U
#define TX_ISP_TUNING_CMD_T41_SHARPNESS		0x08000093U
#define TX_ISP_TUNING_CMD_T41_SATURATION	0x08000094U
#define TX_ISP_TUNING_CMD_T41_CONTRAST		0x08000095U
#define TX_ISP_TUNING_CMD_T41_AWB_RGB_COEFFT	0x08000098U

/* Open implementation extension.  Keep policy names in userspace: the
 * driver only owns the atomic auto/manual AWB transition and gain replay. */
#define TX_ISP_TUNING_CMD_OPEN_AWB_CONTROL	0x08ff0001U
#define TX_ISP_TUNING_CMD_OPEN_AE_TARGET	0x08ff0002U
#define TX_ISP_TUNING_CMD_OPEN_COLOR_MODEL	0x08ff0003U
#define TX_ISP_TUNING_CMD_OPEN_AWB_SCENE	0x08ff0004U
#define TX_ISP_TUNING_CMD_OPEN_AWB_TARGET	0x08ff0005U
#define TX_ISP_TUNING_CMD_OPEN_AWB_OWNER	0x08ff0006U
/* Explicit GET handshake; legacy kernels may silently acknowledge unknown IDs. */
#define TX_ISP_TUNING_AWB_OWNER_NATIVE	0x41574201U

#define TX_ISP_TUNING_AWB_MANUAL		0U
#define TX_ISP_TUNING_AWB_AUTO		1U

#define TX_ISP_TUNING_COLOR_MODEL_DAY		0U
#define TX_ISP_TUNING_COLOR_MODEL_LOW_LIGHT	1U
#define TX_ISP_TUNING_COLOR_MODEL_BRIGHT_DAY	2U

struct tx_isp_tuning_awb_scene {
	u32 raw_r_q10;
	u32 raw_b_q10;
};

/* Calibrated neutral-mesh estimate, already including sensor RGB bias.
 * ENODATA means hold the previous gains, never revert to whole-scene gray
 * world. Separate from AWB_SCENE, whose raw-ratio ABI remains unchanged. */
struct tx_isp_tuning_awb_target {
	u32 r_gain_q10;
	u32 b_gain_q10;
};

#define TX_ISP_TUNING_DIR_GET			0x01U
#define TX_ISP_TUNING_DIR_SET			0x02U

enum tx_isp_tuning_payload_kind {
	TX_ISP_TUNING_PAYLOAD_INLINE = 0,
	TX_ISP_TUNING_PAYLOAD_USER_PTR = 1,
};

/*
 * payload_size is the fixed wire size.  Zero is permitted for an opaque or
 * variable-size payload whose owner validates the size itself.
 */
struct tx_isp_tuning_cmd_desc {
	u32 id;
	u16 payload_size;
	u8 directions;
	u8 payload_kind;
};

/* Eight-byte G_CTRL/S_CTRL envelope used by T23 and T31. */
struct tx_isp_tuning_control {
	u32 id;
	u32 value_or_ptr;
};

/* Twelve-byte extended-control envelope used by T31. */
struct tx_isp_tuning_ext_control {
	u32 direction;
	u32 id;
	u32 value_or_ptr;
};

/* Sixteen-byte extended-control envelope used by T23. */
struct tx_isp_tuning_t23_ext_control {
	u32 count;
	u32 id;
	u32 value_or_ptr;
	u32 sensor;
};

/* Sixteen-byte startup-control envelope used by T41. */
struct tx_isp_tuning_t41_control {
	u32 channel;
	u32 is_get;
	u32 id;
	u32 value_or_ptr;
};

/* Eight-byte payload for TX_ISP_TUNING_CMD_OPEN_AWB_CONTROL.  Manual gains
 * use the ISP's Q10 convention (0x400 is unity).  For AUTO, they seed the
 * bounded controller so a live profile change has no color discontinuity. */
struct tx_isp_tuning_awb_control {
	u32 mode;
	u16 r_gain;
	u16 b_gain;
};

/* Exact libimp response layouts. */
struct tx_isp_tuning_expr {
	u32 mode;
	u16 integration_time;
	u16 integration_time_min;
	u16 integration_time_max;
	u16 one_line_expr_in_us;
};

struct tx_isp_tuning_ev_attr {
	u32 ev;
	u32 expr_us;
	u32 ev_log2;
	u32 again;
	u32 dgain;
	u32 gain_log2;
};

struct tx_isp_tuning_wb {
	u32 mode;
	u16 r_gain;
	u16 b_gain;
};

/* T40/T41 1.2 libimp packed response layouts. */
#define TX_ISP_TUNING_T41_AE_EXPR_BYTES		232U
#define TX_ISP_TUNING_T41_AE_EXPR_INTEGRATION	24U
#define TX_ISP_TUNING_T41_AE_EXPR_AGAIN		28U
#define TX_ISP_TUNING_T41_AE_EXPR_TOTAL_GAIN	204U
#define TX_ISP_TUNING_T41_AE_EXPR_EXPOSURE	216U
#define TX_ISP_TUNING_T41_AE_EXPR_EV_LOG2	224U

#define TX_ISP_TUNING_T41_AE_STATS_BYTES	1934U
#define TX_ISP_TUNING_T41_AE_STATS_HIST5		0U
#define TX_ISP_TUNING_T41_AE_STATS_HIST256	10U
#define TX_ISP_TUNING_T41_AE_STATS_ZONES	1034U
#define TX_ISP_TUNING_T41_AE_HIST_BINS		256U
#define TX_ISP_TUNING_T41_AE_ZONE_COUNT		225U

struct tx_isp_tuning_t41_ae_expr_values {
	u32 integration_time;
	u32 analog_gain_x1024;
	u32 min_integration_time;
	u32 max_integration_time;
	u32 max_analog_gain_x1024;
	u32 total_gain_db;
	u64 exposure_value;
	u32 ev_log2;
};

#define TX_ISP_TUNING_EV_SPARSE_BYTES		0x80U
#define TX_ISP_TUNING_EV_SPARSE_WORDS		0x20U
#define TX_ISP_TUNING_EV_EXPR_MIN_OFFSET	0x6cU
#define TX_ISP_TUNING_EV_EXPR_MAX_OFFSET	0x6eU
#define TX_ISP_TUNING_EV_LINE_US_OFFSET		0x7cU

const struct tx_isp_tuning_cmd_desc *
tx_isp_tuning_cmd_find(const struct tx_isp_tuning_cmd_desc *table,
		       unsigned int count, u32 id, u8 direction);

int tx_isp_tuning_cmd_table_validate(
	const struct tx_isp_tuning_cmd_desc *table, unsigned int count);

int tx_isp_tuning_expr_pack(struct tx_isp_tuning_expr *out, u32 mode,
			    u32 integration_time, u32 integration_time_min,
			    u32 integration_time_max, u32 one_line_expr_in_us);

int tx_isp_tuning_expr_from_sparse(struct tx_isp_tuning_expr *out,
				   const void *sparse,
				   unsigned int sparse_bytes);

int tx_isp_tuning_ev_pack(struct tx_isp_tuning_ev_attr *out, u32 ev,
			  u32 expr_us, u32 ev_log2, u32 again, u32 dgain,
			  u32 gain_log2);

int tx_isp_tuning_wb_pack(struct tx_isp_tuning_wb *out,
			  const u32 *words, u32 mode_count);

u32 tx_isp_tuning_wb_stats_pack(u32 r_gain, u32 b_gain);

int tx_isp_tuning_t41_ae_expr_pack(
	void *out, unsigned int out_bytes,
	const struct tx_isp_tuning_t41_ae_expr_values *values);

int tx_isp_tuning_t41_ae_stats_pack(void *out, unsigned int out_bytes,
				    const u32 *histogram,
				    unsigned int histogram_bins,
				    u32 *mean_q8);

#endif /* TX_ISP_TUNING_ABI_H */
