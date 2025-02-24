#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx-isp-device.h"


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

static struct tx_isp_subdev_ops csi_subdev_ops = { 0 }; // All fields NULL/0

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

