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
    u32 status;

    if (!sd)
        return IRQ_NONE;

    /* Read and clear interrupt status */
    status = csi_read32(CSI_INT_STATUS);
    csi_write32(CSI_INT_STATUS, status);

    if (status & INT_ERROR) {
        ISP_ERROR("CSI error interrupt received\n");
    }

    return IRQ_HANDLED;
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

/* CSI core operations initialization - closely matching OEM implementation */
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
            
            /* Configure lane count first */
            writel((readl(csi_base + 0x04) & 0xfffffffc) | ((attr->mipi.lans - 1) & 0x3), 
                   csi_base + 0x04);
            wmb();
            udelay(10);

            /* Reset PHY - Proper sequence is critical */
            /* PHY_SHUTDOWNZ = 0 */
            writel(0, csi_base + 0x08);
            wmb();
            udelay(10);
            
            /* DPHY_RSTZ = 0 */
            writel(0, csi_base + 0x0C);
            wmb();
            udelay(10);
            
            /* CSI2_RESETN = 0 */
            writel(0, csi_base + 0x10);
            wmb();
            udelay(10);

            /* Configure PHY timing parameters - match OEM implementation */
            writel(0x7d, csi_base + 0x00);  /* VERSION register */
            writel(0x3f, csi_base + 0x128); /* PHY timing parameter */
            
            /* Enable PHY - Correct power-up sequence */
            /* DPHY_RSTZ = 1 first */
            writel(1, csi_base + 0x0C);
            wmb();
            udelay(10);
            
            /* PHY_SHUTDOWNZ = 1 next */
            writel(1, csi_base + 0x08);
            wmb();
            udelay(10);
            
            /* CSI2_RESETN = 1 last */
            writel(1, csi_base + 0x10);
            wmb();
            udelay(10);
            
            /* Additional debug info */
            pr_info("CSI PHY initialized: PHY_STATE=%08x\n", readl(csi_base + 0x14));
            
            /* Configure data format based on sensor format */
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
                        /* Log the unknown format for debugging */
                        pr_info("Unknown sensor format: %d, trying all formats\n", 
                                attr->mipi.mipi_sc.sensor_csi_fmt);
                        
                        /* Try all possible formats one by one */
                        pr_info("Trying RAW8 format (0x2A)\n");
                        writel(0x2A, csi_base + 0x18);  /* DATA_IDS_1 register - RAW8 */
                        writel(0x2A, csi_base + 0x1C);  /* DATA_IDS_2 register - RAW8 */
                        wmb();
                        msleep(10);
                        
                        pr_info("Trying RAW10 format (0x2B)\n");
                        writel(0x2B, csi_base + 0x18);  /* DATA_IDS_1 register - RAW10 */
                        writel(0x2B, csi_base + 0x1C);  /* DATA_IDS_2 register - RAW10 */
                        wmb();
                        msleep(10);
                        
                        pr_info("Trying RAW12 format (0x2C)\n");
                        writel(0x2C, csi_base + 0x18);  /* DATA_IDS_1 register - RAW12 */
                        writel(0x2C, csi_base + 0x1C);  /* DATA_IDS_2 register - RAW12 */
                        wmb();
                        msleep(10);
                        
                        pr_info("Trying YUV422 format (0x1E)\n");
                        writel(0x1E, csi_base + 0x18);  /* DATA_IDS_1 register - YUV422 */
                        writel(0x1E, csi_base + 0x1C);  /* DATA_IDS_2 register - YUV422 */
                        wmb();
                        msleep(10);
                        
                        /* Default to RAW10 if format is unknown */
                        data_type = 0x2B;  /* RAW10 format */
                        format_value = 1;
                        pr_info("Defaulting to RAW10 (0x2B)\n");
                        break;
                }
            }
            
            /* Configure data format */
            writel(data_type, csi_base + 0x18);  /* DATA_IDS_1 register */
            wmb();
            
            /* Also set the second data ID register for completeness */
            writel(data_type, csi_base + 0x1C);  /* DATA_IDS_2 register */
            wmb();
            
            /* Store format value in ISP device for VIC to use */
            if (ourISPdev) {
                /* Store the format value in the ISP device */
                ourISPdev->sensor_format = format_value;
                pr_info("Stored sensor format %d for VIC to use\n", format_value);
            }
            
            /* Configure error masks */
            writel(0, csi_base + 0x28);     /* MASK1 register - Enable all error interrupts */
            writel(0, csi_base + 0x2C);     /* MASK2 register - Enable all error interrupts */
            wmb();
            
            /* Enable CSI controller */
            writel(0x1, csi_base + 0x40);   /* CSI_CTRL register - Enable CSI */
            wmb();
            udelay(10);

            /* Set state to 3 (streaming) */
            *(int *)(((char *)csi_dev) + 0x128) = 3;
            
            /* Dump registers for debugging */
            dump_csi_reg(sd);
            
            /* Check for CSI errors */
            check_csi_error(sd);
            
            pr_info("CSI initialized successfully for MIPI sensor\n");
            
            /* Add a delay to allow the CSI to stabilize */
            msleep(10);
            
            /* Check for CSI errors again after delay */
            check_csi_error(sd);
        } else if (attr->dbus_type == TX_SENSOR_DATA_INTERFACE_DVP) {
            pr_info("Initializing CSI for DVP sensor\n");
            
            /* Reset PHY */
            writel(0, csi_base + 0x0C);
            wmb();
            udelay(10);
            
            writel(1, csi_base + 0x0C);
            wmb();
            udelay(10);
            
            /* Configure DVP-specific registers */
            writel(0x7d, csi_base + 0x00);  /* VERSION register */
            writel(0x3e, csi_base + 0x80);  /* PHY timing parameter */
            writel(1, csi_base + 0x2cc);    /* PHY control */
            
            *(int *)(((char *)csi_dev) + 0x128) = 3;
            pr_info("CSI initialized for DVP sensor\n");
        } else {
            pr_info("Initializing CSI for other sensor type: %d\n", attr->dbus_type);
            
            /* Reset PHY */
            writel(0, csi_base + 0x0C);
            wmb();
            udelay(10);
            
            writel(1, csi_base + 0x0C);
            wmb();
            udelay(10);
            
            writel(0x7d, csi_base + 0x00);  /* VERSION register */
            writel(0x3e, csi_base + 0x80);  /* PHY timing parameter */
            writel(1, csi_base + 0x2cc);    /* PHY control */
            
            *(int *)(((char *)csi_dev) + 0x128) = 3;
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

