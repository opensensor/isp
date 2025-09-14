#ifndef __TX_ISP_CSI_H__
#define __TX_ISP_CSI_H__

/* CSI Functions */
int tx_isp_csi_probe(struct platform_device *pdev);
int tx_isp_csi_remove(struct platform_device *pdev);

/* CSI Operations */
int tx_isp_csi_start(struct tx_isp_subdev *sd);
int tx_isp_csi_stop(struct tx_isp_subdev *sd);
int tx_isp_csi_set_format(struct tx_isp_subdev *sd, struct tx_isp_config *config);

/* CSI Core Operations - Match existing SDK implementations */
int csi_core_ops_init(struct tx_isp_subdev *sd, int enable);
int csi_set_on_lanes(struct tx_isp_csi_device *csi_dev, int lanes);

/* CSI Debug and Utility Functions */
void dump_csi_reg(struct tx_isp_subdev *sd);
void check_csi_error(struct tx_isp_subdev *sd);
int tx_isp_csi_activate_subdev(struct tx_isp_subdev *sd);
int tx_isp_csi_slake_subdev(struct tx_isp_subdev *sd);

/* CSI Video Operations - Remove conflicting declaration */
/* csi_video_s_stream is static in tx-isp-module.c */

/* CSI States */
#define CSI_STATE_OFF       0
#define CSI_STATE_IDLE     1
#define CSI_STATE_ACTIVE   2
#define CSI_STATE_ERROR    3

/* CSI Configuration */
#define CSI_MAX_LANES      4
#define CSI_MIN_LANES      1

/* CSI Error Flags */
#define CSI_ERR_SYNC       BIT(0)
#define CSI_ERR_ECC        BIT(1)
#define CSI_ERR_CRC        BIT(2)
#define CSI_ERR_OVERFLOW   BIT(3)

#endif /* __TX_ISP_CSI_H__ */
