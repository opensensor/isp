#ifndef __TX_ISP_CSI_H__
#define __TX_ISP_CSI_H__

/* ============================================================================
 * CSI Register Definitions - from kernel source tx-isp-csi.h
 * ============================================================================ */

/* CSI Host Registers */
#define CSI_VERSION         0x00
#define CSI_N_LANES         0x04
#define CSI_PHY_SHUTDOWNZ   0x08
#define CSI_DPHY_RSTZ       0x0C
#define CSI_CSI2_RESETN     0x10
#define CSI_PHY_STATE       0x14
#define CSI_DATA_IDS_1      0x18
#define CSI_DATA_IDS_2      0x1C
#define CSI_ERR1            0x20
#define CSI_ERR2            0x24
#define CSI_MASK1           0x28
#define CSI_MASK2           0x2C
#define CSI_PHY_TST_CTRL0   0x30
#define CSI_PHY_TST_CTRL1   0x34

/* PHY Registers (T30/T31) */
#define PHY_CRTL0                0x000
#define PHY_CRTL1                0x080
#define CK_LANE_SETTLE_TIME      0x100
#define CK_LANE_CONFIG           0x128
#define PHY_DT0_LANE_SETTLE_TIME 0x180
#define PHY_DT1_LANE_SETTLE_TIME 0x200
#define PHY_MODEL_SWITCH         0x2CC
#define PHY_LVDS_MODE            0x300
#define PHY_FORCE_MODE           0x34

/* CSI Driver Name */
#define TX_ISP_CSI_NAME "tx-isp-csi"

/* ============================================================================
 * CSI Function Declarations
 * ============================================================================ */

/* Platform driver functions */
int tx_isp_csi_probe(struct platform_device *pdev);
int tx_isp_csi_remove(struct platform_device *pdev);

/* Module state functions */
int tx_isp_csi_activate_subdev(struct tx_isp_subdev *sd);
int tx_isp_csi_slake_subdev(struct tx_isp_subdev *sd);

/* Debug functions */
void dump_csi_reg(struct tx_isp_csi_device *csd);

/* Platform driver structure */
extern struct platform_driver tx_isp_csi_driver;

#endif /* __TX_ISP_CSI_H__ */
