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
int tiziano_isp_init(struct tx_isp_sensor_attribute *sensor_attr, char *param_name);
int tiziano_sync_sensor_attr(struct tx_isp_sensor_attribute *attr);
int tiziano_channel_start(int channel_id, struct tx_isp_channel_attr *attr);

/* Event Handling Functions */
int tx_isp_handle_sync_sensor_attr_event(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr);
int ispcore_sync_sensor_attr(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr);

/* Hardware Reset Functions */
int tx_isp_hardware_reset(int reset_mode);
u32 tx_isp_check_reset_status(void);

/* ISP Device Management */
struct tx_isp_dev *tx_isp_get_device(void);
void tx_isp_set_device(struct tx_isp_dev *isp);

int tx_isp_create_graph_and_nodes(struct tx_isp_dev *isp);

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
