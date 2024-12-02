#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/delay.h>   // For msleep

#include <tx-isp-common.h>

#include "tx-isp.h"
#include "tx-isp-main.h"
#include "tx-isp-hw.h"


// Global state variable
static struct vic_dev *dump_vsd;

struct vic_dev {
    /* ... other fields ... */
    spinlock_t lock;                /* offset 0x130 */
    volatile u32 irq_enabled;       /* offset 0x13c */
    void (*irq_handler)(void *);    /* offset 0x84 */
    void (*irq_disable)(void *);    /* offset 0x88 */
    void *irq_priv;                 /* offset 0x80 */
};

/**
 * Enable VIC IRQ handling
 */
void tx_vic_enable_irq(void)
{
    struct vic_dev *dev;
    unsigned long flags;

    /* Validate global device pointer */
    if (!dump_vsd || (unsigned long)dump_vsd >= 0xfffff001)
        return;

    dev = dump_vsd;

    /* Lock IRQ state changes */
    spin_lock_irqsave(&dev->lock, flags);

    /* Only enable if not already enabled */
    if (!dev->irq_enabled) {
        dev->irq_enabled = 1;

        /* Call handler if registered */
        if (dev->irq_handler)
            dev->irq_handler(dev->irq_priv);
    }

    spin_unlock_irqrestore(&dev->lock, flags);
}

/**
 * Disable VIC IRQ handling
 */
//void tx_vic_disable_irq(void)
//{
//    struct vic_dev *dev;
//    unsigned long flags;
//    int force = 0;  /* matches var_14 in decompiled code */
//
//    /* Validate global device pointer */
//    if (!dump_vsd || (unsigned long)dump_vsd >= 0xfffff001)
//        return;
//
//    dev = dump_vsd;
//
//    /* Lock IRQ state changes */
//    spin_lock_irqsave(&dev->lock, flags);
//
//    if (!force) {
//        /* Only disable if currently enabled */
//        if (dev->irq_enabled) {
//            dev->irq_enabled = 0;
//
//            /* Call disable handler if registered */
//            if (dev->irq_disable)
//                dev->irq_disable(dev->irq_priv);
//        }
//    }
//
//    spin_unlock_irqrestore(&dev->lock, flags);
//}

/* Helper to initialize VIC IRQ handling */
int tx_vic_irq_init(struct vic_dev *dev,
                    void (*handler)(void *),
                    void (*disable)(void *),
                    void *priv)
{
    if (!dev)
        return -EINVAL;

    spin_lock_init(&dev->lock);
    dev->irq_enabled = 0;
    dev->irq_handler = handler;
    dev->irq_disable = disable;
    dev->irq_priv = priv;

    dump_vsd = dev;
    return 0;
}

static void tx_isp_store_periph_clock(struct IMPISPDev *dev, struct clk *clk)
{
    struct periph_clock *periph;
    unsigned long flags;

    periph = kzalloc(sizeof(*periph), GFP_KERNEL);
    if (!periph) {
        pr_err("Failed to allocate peripheral clock storage\n");
        return;
    }

    periph->clk = clk;
    INIT_LIST_HEAD(&periph->list);

    spin_lock_irqsave(&dev->clock_lock, flags);
    list_add_tail(&periph->list, &dev->periph_clocks);
    spin_unlock_irqrestore(&dev->clock_lock, flags);
}

/* Add this cleanup function to your driver cleanup path */
static void tx_isp_cleanup_periph_clocks(struct IMPISPDev *dev)
{
    struct periph_clock *periph, *tmp;
    unsigned long flags;

    spin_lock_irqsave(&dev->clock_lock, flags);
    list_for_each_entry_safe(periph, tmp, &dev->periph_clocks, list) {
        clk_disable_unprepare(periph->clk);
        clk_put(periph->clk);
        list_del(&periph->list);
        kfree(periph);
    }
    spin_unlock_irqrestore(&dev->clock_lock, flags);
}

/* Hardware initialization and management */
/* Hardware initialization and management */
int configure_isp_clocks(struct IMPISPDev *dev)
{
    struct device *device = dev->dev;
    struct device_node *np = device->of_node;
    int ret;

    pr_info("Configuring ISP system clocks\n");

    /* Core clocks - Critical for ISP operation */
    dev->isp_clk = devm_clk_get(device, "isp");
    if (IS_ERR(dev->isp_clk)) {
        ret = PTR_ERR(dev->isp_clk);
        pr_err("Failed to get ISP core clock: %d\n", ret);
        return ret;
    }

    dev->cgu_isp = devm_clk_get(device, "cgu_isp");
    if (IS_ERR(dev->cgu_isp)) {
        ret = PTR_ERR(dev->cgu_isp);
        pr_err("Failed to get CGU ISP clock: %d\n", ret);
        return ret;
    }

    /* Set and enable CGU ISP clock first */
    ret = clk_set_rate(dev->cgu_isp, T31_CGU_ISP_FREQ);
    if (ret) {
        pr_err("Failed to set CGU ISP clock rate: %d\n", ret);
        return ret;
    }

    ret = clk_prepare_enable(dev->cgu_isp);
    if (ret) {
        pr_err("Failed to enable CGU ISP clock: %d\n", ret);
        return ret;
    }

    /* Enable ISP core clock */
    ret = clk_prepare_enable(dev->isp_clk);
    if (ret) {
        pr_err("Failed to enable ISP core clock: %d\n", ret);
        goto err_disable_cgu;
    }

    /* Optional system clocks */
    dev->csi_clk = devm_clk_get(device, "csi");
    if (!IS_ERR(dev->csi_clk)) {
        ret = clk_prepare_enable(dev->csi_clk);
        if (ret) {
            pr_warn("Failed to enable CSI clock: %d\n", ret);
            dev->csi_clk = NULL;
        }
    } else {
        dev->csi_clk = NULL;
    }

    dev->ipu_clk = devm_clk_get(device, "ipu");
    if (!IS_ERR(dev->ipu_clk)) {
        ret = clk_prepare_enable(dev->ipu_clk);
        if (ret) {
            pr_warn("Failed to enable IPU clock: %d\n", ret);
            dev->ipu_clk = NULL;
        }
    } else {
        dev->ipu_clk = NULL;
    }

    /* Optional peripheral clocks from device tree */
    struct device_node *clk_node;
    const char *clk_name;
    u32 clk_rate;

    for_each_child_of_node(np, clk_node) {
        if (of_property_read_string(clk_node, "clock-name", &clk_name))
            continue;

        struct clk *periph_clk = devm_clk_get(device, clk_name);
        if (IS_ERR(periph_clk)) {
            pr_warn("Optional clock %s not available\n", clk_name);
            continue;
        }

        if (!of_property_read_u32(clk_node, "clock-rate", &clk_rate)) {
            ret = clk_set_rate(periph_clk, clk_rate);
            if (ret) {
                pr_warn("Failed to set %s clock rate: %d\n", clk_name, ret);
                continue;
            }
        }

        ret = clk_prepare_enable(periph_clk);
        if (ret) {
            pr_warn("Failed to enable %s clock: %d\n", clk_name, ret);
            continue;
        }

        // Store enabled peripheral clocks in a list if needed for cleanup
        tx_isp_store_periph_clock(dev, periph_clk);
    }

    /* Log configured clock rates */
    pr_info("Clock configuration completed. Rates:\n");
    pr_info("  ISP Core: %lu Hz\n", clk_get_rate(dev->isp_clk));
    pr_info("  CGU ISP: %lu Hz\n", clk_get_rate(dev->cgu_isp));
    if (dev->csi_clk)
        pr_info("  CSI: %lu Hz\n", clk_get_rate(dev->csi_clk));
    if (dev->ipu_clk)
        pr_info("  IPU: %lu Hz\n", clk_get_rate(dev->ipu_clk));

    return 0;

err_disable_cgu:
    clk_disable_unprepare(dev->cgu_isp);
    return ret;
}

void cleanup_isp_clocks(struct IMPISPDev *dev)
{
    /* First cleanup peripheral clocks */
    tx_isp_cleanup_periph_clocks(dev);

    /* Then cleanup core clocks */
    if (dev->cgu_isp) {
        clk_disable_unprepare(dev->cgu_isp);
        clk_put(dev->cgu_isp);
        dev->cgu_isp = NULL;
    }

    if (dev->isp_clk) {
        clk_disable_unprepare(dev->isp_clk);
        clk_put(dev->isp_clk);
        dev->isp_clk = NULL;
    }

    /* Cleanup any system clocks if present */
    if (dev->csi_clk) {
        clk_disable_unprepare(dev->csi_clk);
        clk_put(dev->csi_clk);
        dev->csi_clk = NULL;
    }

    if (dev->ipu_clk) {
        clk_disable_unprepare(dev->ipu_clk);
        clk_put(dev->ipu_clk);
        dev->ipu_clk = NULL;
    }
}

/* Read SoC information from hardware registers */
int read_soc_info(void __iomem *reg_base)
{
    u32 soc_id, cppsr, subsoctype, subremark;
    if (!reg_base)
        return -EINVAL;

    /* Read values from hardware registers */
    soc_id = readl(reg_base + (CPU_ID_ADDR - ISP_BASE_ADDR));
    cppsr = readl(reg_base + (CPPSR_ADDR - ISP_BASE_ADDR));
    subsoctype = readl(reg_base + (SUBTYPE_ADDR - ISP_BASE_ADDR));
    subremark = readl(reg_base + (SUBREMARK_ADDR - ISP_BASE_ADDR));

    /* Validate readings */
    if (soc_id == 0xFFFFFFFF || cppsr == 0xFFFFFFFF)
        return -EIO;

    pr_info("SoC ID: 0x%08x, CPPSR: 0x%08x\n", soc_id, cppsr);
    pr_info("Subtype: 0x%08x, Subremark: 0x%08x\n", subsoctype, subremark);

    return 0;  // Remove incorrect error path
}

/* Register access helpers */
u32 isp_reg_read(void __iomem *base, u32 offset)
{
    offset = ALIGN(offset, 4);  // Ensure 4-byte alignment
    return readl(base + offset);
}

void isp_reg_write(void __iomem *base, u32 offset, u32 value)
{
    offset = ALIGN(offset, 4);  // Ensure 4-byte alignment
    writel(value, base + offset);
}

int reset_vic(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    int timeout = 1000;
    uint32_t status;

    // First write 0 to control to ensure clean state
    writel(0, vic_regs + VIC_CTRL);
    wmb();
    udelay(100);

    // Clear any pending status
    writel(0xFFFFFFFF, vic_regs + VIC_STATUS);
    wmb();
    udelay(100);

    // Read status to verify clear
    status = readl(vic_regs + VIC_STATUS);
    pr_info("VIC status after clear: 0x%08x\n", status);

    return 0;
}

int vic_core_ops_init(struct IMPISPDev *dev, int enable)
{
    void __iomem *regs;
    unsigned long flags;
    int ret = 0;

    if (!dev || !dev->reg_base) {
        pr_err("Invalid device state\n");
        return -EINVAL;
    }

    regs = dev->reg_base;

    if (enable) {
        // Reset VIC first
        writel(0x0, regs + 0x300);
        wmb();
        udelay(100);
        writel(0x0, regs + 0x308);
        wmb();
        udelay(100);

        // Initialize VIC with frame done interrupt
        writel(0x1, regs + 0x308);
        wmb();
    } else {
        writel(0x0, regs + 0x300);
        writel(0x0, regs + 0x308);
        wmb();
    }

    spin_unlock_irqrestore(&dev->vic_lock, flags);
    return ret;
}

/* Buffer and DMA management */
int configure_isp_buffers(struct IMPISPDev *dev)
{
    struct isp_channel *chn = &dev->channels[0];
    void __iomem *regs = dev->reg_base;
    uint32_t buffer_start, buffer_size;
    uint32_t input_offset, y_offset, uv_offset;

    if (!dev || !chn) {
        pr_err("Invalid device state\n");
        return -EINVAL;
    }

    // Calculate aligned dimensions - match OEM alignment
    uint32_t width = ALIGN(chn->width, 32);
    uint32_t height = ALIGN(chn->height, 32);

    // RAW10 input calculations (match OEM stride calculation)
    uint32_t raw10_stride = ALIGN((width * 10) / 8, 32); // 10-bit packed
    uint32_t raw10_size = ALIGN(raw10_stride * height, 4096); // Page align

    // NV12 output calculations (match OEM layout)
    uint32_t y_stride = ALIGN(width, 32);
    uint32_t y_size = ALIGN(y_stride * height, 4096);
    uint32_t uv_stride = ALIGN(width, 32); // Full stride for UV
    uint32_t uv_size = ALIGN(uv_stride * height / 2, 4096);

    // Match OEM buffer layout starting at their offset
    buffer_start = ALIGN(dev->dma_sensor_addr + 0x1094d4, 32);
    input_offset = 0;
    y_offset = raw10_size; // Start Y after RAW10
    uv_offset = y_offset + y_size; // Start UV after Y
    buffer_size = ALIGN(raw10_size + y_size + uv_size, 4096);

    pr_info("ISP Buffer Layout:\n");
    pr_info("  Base addr: 0x%08x\n", buffer_start);
    pr_info("  RAW10: offset=0x%x stride=%d size=%d\n",
            input_offset, raw10_stride, raw10_size);
    pr_info("  Y: offset=0x%x stride=%d size=%d\n",
            y_offset, y_stride, y_size);
    pr_info("  UV: offset=0x%x stride=%d size=%d\n",
            uv_offset, uv_stride, uv_size);

    // Configure RAW10 input buffer
    writel(buffer_start + input_offset, regs + ISP_BUF0_OFFSET);
    writel(raw10_stride, regs + ISP_BUF0_OFFSET + 0x4);
    wmb();

    // Configure Y output buffer
    writel(buffer_start + y_offset, regs + ISP_BUF1_OFFSET);
    writel(y_stride, regs + ISP_BUF1_OFFSET + 0x4);
    wmb();

    // Configure UV output buffer
    writel(buffer_start + uv_offset, regs + ISP_BUF2_OFFSET);
    writel(uv_stride, regs + ISP_BUF2_OFFSET + 0x4);
    wmb();

    // Match OEM processing setup
    writel(1, regs + ISP_CTRL_PATH_REG);
    writel(0, regs + ISP_BYPASS_REG);
    writel(0x736b0, regs + ISP_FRAME_STATUS_REG); // Magic value from OEM
    writel(1, regs + ISP_FRAME_DONE_REG);
    wmb();

    // Set formats/dimensions
    writel(ISP_FMT_RAW10, regs + ISP_INPUT_FMT_REG);
    writel(ISP_FMT_NV12, regs + ISP_OUTPUT_FMT_REG);
    writel(width, regs + ISP_FRAME_WIDTH);
    writel(height, regs + ISP_FRAME_HEIGHT);
    wmb();

    // Store in frame channel
    chn->dma_addr = buffer_start;
    chn->buf_size = buffer_size;
    //fc->write_idx = 0;

    // Initialize completion mechanism
    // TODO
    //init_completion(&fc->frame_complete);

    // Enable hardware path
    writel(1, regs + 0x120); // Enable buffer
    writel(1, regs + 0x124); // Enable pipeline
    writel(1, regs + ISP_DMA_CTRL); // Enable DMA
    wmb();

    pr_info("ISP Configuration:\n");
    pr_info("  Frame done: 0x%x\n", readl(regs + ISP_FRAME_DONE_REG));
    pr_info("  Frame status: 0x%x\n", readl(regs + ISP_FRAME_STATUS_REG));
    pr_info("  Input format: 0x%x\n", readl(regs + ISP_INPUT_FMT_REG));
    pr_info("  Output format: 0x%x\n", readl(regs + ISP_OUTPUT_FMT_REG));

    return 0;
}

int configure_csi_for_streaming(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->reg_base;
    void __iomem *csi_base = regs + 0xb8;  // CSI register base from OEM offset
    u32 val;

    pr_info("Configuring ISP streaming hardware...\n");

    // 1. First disable everything and clear interrupts
    writel(0x0, regs + ISP_CTRL_REG);  // Disable core
    writel(0xFFFFFFFF, regs + ISP_INT_CLEAR_REG); // Clear IRQs
    wmb();
    msleep(1);

    // 2. Configure CSI with exact timing from decompiled code
    writel(0x0, csi_base + 0x0);   // VERSION = 0
    writel(0x1, csi_base + 0x4);   // N_LANES = 1 (2 lanes)
    writel(0x0, csi_base + 0x8);   // PHY_SHUTDOWNZ = 0
    writel(0x0, csi_base + 0xc);   // DPHY_RSTZ = 0
    writel(0x0, csi_base + 0x10);  // CSI2_RESETN = 0
    wmb();
    msleep(1);

    // Enable CSI in sequence from decompiled CSI core init
    writel(0x1, csi_base + 0x8);   // PHY_SHUTDOWNZ = 1
    wmb();
    msleep(1);

    writel(0x1, csi_base + 0xc);   // DPHY_RSTZ = 1
    wmb();
    msleep(1);

    writel(0x1, csi_base + 0x10);  // CSI2_RESETN = 1
    writel(0xc, csi_base + 0x1c);  // DATA_IDS_2 = 0xc
    wmb();
    msleep(10);

    // TODO
//    // 3. Set up DMA buffers
//    uint32_t y_stride = ALIGN(fs->width, 32);
//    uint32_t uv_stride = ALIGN(fs->width/2, 32);
//    uint32_t y_size = y_stride * fs->height;
//
//    // Y plane
//    writel(ALIGN(dma_addr, 8), regs + ISP_BUF0_OFFSET);
//    writel(y_stride, regs + ISP_BUF0_OFFSET + 0x4);
//
//    // UV plane
//    writel(ALIGN(dma_addr + y_size, 8), regs + ISP_BUF0_OFFSET + 0x8);
//    writel(uv_stride, regs + ISP_BUF0_OFFSET + 0xC);
//    wmb();

    // 4. Set format registers - use NV12
    writel(ISP_FMT_NV12, regs + ISP_INPUT_FORMAT_REG);
    writel(ISP_FMT_NV12, regs + ISP_OUTPUT_FORMAT_REG);
    wmb();

    // 5. Set up interrupts exactly as decompiled code shows
    writel(0x1, regs + ISP_INT_MASK_REG);  // Only frame done IRQ
    writel(0x100, regs + 0x28);  // MASK1 = 0x100
    writel(0x400040, regs + 0x2c); // MASK2 = 0x400040
    wmb();

    // 6. Finally enable core with interrupts
    val = 0x7;  // ENABLE | START | IRQ_EN from decompiled
    writel(val, regs + ISP_CTRL_REG);
    wmb();

    // Debug output
    pr_info("ISP Streaming Config:\n");
    pr_info("CSI: lanes=0x%x phy=0x%x dphy=0x%x csi2=0x%x\n",
            readl(csi_base + 0x4),  // N_LANES
            readl(csi_base + 0x8),  // PHY_SHUTDOWNZ
            readl(csi_base + 0xc),  // DPHY_RSTZ
            readl(csi_base + 0x10)); // CSI2_RESETN

//    pr_info("DMA: Y=0x%08x stride=%d UV=0x%08x stride=%d\n",
//            readl(regs + ISP_BUF0_OFFSET), y_stride,
//            readl(regs + ISP_BUF0_OFFSET + 0x8), uv_stride);

    pr_info("Control: 0x%x IRQ mask=0x%x\n",
            readl(regs + ISP_CTRL_REG),
            readl(regs + ISP_INT_MASK_REG));

    dump_csi_reg(dev);  // Full CSI register dump

    return 0;
}


void verify_isp_state(struct IMPISPDev *dev)
{
    void __iomem *isp_regs = dev->reg_base + ISP_BASE;
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;

    pr_info("ISP State:\n");
    pr_info("  Control: 0x%08x\n", readl(isp_regs + ISP_CTRL_OFFSET));
    pr_info("  Status: 0x%08x\n", readl(isp_regs + ISP_STATUS_OFFSET));
    pr_info("  Int mask: 0x%08x\n", readl(isp_regs + ISP_INT_MASK_OFFSET));

    pr_info("VIC State:\n");
    pr_info("  Control: 0x%08x\n", readl(vic_regs + VIC_CTRL));
    pr_info("  Mode: 0x%08x\n", readl(vic_regs + VIC_MODE));
    pr_info("  Route: 0x%08x\n", readl(vic_regs + VIC_ROUTE));
    pr_info("  Status: 0x%08x\n", readl(vic_regs + VIC_STATUS));
    pr_info("  IRQ Enable: 0x%08x\n", readl(vic_regs + VIC_IRQ_EN));
    pr_info("  IRQ Mask: 0x%08x\n", readl(vic_regs + VIC_IRQ_MASK));
    pr_info("  Frame Size: 0x%08x\n", readl(vic_regs + VIC_FRAME_SIZE));
    pr_info("  Frame Stride: 0x%08x\n", readl(vic_regs + VIC_FRAME_STRIDE));
    pr_info("  DMA Addr: 0x%08x\n", readl(vic_regs + VIC_DMA_ADDR));
}

// Helper function to dump VIC state for debugging
static void dump_vic_state(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;

    pr_info("VIC State:\n");
    pr_info("  Control: 0x%08x\n", readl(vic_regs + 0x0));
    pr_info("  Mode: 0x%08x\n", readl(vic_regs + 0x4));
    pr_info("  Route: 0x%08x\n", readl(vic_regs + 0x10));
    pr_info("  Status: 0x%08x\n", readl(vic_regs + 0xb4));
    pr_info("  IRQ Enable: 0x%08x\n", readl(vic_regs + 0x13c));
    pr_info("  IRQ Mask: 0x%08x\n", readl(vic_regs + 0x140));
    pr_info("  Frame Size: 0x%08x\n", readl(vic_regs + 0x100));
    pr_info("  Frame Stride: 0x%08x\n", readl(vic_regs + 0x104));
    pr_info("  DMA Addr: 0x%08x\n", readl(vic_regs + 0x1a0));
}

// Add to our ISP driver
static int detect_sensor_type(struct IMPISPDev *dev)
{
    struct tx_isp_sensor *sensor;
    u32 sensor_type = 0;

    // Get sensor directly from I2C client
    if (!dev->sensor_i2c_client) {
        pr_err("No sensor I2C client available\n");
        return -EINVAL;
    }

    sensor = i2c_get_clientdata(dev->sensor_i2c_client);
    if (!sensor) {
        pr_err("No sensor data available\n");
        return -EINVAL;
    }

    // Check interface type first
    if (sensor->video.attr->dbus_type == TX_SENSOR_DATA_INTERFACE_MIPI) {
        sensor_type = 1; // MIPI type (generic)
    } else if (sensor->video.attr->dbus_type == TX_SENSOR_DATA_INTERFACE_DVP) {
        // DVP sensors - check timing mode
        switch(sensor->video.attr->dvp.mode) {
            case SENSOR_DVP_TIMING_BT601:
                sensor_type = 3; // BT601
            break;

            case SENSOR_DVP_TIMING_BT656:
                sensor_type = 4; // BT656
            break;

            case SENSOR_DVP_TIMING_BT1120:
                sensor_type = 5; // BT1120
            break;

            default:
                pr_err("Unknown DVP timing mode %d\n",
                       sensor->video.attr->dvp.mode);
            return -EINVAL;
        }
    }

    pr_info("Detected sensor type %d based on interface %d mode %d\n",
            sensor_type,
            sensor->video.attr->dbus_type,
            sensor->video.attr->dvp.mode);

    // Store the type in hardware register and our dev structure
    writel(sensor_type, dev->reg_base + 0x110 + 0x14);
    wmb();
    dev->sensor_type = sensor_type;
    dev->sensor_mode = sensor->video.attr->dvp.mode;
    dev->sensor_interface_type = sensor->video.attr->dbus_type;

    return 0;
}



int configure_vic_for_streaming(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    void __iomem *isp_regs = dev->reg_base;
    void __iomem *sensor_info = dev->reg_base + 0x110;
    u32 status, route_config;
    int ret = 0;
    int timeout;

    // Reset control first
    writel(2, vic_regs + 0x0);  // OEM starts with reset state
    wmb();
    udelay(100);

    // Detect sensor type first
    detect_sensor_type(dev);
    u32 sensor_type = dev->sensor_type;
    u32 interface_type = dev->sensor_interface_type;
    u32 mode = dev->sensor_mode;

    pr_info("Detected sensor type %d based on interface %d mode %d (0x%x)\n",
            sensor_type, interface_type, mode, mode);

    // Configure based on sensor type
    switch (sensor_type) {
    case 1: // MIPI
        {
            if (mode == 0x195) {
                pr_info("Configuring for SC2336 MIPI mode\n");

                // Basic route config for non-Sony MIPI
                writel(0xa000a, vic_regs + 0x10);
                wmb();
                udelay(10);

                // Magic config matches route for SC2336
                writel(0xa000a, vic_regs + 0x1a4);
                wmb();
                udelay(10);

                // Frame configuration - SC2336 specific
                u32 frame_param = readl(sensor_info + 0x7c) & 0xff;
                u32 base_size = readl(sensor_info + 0x2c);
                u32 frame_mul = 8; // Default for SC2336

                // SC2336 frame handling
                u32 frame_size = frame_mul * base_size;
                u32 frame_value = (frame_size >> 5) + ((frame_size & 0x1f) ? 1 : 0);

                // Set frame parameters
                writel(frame_value, vic_regs + 0x100);
                writel(frame_size, vic_regs + 0x104);
                wmb();
                udelay(10);

                // DMA Setup
                u32 dma_addr = ALIGN(dev->dma_sensor_addr, 32);
                writel(dma_addr, vic_regs + 0x1a0);
                writel(0x4440, vic_regs + 0x1ac);
                wmb();
                udelay(10);

                // Clear status before state transitions
                writel(0xffffffff, vic_regs + 0xb4);
                wmb();
                udelay(10);

                // State transition sequence for SC2336
                writel(4, vic_regs + 0x0);  // Enable state
                wmb();
                udelay(100);

                timeout = 1000;
                while (readl(vic_regs + 0x0) != 0) {
                    if (timeout-- <= 0) {
                        pr_err("Timeout waiting for VIC ready\n");
                        dump_vic_state(dev);
                        ret = -ETIMEDOUT;
                        goto out;
                    }
                    udelay(1);
                }

                // Normal operation mode
                writel(1, vic_regs + 0x0);
                wmb();
                udelay(100);
            } else {
                pr_err("Unexpected MIPI mode: 0x%x\n", mode);
                ret = -EINVAL;
                goto out;
            }
        }
        break;

    case 3: // BT601
        {
            pr_info("sensor type is BT601!\n");

            u32 gpio_mode = readl(sensor_info + 0x18);
            if (gpio_mode > 1) {
                pr_err("not support the gpio mode!\n");
                ret = -EINVAL;
                goto out;
            }

            route_config = gpio_mode ? 0x88060820 : 0x800c8000;
            writel(gpio_mode ? gpio_mode : 1, vic_regs + 0xc);

            // BT601 register configuration
            u32 reg_dc = readl(dev->reg_base + 0xdc);
            u32 reg_e0 = readl(dev->reg_base + 0xe0);

            writel((reg_dc << 16) | reg_e0, vic_regs + 4);
            writel(route_config, vic_regs + 0x10);
            writel((reg_dc << 1) | 0x100000, vic_regs + 0x18);
            writel(0x30, vic_regs + 0x3c);
            writel(0x1b8, vic_regs + 0x1c);
            writel(0x1402d0, vic_regs + 0x30);
            writel(0x50014, vic_regs + 0x34);
            writel(0x2d00014, vic_regs + 0x38);
            writel(0, vic_regs + 0x1a0);
            writel(0x100010, vic_regs + 0x1a4);
            writel(0x4440, vic_regs + 0x1ac);
        }
        break;

    case 4: // BT656
    case 5: // BT1120
        {
            pr_info("sensor type is %s!\n", sensor_type == 4 ? "BT656" : "BT1120");

            writel(sensor_type == 4 ? 0 : 4, vic_regs + 0xc);

            if (readl(sensor_info + 0x18) != 0) {
                pr_err("not support the gpio mode!\n");
                ret = -EINVAL;
                goto out;
            }

            u32 reg_dc = readl(dev->reg_base + 0xdc);
            u32 reg_e0 = readl(dev->reg_base + 0xe0);

            writel((reg_dc << 16) | reg_e0, vic_regs + 4);
            writel(0x800c0000, vic_regs + 0x10);
            writel(reg_dc << 1, vic_regs + 0x18);
            writel(0x100010, vic_regs + 0x1a4);
            writel(0x4440, vic_regs + 0x1ac);

            if (sensor_type == 4) {
                writel(0x200, vic_regs + 0x1d0);
                writel(0x200, vic_regs + 0x1d4);
            }
        }
        break;

    default:
        pr_err("Unsupported sensor type: %d\n", sensor_type);
        ret = -EINVAL;
        goto out;
    }

    // Enable ISP streaming
    writel(0x1, isp_regs + ISP_STREAM_START);
    wmb();
    writel(0x1, isp_regs + ISP_STREAM_CTRL);
    wmb();

    pr_info("Final VIC State:\n");
    dump_vic_state(dev);

out:
    // Update status flag as seen in OEM code before enabling IRQs
    if (ret == 0) {
        dev->vic_status = 4;  // Matches OEM status update
    }
    return ret;
}


/* Power management */
int isp_power_on(struct IMPISPDev *dev)
{
    void __iomem *cpm_base;
    u32 val;

    /* Map CPM registers */
    cpm_base = ioremap(CPM_BASE, 0x100);
    if (!cpm_base) {
        pr_err("Failed to map CPM registers\n");
        return -ENOMEM;
    }

    /* Enable clocks */
    val = readl(cpm_base + CPM_CLKGR);
    val &= ~(CPM_ISP_CLK_GATE | CPM_MIPI_CLK_GATE);
    writel(val, cpm_base + CPM_CLKGR);

    /* Enable ISP power domain if needed */
    val = readl(cpm_base + CPM_CLKGR1);
    val &= ~BIT(2);  // ISP power domain bit
    writel(val, cpm_base + CPM_CLKGR1);

    /* Wait for power stabilization */
    usleep_range(1000, 2000);

    iounmap(cpm_base);
    return 0;
}


int isp_reset_hw(struct IMPISPDev *dev)
{
    u32 val;

    /* Assert reset */
    writel(0x1, dev->reg_base + ISP_RST_CTRL);
    usleep_range(1000, 2000);

    /* De-assert reset */
    writel(0x0, dev->reg_base + ISP_RST_CTRL);
    usleep_range(1000, 2000);

    /* Enable core clock */
    val = readl(dev->reg_base + ISP_CLK_CTRL);
    val |= 0x1;
    writel(val, dev->reg_base + ISP_CLK_CTRL);

    /* Enable power */
    val = readl(dev->reg_base + ISP_POWER_CTRL);
    val |= 0x1;
    writel(val, dev->reg_base + ISP_POWER_CTRL);

    /* Wait for power stabilization */
    usleep_range(5000, 10000);

    return 0;
}

/* Memory management */
int tx_isp_init_memory(struct IMPISPDev *dev)
{
    dma_addr_t aligned_base, mem_offset;
    size_t aligned_size;
    int ret = 0;

    /* Configure base memory parameters */
    aligned_base = ALIGN(0x02a80000, 32);  // Base from TX-ISP
    aligned_size = ALIGN(0x1580000, 32);   // ~22MB total

    /* Request memory region */
    if (!request_mem_region(aligned_base, aligned_size, "tx-isp")) {
        dev_err(dev->dev, "Failed to request ISP memory region\n");
        return -EBUSY;
    }

    /* Map the memory region */
    dev->dma_buf = devm_ioremap(dev->dev, aligned_base, aligned_size);
    if (!dev->dma_buf) {
        dev_err(dev->dev, "Failed to map ISP memory\n");
        return -ENOMEM;
    }

    /* Store DMA info */
    dev->rmem_addr = aligned_base;
    dev->rmem_size = aligned_size;
    dev->dma_sensor_addr = aligned_base;
    dev->dma_buffer_addr = aligned_base + DMA_BUFFER_OFFSET;

    /* Setup parameter region (0x1000 offset) */
    mem_offset = ALIGN(0x1000, 32);
    if (mem_offset + PAGE_SIZE > aligned_size) {
        ret = -ENOMEM;
        goto err_unmap;
    }

    dev->param_addr = aligned_base + mem_offset;
    dev->param_virt = dev->dma_buf + mem_offset;

    /* Setup frame buffer region */
    mem_offset = ALIGN(mem_offset + PAGE_SIZE, 32);
    dev->frame_buf_offset = mem_offset;
    if (dev->frame_buf_offset >= aligned_size) {
        ret = -ENOMEM;
        goto err_unmap;
    }

    dev_info(dev->dev, "Memory initialized:\n"
             "  Physical: 0x%08x\n"
             "  Virtual: %p\n"
             "  Size: %zu bytes\n"
             "  Params: %p\n",
             (uint32_t)dev->rmem_addr,
             dev->dma_buf,
             dev->rmem_size,
             dev->param_virt);

    return 0;

    err_unmap:
        devm_iounmap(dev->dev, dev->dma_buf);
    devm_release_mem_region(dev->dev, aligned_base, aligned_size);
    return ret;
}

void tx_isp_cleanup_memory(struct IMPISPDev *dev)
{
    if (!dev)
        return;

    /* Unmap memory regions */
    if (dev->dma_buf) {
        devm_iounmap(dev->dev, dev->dma_buf);
        dev->dma_buf = NULL;
    }

    /* Release memory regions */
    if (dev->rmem_addr) {
        devm_release_mem_region(dev->dev, dev->rmem_addr, dev->rmem_size);
        dev->rmem_addr = 0;
    }

    /* Clear memory info */
    dev->param_virt = NULL;
    dev->frame_buf_offset = 0;
    dev->rmem_size = 0;
}

/* Hardware debug helpers */
void dump_dma_registers(struct IMPISPDev *dev, int channel)
{
    void __iomem *regs = dev->reg_base;
    uint32_t buf_offset = ISP_BUF0_OFFSET + (channel * ISP_BUF_SIZE_STEP);

    pr_info("DMA Register Dump for Channel %d:\n", channel);
    pr_info("  Buffer Address: 0x%08x\n", readl(regs + buf_offset));
    pr_info("  Buffer Size: %d\n", readl(regs + buf_offset + 0x4));
    pr_info("  Control Register: 0x%08x\n", readl(regs + ISP_STREAM_CTRL + (channel * 0x100)));
    pr_info("  Start Register: 0x%08x\n", readl(regs + ISP_STREAM_START + (channel * 0x100)));
    pr_info("  Status Register: 0x%08x\n", readl(regs + ISP_STATUS_REG));
    pr_info("  Interrupt Status: 0x%08x\n", readl(regs + ISP_INT_STATUS_REG));
}

void debug_isp_registers(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->reg_base;

    pr_info("ISP Register State:\n");
    pr_info("CTRL: 0x%08x\n", readl(regs + ISP_CTRL_REG));
    pr_info("STATUS: 0x%08x\n", readl(regs + ISP_STATUS_REG));
    pr_info("Stream Control: 0x%08x\n", readl(regs + ISP_STREAM_CTRL));
    pr_info("Stream Start: 0x%08x\n", readl(regs + ISP_STREAM_START));

    // Buffer configuration
    pr_info("Buffer 0: addr=0x%08x size=0x%08x\n",
            readl(regs + ISP_BUF0_OFFSET),
            readl(regs + ISP_BUF0_OFFSET + 0x4));

    // Format configuration
    pr_info("Input format: 0x%08x\n", readl(regs + ISP_INPUT_FORMAT_REG));
    pr_info("Output format: 0x%08x\n", readl(regs + ISP_OUTPUT_FORMAT_REG));
}


int init_hw_resources(struct IMPISPDev *dev)
{
    void __iomem *base;
    int ret;

    /* Map ISP registers */
    base = devm_ioremap(dev->dev, ISP_PHYS_BASE, ISP_REG_SIZE);
    if (!base) {
        dev_err(dev->dev, "Failed to map ISP registers\n");
        return -ENOMEM;
    }
    dev->reg_base = base;

    /* Basic hardware init */
    writel(0, base + ISP_INT_MASK_REG);
    writel(0xFFFFFFFF, base + ISP_INT_CLEAR_REG);
    wmb();

    dev_dbg(dev->dev, "Initial control register: 0x%08x\n",
            readl(base + ISP_CTRL_OFFSET));

    return 0;
}
