#ifndef TX_ISP_TUNING_ABI_H
#define TX_ISP_TUNING_ABI_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#endif

/*
 * Proprietary libimp control IDs shared by the legacy T23/T31 generation.
 * T41 moved the startup frame-rate and running-mode controls, so those IDs
 * remain explicitly generation-qualified below.
 */
#define TX_ISP_TUNING_CMD_WB			0x08000004U
#define TX_ISP_TUNING_CMD_WB_STATS		0x08000005U
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

#endif /* TX_ISP_TUNING_ABI_H */
