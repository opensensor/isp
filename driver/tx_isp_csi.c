#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx-isp-device.h"

/* Forward declarations */
int csi_core_ops_init(struct tx_isp_subdev *sd, int enable);
void dump_csi_reg(struct tx_isp_subdev *sd);
void check_csi_error(struct tx_isp_subdev *sd);
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
    struct csi_device *csi_dev;
    void __iomem *csi_base;
    u32 status, err1, err2, phy_state;
    irqreturn_t ret = IRQ_NONE;
    unsigned long flags;

    if (!sd)
        return IRQ_NONE;

    csi_dev = (struct csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev)
        return IRQ_NONE;

    csi_base = *(void **)(((char *)csi_dev) + 0x13c);
    if (!csi_base)
        return IRQ_NONE;

    spin_lock_irqsave(&csi_dev->lock, flags);

    /* Read interrupt status registers */
    err1 = readl(csi_base + 0x20);  /* ERR1 register */
    err2 = readl(csi_base + 0x24);  /* ERR2 register */
    phy_state = readl(csi_base + 0x14);  /* PHY_STATE register */

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

    if (!sd)
        return -EINVAL;

    mutex_lock(&sd->csi_lock);

    /* Enable CSI */
    ctrl = csi_read32(CSI_CTRL);
    ctrl |= CSI_CTRL_EN;
    csi_write32(CSI_CTRL, ctrl);

    /* Enable interrupts */
    csi_write32(CSI_INT_MASK, ~(INT_ERROR | INT_FRAME_DONE));

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

/* CSI video streaming control */
int csi_video_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_sensor_attribute *attr;
    struct csi_device *csi_dev;
    int ret = 0;

    if (!sd)
        return -EINVAL;

    /* Get the CSI device from the subdevice */
    csi_dev = (struct csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev) {
        pr_err("CSI device is NULL\n");
        
        /* Try to get the CSI device from ourISPdev as a fallback */
        if (ourISPdev && ourISPdev->csi_dev) {
            csi_dev = ourISPdev->csi_dev;
            pr_info("Using CSI device from ourISPdev: %p\n", csi_dev);
            
            /* Update the subdevice data with the CSI device */
            tx_isp_set_subdevdata(sd, csi_dev);
        } else {
            return -EINVAL;
        }
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
        sensor->attr.total_width = 1920;  /* Default to 1080p */
        sensor->attr.total_height = 1080;
        
        /* Store in subdevice */
        sd->active_sensor = sensor;
        attr = &sensor->attr;
        
        pr_info("Created default sensor attribute: 1920x1080, MIPI, RAW10, 2 lanes\n");
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

    /* Set state based on enable flag */
    csi_dev->state = enable ? 4 : 3;

    return 0;
}

/* CSI sensor operations IOCTL handler */
int csi_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    struct csi_device *csi_dev;

    if (!sd)
        return -EINVAL;

    /* Get the CSI device from the subdevice */
    csi_dev = (struct csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev) {
        pr_err("CSI device is NULL\n");
        return -EINVAL;
    }

    switch (cmd) {
    case TX_ISP_EVENT_SENSOR_RESIZE:  /* This is the correct event for reset */
        /* Reset CSI */
        if (csi_dev->state >= 3) {
            csi_dev->state = 3;
        }
        break;
    case TX_ISP_EVENT_SENSOR_FPS:
        /* Update FPS */
        if (csi_dev->state >= 3) {
            csi_dev->state = 4;
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

/* Frame rate detection and PHY timing configuration - Critical for proper CSI operation */
static int tx_isp_csi_detect_frame_rate_and_configure_phy(void __iomem *csi_base,
                                                         struct tx_isp_sensor_attribute *attr)
{
    int frame_rate = 30; /* Default frame rate */
    u32 phy_timing_value = 1; /* Default PHY timing */
    u32 current_val, new_val;
    
    /* Try to detect frame rate from sensor attributes */
    if (attr && attr->max_fps > 0) {
        frame_rate = attr->max_fps;
    } else if (ourISPdev && ourISPdev->sensor_width > 0 && ourISPdev->sensor_height > 0) {
        /* Estimate frame rate based on resolution */
        u32 pixel_count = ourISPdev->sensor_width * ourISPdev->sensor_height;
        if (pixel_count <= (640 * 480)) {
            frame_rate = 60;
        } else if (pixel_count <= (1280 * 720)) {
            frame_rate = 45;
        } else if (pixel_count <= (1920 * 1080)) {
            frame_rate = 30;
        } else {
            frame_rate = 15;
        }
    }
    
    pr_info("Detected frame rate: %d fps\n", frame_rate);
    
    /* PHY timing configuration based on frame rate - matches reference driver logic */
    if (frame_rate >= 80 && frame_rate < 110) {
        phy_timing_value = 1;
    } else if (frame_rate >= 110 && frame_rate < 150) {
        phy_timing_value = 2;
    } else if (frame_rate >= 150 && frame_rate < 200) {
        phy_timing_value = 3;
    } else if (frame_rate >= 200 && frame_rate < 250) {
        phy_timing_value = 4;
    } else if (frame_rate >= 250 && frame_rate < 300) {
        phy_timing_value = 5;
    } else if (frame_rate >= 300 && frame_rate < 400) {
        phy_timing_value = 6;
    } else if (frame_rate >= 400 && frame_rate < 500) {
        phy_timing_value = 7;
    } else if (frame_rate >= 500 && frame_rate < 600) {
        phy_timing_value = 8;
    } else if (frame_rate >= 600 && frame_rate < 700) {
        phy_timing_value = 9;
    } else if (frame_rate >= 700 && frame_rate < 800) {
        phy_timing_value = 10;
    } else if (frame_rate >= 800 && frame_rate < 1000) {
        phy_timing_value = 11;
    } else {
        /* Default value for other ranges */
        phy_timing_value = 1;
    }
    
    pr_info("Using PHY timing value: %d for frame rate %d\n", phy_timing_value, frame_rate);
    
    /* Configure the critical PHY timing registers - this is what was missing! */
    current_val = readl(csi_base + 0x160);
    new_val = (current_val & 0xfffffff0) | (phy_timing_value & 0xf);
    writel(new_val, csi_base + 0x160);
    wmb();
    
    /* Mirror the value to other PHY timing registers as per reference driver */
    writel(new_val, csi_base + 0x1e0);
    wmb();
    writel(new_val, csi_base + 0x260);
    wmb();
    
    pr_info("PHY timing configured: 0x160=0x%08x, 0x1e0=0x%08x, 0x260=0x%08x\n",
            readl(csi_base + 0x160), readl(csi_base + 0x1e0), readl(csi_base + 0x260));
    
    return 0;
}

/* CSI core operations initialization - matching reference driver implementation */
int csi_core_ops_init(struct tx_isp_subdev *sd, int enable)
{
    void __iomem *csi_base;
    struct tx_isp_sensor_attribute *attr;
    struct csi_device *csi_dev;
    int ret = 0;

    if (!sd)
        return -EINVAL;

    /* Get the CSI device from the subdevice */
    csi_dev = (struct csi_device *)tx_isp_get_subdevdata(sd);
    if (!csi_dev) {
        pr_err("CSI device is NULL\n");
        return -EINVAL;
    }

    /* Check if state is valid - match binary implementation */
    if (*(int *)(((char *)csi_dev) + 0x128) < 2) {
        pr_info("CSI device state is %d, setting to 2 (READY)\n",
                *(int *)(((char *)csi_dev) + 0x128));
        *(int *)(((char *)csi_dev) + 0x128) = 2;
    }

    /* Get the CSI base address from offset 0x13c */
    csi_base = *(void **)(((char *)csi_dev) + 0x13c);
    if (!csi_base) {
        pr_err("CSI base address is NULL\n");
        return -EINVAL;
    }

    /* Get the sensor attribute */
    if (sd->active_sensor) {
        attr = &sd->active_sensor->attr;
    } else if (ourISPdev && ourISPdev->sensor_sd && ourISPdev->sensor_sd->active_sensor) {
        attr = &ourISPdev->sensor_sd->active_sensor->attr;
    } else {
        pr_err("Active sensor is NULL\n");
        return -EINVAL;
    }

    pr_info("CSI core ops init: enable=%d, dbus_type=%d\n",
            enable, attr->dbus_type);

    if (enable) {
        /* Initialize CSI for streaming */
        if (attr->dbus_type == TX_SENSOR_DATA_INTERFACE_MIPI) {
            pr_info("Initializing CSI for MIPI sensor with %d lanes\n",
                    attr->mipi.lans);
            
            /* STEP 1: Configure lane count first */
            writel((readl(csi_base + 0x04) & 0xfffffffc) | ((attr->mipi.lans - 1) & 0x3),
                   csi_base + 0x04);
            wmb();
            private_msleep(1);

            /* STEP 2: Reset PHY - Proper sequence is critical */
            /* PHY_SHUTDOWNZ = 0 */
            writel(0, csi_base + 0x08);
            wmb();
            private_msleep(1);
            
            /* DPHY_RSTZ = 0 */
            writel(0, csi_base + 0x0C);
            wmb();
            private_msleep(1);
            
            /* CSI2_RESETN = 0 */
            writel(0, csi_base + 0x10);
            wmb();
            private_msleep(1);

            /* STEP 3: CRITICAL - Frame rate detection and PHY timing configuration */
            /* This is the missing piece that causes green stream! */
            ret = tx_isp_csi_detect_frame_rate_and_configure_phy(csi_base, attr);
            if (ret) {
                pr_err("Failed to configure PHY timing\n");
                return ret;
            }

            /* STEP 4: Configure base PHY timing parameters */
            writel(0x7d, csi_base + 0x00);  /* VERSION/BASE register */
            wmb();
            writel(0x3f, csi_base + 0x128); /* PHY timing parameter */
            wmb();
            
            /* STEP 5: Enable PHY - Correct power-up sequence */
            /* DPHY_RSTZ = 1 first */
            writel(1, csi_base + 0x0C);
            wmb();
            private_msleep(10);
            
            /* PHY_SHUTDOWNZ = 1 next */
            writel(1, csi_base + 0x08);
            wmb();
            private_msleep(10);
            
            /* CSI2_RESETN = 1 last */
            writel(1, csi_base + 0x10);
            wmb();
            private_msleep(10);
            
            /* STEP 6: Check PHY status */
            pr_info("CSI PHY initialized: PHY_STATE=0x%08x\n", readl(csi_base + 0x14));
            
            /* STEP 7: Configure data format based on sensor format */
            u32 data_type;
            u32 format_value = 0;
            
            /* First check if the sensor has a specific data type set */
            if (attr->mipi.mipi_sc.data_type_en && attr->mipi.mipi_sc.data_type_value) {
                /* Use the explicitly provided data type */
                data_type = attr->mipi.mipi_sc.data_type_value;
                pr_info("Using explicit data type: 0x%02x\n", data_type);
            } else {
                /* Map sensor format to MIPI CSI-2 data type - match OEM implementation */
                switch (attr->mipi.mipi_sc.sensor_csi_fmt) {
                    case TX_SENSOR_RAW8:
                        data_type = 0x2A;  /* RAW8 format */
                        format_value = 0;
                        pr_info("Using RAW8 format (0x2A)\n");
                        break;
                    case TX_SENSOR_RAW10:
                        data_type = 0x2B;  /* RAW10 format */
                        format_value = 1;
                        pr_info("Using RAW10 format (0x2B)\n");
                        break;
                    case TX_SENSOR_RAW12:
                        data_type = 0x2C;  /* RAW12 format */
                        format_value = 2;
                        pr_info("Using RAW12 format (0x2C)\n");
                        break;
                    case TX_SENSOR_YUV422:
                        data_type = 0x1E;  /* YUV422 format */
                        format_value = 3;
                        pr_info("Using YUV422 format (0x1E)\n");
                        break;
                    default:
                        /* Default to RAW10 if format is unknown */
                        data_type = 0x2B;  /* RAW10 format */
                        format_value = 1;
                        pr_info("Unknown sensor format: %d, defaulting to RAW10 (0x2B)\n",
                                attr->mipi.mipi_sc.sensor_csi_fmt);
                        break;
                }
            }
            
            /* Configure data format registers */
            writel(data_type, csi_base + 0x18);  /* DATA_IDS_1 register */
            wmb();
            writel(data_type, csi_base + 0x1C);  /* DATA_IDS_2 register */
            wmb();
            
            /* Store format value in ISP device for VIC to use */
            if (ourISPdev) {
                ourISPdev->sensor_format = format_value;
                pr_info("Stored sensor format %d for VIC to use\n", format_value);
            }
            
            /* STEP 8: Configure error detection and interrupt masks */
            writel(0, csi_base + 0x28);     /* MASK1 register - Enable error interrupts */
            writel(0, csi_base + 0x2C);     /* MASK2 register - Enable error interrupts */
            wmb();
            
            /* STEP 9: Final CSI controller enable */
            writel(0x1, csi_base + 0x40);   /* CSI_CTRL register - Enable CSI */
            wmb();
            private_msleep(10);

            /* STEP 10: Set state to 3 (active/streaming) */
            *(int *)(((char *)csi_dev) + 0x128) = 3;
            
            /* STEP 11: Dump registers and check for errors */
            dump_csi_reg(sd);
            check_csi_error(sd);
            
            pr_info("CSI initialized successfully for MIPI sensor\n");
            
            /* Allow CSI to stabilize before checking errors */
            private_msleep(10);
            check_csi_error(sd);
        } else if (attr->dbus_type == TX_SENSOR_DATA_INTERFACE_DVP) {
            pr_info("Initializing CSI for DVP sensor\n");
            
            /* DVP mode initialization - different from MIPI */
            writel(0, csi_base + 0x0C);    /* Reset PHY */
            wmb();
            private_msleep(1);
            
            writel(1, csi_base + 0x0C);    /* Release PHY reset */
            wmb();
            private_msleep(1);
            
            /* Configure DVP-specific registers - match reference driver */
            writel(0x7d, csi_base + 0x00);  /* VERSION register */
            wmb();
            writel(0x3e, csi_base + 0x80);  /* DVP PHY timing parameter */
            wmb();
            writel(1, csi_base + 0x2cc);    /* DVP PHY control */
            wmb();
            
            *(int *)(((char *)csi_dev) + 0x128) = 3;
            pr_info("CSI initialized for DVP sensor\n");
        } else {
            pr_warn("Unsupported sensor interface type: %d\n", attr->dbus_type);
            return -EINVAL;
        }
    } else {
        /* Disable CSI */
        pr_info("Disabling CSI\n");
        
        /* Disable CSI controller first */
        writel(0, csi_base + 0x40);         /* CSI_CTRL register - Disable CSI */
        wmb();
        udelay(10);
        
        /* Then disable PHY in reverse order of enable */
        writel(0, csi_base + 0x10);         /* CSI2_RESETN = 0 */
        wmb();
        udelay(10);
        
        writel(0, csi_base + 0x08);         /* PHY_SHUTDOWNZ = 0 */
        wmb();
        udelay(10);
        
        writel(0, csi_base + 0x0C);         /* DPHY_RSTZ = 0 */
        wmb();
        udelay(10);
        
        *(int *)(((char *)csi_dev) + 0x128) = 2;
        pr_info("CSI disabled\n");
    }

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
    struct tx_isp_subdev *sd = NULL;
    struct resource *res;
    int32_t ret = 0;

    printk("tx_isp_csi_probe\n");

    // Add resources to platform device if not already present
    if (!platform_get_resource(pdev, IORESOURCE_MEM, 0)) {
        ret = platform_device_add_resources(pdev, tx_isp_csi_resources,
                                          ARRAY_SIZE(tx_isp_csi_resources));
        if (ret) {
            ISP_ERROR("Failed to add CSI resources\n");
            return ret;
        }
    }

    sd = private_kmalloc(sizeof(struct tx_isp_subdev), GFP_KERNEL);
    if (!sd) {
        ISP_ERROR("Failed to allocate CSI subdev\n");
        return -ENOMEM;
    }

    memset(sd, 0, sizeof(struct tx_isp_subdev));

    // Now we can safely get the resource
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        ISP_ERROR("No memory resource specified\n");
        ret = -ENODEV;
        goto err_deinit_sd;
    }

    sd->base = ioremap(res->start, resource_size(res));
    if (!sd->base) {
        ISP_ERROR("Failed to map CSI registers\n");
        ret = -ENOMEM;
        goto err_release_mem;
    }

    private_raw_mutex_init(&sd->csi_lock, "csi_lock", NULL);
    private_platform_set_drvdata(pdev, sd);

    return 0;

err_release_mem:
    release_mem_region(res->start, resource_size(res));
err_deinit_sd:
    tx_isp_subdev_deinit(sd);
    private_kfree(sd);
    return ret;
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
struct csi_device *csi_dev;
void __iomem *csi_base;

if (!sd) {
    pr_err("dump_csi_reg: sd is NULL\n");
    return;
}

csi_dev = (struct csi_device *)tx_isp_get_subdevdata(sd);
if (!csi_dev) {
    pr_err("dump_csi_reg: csi_dev is NULL\n");
    return;
}

csi_base = *(void **)(((char *)csi_dev) + 0x13c);
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

/* CSI error checking function */
void check_csi_error(struct tx_isp_subdev *sd)
{
struct csi_device *csi_dev;
void __iomem *csi_base;
u32 err1, err2, phy_state;

if (!sd) {
    pr_err("check_csi_error: sd is NULL\n");
    return;
}

csi_dev = (struct csi_device *)tx_isp_get_subdevdata(sd);
if (!csi_dev) {
    pr_err("check_csi_error: csi_dev is NULL\n");
    return;
}

csi_base = *(void **)(((char *)csi_dev) + 0x13c);
if (!csi_base) {
    pr_err("check_csi_error: csi_base is NULL\n");
    return;
}

/* Read error registers */
err1 = readl(csi_base + 0x20);
err2 = readl(csi_base + 0x24);
phy_state = readl(csi_base + 0x14);

/* Check for PHY errors */
if (phy_state & 0x1) {
    pr_info("CSI PHY: Clock lane in Stop State\n");
}
if (phy_state & 0x10) {
    pr_info("CSI PHY: Data lane 0 in Stop State\n");
}
if (phy_state & 0x100) {
    pr_info("CSI PHY: Data lane 1 in Stop State\n");
}

/* Check for protocol errors */
if (err1) {
    pr_warn("CSI ERR1 (Protocol errors): 0x%08x\n", err1);
    if (err1 & 0x1) pr_warn("  - SOT Error\n");
    if (err1 & 0x2) pr_warn("  - SOTHS Error\n");
    if (err1 & 0x4) pr_warn("  - ECC Single-bit Error\n");
    if (err1 & 0x8) pr_warn("  - ECC Multi-bit Error\n");
    if (err1 & 0x10) pr_warn("  - CRC Error\n");
    if (err1 & 0x20) pr_warn("  - Packet Size Error\n");
    if (err1 & 0x40) pr_warn("  - ECC Corrected Error\n");
    
    /* Clear errors by writing back */
    writel(err1, csi_base + 0x20);
}

if (err2) {
    pr_warn("CSI ERR2 (Application errors): 0x%08x\n", err2);
    if (err2 & 0x1) pr_warn("  - Data ID Error\n");
    if (err2 & 0x2) pr_warn("  - Frame Sync Error\n");
    if (err2 & 0x4) pr_warn("  - Frame Data Error\n");
    
    /* Clear errors by writing back */
    writel(err2, csi_base + 0x24);
}

if (!err1 && !err2) {
    pr_info("CSI: No errors detected\n");
}
}

/* CSI activation function - matching reference driver */
int tx_isp_csi_activate_subdev(struct tx_isp_subdev *sd)
{
    struct csi_device *csi_dev;
    
    if (!sd)
        return -EINVAL;
    
    csi_dev = (struct csi_device *)tx_isp_get_subdevdata(sd);
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
    struct csi_device *csi_dev;
    
    if (!sd)
        return -EINVAL;
        
    csi_dev = (struct csi_device *)tx_isp_get_subdevdata(sd);
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
EXPORT_SYMBOL(check_csi_error);
EXPORT_SYMBOL(tx_isp_csi_activate_subdev);
EXPORT_SYMBOL(tx_isp_csi_slake_subdev);


