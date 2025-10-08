#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx-isp-device.h"

/* Forward declarations */
int csi_core_ops_init(struct tx_isp_subdev *sd, int enable);
int csi_set_on_lanes(struct tx_isp_csi_device *csi_dev, int lanes);
void dump_csi_reg(struct tx_isp_subdev *sd);
extern struct tx_isp_dev *ourISPdev;


static void __iomem *tx_isp_core_regs = NULL;

u32 isp_read32(u32 reg)
{
    if (!tx_isp_core_regs) {
        tx_isp_core_regs = ioremap(0x13300000, 0x10000);  // Core base from /proc/iomem
        if (!tx_isp_core_regs) {
            pr_err("Failed to map core registers\n");
            return 0;
        }
    }
    return readl(tx_isp_core_regs + reg);
}

void isp_write32(u32 reg, u32 val)
{
    if (!tx_isp_core_regs) {
        tx_isp_core_regs = ioremap(0x13300000, 0x10000);
        if (!tx_isp_core_regs) {
            pr_err("Failed to map core registers\n");
            return;
        }
    }
    writel(val, tx_isp_core_regs + reg);
}

static void __iomem *tx_isp_vic_regs = NULL;

u32 vic_read32(u32 reg)
{
    if (!tx_isp_vic_regs) {
        tx_isp_vic_regs = ioremap(0x10023000, 0x1000);  // VIC base from /proc/iomem
        if (!tx_isp_vic_regs) {
            pr_err("Failed to map VIC registers\n");
            return 0;
        }
    }
    return readl(tx_isp_vic_regs + reg);
}

void vic_write32(u32 reg, u32 val)
{
    if (!tx_isp_vic_regs) {
        tx_isp_vic_regs = ioremap(0x10023000, 0x1000);
        if (!tx_isp_vic_regs) {
            pr_err("Failed to map VIC registers\n");
            return;
        }
    }
    writel(val, tx_isp_vic_regs + reg);
}

/* Similarly for CSI... */
static void __iomem *tx_isp_csi_regs = NULL;

u32 csi_read32(u32 reg)
{
    if (!tx_isp_csi_regs) {
        tx_isp_csi_regs = ioremap(0x10022000, 0x1000);
        if (!tx_isp_csi_regs) {
            pr_err("Failed to map CSI registers\n");
            return 0;
        }
    }
    return readl(tx_isp_csi_regs + reg);
}

void csi_write32(u32 reg, u32 val)
{
    if (!tx_isp_csi_regs) {
        tx_isp_csi_regs = ioremap(0x10022000, 0x1000);
        if (!tx_isp_csi_regs) {
            pr_err("Failed to map CSI registers\n");
            return;
        }
    }
    writel(val, tx_isp_csi_regs + reg);
}

/* CSI interrupt handler */
static irqreturn_t tx_isp_csi_irq_handler(int irq, void *dev_id)
{
    struct tx_isp_subdev *sd = dev_id;
    struct tx_isp_csi_device *csi_dev;
    void __iomem *csi_base;
    u32 status, err1, err2, phy_state;
    irqreturn_t ret = IRQ_NONE;
    unsigned long flags;
    static unsigned long last_interrupt_time = 0;
    unsigned long current_time = jiffies;

    /* CRITICAL: Log CSI interrupt activity to understand timing */
    pr_info("*** CSI INTERRUPT: irq=%d, dev_id=%p, time_delta=%lu ms ***\n",
            irq, dev_id, jiffies_to_msecs(current_time - last_interrupt_time));
    last_interrupt_time = current_time;

    if (!sd) {
        pr_warn("*** CSI INTERRUPT: sd is NULL - returning IRQ_NONE ***\n");
        return IRQ_NONE;
    }

    csi_dev = ourISPdev->csi_dev;
    if (!csi_dev) {
        pr_warn("*** CSI INTERRUPT: csi_dev is NULL - returning IRQ_NONE ***\n");
        return IRQ_NONE;
    }

    csi_base = csi_dev->csi_regs;
    if (!csi_base) {
        pr_warn("*** CSI INTERRUPT: csi_base is NULL - returning IRQ_NONE ***\n");
        return IRQ_NONE;
    }

    spin_lock_irqsave(&csi_dev->lock, flags);

    /* Read interrupt status registers */
    err1 = readl(csi_base + 0x20);  /* ERR1 register */
    err2 = readl(csi_base + 0x24);  /* ERR2 register */
    phy_state = readl(csi_base + 0x14);  /* PHY_STATE register */

    /* CRITICAL: Log all CSI register states for analysis */
    pr_info("*** CSI INTERRUPT STATUS: err1=0x%08x, err2=0x%08x, phy_state=0x%08x ***\n",
            err1, err2, phy_state);


    if (err1 || err2) {
        ret = IRQ_HANDLED;

        /* Handle protocol errors (ERR1) */
        if (err1) {
            ISP_ERROR("CSI Protocol errors (ERR1): 0x%08x\n", err1);

            if (err1 & 0x1) ISP_ERROR("  - SOT Sync Error\n");
            if (err1 & 0x2) ISP_ERROR("  - SOTHS Sync Error\n");
            if (err1 & 0x4) ISP_ERROR("  - ECC Single-bit Error (corrected)\n");
            if (err1 & 0x8) ISP_ERROR("  - ECC Multi-bit Error (uncorrectable)\n");
            if (err1 & 0x10) ISP_ERROR("  - CRC Error\n");
            if (err1 & 0x20) ISP_ERROR("  - Packet Size Error\n");
            if (err1 & 0x40) ISP_ERROR("  - EoTp Error\n");

            /* Clear errors by writing back the status */
            writel(err1, csi_base + 0x20);
            wmb();
        }

        /* Handle application errors (ERR2) */
        if (err2) {
            ISP_ERROR("CSI Application errors (ERR2): 0x%08x\n", err2);

            if (err2 & 0x1) ISP_ERROR("  - Data ID Error\n");
            if (err2 & 0x2) ISP_ERROR("  - Frame Sync Error\n");
            if (err2 & 0x4) ISP_ERROR("  - Frame Data Error\n");
            if (err2 & 0x8) ISP_ERROR("  - Frame Sequence Error\n");

            /* Clear errors by writing back the status */
            writel(err2, csi_base + 0x24);
            wmb();
        }

        /* Update error state */
        if ((err1 & 0x38) || (err2 & 0xE)) { /* Serious errors */
            pr_err("CSI: Serious errors detected, may need recovery\n");
        }
    }

    /* Check for PHY state changes */
    if (phy_state & 0x111) { /* Clock lane or data lanes in stop state */
        /* This is normal during idle periods, only log if debugging */
        pr_debug("CSI PHY lanes in stop state: 0x%08x\n", phy_state);
    }

    /* CRITICAL: Log CSI interrupt completion status */
    if (ret == IRQ_HANDLED) {
        pr_info("*** CSI INTERRUPT: Handled successfully - errors processed ***\n");
    } else {
        pr_info("*** CSI INTERRUPT: No action taken - no errors detected ***\n");
    }

    spin_unlock_irqrestore(&csi_dev->lock, flags);
    return ret;
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

/* CSI start operation */
int tx_isp_csi_start(struct tx_isp_subdev *sd)
{
    u32 ctrl;
    int ret;

    if (!sd)
        return -EINVAL;

    mutex_lock(&sd->csi_lock);

    /* CRITICAL: Register CSI interrupt handler if not already registered */
    static int csi_irq_registered = 0;
    if (!csi_irq_registered) {
        ret = request_irq(38, tx_isp_csi_irq_handler, IRQF_SHARED, "tx-isp-csi", sd);
        if (ret == 0) {
            pr_info("*** CSI INTERRUPT: Handler registered for IRQ 38 ***\n");
            csi_irq_registered = 1;
        } else {
            pr_err("*** CSI INTERRUPT: Failed to register handler for IRQ 38: %d ***\n", ret);
        }
    }

    /* Enable CSI */
    ctrl = csi_read32(CSI_CTRL);
    ctrl |= CSI_CTRL_EN;
    csi_write32(CSI_CTRL, ctrl);

    /* Enable interrupts */
    csi_write32(CSI_INT_MASK, ~(INT_ERROR | INT_FRAME_DONE));

    pr_info("*** CSI INTERRUPT: CSI started with interrupts enabled (mask=0x%08x) ***\n",
            ~(INT_ERROR | INT_FRAME_DONE));

    mutex_unlock(&sd->csi_lock);
    return 0;
}

/* CSI stop operation */
int tx_isp_csi_stop(struct tx_isp_subdev *sd)
{
    if (!sd)
        return -EINVAL;

    mutex_lock(&sd->csi_lock);

    /* Disable CSI */
    csi_write32(CSI_CTRL, 0);

    /* Mask all interrupts */
    csi_write32(CSI_INT_MASK, 0xFFFFFFFF);

    mutex_unlock(&sd->csi_lock);
    return 0;
}

/* CSI format configuration */
int tx_isp_csi_set_format(struct tx_isp_subdev *sd, struct tx_isp_config *config)
{
    u32 ctrl;

    if (!sd || !config)
        return -EINVAL;

    if (config->lane_num < CSI_MIN_LANES || config->lane_num > CSI_MAX_LANES)
        return -EINVAL;

    mutex_lock(&sd->csi_lock);

    /* Configure lane count */
    ctrl = csi_read32(CSI_CTRL);
    ctrl &= ~(3 << 2); /* Clear lane bits */
    switch (config->lane_num) {
    case 1:
        ctrl |= CSI_CTRL_LANES_1;
        break;
    case 2:
        ctrl |= CSI_CTRL_LANES_2;
        break;
    case 4:
        ctrl |= CSI_CTRL_LANES_4;
        break;
    default:
        mutex_unlock(&sd->csi_lock);
        return -EINVAL;
    }
    csi_write32(CSI_CTRL, ctrl);

    mutex_unlock(&sd->csi_lock);
    return 0;
}

/* CSI video streaming control - FIXED: MIPS memory alignment */

/* CSI video streaming control - EXACT Binary Ninja MCP implementation */
int csi_video_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_csi_device *csi_dev;

    pr_info("*** csi_video_s_stream: EXACT Binary Ninja MCP implementation ***\n");
    pr_info("csi_video_s_stream: sd=%p, enable=%d\n", sd, enable);

    /* Binary Ninja: if (arg1 == 0 || arg1 u>= 0xfffff001) */
    if (sd == NULL || (unsigned long)sd >= 0xfffff001) {
        /* Binary Ninja: isp_printf(2, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n", arg3) */
        isp_printf(2, "%s[%d] VIC failed to config DVP SONY mode!(10bits-sensor)\n", enable);
        /* Binary Ninja: return 0xffffffea */
        return 0xffffffea;
    }

    /* Get CSI device from subdev private data */
    csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev) {
        pr_err("CSI device is NULL from subdev private data\n");
        return 0xffffffea;
    }

    /* Binary Ninja: if (*(*(arg1 + 0x110) + 0x14) != 1) return 0 */
    /* This checks CSI device interface type - if not MIPI (1), return 0 */
    if (csi_dev->interface_type != 1) {
        pr_info("csi_video_s_stream: Interface type %d != 1 (MIPI), returning 0\n", csi_dev->interface_type);
        return 0;
    }

    /* Binary Ninja: int32_t $v0_4 = 4 */
    int v0_4 = 4;

    /* Binary Ninja: if (arg2 == 0) $v0_4 = 3 */
    if (enable == 0) {
        v0_4 = 3;
    }

    /* Binary Ninja: *(arg1 + 0x128) = $v0_4 */
    csi_dev->state = v0_4;

    pr_info("csi_video_s_stream: EXACT Binary Ninja MCP - CSI state set to %d (enable=%d)\n", v0_4, enable);

    /* Binary Ninja: return 0 */
    return 0;
}

/* CSI sensor operations IOCTL handler */
int csi_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    struct tx_isp_csi_device *csi_dev;

    if (!sd)
        return -EINVAL;

    /* Get the CSI device from the subdevice */
    csi_dev = ourISPdev->csi_dev;
    if (!csi_dev) {
        pr_err("CSI device is NULL\n");
        return -EINVAL;
    }

    switch (cmd) {
    case TX_ISP_EVENT_SENSOR_RESIZE:  /* This is the correct event for reset */
        /* Reset CSI */
        /* CRITICAL FIX: Don't set to ERROR state (3)! Reset to IDLE (1) */
        if (csi_dev->state >= CSI_STATE_ACTIVE) {
            csi_dev->state = CSI_STATE_IDLE;  /* 1 = IDLE, not 3 = ERROR */
            pr_info("CSI reset to IDLE state due to sensor resize\n");
        }
        break;
    case TX_ISP_EVENT_SENSOR_FPS:
        /* Update FPS */
        /* CRITICAL FIX: Don't check for ERROR state (3)! Check for ACTIVE (2) */
        if (csi_dev->state >= CSI_STATE_ACTIVE) {
            /* Stay in ACTIVE state for FPS changes */
            pr_info("CSI FPS update while in ACTIVE state\n");
        }
        break;
    case TX_ISP_EVENT_SENSOR_PREPARE_CHANGE:  /* This is the correct event for start */
        /* Start CSI */
        csi_core_ops_init(sd, 1);
        break;
    }

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
                    if (ourISPdev->sensor && ourISPdev->sensor->video.attr) {
                        sensor_attr = ourISPdev->sensor->video.attr;
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
                        pr_info("*** CSI MIPI INIT: Configuring MIPI CSI for %d lanes ***\n", sensor_attr->mipi.lans);
                        /* Binary Ninja: *(*($s0_1 + 0xb8) + 4) = zx.d(*($v1_5 + 0x24)) - 1 */
                        writel(sensor_attr->mipi.lans - 1, csi_dev->csi_regs + 4);
                        pr_info("*** CSI[0x4] = %d (lanes - 1) ***\n", sensor_attr->mipi.lans - 1);

                        /* Binary Ninja: void* $v0_2 = *($s0_1 + 0xb8) */
                        csi_regs = csi_dev->csi_regs;

                        /* Binary Ninja: *($v0_2 + 8) &= 0xfffffffe */
                        writel(readl(csi_regs + 8) & 0xfffffffe, csi_regs + 8);

                        /* Binary Ninja: *(*($s0_1 + 0xb8) + 0xc) = 0 */
                        writel(0, csi_regs + 0xc);

                        /* Binary Ninja: private_msleep(1) */
                        private_msleep(1);

                        /* Binary Ninja: void* $v1_9 = *($s0_1 + 0xb8) */
                        /* Binary Ninja: *($v1_9 + 0x10) &= 0xfffffffe */
                        writel(readl(csi_regs + 0x10) & 0xfffffffe, csi_regs + 0x10);

                        /* Binary Ninja: private_msleep(1) */
                        private_msleep(1);

                        /* Binary Ninja: *(*($s0_1 + 0xb8) + 0xc) = $s2_1 */
                        writel(interface_type, csi_regs + 0xc);

                        /* Binary Ninja: private_msleep(1) */
                        private_msleep(1);

                        /* Binary Ninja: void* $v0_7 = *($s0_1 + 0x110) */
                        /* Binary Ninja: int32_t $v1_10 = *($v0_7 + 0x3c) */
                        int v1_10 = sensor_attr->fps;
                        void *v0_8;

                        /* Binary Ninja: if ($v1_10 != 0) */
                        if (v1_10 != 0) {
                            /* Binary Ninja: $v0_8 = *($s0_1 + 0x13c) */
                            /* SAFE: Use proper struct member access instead of dangerous offset */
                            isp_csi_regs = csi_dev->csi_regs;
                            v0_8 = isp_csi_regs;
                        } else {
                            /* Binary Ninja: int32_t $v0_9 = *($v0_7 + 0x1c) */
                            int v0_9 = sensor_attr->total_width;

                            /* Binary Ninja: Complex frame rate calculation based on width */
                            if (v0_9 - 0x50 < 0x1e) {
                                /* Binary Ninja: $a0_2 = *($s0_1 + 0x13c) */
                                /* SAFE: Use proper struct member access instead of dangerous offset */
                                isp_csi_regs = csi_dev->csi_regs;
                            } else {
                                v1_10 = 1;
                                if (v0_9 - 0x6e >= 0x28) {
                                    v1_10 = 2;
                                    if (v0_9 - 0x96 >= 0x32) {
                                        v1_10 = 3;
                                        if (v0_9 - 0xc8 >= 0x32) {
                                            v1_10 = 4;
                                            if (v0_9 - 0xfa >= 0x32) {
                                                v1_10 = 5;
                                                if (v0_9 - 0x12c >= 0x64) {
                                                    v1_10 = 6;
                                                    if (v0_9 - 0x190 >= 0x64) {
                                                        v1_10 = 7;
                                                        if (v0_9 - 0x1f4 >= 0x64) {
                                                            v1_10 = 8;
                                                            if (v0_9 - 0x258 >= 0x64) {
                                                                v1_10 = 9;
                                                                if (v0_9 - 0x2bc >= 0x64) {
                                                                    v1_10 = 0xa;
                                                                    if (v0_9 - 0x320 >= 0xc8) {
                                                                        v1_10 = 0xb;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                /* SAFE: Use proper struct member access instead of dangerous offset */
                                isp_csi_regs = csi_dev->csi_regs;
                            }

                            /* Binary Ninja: int32_t $v0_14 = (*($a0_2 + 0x160) & 0xfffffff0) | $v1_10 */
                            int v0_14 = (readl(isp_csi_regs + 0x160) & 0xfffffff0) | v1_10;
                            writel(v0_14, isp_csi_regs + 0x160);
                            writel(v0_14, isp_csi_regs + 0x1e0);
                            writel(v0_14, isp_csi_regs + 0x260);
                            v0_8 = isp_csi_regs;
                        }

                        /* Binary Ninja: *$v0_8 = 0x7d */
                        writel(0x7d, v0_8);
                        pr_info("*** CSI MIPI: Writing ISP_CSI[0x0] = 0x7d (timing config) ***\n");

                        /* Binary Ninja: *(*($s0_1 + 0x13c) + 0x128) = 0x3f */
                        writel(0x3f, isp_csi_regs + 0x128);
                        pr_info("*** CSI MIPI: Writing ISP_CSI[0x128] = 0x3f (lane config) ***\n");

                        /* Binary Ninja: *(*($s0_1 + 0xb8) + 0x10) = 1 */
                        writel(1, csi_dev->csi_regs + 0x10);
                        pr_info("*** CSI MIPI: CRITICAL - Enabling CSI PHY: CSI[0x10] = 1 ***\n");
                        pr_info("*** CSI MIPI: PHY ENABLED - MIPI data should now flow! ***\n");

                        /* Binary Ninja: private_msleep(0xa) */
                        private_msleep(0xa);

                        v0_17 = 3;

                    } else if (interface_type != 2) {
                        /* Binary Ninja: isp_printf(1, "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\n", $s2_1) */
                        isp_printf(1, "%s[%d] VIC failed to config DVP mode!(10bits-sensor)\n", interface_type);
                        v0_17 = 3;
                    } else {
                        /* Binary Ninja: DVP interface configuration */
                        /* *(*($s0_1 + 0xb8) + 0xc) = 0 */
                        writel(0, csi_dev->csi_regs + 0xc);

                        /* *(*($s0_1 + 0xb8) + 0xc) = 1 */
                        writel(1, csi_dev->csi_regs + 0xc);

                        /* **($s0_1 + 0x13c) = 0x7d */
                        writel(0x7d, isp_csi_regs);

                        /* *(*($s0_1 + 0x13c) + 0x80) = 0x3e */
                        writel(0x3e, isp_csi_regs + 0x80);

                        /* *(*($s0_1 + 0x13c) + 0x2cc) = 1 */
                        writel(1, isp_csi_regs + 0x2cc);

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

/* CSI link setup stub - CSI doesn't need complex link management */
static int csi_video_link_setup(struct tx_isp_subdev_pad *local,
                                  struct tx_isp_subdev_pad *remote,
                                  u32 flags)
{
    pr_info("*** csi_video_link_setup: CSI link setup (stub) - flags=0x%x ***\n", flags);
    /* CSI links are managed by hardware configuration, not software links */
    return 0;
}

/* Define the core operations */
static struct tx_isp_subdev_core_ops csi_core_ops = {
    .init = csi_core_ops_init,
};

/* Define the video operations */
static struct tx_isp_subdev_video_ops csi_video_ops = {
    .s_stream = csi_video_s_stream,
    .link_setup = csi_video_link_setup,
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
static struct tx_isp_subdev_sensor_ops csi_sensor_ops = {
    .sync_sensor_attr = csi_sensor_ops_sync_sensor_attr_wrapper,
    .ioctl = csi_sensor_ops_ioctl,
};

/* CSI internal ops for activate/slake */
static struct tx_isp_subdev_internal_ops csi_subdev_internal_ops = {
    .activate_module = tx_isp_csi_activate_subdev,
    .slake_module = tx_isp_csi_slake_subdev,
};

/* Initialize the subdev ops structure with pointers to the operations */
struct tx_isp_subdev_ops csi_subdev_ops = {
    .core = &csi_core_ops,
    .video = &csi_video_ops,
    .sensor = &csi_sensor_ops,
    .internal = &csi_subdev_internal_ops,
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

int tx_isp_csi_probe(struct platform_device *pdev)
{
    struct tx_isp_csi_device *csi_dev = NULL;
    struct tx_isp_subdev *sd = NULL;
    struct resource *res;
    int32_t ret = 0;

    pr_info("*** tx_isp_csi_probe: Starting CSI device probe ***\n");

    /* CRITICAL: Use existing CSI device from ourISPdev, DO NOT create a new one!
     * The CSI device is already created by csi_device_probe in tx-isp-module.c
     */
    if (ourISPdev && ourISPdev->csi_dev) {
        csi_dev = ourISPdev->csi_dev;
        pr_info("*** tx_isp_csi_probe: Using existing CSI device at %p ***\n", csi_dev);
    } else {
        pr_err("*** tx_isp_csi_probe: No existing CSI device found! ***\n");
        return -EINVAL;
    }

    /* Get subdev pointer from CSI device */
    sd = &csi_dev->sd;

    /* Add resources to platform device if not already present */
    if (!platform_get_resource(pdev, IORESOURCE_MEM, 0)) {
        ret = platform_device_add_resources(pdev, tx_isp_csi_resources,
                                          ARRAY_SIZE(tx_isp_csi_resources));
        if (ret) {
            pr_err("Failed to add CSI resources\n");
            return ret;
        }
    }

    /* Get platform resource */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    /* CRITICAL FIX: Set subdev private data BEFORE calling tx_isp_subdev_init
     * because tx_isp_subdev_init calls tx_isp_subdev_auto_link which needs
     * to retrieve csi_dev via tx_isp_get_subdevdata(sd)
     */
    tx_isp_set_subdevdata(sd, csi_dev);
    pr_info("*** tx_isp_csi_probe: Set subdev private data to csi_dev=%p ***\n", csi_dev);

    /* CRITICAL: Initialize subdev AFTER setting private data */
    ret = tx_isp_subdev_init(pdev, sd, &csi_subdev_ops);
    if (ret != 0) {
        pr_err("Failed to init CSI subdev module(%d.%d)\n",
               res ? MAJOR(res->start) : 0,
               res ? MINOR(res->start) : 0);
        return -EFAULT;  /* Binary returns -12 (EFAULT) */
    }

    /* Set platform driver data after successful init */
    platform_set_drvdata(pdev, csi_dev);

    pr_info("*** tx_isp_csi_probe: CSI device initialized successfully ***\n");
    pr_info("CSI device: csi_dev=%p, size=%zu\n", csi_dev, sizeof(struct tx_isp_csi_device));
    pr_info("  sd: %p\n", sd);
    pr_info("  state: %d\n", csi_dev->state);
    pr_info("  csi_regs: %p\n", csi_dev->csi_regs);

    return 0;
}

/* CSI remove function */
int tx_isp_csi_remove(struct platform_device *pdev)
{
    struct tx_isp_csi_device *csi_dev = platform_get_drvdata(pdev);
    struct tx_isp_subdev *sd;
    struct resource *res = NULL;

    if (!csi_dev)
        return -EINVAL;

    sd = &csi_dev->sd;

    pr_info("*** tx_isp_csi_remove: Removing CSI device ***\n");

    /* Free interrupt if it was requested */
    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (res)
        free_irq(res->start, sd);

    /* Disable CSI */
    tx_isp_csi_stop(sd);

    /* Deinitialize subdev */
    tx_isp_subdev_deinit(sd);

    pr_info("*** tx_isp_csi_remove: CSI device removed ***\n");
    return 0;
}

/* CSI register dump function for debugging */
void dump_csi_reg(struct tx_isp_subdev *sd)
{
    struct tx_isp_csi_device *csi_dev;
    void __iomem *csi_base;

    if (!sd) {
        pr_err("dump_csi_reg: sd is NULL\n");
        return;
    }

    csi_dev = ourISPdev->csi_dev;
    if (!csi_dev) {
        pr_err("dump_csi_reg: csi_dev is NULL\n");
        return;
    }

    /* CRITICAL FIX: Use safe struct member access instead of dangerous offset 0x13c */
    csi_base = csi_dev->csi_regs;
    if (!csi_base) {
        pr_err("dump_csi_reg: csi_base is NULL\n");
        return;
    }

pr_info("=== CSI Register Dump ===\n");
pr_info("VERSION (0x00): 0x%08x\n", readl(csi_base + 0x00));
pr_info("N_LANES (0x04): 0x%08x\n", readl(csi_base + 0x04));
pr_info("PHY_SHUTDOWNZ (0x08): 0x%08x\n", readl(csi_base + 0x08));
pr_info("DPHY_RSTZ (0x0C): 0x%08x\n", readl(csi_base + 0x0C));
pr_info("CSI2_RESETN (0x10): 0x%08x\n", readl(csi_base + 0x10));
pr_info("PHY_STATE (0x14): 0x%08x\n", readl(csi_base + 0x14));
pr_info("DATA_IDS_1 (0x18): 0x%08x\n", readl(csi_base + 0x18));
pr_info("DATA_IDS_2 (0x1C): 0x%08x\n", readl(csi_base + 0x1C));
pr_info("ERR1 (0x20): 0x%08x\n", readl(csi_base + 0x20));
pr_info("ERR2 (0x24): 0x%08x\n", readl(csi_base + 0x24));
pr_info("MASK1 (0x28): 0x%08x\n", readl(csi_base + 0x28));
pr_info("MASK2 (0x2C): 0x%08x\n", readl(csi_base + 0x2C));
pr_info("CSI_CTRL (0x40): 0x%08x\n", readl(csi_base + 0x40));
pr_info("PHY_TST_CTRL0 (0x50): 0x%08x\n", readl(csi_base + 0x50));
pr_info("PHY_TST_CTRL1 (0x54): 0x%08x\n", readl(csi_base + 0x54));
pr_info("PHY_TIMING (0x128): 0x%08x\n", readl(csi_base + 0x128));
pr_info("PHY_TIMING_0x160: 0x%08x\n", readl(csi_base + 0x160));
pr_info("PHY_TIMING_0x1e0: 0x%08x\n", readl(csi_base + 0x1e0));
pr_info("PHY_TIMING_0x260: 0x%08x\n", readl(csi_base + 0x260));
pr_info("========================\n");
}

/* CSI activation function - matching reference driver */

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

/* Export symbols for use by other parts of the driver */
EXPORT_SYMBOL(tx_isp_csi_start);
EXPORT_SYMBOL(tx_isp_csi_stop);
EXPORT_SYMBOL(tx_isp_csi_set_format);
EXPORT_SYMBOL(dump_csi_reg);
EXPORT_SYMBOL(tx_isp_csi_activate_subdev);
EXPORT_SYMBOL(tx_isp_csi_slake_subdev);
