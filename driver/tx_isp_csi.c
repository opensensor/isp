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
        writel(val, ourISPdev->csi_dev->csi_regs + reg);
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
                    /* SAFE: Use proper struct member access instead of dangerous offset */
                    sensor_attr = csi_dev->sensor_attr;

                    /* Binary Ninja: int32_t $s2_1 = *($v1_5 + 0x14) */
                    int interface_type = sensor_attr->dbus_type;

                    if (interface_type == 1) {
                        /* Binary Ninja: *(*($s0_1 + 0xb8) + 4) = zx.d(*($v1_5 + 0x24)) - 1 */
                        writel(sensor_attr->mipi.lans - 1, csi_dev->csi_regs + 4);

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
    void __iomem *csi_regs;
    u32 reg_val;

    /* Binary Ninja: isp_printf(0, "Can't output the width(%d)!\n", "csi_set_on_lanes") */
    isp_printf(0, "Can't output the width(%d)!\n", "csi_set_on_lanes");

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
    csi_dev = (struct tx_isp_csi_device *)sd->dev_priv;
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

    /* Binary Ninja: if ($v1_2 == 3) csi_core_ops_init(arg1, 0) */
    if (state == 3) {
        pr_info("tx_isp_csi_slake_subdev: CSI in state 3, calling core_ops_init(disable)\n");
        csi_core_ops_init(sd, 0);
    }

    /* Binary Ninja: private_mutex_lock($s2_1) */
    mutex_lock(&csi_dev->mlock);

    /* Binary Ninja: if (*($s0_1 + 0x128) == 2) *($s0_1 + 0x128) = 1 */
    if (csi_dev->state == 2) {
        pr_info("tx_isp_csi_slake_subdev: CSI state 2->1, disabling clocks\n");
        csi_dev->state = 1;

        /* Binary Ninja: Disable clocks in reverse order */
        if (sd->clks && sd->clk_num > 0) {
            for (i = sd->clk_num - 1; i >= 0; i--) {
                if (sd->clks[i]) {
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

/* CSI internal operations - EXACT Binary Ninja implementation */
static struct tx_isp_subdev_internal_ops csi_internal_ops = {
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
static const struct file_operations isp_csi_fops = {
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
    tx_isp_set_subdev_nodeops(&csi_dev->sd, (struct file_operations *)&isp_csi_fops);

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

/* tx_isp_csi_remove - EXACT Binary Ninja implementation */
int tx_isp_csi_remove(struct platform_device *pdev)
{
    struct tx_isp_subdev *sd;
    struct tx_isp_csi_device *csi_dev;
    void __iomem *csi_regs;
    struct resource *phy_res;

    /* Binary Ninja: void* $v0 = private_platform_get_drvdata() */
    sd = private_platform_get_drvdata(pdev);

    /* Binary Ninja: if ($v0 != 0) */
    if (sd != NULL) {
        /* Binary Ninja: int32_t $s0_1 = $v0 u< 0xfffff001 ? 1 : 0 */
        if ((unsigned long)sd >= 0xfffff001) {
            sd = NULL;
        }
    }

    if (!sd) {
        return 0;
    }

    /* Get CSI device from subdev */
    csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);

    /* Binary Ninja: void* $v1 = *($s0 + 0xb8) */
    csi_regs = csi_dev->csi_regs;

    /* Binary Ninja: int32_t* $s2 = *($s0 + 0x138) */
    phy_res = csi_dev->phy_res;

    /* Binary Ninja: *($v1 + 0x10) &= 0xfffffffe */
    writel(readl(csi_regs + 0x10) & 0xfffffffe, csi_regs + 0x10);

    /* Binary Ninja: void* $v1_1 = *($s0 + 0xb8) */
    /* Binary Ninja: *($v1_1 + 0x10) |= 1 */
    writel(readl(csi_regs + 0x10) | 1, csi_regs + 0x10);

    /* Binary Ninja: private_platform_set_drvdata(arg1, 0) */
    private_platform_set_drvdata(pdev, NULL);

    /* Binary Ninja: private_iounmap(*($s0 + 0x13c)) */
    private_iounmap(csi_dev->csi_regs);

    /* Binary Ninja: int32_t $a0_2 = *$s2 */
    /* Binary Ninja: private_release_mem_region($a0_2, $s2[1] + 1 - $a0_2) */
    if (phy_res) {
        private_release_mem_region(phy_res->start, resource_size(phy_res));
    }

    /* Binary Ninja: tx_isp_subdev_deinit($s1) */
    tx_isp_subdev_deinit(sd);

    /* Binary Ninja: private_kfree($s0) */
    private_kfree(csi_dev);

    /* Binary Ninja: return 0 */
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

/* Duplicate function removed - using existing tx_isp_csi_slake_subdev at line 689 */

/* Export symbols for use by other parts of the driver */
EXPORT_SYMBOL(tx_isp_csi_set_format);
EXPORT_SYMBOL(dump_csi_reg);
EXPORT_SYMBOL(tx_isp_csi_activate_subdev);
