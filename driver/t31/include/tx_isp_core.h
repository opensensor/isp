#ifndef __TX_ISP_CORE_H__
#define __TX_ISP_CORE_H__

/* Core Functions */
int tx_isp_core_remove(struct platform_device *pdev);
int tx_isp_core_probe(struct platform_device *pdev);

/* Core Operations */
int tx_isp_core_start(struct tx_isp_subdev *sd);
int tx_isp_core_stop(struct tx_isp_subdev *sd);
int tx_isp_core_set_format(struct tx_isp_subdev *sd, struct tx_isp_config *config);

/* Tiziano ISP Core Functions */
#define TISP_SENSOR_INFO_SIZE 0x60

struct tisp_sensor_info_blob {
	u32 words[TISP_SENSOR_INFO_SIZE / sizeof(u32)];
};

enum tisp_sensor_info_word {
	TISP_SI_WORD_WIDTH = 0,
	TISP_SI_WORD_HEIGHT = 1,
	TISP_SI_WORD_BAYER = 2,
	TISP_SI_WORD_FPS = 3,
	TISP_SI_WORD_INTEGRATION_TIME = 6,
	TISP_SI_WORD_AGAIN = 7,
	TISP_SI_WORD_DGAIN = 8,
	TISP_SI_WORD_MAX_AGAIN = 9,
	TISP_SI_WORD_MAX_DGAIN = 0xa,
	TISP_SI_WORD_LINE_TIME = 0xb,
	TISP_SI_WORD_MIN_IT = 0xc,
	TISP_SI_WORD_IT_LIMITS = 0xd,
	TISP_SI_WORD_TOTAL_SIZE = 0xf,
	TISP_SI_WORD_MAX_IT = 0x10,
	TISP_SI_WORD_SHORT_IT = 0x11,
	TISP_SI_WORD_SHORT_MISC = 0x12,
	TISP_SI_WORD_MAX_IT_SHORT = 0x13,
	TISP_SI_WORD_MAX_AGAIN_LIMIT = 0x15,
	TISP_SI_WORD_MODE = 0x16,
	TISP_SI_WORD_FLAGS = 0x17,
};

static inline u32 tisp_si_word(const struct tisp_sensor_info_blob *info, unsigned int idx)
{
	return info->words[idx];
}

static inline void tisp_si_set_word(struct tisp_sensor_info_blob *info,
					    unsigned int idx, u32 value)
{
	info->words[idx] = value;
}

static inline u32 tisp_si_pack_u16(u32 lo, u32 hi)
{
	return (lo & 0xffff) | ((hi & 0xffff) << 16);
}

static inline u32 tisp_si_width(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_WIDTH); }
static inline u32 tisp_si_height(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_HEIGHT); }
static inline u32 tisp_si_bayer(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_BAYER); }
static inline u32 tisp_si_fps(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_FPS); }
static inline u32 tisp_si_again(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_AGAIN); }
static inline u32 tisp_si_dgain(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_DGAIN); }
static inline u32 tisp_si_mode(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_MODE); }
static inline u32 tisp_si_flags(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_FLAGS); }
static inline u32 tisp_si_raw_integration_time(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_INTEGRATION_TIME); }
static inline u32 tisp_si_line_time_token(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_LINE_TIME); }
static inline u32 tisp_si_line_time_num(const struct tisp_sensor_info_blob *info) { return tisp_si_line_time_token(info) & 0xffff; }
static inline u32 tisp_si_line_time_us(const struct tisp_sensor_info_blob *info) { return tisp_si_line_time_token(info) >> 16; }
static inline u32 tisp_si_min_integration_time(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_MIN_IT) & 0xffff; }
static inline u32 tisp_si_integration_time_limit(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_IT_LIMITS) & 0xffff; }
static inline u32 tisp_si_max_integration_time_native(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_IT_LIMITS) >> 16; }
static inline u32 tisp_si_total_width(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_TOTAL_SIZE) & 0xffff; }
static inline u32 tisp_si_total_height(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_TOTAL_SIZE) >> 16; }
static inline u32 tisp_si_max_integration_time(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_MAX_IT) & 0xffff; }
static inline u32 tisp_si_integration_time_short(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_SHORT_IT) & 0xffff; }
static inline u32 tisp_si_again_short(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_SHORT_MISC) & 0xffff; }
static inline u32 tisp_si_min_integration_time_short(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_SHORT_MISC) >> 16; }
static inline u32 tisp_si_max_integration_time_short(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_MAX_IT_SHORT) & 0xffff; }
static inline u32 tisp_si_max_again_limit(const struct tisp_sensor_info_blob *info) { return tisp_si_word(info, TISP_SI_WORD_MAX_AGAIN_LIMIT); }

int tiziano_isp_init(struct tx_isp_sensor_attribute *sensor_attr, char *param_name);
int tiziano_sync_sensor_attr(const struct tisp_sensor_info_blob *attr);
int tiziano_channel_start(int channel_id, struct tx_isp_channel_attr *attr);
int tisp_channel_start(int channel_id, struct tx_isp_channel_attr *attr);
int tisp_channel_attr_set(uint32_t channel_id, void *attr);
int tisp_sensor_info_update(const struct tisp_sensor_info_blob *info);

/* Event Handling Functions */
int tx_isp_handle_sync_sensor_attr_event(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr);
int ispcore_sync_sensor_attr(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr);

/* Hardware Reset Functions */
u32 tx_isp_check_reset_status(void);

/* ISP Device Management */
struct tx_isp_dev *tx_isp_get_device(void);
void tx_isp_set_device(struct tx_isp_dev *isp);
int tx_isp_core_prepare_prestream(struct tx_isp_dev *isp, const char *origin);

int tx_isp_create_graph_and_nodes(struct tx_isp_dev *isp);
void tx_isp_core_bind_event_dispatch_tables(struct tx_isp_dev *isp_dev);

/* Memory Management Functions */
int isp_malloc_buffer(struct tx_isp_dev *isp, uint32_t size, void **virt_addr, dma_addr_t *phys_addr);

/* Frame Synchronization Functions - CRITICAL for frame data transfer */
void isp_frame_done_wakeup(void);
int isp_frame_done_wait(int timeout_ms, uint64_t *frame_count);


/* Core States */
#define CORE_STATE_OFF       0
#define CORE_STATE_IDLE     1
#define CORE_STATE_ACTIVE   2
#define CORE_STATE_ERROR    3

/* Core Error Flags */
#define CORE_ERR_CONFIG     BIT(0)
#define CORE_ERR_TIMEOUT    BIT(1)
#define CORE_ERR_OVERFLOW   BIT(2)
#define CORE_ERR_HARDWARE   BIT(3)

#endif /* __TX_ISP_CORE_H__ */
