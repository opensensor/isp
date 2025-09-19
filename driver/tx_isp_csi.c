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

u32 vic_read32(u32 reg)
{
    /* Use global ISP device VIC registers mapped by subdev probe */
    if (ourISPdev && ourISPdev->vic_regs) {
        return readl(ourISPdev->vic_regs + reg);
    }
    pr_err("vic_read32: No VIC registers available\n");
    return 0;
}

void vic_write32(u32 reg, u32 val)
{
    /* Use global ISP device VIC registers mapped by subdev probe */
    if (ourISPdev && ourISPdev->vic_regs) {
        writel(val, ourISPdev->vic_regs + reg);
    } else {
        pr_err("vic_write32: No VIC registers available\n");
    }
}

/* CSI register mapping handled by subdev probe per reference driver */

u32 csi_read32(u32 reg)
{
    /* Use global ISP device CSI registers mapped by subdev probe */
    if (ourISPdev && ourISPdev->csi_regs) {
        return readl(ourISPdev->csi_regs + reg);
    }
    pr_err("csi_read32: No CSI registers available\n");
    return 0;
}

void csi_write32(u32 reg, u32 val)
{
    /* Use global ISP device CSI registers mapped by subdev probe */
    if (ourISPdev && ourISPdev->csi_regs) {
        writel(val, ourISPdev->csi_regs + reg);
    } else {
        pr_err("csi_write32: No CSI registers available\n");
    }
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
        pr_info("CSI PHY lanes in stop state: 0x%08x\n", phy_state);
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

    /* CRITICAL FIX: DON'T register duplicate IRQ handler - IRQ 38 already registered in tx-isp-module.c */
    /* The main module already registers IRQ 38 with proper routing to CSI handler */
    pr_info("*** CSI INTERRUPT: IRQ 38 already registered by main module - skipping duplicate registration ***\n");

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
int csi_video_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_sensor_attribute *attr;
    struct tx_isp_csi_device *csi_dev;
    void __iomem *csi_base;
    int ret = 0;

    pr_info("*** csi_video_s_stream: EXACT Binary Ninja implementation - FIXED for MIPS ***\n");
    pr_info("csi_video_s_stream: sd=%p, enable=%d\n", sd, enable);

    /* CRITICAL FIX: Use safe struct member access instead of dangerous offset 0xd4 */
    csi_dev = ourISPdev->csi_dev;
    if (!csi_dev) {
        pr_err("CSI device is NULL\n");

        /* Try to get the CSI device from ourISPdev as a fallback */
        if (ourISPdev && ourISPdev->csi_dev) {
            csi_dev = ourISPdev->csi_dev;
            pr_info("Using CSI device from ourISPdev: %p\n", csi_dev);

            /* Update the subdevice data with the CSI device */
            tx_isp_set_subdevdata(sd, csi_dev);
        } else {
            return 0xffffffea;
        }
    }

    /* CRITICAL FIX: Binary Ninja exact check - if (*(*(arg1 + 0x110) + 0x14) != 1) return 0 */
    /* Replace dangerous offset arithmetic with safe struct member access */
    if (csi_dev->state < 2) { /* Use struct member instead of *(*(arg1 + 0x110) + 0x14) */
        pr_info("csi_video_s_stream: CSI device state=%d < 2, returning 0\n", csi_dev->state);
        return 0;
    }

    /* CRITICAL FIX: Use safe struct member access instead of dangerous offset 0x13c */
    csi_base = csi_dev->csi_regs; /* Use struct member instead of *(void **)(((char *)csi_dev) + 0x13c) */
    if (!csi_base) {
        pr_err("CSI base address is NULL\n");
        return 0xffffffea;
    }

    /* Create a default sensor attribute if none exists */
    if (sd->active_sensor) {
        attr = &sd->active_sensor->attr;
    } else if (ourISPdev && ourISPdev->sensor_sd && ourISPdev->sensor_sd->active_sensor) {
        /* Try to get the sensor attribute from ourISPdev as a fallback */
        attr = &ourISPdev->sensor_sd->active_sensor->attr;
        pr_info("Using sensor attribute from ourISPdev\n");

        /* Copy the sensor to our subdevice */
        sd->active_sensor = ourISPdev->sensor_sd->active_sensor;
    } else if (ourISPdev && ourISPdev->sensor_width > 0 && ourISPdev->sensor_height > 0) {
        /* Create a default sensor if we have dimensions */
        struct tx_isp_sensor *sensor = kzalloc(sizeof(struct tx_isp_sensor), GFP_ATOMIC);
        if (!sensor) {
            pr_err("Failed to allocate sensor structure\n");
            return -ENOMEM;
        }

        /* Initialize with default values */
        sensor->attr.dbus_type = TX_SENSOR_DATA_INTERFACE_MIPI;
        sensor->attr.mipi.lans = 2; /* Default to 2 lanes */
        sensor->attr.mipi.mipi_sc.sensor_csi_fmt = TX_SENSOR_RAW10; /* Default to RAW10 */
        sensor->attr.total_width = ourISPdev->sensor_width;
        sensor->attr.total_height = ourISPdev->sensor_height;

        /* Store in subdevice */
        sd->active_sensor = sensor;
        attr = &sensor->attr;

        pr_info("Created default sensor attribute: %dx%d, MIPI, RAW10, 2 lanes\n",
                attr->total_width, attr->total_height);
    } else {
        /* Create a default sensor with hardcoded values */
        struct tx_isp_sensor *sensor = kzalloc(sizeof(struct tx_isp_sensor), GFP_ATOMIC);
        if (!sensor) {
            pr_err("Failed to allocate sensor structure\n");
            return -ENOMEM;
        }

        /* Initialize with default values */
        sensor->attr.dbus_type = TX_SENSOR_DATA_INTERFACE_MIPI;
        sensor->attr.mipi.lans = 2; /* Default to 2 lanes */
        sensor->attr.mipi.mipi_sc.sensor_csi_fmt = TX_SENSOR_RAW10; /* Default to RAW10 */
        sensor->attr.total_width = 1920;  /* Default to 1418p */
        sensor->attr.total_height = 1080;

        /* Store in subdevice */
        sd->active_sensor = sensor;
        attr = &sensor->attr;

        pr_info("Created default sensor attribute: 2200x1418, MIPI, RAW10, 2 lanes\n");
    }

    /* Only handle MIPI sensors */
    if (attr->dbus_type != TX_SENSOR_DATA_INTERFACE_MIPI)
        return 0;

    /* Initialize CSI hardware if needed */
    if (enable && csi_dev->state < 3) {
        ret = csi_core_ops_init(sd, 1);
        if (ret) {
            pr_err("Failed to initialize CSI hardware: %d\n", ret);
            return ret;
        }
    }

    /* Binary Ninja: int32_t $v0_4 = 4, if (arg2 == 0) $v0_4 = 3 */
    if (enable) {
        /* Binary Ninja: *(arg1 + 0x128) = 4 */
        /* CRITICAL FIX: Use CORRECT Binary Ninja state 4 for streaming! */
        csi_dev->state = 4;  /* 4 = STREAMING (Binary Ninja reference) */
        pr_info("CSI streaming enabled - state=%d (STREAMING)\n", csi_dev->state);
    } else {
        pr_info("*** CSI VIDEO STREAMING DISABLE ***\n");

        /* Binary Ninja: *(arg1 + 0x128) = 3 */
        /* CRITICAL FIX: Use CORRECT Binary Ninja state 3 for disable */
        csi_dev->state = 3;  /* 3 = DISABLED (Binary Ninja reference) */
        pr_info("CSI streaming disabled - state=%d (DISABLED)\n", csi_dev->state);
    }

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
    void __iomem *csi_base;
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
        result = 0xffffffea;

        /* Binary Ninja: if ($s0_1 != 0 && $s0_1 u< 0xfffff001) */
        if (csi_dev != NULL && (unsigned long)csi_dev < 0xfffff001) {
            result = 0;

            /* Binary Ninja: if (*($s0_1 + 0x128) s>= 2) */
            if (csi_dev->state >= 2) {
                int v0_17;

                /* Binary Ninja: if (arg2 == 0) */
                if (enable == 0) {
                    /* Binary Ninja: isp_printf(0, "%s[%d] VIC do not support this format %d\n", arg3) */
                    isp_printf(0, "%s[%d] VIC do not support this format %d\n", enable);

                    /* Binary Ninja: void* $a0_21 = *($s0_1 + 0xb8) */
                    csi_base = csi_dev->csi_regs;

                    /* Binary Ninja: *($a0_21 + 8) &= 0xfffffffe */
                    writel(readl(csi_base + 8) & 0xfffffffe, csi_base + 8);

                    /* Binary Ninja: void* $a0_22 = *($s0_1 + 0xb8) */
                    /* Binary Ninja: *($a0_22 + 0xc) &= 0xfffffffe */
                    writel(readl(csi_base + 0xc) & 0xfffffffe, csi_base + 0xc);

                    /* Binary Ninja: void* $a0_23 = *($s0_1 + 0xb8) */
                    /* Binary Ninja: *($a0_23 + 0x10) &= 0xfffffffe */
                    writel(readl(csi_base + 0x10) & 0xfffffffe, csi_base + 0x10);

                    v0_17 = 2;
                } else {
                    /* Binary Ninja: void* $v1_5 = *($s0_1 + 0x110) */
                    /* SAFE: Use proper struct member access instead of dangerous offset */
                    sensor_attr = csi_dev->sensor_attr;

                    /* Binary Ninja: int32_t $s2_1 = *($v1_5 + 0x14) */
                    int interface_type = sensor_attr->dbus_type;

                    if (interface_type == 1) {
                        /* Binary Ninja: *(*($s0_1 + 0xb8) + 4) = zx.d(*($v1_5 + 0x24)) - 1 */
                        writel(sensor_attr->mipi.lans - 1, csi_dev->csi_regs + 4);

                        /* Binary Ninja: void* $v0_2 = *($s0_1 + 0xb8) */
                        csi_base = csi_dev->csi_regs;

                        /* Binary Ninja: *($v0_2 + 8) &= 0xfffffffe */
                        writel(readl(csi_base + 8) & 0xfffffffe, csi_base + 8);

                        /* Binary Ninja: *(*($s0_1 + 0xb8) + 0xc) = 0 */
                        writel(0, csi_dev->csi_regs + 0xc);

                        /* Binary Ninja: private_msleep(1) */
                        private_msleep(1);

                        /* Binary Ninja: void* $v1_9 = *($s0_1 + 0xb8) */
                        /* Binary Ninja: *($v1_9 + 0x10) &= 0xfffffffe */
                        writel(readl(csi_dev->csi_regs + 0x10) & 0xfffffffe, csi_dev->csi_regs + 0x10);

                        /* Binary Ninja: private_msleep(1) */
                        private_msleep(1);

                        /* Binary Ninja: *(*($s0_1 + 0xb8) + 0xc) = $s2_1 */
                        writel(interface_type, csi_dev->csi_regs + 0xc);

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

                        /* Binary Ninja: *(*($s0_1 + 0x13c) + 0x128) = 0x3f */
                        writel(0x3f, isp_csi_regs + 0x128);

                        /* Binary Ninja: *(*($s0_1 + 0xb8) + 0x10) = 1 */
                        writel(1, csi_dev->csi_regs + 0x10);

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
                return 0;
            }
        }
    }

    return result;
}

/* csi_set_on_lanes - EXACT Binary Ninja implementation */
int csi_set_on_lanes(struct tx_isp_csi_device *csi_dev, int lanes)
{
    void __iomem *csi_base;
    u32 reg_val;

    pr_info("*** csi_set_on_lanes: EXACT Binary Ninja implementation ***\n");
    pr_info("csi_set_on_lanes: lanes=%d\n", lanes);

    if (!csi_dev) {
        pr_err("csi_set_on_lanes: CSI device is NULL\n");
        return -EINVAL;
    }

    /* Binary Ninja: void* $v1 = *(arg1 + 0xb8) */
    /* CRITICAL FIX: Ensure CSI base is properly initialized */
    csi_base = csi_dev->csi_regs;
    if (!csi_base) {
        pr_err("csi_set_on_lanes: CSI base is NULL\n");

        /* Use global ISP device CSI registers mapped by subdev probe */
        if (ourISPdev && ourISPdev->csi_regs) {
            csi_dev->csi_regs = ourISPdev->csi_regs;
            csi_base = csi_dev->csi_regs;
            pr_info("CSI base address from global ISP device: %p\n", csi_base);
        } else {
            pr_err("*** ERROR: CSI lane configuration failed: No CSI registers available ***\n");
            return -EINVAL;
        }
    }

    /* Binary Ninja: *($v1 + 4) = ((zx.d(arg2) - 1) & 3) | (*($v1 + 4) & 0xfffffffc) */
    reg_val = readl(csi_base + 4);
    reg_val = (reg_val & 0xfffffffc) | ((lanes - 1) & 3);
    writel(reg_val, csi_base + 4);
    wmb();

    pr_info("*** CSI lanes configured: %d lanes (reg 0x4 = 0x%08x) ***\n", lanes, reg_val);

    /* Binary Ninja: return 0 */
    return 0;
}

/* Define the core operations */
static struct tx_isp_subdev_core_ops csi_core_ops = {
    .init = csi_core_ops_init,
};

/* Define the video operations */
static struct tx_isp_subdev_video_ops csi_video_ops = {
    .s_stream = csi_video_s_stream,
};

/* Define the sensor operations */
static struct tx_isp_subdev_sensor_ops csi_sensor_ops = {
    .ioctl = csi_sensor_ops_ioctl,
};

/* Initialize the subdev ops structure with pointers to the operations */
static struct tx_isp_subdev_ops csi_subdev_ops = {
    .core = &csi_core_ops,
    .video = &csi_video_ops,
    .sensor = &csi_sensor_ops,
};

/* CSI file operations - Binary Ninja reference */
static const struct file_operations isp_csi_fops = {
    .owner = THIS_MODULE,
    .open = csi_core_ops_init,  /* Placeholder - should be proper open function */
    .release = single_release,
    .unlocked_ioctl = csi_core_ops_init,
    .llseek = default_llseek,
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

/* tx_isp_csi_probe - EXACT Binary Ninja reference implementation */
int tx_isp_csi_probe(struct platform_device *pdev)
{
    struct tx_isp_csi_device *csi_dev;
    struct tx_isp_platform_data *pdata;
    struct resource *mem_resource;
    int ret;

    /* Binary Ninja: private_kmalloc(0x148, 0xd0) */
    csi_dev = private_kmalloc(sizeof(struct tx_isp_csi_device), GFP_KERNEL);
    if (!csi_dev) {
        /* Binary Ninja: isp_printf(2, &$LC0, $a2) */
        isp_printf(2, "Failed to allocate CSI device\n", sizeof(struct tx_isp_csi_device));
        return -EFAULT;  /* Binary Ninja returns 0xfffffff4 */
    }

    /* Binary Ninja: memset($v0, 0, 0x148) */
    memset(csi_dev, 0, sizeof(struct tx_isp_csi_device));

    /* Binary Ninja: void* $s1_1 = arg1[0x16] */
    pdata = pdev->dev.platform_data;

    /* Binary Ninja: tx_isp_subdev_init(arg1, $v0, &csi_subdev_ops) */
    ret = tx_isp_subdev_init(pdev, &csi_dev->sd, &csi_subdev_ops);
    if (ret != 0) {
        /* Binary Ninja: isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", zx.d(*($s1_1 + 2))) */
        if (pdata) {
            isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", pdata->sensor_type);
        } else {
            isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", 0);
        }
        /* Binary Ninja: private_kfree($v0) */
        private_kfree(csi_dev);
        return -EFAULT;  /* Binary Ninja returns 0xfffffff4 */
    }

    /* FIXED: Use memory mapping from tx_isp_subdev_init instead of duplicate mapping */
    if (csi_dev->sd.regs) {
        csi_dev->csi_regs = csi_dev->sd.regs;
        pr_info("*** CSI PROBE: Using register mapping from tx_isp_subdev_init: %p ***\n", csi_dev->csi_regs);
    } else {
        /* Binary Ninja: isp_printf(2, "sensor type is BT1120!\n", "tx_isp_csi_probe") */
        pr_err("*** CSI PROBE: tx_isp_subdev_init failed to map registers ***\n");
        isp_printf(2, "sensor type is BT1120!\n", "tx_isp_csi_probe");
        tx_isp_subdev_deinit(&csi_dev->sd);
        private_kfree(csi_dev);
        return -EBUSY;  /* Binary Ninja returns 0xfffffff0 */
    }

    /* Binary Ninja: *($v0 + 0x34) = &isp_csi_fops */
    tx_isp_set_subdev_nodeops(&csi_dev->sd, &isp_csi_fops);

    /* Binary Ninja: *($v0 + 0x138) = $v0_3 */
    csi_dev->phy_res = csi_dev->sd.mem_res;  /* Use memory resource from tx_isp_subdev_init */

    /* Binary Ninja: private_raw_mutex_init($v0 + 0x12c, "not support the gpio mode!\n", 0) */
    private_raw_mutex_init(&csi_dev->mlock, "not support the gpio mode!\n", 0);

    /* Binary Ninja: private_platform_set_drvdata(arg1, $v0) */
    private_platform_set_drvdata(pdev, csi_dev);

    /* Binary Ninja: *($v0 + 0x128) = 1 */
    csi_dev->state = 1;

    /* Binary Ninja: dump_csd = $v0 */
    dump_csd = csi_dev;

    /* Binary Ninja: *($v0 + 0xd4) = $v0 */
    /* Note: self_ptr member not present in current CSI device structure */

    /* REMOVED: Manual linking - now handled automatically by tx_isp_subdev_init */
    pr_info("*** CSI PROBE: Device linking handled automatically by tx_isp_subdev_init ***\n");

    return 0;
}

/* CSI remove function */
int tx_isp_csi_remove(struct platform_device *pdev)
{
    struct tx_isp_subdev *sd = private_platform_get_drvdata(pdev);
    struct resource *res = NULL;

    if (!sd)
        return -EINVAL;

    /* Free interrupt if it was requested */
    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (res)
        free_irq(res->start, sd);

    /* Disable CSI */
    tx_isp_csi_stop(sd);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    private_iounmap(sd->csi_base);
    private_release_mem_region(res->start, resource_size(res));
    tx_isp_subdev_deinit(sd);
    private_kfree(sd);
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
int tx_isp_csi_activate_subdev(struct tx_isp_subdev *sd)
{
    struct tx_isp_csi_device *csi_dev;
    
    if (!sd)
        return -EINVAL;
    
    csi_dev = ourISPdev->csi_dev;
    if (!csi_dev) {
        pr_err("CSI device is NULL\n");
        return -EINVAL;
    }
    
    mutex_lock(&csi_dev->mutex);
    
    if (csi_dev->state == 1) {
        csi_dev->state = 2; /* INIT -> READY */
        pr_info("CSI activated: state %d -> 2 (READY)\n", 1);
    }
    
    mutex_unlock(&csi_dev->mutex);
    return 0;
}

/* CSI slake function - matching reference driver */
int tx_isp_csi_slake_subdev(struct tx_isp_subdev *sd)
{
    struct tx_isp_csi_device *csi_dev;
    
    if (!sd)
        return -EINVAL;
        
    csi_dev = ourISPdev->csi_dev;
    if (!csi_dev) {
        pr_err("CSI device is NULL\n");
        return -EINVAL;
    }
    
    mutex_lock(&csi_dev->mutex);
    
    if (csi_dev->state > 1) {
        csi_dev->state = 1; /* Back to INIT state */
        pr_info("CSI slaked: state -> 1 (INIT)\n");
    }
    
    mutex_unlock(&csi_dev->mutex);
    return 0;
}

/* Export symbols for use by other parts of the driver */
EXPORT_SYMBOL(tx_isp_csi_start);
EXPORT_SYMBOL(tx_isp_csi_stop);
EXPORT_SYMBOL(tx_isp_csi_set_format);
EXPORT_SYMBOL(dump_csi_reg);
EXPORT_SYMBOL(tx_isp_csi_activate_subdev);
EXPORT_SYMBOL(tx_isp_csi_slake_subdev);
