#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx_isp_vic.h"
#include "../include/tx-isp-device.h"

/* Forward declarations */
int csi_core_ops_init(struct tx_isp_subdev *sd, int enable);
int csi_set_on_lanes(struct tx_isp_csi_device *csi_dev, int lanes);
void dump_csi_reg(struct tx_isp_subdev *sd);
int tx_isp_csi_slake_subdev(struct tx_isp_subdev *sd);
int csi_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg);
int csi_sensor_ops_sync_sensor_attr(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr);
struct tx_isp_sensor *tx_isp_get_sensor(void);
extern struct tx_isp_dev *ourISPdev;
extern bool is_valid_kernel_pointer(const void *ptr);
void system_reg_write(u32 reg, u32 value);

/* Binary Ninja reference global variables */
static struct tx_isp_csi_device *dump_csd = NULL;  /* Global CSI device pointer */


/* Manual register mappings removed - use subdev-managed registers per reference driver */

/* isp_read32 removed - use system_reg_read from reference driver instead */

void isp_write32(u32 reg, u32 val)
{
    /* Use system_reg_write from reference driver instead of manual mapping */
    system_reg_write(reg, val);
}

static void __iomem *tx_isp_vic_regs = NULL;


void csi_write32(u32 reg, u32 val)
{
    /* Use CSI device registers */
    if (ourISPdev && ourISPdev->csi_dev && ourISPdev->csi_dev->csi_regs) {
        void __iomem *base = ourISPdev->csi_dev->csi_regs;
        bool streaming = false;
        if (ourISPdev->vic_dev) {
            int st = ourISPdev->vic_dev->state;
            streaming = (st >= 3); /* INITIALIZED or STREAMING */
        }

        /* Protect CSI PHY/CTRL from being turned off while streaming */
        if (streaming && (reg == 0x10 || reg == 0x0c || reg == 0x1e8)) {
            u32 prev = readl(base + reg);
            if (val == 0) {
                pr_warn("*** CSI WRITE BLOCKED during streaming: reg[0x%03x] prev=0x%08x -> val=0x%08x (blocked) ***\n", reg, prev, val);
                return; /* Block disabling writes while streaming */
            }
            /* Otherwise allow, but log */
            pr_info("*** CSI WRITE during streaming: reg[0x%03x] prev=0x%08x -> val=0x%08x ***\n", reg, prev, val);
        }

        writel(val, base + reg);
    } else {
        pr_err("csi_write32: No CSI registers available\n");
    }
}

/* CSI error checking function - called from VIC interrupt handler */
void tx_isp_csi_check_errors(struct tx_isp_dev *isp_dev)
{
    struct tx_isp_csi_device *csi_dev;
    void __iomem *csi_base;
    u32 err1, err2, phy_state;
    static unsigned long last_check_time = 0;
    unsigned long current_time = jiffies;

    if (!isp_dev) {
        return;
    }

    csi_dev = isp_dev->csi_dev;
    if (!csi_dev) {
        return;
    }

    csi_base = csi_dev->csi_regs;
    if (!csi_base) {
        return;
    }

    /* Throttle error checking to avoid spam */
    if (time_before(current_time, last_check_time + msecs_to_jiffies(100))) {
        return;
    }
    last_check_time = current_time;

    unsigned long flags;

    spin_lock_irqsave(&csi_dev->lock, flags);

    /* Read CSI error registers */
    err1 = readl(csi_base + 0x20);  /* ERR1 register */
    err2 = readl(csi_base + 0x24);  /* ERR2 register */
    phy_state = readl(csi_base + 0x14);  /* PHY_STATE register */

    /* Only log if there are actual errors */
    if (err1 || err2) {
        pr_info("*** CSI ERROR CHECK: err1=0x%08x, err2=0x%08x, phy_state=0x%08x ***\n",
                err1, err2, phy_state);

        /* Handle protocol errors (ERR1) */
        if (err1) {
            pr_err("CSI Protocol errors (ERR1): 0x%08x\n", err1);

            if (err1 & 0x1) pr_err("  - SOT Sync Error\n");
            if (err1 & 0x2) pr_err("  - SOTHS Sync Error\n");
            if (err1 & 0x4) pr_err("  - ECC Single-bit Error (corrected)\n");
            if (err1 & 0x8) pr_err("  - ECC Multi-bit Error (uncorrectable)\n");
            if (err1 & 0x10) pr_err("  - CRC Error\n");
            if (err1 & 0x20) pr_err("  - Packet Size Error\n");
            if (err1 & 0x40) pr_err("  - EoTp Error\n");

            /* Clear errors by writing back the status */
            writel(err1, csi_base + 0x20);
            wmb();
        }

        /* Handle application errors (ERR2) */
        if (err2) {
            pr_err("CSI Application errors (ERR2): 0x%08x\n", err2);

            if (err2 & 0x1) pr_err("  - Data ID Error\n");
            if (err2 & 0x2) pr_err("  - Frame Sync Error\n");
            if (err2 & 0x4) pr_err("  - Frame Data Error\n");
            if (err2 & 0x8) pr_err("  - Frame Sequence Error\n");

            /* Clear errors by writing back the status */
            writel(err2, csi_base + 0x24);
            wmb();
        }

        /* Update error state */
        if ((err1 & 0x38) || (err2 & 0xE)) { /* Serious errors */
            pr_err("CSI: Serious errors detected, may need recovery\n");
        }
    }

    spin_unlock_irqrestore(&csi_dev->lock, flags);
}

/* Initialize CSI hardware */
static int tx_isp_csi_hw_init(struct tx_isp_subdev *sd)
{
    if (!sd)
        return -EINVAL;

    /* Reset CSI */
    csi_write32(CSI_CTRL, CSI_CTRL_RST);
    udelay(10);
    csi_write32(CSI_CTRL, 0);

    /* Clear and mask all interrupts initially */
    csi_write32(CSI_INT_STATUS, 0xFFFFFFFF);
    csi_write32(CSI_INT_MASK, 0xFFFFFFFF);

    return 0;
}

/* CSI video streaming control - FIXED to call csi_core_ops_init like kernel source! */
int csi_video_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_csi_device *csi_dev;
    int ret = 0;

    pr_info("*** csi_video_s_stream: FIXED to match kernel source ***\n");
    pr_info("csi_video_s_stream: sd=%p, enable=%d\n", sd, enable);

    if (sd == NULL || (unsigned long)sd >= 0xfffff001) {
        pr_err("csi_video_s_stream: Invalid subdev pointer\n");
        return -EINVAL;
    }

    /* Get CSI device from subdev private data */
    csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev) {
        pr_err("CSI device is NULL from subdev private data\n");
        return -EINVAL;
    }

    pr_info("csi_video_s_stream: interface_type=%d (1=MIPI, 2=DVP)\n", csi_dev->interface_type);

    /*
     * CRITICAL FIX: The kernel source (tx-isp-csi.c line 220-228) shows:
     *
     *   if(csd->vin.attr->dbus_type == TX_SENSOR_DATA_INTERFACE_MIPI){
     *       if (enable) {
     *           ret = csi_phy_start(csd);  // Just sets state
     *       } else {
     *           ret = csi_phy_stop(csd);
     *       }
     *   }
     *
     * BUT the actual PHY initialization happens in csi_core_ops_init()
     * which is registered as the .init callback and gets called during
     * the module initialization sequence.
     *
     * Our driver was NEVER calling csi_core_ops_init with enable=1!
     * This explains why PHY registers were never written.
     */

    if (csi_dev->interface_type == 1) {  /* MIPI */
        if (enable) {
            /* CRITICAL: Call csi_core_ops_init to initialize PHY! */
            pr_info("*** csi_video_s_stream: Calling csi_core_ops_init(1) for MIPI PHY init ***\n");
            ret = csi_core_ops_init(sd, 1);
            if (ret) {
                pr_err("csi_core_ops_init(1) failed: %d\n", ret);
                return ret;
            }
            csi_dev->state = 4;  /* TX_ISP_MODULE_RUNNING */
            pr_info("*** csi_video_s_stream: CSI PHY initialized, state=4 (RUNNING) ***\n");
        } else {
            csi_dev->state = 3;  /* TX_ISP_MODULE_INIT */
            pr_info("csi_video_s_stream: CSI state set to 3 (stream off)\n");
        }
    } else if (csi_dev->interface_type == 2) {  /* DVP */
        if (enable) {
            pr_info("*** csi_video_s_stream: Calling csi_core_ops_init(1) for DVP init ***\n");
            ret = csi_core_ops_init(sd, 1);
            if (ret) {
                pr_err("csi_core_ops_init(1) failed: %d\n", ret);
                return ret;
            }
            csi_dev->state = 4;
        } else {
            csi_dev->state = 3;
        }
    } else {
        pr_info("csi_video_s_stream: Unknown interface type %d\n", csi_dev->interface_type);
    }

    return ret;
}

/* CSI sensor operations IOCTL handler - EXACT Binary Ninja reference implementation */
int csi_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    struct tx_isp_csi_device *csi_dev;

    /* Binary Ninja: if (arg1 != 0 && arg1 u< 0xfffff001) */
    if (sd != NULL && (unsigned long)sd < 0xfffff001) {

        /* Binary Ninja: *(arg1 + 0x110) - get CSI device from subdev private data */
        csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
        if (!csi_dev) {
            /* Binary Ninja: return 0 on error */
            return 0;
        }

        /* Binary Ninja: if (arg2 != 0x200000e) */
        if (cmd != 0x200000e) {
            /* Binary Ninja: if (arg2 != 0x200000f) */
            if (cmd != 0x200000f) {
                /* Binary Ninja: if (arg2 == 0x200000c) */
                if (cmd == 0x200000c) {
                    /* Binary Ninja: csi_core_ops_init(arg1, 1, 0x200000f) */
                    csi_core_ops_init(sd, 1);
                }
            } else {
                /* Binary Ninja: else if (*(*(arg1 + 0x110) + 0x14) == 1) */
                /* Check if CSI device interface type is 1 (MIPI) */
                /* 0x14 offset in CSI device structure is interface_type */
                if (csi_dev->interface_type == 1) {
                    /* Binary Ninja: *(arg1 + 0x128) = 4 */
                    /* CRITICAL FIX: Set state in CSI device, not subdev */
                    csi_dev->state = 4;  /* Set to streaming_on state */
                }
            }
        } else {
            /* Binary Ninja: else if (*(*(arg1 + 0x110) + 0x14) == 1) */
            /* Check if CSI device interface type is 1 (MIPI) */
            if (csi_dev->interface_type == 1) {
                /* Binary Ninja: *(arg1 + 0x128) = 3 */
                /* CRITICAL FIX: Set state in CSI device, not subdev */
                csi_dev->state = 3;  /* Set to streaming_off state */
            }
        }
    }

    /* Binary Ninja: return 0 */
    return 0;
}

/* CSI sensor attribute synchronization */
int csi_sensor_ops_sync_sensor_attr(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr)
{
    struct tx_isp_sensor *sensor;

    if (!sd || !attr)
        return -EINVAL;

    /* Find or create the sensor */
    if (!sd->active_sensor) {
        /* In a real implementation, we would create a new sensor here */
        pr_err("No active sensor to sync attributes with\n");
        return -EINVAL;
    }

    /* Store the attribute in the active sensor */
    sensor = sd->active_sensor;
    memcpy(&sensor->attr, attr, sizeof(struct tx_isp_sensor_attribute));

    return 0;
}

/* csi_core_ops_init - EXACT Binary Ninja reference implementation */
int csi_core_ops_init(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_csi_device *csi_dev;
    void __iomem *csi_regs;
    void __iomem *isp_csi_regs;
    struct tx_isp_sensor_attribute *sensor_attr;
    int result = 0xffffffea; /* Binary Ninja: int32_t result = 0xffffffea */

    /* Binary Ninja: if (arg1 != 0) */
    if (sd != NULL) {
        /* Binary Ninja: if (arg1 u>= 0xfffff001) return 0xffffffea */
        if ((unsigned long)sd >= 0xfffff001) {
            return 0xffffffea;
        }

        /* Binary Ninja: void* $s0_1 = *(arg1 + 0xd4) */
        /* SAFE: Use proper struct member access instead of dangerous offset */
        csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);

        /* CRITICAL SAFETY CHECK: Prevent BadVA crash */
        if (!csi_dev) {
            pr_err("csi_core_ops_init: CRITICAL ERROR - sd->dev_priv is NULL! sd=%p\n", sd);
            pr_err("csi_core_ops_init: This means CSI device was not properly set during probe\n");
            return 0xffffffea;
        }

        pr_info("csi_core_ops_init: sd=%p, csi_dev=%p, enable=%d\n", sd, csi_dev, enable);
        pr_info("*** csi_core_ops_init: CSI state=%d (need >= 2 for init) ***\n", csi_dev->state);
        result = 0xffffffea;

        /* Binary Ninja: if ($s0_1 != 0 && $s0_1 u< 0xfffff001) */
        if (csi_dev != NULL && (unsigned long)csi_dev < 0xfffff001) {
            result = 0;
            pr_info("*** csi_core_ops_init: CSI device valid, checking state ***\n");

            /* Binary Ninja: if (*($s0_1 + 0x128) s>= 2) */
            if (csi_dev->state >= 2) {
                pr_info("*** csi_core_ops_init: CSI state >= 2, proceeding with init ***\n");
                int v0_17;

                /* Binary Ninja: if (arg2 == 0) */
                if (enable == 0) {
                    /* Binary Ninja: isp_printf(0, "%s[%d] VIC do not support this format %d\n", arg3) */
                    isp_printf(0, "%s[%d] VIC do not support this format %d\n", enable);

                    /* Binary Ninja: void* $a0_21 = *($s0_1 + 0xb8) */
                    csi_regs = csi_dev->csi_regs;

                    /* Binary Ninja: *($a0_21 + 8) &= 0xfffffffe */
                    writel(readl(csi_regs + 8) & 0xfffffffe, csi_regs + 8);

                    /* Binary Ninja: void* $a0_22 = *($s0_1 + 0xb8) */
                    /* Binary Ninja: *($a0_22 + 0xc) &= 0xfffffffe */
                    writel(readl(csi_regs + 0xc) & 0xfffffffe, csi_regs + 0xc);

                    /* Binary Ninja: void* $a0_23 = *($s0_1 + 0xb8) */
                    /* Binary Ninja: *($a0_23 + 0x10) &= 0xfffffffe */
                    writel(readl(csi_regs + 0x10) & 0xfffffffe, csi_regs + 0x10);

                    v0_17 = 2;
                } else {
                    /* Binary Ninja: void* $v1_5 = *($s0_1 + 0x110) */
                    /* CRITICAL FIX: Get sensor attributes from connected sensor */
                    extern struct tx_isp_sensor *tx_isp_get_sensor(void);
                    struct tx_isp_sensor *sensor = tx_isp_get_sensor();
                    if (sensor && sensor->video.attr) {
                        sensor_attr = sensor->video.attr;
                    } else {
                        pr_err("csi_core_ops_init: CRITICAL ERROR - No sensor attributes available\n");
                        return -EINVAL;
                    }

                    /* Binary Ninja: int32_t $s2_1 = *($v1_5 + 0x14) */
                    int interface_type = sensor_attr->dbus_type;
                    pr_info("*** csi_core_ops_init: interface_type=%d (1=MIPI, 2=DVP) ***\n", interface_type);

                    /* CRITICAL FIX: Store interface type in CSI device for later use */
                    csi_dev->interface_type = interface_type;
                    pr_info("*** csi_core_ops_init: Set csi_dev->interface_type = %d ***\n", interface_type);

                    if (interface_type == 1) {
                        /* ============================================================
                         * MIPI CSI-2 Initialization - FROM KERNEL SOURCE tx-isp-csi.c
                         *
                         * CRITICAL: There are TWO register spaces:
                         *   1. csi_regs (sd.base) - CSI core registers
                         *   2. phy_regs (phy_base at 0x10022000) - MIPI PHY registers
                         *
                         * The kernel source uses:
                         *   csi_core_write() -> sd.base (CSI core)
                         *   csi_phy_write()  -> phy_base (PHY at 0x10022000)
                         * ============================================================ */
                        void __iomem *phy_regs = csi_dev->phy_regs;

                        pr_info("*** CSI MIPI INIT: Configuring MIPI CSI for %d lanes ***\n", sensor_attr->mipi.lans);
                        pr_info("*** CSI MIPI INIT: csi_regs=%p, phy_regs=%p ***\n", csi_dev->csi_regs, phy_regs);

                        if (!phy_regs) {
                            pr_err("*** CSI MIPI INIT: CRITICAL ERROR - phy_regs is NULL! ***\n");
                            pr_err("*** CSI MIPI INIT: PHY registers at 0x10022000 not mapped! ***\n");
                            return -EINVAL;
                        }

                        csi_regs = csi_dev->csi_regs;

                        /* Step 1: Set number of lanes (CSI core register) */
                        /* csi_set_on_lanes(csd, csd->vin.attr->mipi.lans) */
                        writel(sensor_attr->mipi.lans - 1, csi_regs + 0x04);  /* N_LANES register */
                        pr_info("*** CSI[0x04] = %d (N_LANES = lanes-1) ***\n", sensor_attr->mipi.lans - 1);

                        /* Step 2: PHY shutdown sequence (CSI core registers) */
                        /* csi_core_write_part(csd, PHY_SHUTDOWNZ, 0, 0, 1) */
                        writel(readl(csi_regs + 0x08) & ~0x01, csi_regs + 0x08);  /* PHY_SHUTDOWNZ = 0 */
                        mdelay(10);
                        writel(readl(csi_regs + 0x08) | 0x01, csi_regs + 0x08);   /* PHY_SHUTDOWNZ = 1 */

                        /* csi_core_write_part(csd, DPHY_RSTZ, 0, 0, 1) */
                        writel(readl(csi_regs + 0x0c) & ~0x01, csi_regs + 0x0c);  /* DPHY_RSTZ = 0 */
                        msleep(10);
                        writel(readl(csi_regs + 0x0c) | 0x01, csi_regs + 0x0c);   /* DPHY_RSTZ = 1 */

                        /* Step 3: PHY test control (CSI core registers) */
                        /* csi_core_write_part(csd, PHY_TST_CTRL0, 0, 0, 1) */
                        writel(readl(csi_regs + 0x14) & ~0x01, csi_regs + 0x14);  /* PHY_TST_CTRL0 bit 0 = 0 */
                        /* csi_core_write(csd, PHY_TST_CTRL1, 0) */
                        writel(0, csi_regs + 0x18);  /* PHY_TST_CTRL1 = 0 */
                        /* csi_core_write_part(csd, PHY_TST_CTRL1, 0, 16, 1) */
                        writel(readl(csi_regs + 0x18) & ~(1 << 16), csi_regs + 0x18);
                        /* csi_core_write_part(csd, PHY_TST_CTRL0, 0, 1, 1) */
                        writel(readl(csi_regs + 0x14) & ~(1 << 1), csi_regs + 0x14);

                        /* ============================================================
                         * Step 4: PHY INIT - WRITES TO phy_regs (0x10022000)
                         * THIS IS THE CRITICAL PART THAT WAS WRONG!
                         * ============================================================ */
                        pr_info("*** CSI MIPI: PHY INIT - Writing to PHY registers at %p ***\n", phy_regs);

                        /* csi_phy_write(csd, PHY_CRTL0, 0x7d) - offset 0x000 */
                        writel(0x7d, phy_regs + 0x000);
                        pr_info("*** PHY[0x000] = 0x7d (PHY_CRTL0) ***\n");

                        /* csi_phy_write(csd, CK_LANE_CONFIG, 0x3f) - offset 0x128 */
                        writel(0x3f, phy_regs + 0x128);
                        pr_info("*** PHY[0x128] = 0x3f (CK_LANE_CONFIG) - ALWAYS 0x3f! ***\n");

                        /* csi_phy_write(csd, PHY_CRTL1, 0x3f) - offset 0x080 */
                        writel(0x3f, phy_regs + 0x080);
                        pr_info("*** PHY[0x080] = 0x3f (PHY_CRTL1) - ALWAYS 0x3f! ***\n");

                        /* csi_phy_write(csd, PHY_LVDS_MODE, 0x1e) - offset 0x300 */
                        writel(0x1e, phy_regs + 0x300);
                        pr_info("*** PHY[0x300] = 0x1e (PHY_LVDS_MODE) ***\n");

                        /* csi_phy_write(csd, PHY_CRTL1, 0x1f) - offset 0x080 (second write) */
                        writel(0x1f, phy_regs + 0x080);
                        pr_info("*** PHY[0x080] = 0x1f (PHY_CRTL1 - second write) ***\n");

                        msleep(10);

                        /* Step 5: Sony MIPI mode settle time (if applicable) */
                        if (sensor_attr->mipi.mode == 1) {  /* SENSOR_MIPI_SONY_MODE */
                            pr_info("*** CSI MIPI: Sony mode - setting settle times to 0x86 ***\n");
                            writel(0x86, phy_regs + 0x100);  /* CK_LANE_SETTLE_TIME */
                            writel(0x86, phy_regs + 0x180);  /* PHY_DT0_LANE_SETTLE_TIME */
                            writel(0x86, phy_regs + 0x200);  /* PHY_DT1_LANE_SETTLE_TIME */
                            msleep(10);
                        }

                        /* Step 6: CSI2 reset sequence (CSI core registers) */
                        /* csi_core_write_part(csd, CSI2_RESETN, 0, 0, 1) */
                        writel(readl(csi_regs + 0x10) & ~0x01, csi_regs + 0x10);  /* CSI2_RESETN = 0 */
                        mdelay(10);
                        writel(readl(csi_regs + 0x10) | 0x01, csi_regs + 0x10);   /* CSI2_RESETN = 1 */
                        mdelay(10);

                        pr_info("*** CSI MIPI INIT COMPLETE - PHY enabled! ***\n");

                        v0_17 = 3;

                    } else if (interface_type != 2) {
                        isp_printf(1, "%s[%d] Unsupported interface type %d\n", __func__, __LINE__, interface_type);
                        v0_17 = 3;
                    } else {
                        /* ============================================================
                         * DVP Interface Configuration - FROM KERNEL SOURCE
                         * ============================================================ */
                        void __iomem *phy_regs = csi_dev->phy_regs;
                        csi_regs = csi_dev->csi_regs;

                        pr_info("*** CSI DVP INIT: Configuring DVP interface ***\n");

                        if (!phy_regs) {
                            pr_err("*** CSI DVP INIT: WARNING - phy_regs is NULL! ***\n");
                        }

                        /* csi_core_write(csd, DPHY_RSTZ, 0x0) */
                        writel(0, csi_regs + 0x0c);  /* DPHY_RSTZ = 0 */
                        /* csi_core_write(csd, DPHY_RSTZ, 0x1) */
                        writel(1, csi_regs + 0x0c);  /* DPHY_RSTZ = 1 */

                        if (phy_regs) {
                            /* csi_phy_write(csd, PHY_CRTL0, 0x7d) */
                            writel(0x7d, phy_regs + 0x000);
                            /* csi_phy_write(csd, PHY_CRTL1, 0x3e) - Note: 0x3e for DVP, not 0x3f */
                            writel(0x3e, phy_regs + 0x080);
                            /* csi_phy_write(csd, PHY_LVDS_MODE, 0x1e) */
                            writel(0x1e, phy_regs + 0x300);
                            /* csi_phy_write(csd, PHY_MODEL_SWITCH, 0x1) */
                            writel(0x01, phy_regs + 0x2cc);
                        }

                        pr_info("*** CSI DVP INIT COMPLETE ***\n");
                        v0_17 = 3;
                    }
                }

                /* Binary Ninja: *($s0_1 + 0x128) = $v0_17 */
                csi_dev->state = v0_17;
                pr_info("*** csi_core_ops_init: CSI init complete, new state=%d ***\n", v0_17);
                return 0;
            } else {
                pr_err("*** csi_core_ops_init: CSI state < 2 (state=%d), skipping init! ***\n", csi_dev->state);
            }
        } else {
            pr_err("*** csi_core_ops_init: CSI device invalid! ***\n");
        }
    } else {
        pr_err("*** csi_core_ops_init: sd is NULL! ***\n");
    }

    pr_err("*** csi_core_ops_init: FAILED, returning result=%d ***\n", result);
    return result;
}

/* csi_set_on_lanes - EXACT Binary Ninja implementation */
int csi_set_on_lanes(struct tx_isp_csi_device *csi_dev, int lanes)
{
    void __iomem *csi_regs;
    u32 reg_val;

    /* Binary Ninja: isp_printf(0, "%s:----------> lane num: %d\n", "csi_set_on_lanes", lane_num) */
    isp_printf(0, "%s:----------> lane num: %d\n", "csi_set_on_lanes", lanes);

    /* Binary Ninja: void* $v1 = *(arg1 + 0xb8) */
    csi_regs = csi_dev->csi_regs;

    /* Binary Ninja: *($v1 + 4) = ((zx.d(arg2) - 1) & 3) | (*($v1 + 4) & 0xfffffffc) */
    reg_val = readl(csi_regs + 4);
    reg_val = (reg_val & 0xfffffffc) | ((lanes - 1) & 3);
    writel(reg_val, csi_regs + 4);

    /* Binary Ninja: return 0 */
    return 0;
}

/* tx_isp_csi_slake_subdev - EXACT Binary Ninja reference implementation */
int tx_isp_csi_slake_subdev(struct tx_isp_subdev *sd)
{
    struct tx_isp_csi_device *csi_dev;
    int state;
    int i;

    /* Binary Ninja: if (arg1 == 0 || arg1 u>= 0xfffff001) return 0xffffffea */
    if (!sd || (unsigned long)sd >= 0xfffff001) {
        return -EINVAL;
    }

    /* Binary Ninja: void* $s0_1 = *(arg1 + 0xd4) */
    /* SAFE: Use proper function instead of offset-based access */
    csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev || (unsigned long)csi_dev >= 0xfffff001) {
        return -EINVAL;
    }

    pr_info("*** tx_isp_csi_slake_subdev: CSI slake/shutdown - current state=%d ***\n", csi_dev->state);

    /* Binary Ninja: int32_t $v1_2 = *($s0_1 + 0x128) */
    state = csi_dev->state;

    /* Binary Ninja: if ($v1_2 == 4) csi_video_s_stream(arg1, 0) */
    if (state == 4) {
        pr_info("tx_isp_csi_slake_subdev: CSI in streaming state, stopping stream\n");
        csi_video_s_stream(sd, 0);
        state = csi_dev->state;  /* Update state after s_stream */
    }

    /* Binary Ninja: void* $s2_1 = $s0_1 + 0x12c - Get mutex */
    /* Binary Ninja: if ($v1_2 == 3) csi_core_ops_init(arg1, 0) */
    if (csi_dev->state == 3) {
        pr_info("tx_isp_csi_slake_subdev: CSI in state 3, calling core_ops_init(disable)\n");
        csi_core_ops_init(sd, 0);
    }

    /* Binary Ninja: private_mutex_lock($s2_1) */
    mutex_lock(&csi_dev->mlock);

    /* Binary Ninja: if (*($s0_1 + 0x128) == 2) *($s0_1 + 0x128) = 1 */
    if (csi_dev->state == 2) {
        pr_info("tx_isp_csi_slake_subdev: CSI state 2->1, disabling clocks\n");
        csi_dev->state = 1;

        /* Binary Ninja: void* $v0 = *(arg1 + 0xbc) - Get clocks array */
        /* Binary Ninja: if ($v0 != 0 && $v0 u< 0xfffff001) - Clock disabling loop */
        if (sd->clks && sd->clk_num > 0) {
            /* Binary Ninja: int32_t $s0_2 = *(arg1 + 0xc0) - Get clock count */
            /* Binary Ninja: Clock disabling loop in reverse order */
            for (i = sd->clk_num - 1; i >= 0; i--) {
                if (sd->clks[i]) {
                    /* Binary Ninja: private_clk_disable(*$s0_4) */
                    clk_disable(sd->clks[i]);
                    pr_info("tx_isp_csi_slake_subdev: Disabled clock %d\n", i);
                }
            }
        }
    }

    mutex_unlock(&csi_dev->mlock);
    pr_info("*** tx_isp_csi_slake_subdev: CSI slake complete, final state=%d ***\n", csi_dev->state);
    return 0;
}

/* Duplicate function removed - using existing csi_sensor_ops_ioctl at line 406 */

/* Define the core operations */
struct tx_isp_subdev_core_ops csi_core_ops = {
    .init = csi_core_ops_init,
    .reset = NULL,
    .ioctl = NULL,
};

/* Define the video operations */
struct tx_isp_subdev_video_ops csi_video_ops = {
    .s_stream = csi_video_s_stream,
    .link_stream = csi_video_s_stream,  /* CRITICAL FIX: tx_isp_video_link_stream calls link_stream! */
};

/* CSI sensor ops sync_sensor_attr wrapper - calls csi_set_on_lanes */
static int csi_sensor_ops_sync_sensor_attr_wrapper(struct tx_isp_subdev *sd, void *arg)
{
    struct tx_isp_csi_device *csi_dev;
    struct tx_isp_sensor_attribute *sensor_attr = (struct tx_isp_sensor_attribute *)arg;

    if (!sd || !arg) {
        return -EINVAL;
    }

    csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev) {
        pr_err("csi_sensor_ops_sync_sensor_attr_wrapper: No CSI device\n");
        return -ENODEV;
    }

    /* Call csi_set_on_lanes if this is a MIPI interface */
    if (sensor_attr->dbus_type == TX_SENSOR_DATA_INTERFACE_MIPI) {
        int lanes = sensor_attr->mipi.lans;
        pr_info("*** csi_sensor_ops_sync_sensor_attr_wrapper: Calling csi_set_on_lanes with %d lanes ***\n", lanes);
        csi_set_on_lanes(csi_dev, lanes);
    }

    /* Also call the original sync function if it exists */
    return csi_sensor_ops_sync_sensor_attr(sd, arg);
}

/* Define the sensor operations */
struct tx_isp_subdev_sensor_ops csi_sensor_ops = {
    .sync_sensor_attr = csi_sensor_ops_sync_sensor_attr_wrapper,
    .ioctl = csi_sensor_ops_ioctl,
};

/* CSI internal operations - EXACT Binary Ninja implementation */
struct tx_isp_subdev_internal_ops csi_internal_ops = {
    .slake_module = tx_isp_csi_slake_subdev,
};

/* Initialize the subdev ops structure with pointers to the operations */
struct tx_isp_subdev_ops csi_subdev_ops = {
    .core = &csi_core_ops,
    .video = &csi_video_ops,
    .sensor = &csi_sensor_ops,
    .internal = &csi_internal_ops,
};
EXPORT_SYMBOL(csi_subdev_ops);
EXPORT_SYMBOL(tx_isp_csi_slake_subdev);

/* Forward declarations for CSI file operations */
static int dump_isp_csi_open(struct inode *inode, struct file *file);
static int isp_csi_show(struct seq_file *m, void *v);

/* dump_isp_csi_open - EXACT Binary Ninja implementation */
static int dump_isp_csi_open(struct inode *inode, struct file *file)
{
    /* Binary Ninja: return private_single_open_size(arg2, isp_csi_show, PDE_DATA(), 0x400) __tailcall */
    return single_open_size(file, isp_csi_show, PDE_DATA(inode), 0x400);
}

/* isp_csi_show - EXACT Binary Ninja implementation */
static int isp_csi_show(struct seq_file *m, void *v)
{
    void *csi_data = m->private;
    struct tx_isp_csi_device *csi_dev;
    void __iomem *csi_regs;
    u32 reg_20, reg_24;
    int result = 0;

    /* Binary Ninja: void* $v0 = *(arg1 + 0x3c) */
    if (csi_data != NULL && (unsigned long)csi_data < 0xfffff001) {
        /* Binary Ninja: void* $s1_1 = *($v0 + 0xd4) */
        csi_dev = (struct tx_isp_csi_device *)csi_data;

        if (csi_dev != NULL && (unsigned long)csi_dev < 0xfffff001) {
            /* Binary Ninja: void* $v0_2 = *($s1_1 + 0xb8) */
            csi_regs = csi_dev->csi_regs;

            /* Binary Ninja: int32_t $v1_1 = *($v0_2 + 0x20) */
            reg_20 = readl(csi_regs + 0x20);
            /* Binary Ninja: int32_t $v0_4 = *($v0_2 + 0x24) */
            reg_24 = readl(csi_regs + 0x24);

            /* Binary Ninja: if ($v1_1 != 0) result = seq_printf(arg1, "sensor type is BT656!\n", $v1_1) */
            if (reg_20 != 0) {
                result = seq_printf(m, "sensor type is BT656!\n", reg_20);
            }

            /* Binary Ninja: if ($v0_4 != 0) result += seq_printf(arg1, "sensor type is BT601!\n", $v0_4) */
            if (reg_24 != 0) {
                result += seq_printf(m, "sensor type is BT601!\n", reg_24);
            }

            /* Binary Ninja: return result + seq_printf(arg1, "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\n", *($v0_10 + 0x14)) */
            if (reg_20 != 0 || reg_24 != 0) {
                u32 reg_14 = readl(csi_regs + 0x14);
                return result + seq_printf(m, "%s[%d] VIC failed to config DVP mode!(8bits-sensor)\n", reg_14);
            }
        }
    }

    /* Binary Ninja: isp_printf(2, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n", entry_$a2) */
    isp_printf(2, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n", 0);
    return 0;
}

/* CSI file operations - Binary Ninja reference */
const struct file_operations isp_csi_fops = {
    .owner = THIS_MODULE,
    .open = dump_isp_csi_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

// Define resources outside probe
static struct resource tx_isp_csi_resources[] = {
    [0] = {
        .start  = 0x10022000,  // CSI base address
        .end    = 0x10022000 + 0x1000 - 1,
        .flags  = IORESOURCE_MEM,
        .name   = "csi-regs",
    }
};

/* MIPI PHY base address - separate from CSI core registers */
#define MIPI_PHY_IOBASE  0x10022000
#define MIPI_PHY_SIZE    0x1000

/* tx_isp_csi_probe - EXACT Binary Ninja reference implementation */
/* CRITICAL FIX: Must map BOTH CSI core (sd.base) AND PHY (phy_base) registers */
int tx_isp_csi_probe(struct platform_device *pdev)
{
    struct tx_isp_csi_device *csi_dev;
    struct tx_isp_platform_data *pdata;
    struct resource *phy_res;
    int ret;

    /* Binary Ninja: private_kmalloc(0x148, 0xd0) */
    csi_dev = private_kmalloc(sizeof(struct tx_isp_csi_device), GFP_KERNEL);
    if (!csi_dev) {
        isp_printf(2, "Failed to allocate CSI device\n", sizeof(struct tx_isp_csi_device));
        return -EFAULT;
    }

    memset(csi_dev, 0, sizeof(struct tx_isp_csi_device));

    pdata = pdev->dev.platform_data;

    /* Initialize subdev - this maps CSI core registers to sd.base */
    ret = tx_isp_subdev_init(pdev, &csi_dev->sd, &csi_subdev_ops);
    if (ret != 0) {
        if (pdata) {
            isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", pdata->sensor_type);
        }
        private_kfree(csi_dev);
        return -EFAULT;
    }

    /* Set dev_priv and host_priv AFTER tx_isp_subdev_init to prevent overwrite */
    tx_isp_set_subdevdata(&csi_dev->sd, csi_dev);
    tx_isp_set_subdev_hostdata(&csi_dev->sd, csi_dev);
    pr_info("*** CSI PROBE: Set dev_priv/host_priv to csi_dev %p ***\n", csi_dev);

    /* Use memory mapping from tx_isp_subdev_init for CSI core registers */
    if (csi_dev->sd.regs) {
        csi_dev->csi_regs = csi_dev->sd.regs;
        pr_info("*** CSI PROBE: CSI core regs from subdev_init: %p ***\n", csi_dev->csi_regs);
    } else {
        pr_err("*** CSI PROBE: tx_isp_subdev_init failed to map CSI core registers ***\n");
        tx_isp_subdev_deinit(&csi_dev->sd);
        private_kfree(csi_dev);
        return -EBUSY;
    }

    /* ============================================================================
     * CRITICAL FIX: Map MIPI PHY registers separately at 0x10022000
     * The kernel source (tx-isp-csi.c) shows TWO register mappings:
     *   1. sd.base - CSI core registers (N_LANES, PHY_SHUTDOWNZ, etc.)
     *   2. phy_base - PHY registers at MIPI_PHY_IOBASE (PHY_CRTL0, CK_LANE_CONFIG, etc.)
     *
     * The PHY registers are where the 0x3f lane enable mask MUST be written!
     * ============================================================================ */
    phy_res = private_request_mem_region(MIPI_PHY_IOBASE, MIPI_PHY_SIZE, "mipi-phy");
    if (!phy_res) {
        pr_err("*** CSI PROBE: Failed to request MIPI PHY memory region at 0x%x ***\n", MIPI_PHY_IOBASE);
        /* Continue without PHY - may work for DVP mode */
        csi_dev->phy_regs = NULL;
        csi_dev->phy_res = NULL;
    } else {
        csi_dev->phy_regs = private_ioremap(phy_res->start, phy_res->end - phy_res->start + 1);
        if (!csi_dev->phy_regs) {
            pr_err("*** CSI PROBE: Failed to ioremap MIPI PHY registers ***\n");
            private_release_mem_region(phy_res->start, phy_res->end - phy_res->start + 1);
            csi_dev->phy_res = NULL;
        } else {
            csi_dev->phy_res = phy_res;
            pr_info("*** CSI PROBE: MIPI PHY regs mapped at %p (phys 0x%x) ***\n",
                    csi_dev->phy_regs, MIPI_PHY_IOBASE);
        }
    }

    tx_isp_set_subdev_nodeops(&csi_dev->sd, (struct file_operations *)&isp_csi_fops);
    private_raw_mutex_init(&csi_dev->mlock, "not support the gpio mode!\n", 0);
    private_platform_set_drvdata(pdev, csi_dev);

    csi_dev->state = 1;
    dump_csd = csi_dev;

    pr_info("*** CSI PROBE: Complete - csi_regs=%p, phy_regs=%p ***\n",
            csi_dev->csi_regs, csi_dev->phy_regs);

    return 0;
}

/* tx_isp_csi_remove - EXACT Binary Ninja implementation */
int tx_isp_csi_remove(struct platform_device *pdev)
{
    struct tx_isp_subdev *sd;
    struct tx_isp_csi_device *csi_dev;
    void __iomem *csi_regs;
    struct resource *phy_res;

    sd = private_platform_get_drvdata(pdev);

    if (sd != NULL) {
        if ((unsigned long)sd >= 0xfffff001) {
            sd = NULL;
        }
    }

    if (!sd) {
        return 0;
    }

    csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev) {
        return 0;
    }

    csi_regs = csi_dev->csi_regs;
    phy_res = csi_dev->phy_res;

    /* Disable CSI */
    if (csi_regs) {
        writel(readl(csi_regs + 0x10) & 0xfffffffe, csi_regs + 0x10);
        writel(readl(csi_regs + 0x10) | 1, csi_regs + 0x10);
    }

    private_platform_set_drvdata(pdev, NULL);

    /* Release PHY registers (separately mapped at 0x10022000) */
    if (csi_dev->phy_regs) {
        private_iounmap(csi_dev->phy_regs);
        csi_dev->phy_regs = NULL;
    }
    if (phy_res) {
        private_release_mem_region(phy_res->start, resource_size(phy_res));
    }

    /* CSI core registers are released by tx_isp_subdev_deinit */
    tx_isp_subdev_deinit(&csi_dev->sd);
    private_kfree(csi_dev);

    return 0;
}

/* dump_csi_reg - EXACT Binary Ninja implementation */
void dump_csi_reg(struct tx_isp_subdev *sd)
{
    void __iomem *csi_regs;

    /* Binary Ninja: isp_printf(0, "%s[%d] do not support this interface\n", entry_$a2) */
    isp_printf(0, "%s[%d] do not support this interface\n", "dump_csi_reg");

    /* Binary Ninja: **(arg1 + 0xb8) */
    csi_regs = ((struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd))->csi_regs;

    /* Binary Ninja: All the register reads and prints */
    isp_printf(0, "%s:%d::linear mode\n", readl(csi_regs + 0x00));
    isp_printf(0, "%s:%d::wdr mode\n", readl(csi_regs + 0x04));
    isp_printf(0, "qbuffer null\n", readl(csi_regs + 0x08));
    isp_printf(0, "bank no free\n", readl(csi_regs + 0x0c));
    isp_printf(0, "Failed to allocate vic device\n", readl(csi_regs + 0x10));
    isp_printf(0, "Failed to init isp module(%d.%d)\n", readl(csi_regs + 0x14));
    isp_printf(0, "&vsd->mlock", readl(csi_regs + 0x18));
    isp_printf(0, "&vsd->snap_mlock", readl(csi_regs + 0x1c));
    isp_printf(0, " %d, %d\n", readl(csi_regs + 0x20));
    isp_printf(0, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", readl(csi_regs + 0x24));
    isp_printf(0, "The parameter is invalid!\n", readl(csi_regs + 0x28));
    isp_printf(0, "vic_done_gpio%d", readl(csi_regs + 0x2c));
    isp_printf(0, "register is 0x%x, value is 0x%x\n", readl(csi_regs + 0x30));

    /* Binary Ninja: return isp_printf(0, "count is %d\n", *(*(arg1 + 0xb8) + 0x34)) __tailcall */
    isp_printf(0, "count is %d\n", readl(csi_regs + 0x34));
}

/* tx_isp_csi_activate_subdev - EXACT Binary Ninja implementation */
int tx_isp_csi_activate_subdev(struct tx_isp_subdev *sd)
{
    struct tx_isp_csi_device *csi_dev;
    struct clk **clks;
    int clk_count;
    int i;
    int result = 0xffffffea; /* Binary Ninja: int32_t result = 0xffffffea */

    /* Binary Ninja: if (arg1 != 0) */
    if (sd != NULL) {
        /* Binary Ninja: if (arg1 u>= 0xfffff001) return 0xffffffea */
        if ((unsigned long)sd >= 0xfffff001) {
            return 0xffffffea;
        }

        /* Binary Ninja: void* $s1_1 = *(arg1 + 0xd4) */
        csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
        result = 0xffffffea;

        /* Binary Ninja: if ($s1_1 != 0 && $s1_1 u< 0xfffff001) */
        if (csi_dev != NULL && (unsigned long)csi_dev < 0xfffff001) {
            /* Binary Ninja: private_mutex_lock($s1_1 + 0x12c) */
            mutex_lock(&csi_dev->mlock);

            /* Binary Ninja: if (*($s1_1 + 0x128) == 1) */
            if (csi_dev->state == 1) {
                /* Binary Ninja: *($s1_1 + 0x128) = 2 */
                csi_dev->state = 2;

                /* Ensure CSI clocks are initialized before enabling */
                if (!sd->clks && sd->clk_num > 0) {
                    extern int isp_subdev_init_clks(struct tx_isp_subdev *sd, int clk_num);
                    pr_info("tx_isp_csi_activate_subdev: Initializing %d clocks for CSI before enabling\n", sd->clk_num);
                    if (isp_subdev_init_clks(sd, sd->clk_num) != 0) {
                        pr_warn("tx_isp_csi_activate_subdev: isp_subdev_init_clks failed; continuing without clocks\n");
                    }
                }

                /* Binary Ninja: int32_t* $s1_2 = *(arg1 + 0xbc) */
                clks = sd->clks;

                /* Binary Ninja: if ($s1_2 != 0) */
                if (clks != NULL) {
                    /* Binary Ninja: if ($s1_2 u< 0xfffff001) */
                    if ((unsigned long)clks < 0xfffff001) {
                        /* Binary Ninja: while (i u< *(arg1 + 0xc0)) */
                        clk_count = sd->clk_num;
                        for (i = 0; i < clk_count; i++) {
                            /* Binary Ninja: private_clk_enable(*$s1_2) */
                            clk_enable(clks[i]);
                        }
                    }
                } else {
                    pr_warn("tx_isp_csi_activate_subdev: No CSI clocks available to enable (clks=NULL, clk_num=%d)\n", sd->clk_num);
                }
            }

            /* Binary Ninja: private_mutex_unlock($s1_1 + 0x12c) */
            mutex_unlock(&csi_dev->mlock);
            /* Binary Ninja: return 0 */
            return 0;
        }
    }

    /* Binary Ninja: return result */
    return result;
}

EXPORT_SYMBOL(dump_csi_reg);
EXPORT_SYMBOL(tx_isp_csi_activate_subdev);
EXPORT_SYMBOL(csi_core_ops_init);
