#ifndef __TX_ISP_VIN_H__
#define __TX_ISP_VIN_H__

/* VIN Functions */
int tx_isp_vin_probe(struct platform_device *pdev);
int tx_isp_vin_remove(struct platform_device *pdev);

/* VIN Operations */
int tx_isp_vin_start(struct tx_isp_subdev *sd);
int tx_isp_vin_stop(struct tx_isp_subdev *sd);
int tx_isp_vin_set_format(struct tx_isp_subdev *sd, struct tx_isp_config *config);

/* VIN States */
#define VIN_STATE_OFF       0
#define VIN_STATE_IDLE     1
#define VIN_STATE_ACTIVE   2
#define VIN_STATE_ERROR    3

/* VIN Configuration */
#define VIN_MAX_WIDTH      2592
#define VIN_MAX_HEIGHT     1944
#define VIN_MIN_WIDTH      64
#define VIN_MIN_HEIGHT     64

/* VIN Error Flags */
#define VIN_ERR_OVERFLOW   BIT(0)
#define VIN_ERR_SYNC      BIT(1)
#define VIN_ERR_FORMAT    BIT(2)
#define VIN_ERR_SIZE      BIT(3)

/* VIN Format Flags */
#define VIN_FMT_YUV422    BIT(0)
#define VIN_FMT_RGB888    BIT(1)
#define VIN_FMT_RAW8      BIT(2)
#define VIN_FMT_RAW10     BIT(3)
#define VIN_FMT_RAW12     BIT(4)

#endif /* __TX_ISP_VIN_H__ */
