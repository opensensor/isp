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
static struct irq_dev *dump_vsd;
static struct irq_dev *dump_isd;
static u32 vic_mdma_ch0_set_buff_index;  // Channel 0 buffer index
static u32 vic_mdma_ch1_set_buff_index;  // Channel 1 buffer index



/* Helper to initialize VIC IRQ handling */
int tx_isp_irq_init(struct irq_dev *dev,
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

    dump_isd = dev;
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
    void __iomem *cpm_base;
    u32 val;
    int ret;

    // Map CPM for VIC reset control
    cpm_base = ioremap(0x10000000, 0x100);
    if (!cpm_base)
        return -ENOMEM;

    // Assert VIC reset
    val = readl(cpm_base + 0xc4);  // SRSR reg
    val |= BIT(18);  // VIC reset bit
    writel(val, cpm_base + 0xc4);
    wmb();
    msleep(10);

    // Deassert reset
    val &= ~BIT(18);
    writel(val, cpm_base + 0xc4);
    wmb();
    msleep(10);

    iounmap(cpm_base);

    // Clear all VIC registers
    writel(0, vic_regs + 0x0);  // Control
    writel(0, vic_regs + 0xc);  // Mode
    writel(0, vic_regs + 0x10); // Route
    writel(0, vic_regs + 0x93c);// IRQ Enable
    writel(0, vic_regs + 0x9e8);// IRQ Mask
    wmb();

    pr_info("VIC reset complete - state:\n");
    dump_vic_state(dev);
    return 0;
}

static void dump_initial_vic_state(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    int i;

    pr_info("Initial VIC register state:\n");
    for (i = 0; i <= 0x9e8; i += 4) {
        u32 val = readl(vic_regs + i);
        if (val != 0x20151120)
            pr_info("  0x%03x: 0x%08x\n", i, val);
    }
}

/* Buffer and DMA management */
int configure_isp_buffers(struct IMPISPDev *dev)
{
    struct isp_channel *chn = &dev->channels[0];
    void __iomem *regs = dev->reg_base;
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    uint32_t buffer_start, buffer_size;
    uint32_t input_offset, y_offset, uv_offset;

    if (!dev || !chn) {
        pr_err("Invalid device state\n");
        return -EINVAL;
    }

    dump_initial_vic_state(dev);

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
    buffer_start = ALIGN(dev->dma_addr + 0x1094d4, 32);
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

    uint32_t frame_size = ALIGN(chn->width * chn->height * 3/2, 4096);
    uint32_t stride = ALIGN(chn->width, 32);

    // ISP frame dimensions
    writel(width, regs + ISP_FRAME_WIDTH);
    writel(height, regs + ISP_FRAME_HEIGHT);
    wmb();

    // VIC frame dimensions (format is width<<16 | height)
    writel((width << 16) | height, vic_regs + VIC_FRAME_SIZE);
    writel(stride, vic_regs + VIC_FRAME_STRIDE);
    writel(ALIGN(chn->dma_addr, 32), vic_regs + VIC_DMA_ADDR);
    wmb();

    // Initialize completion mechanism
    // TODO
    //init_completion(&fc->frame_complete);

    // Enable hardware path
    writel(1, regs + 0x120); // Enable buffer
    writel(1, regs + 0x124); // Enable pipeline
    writel(1, regs + ISP_DMA_CTRL); // Enable DMA
    wmb();

    dump_vic_state(dev);

    return 0;
}

int configure_csi_for_streaming(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->reg_base;
    void __iomem *csi_base = regs + 0xb8;  // CSI register base from OEM offset
    u32 val;

    dump_initial_vic_state(dev);

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
//    uint32_t y_stride = ALIGN(chn->width, 32);
//    uint32_t uv_stride = ALIGN(chn->width/2, 32);
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

void dump_vic_state(struct IMPISPDev *dev)
{
    void __iomem *isp_base = dev->reg_base;
    int i;

    pr_info("VIC State:\n");
    // Scan a wide range around VIC
    for (i = 0x7000; i < 0x8000; i += 4) {
        u32 val = readl(isp_base + i);
        if (i == 0x7000) {
            pr_info("  0x%04x: 0x%08x\n", i, val);
            continue;
        }
        if (val != 0x20151120 && val != 0) {
            pr_info("  0x%04x: 0x%08x\n", i, val);
        }
    }
}

void verify_isp_state(struct IMPISPDev *dev)
{
    void __iomem *isp_regs = dev->reg_base + ISP_BASE;
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;

    pr_info("ISP State:\n");
    pr_info("  Control: 0x%08x\n", readl(isp_regs + ISP_CTRL_OFFSET));
    pr_info("  Status: 0x%08x\n", readl(isp_regs + ISP_STATUS_OFFSET));
    pr_info("  Int mask: 0x%08x\n", readl(isp_regs + 0xb0));

    dump_vic_state(dev);
}

int init_vic_control(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;

    // First enable VIC core
    writel(0x00060003, vic_regs + 0x0);
    wmb();

    // Then setup IRQs
    writel(0x000033fb, vic_regs + 0x93c); // IRQ Enable with OEM value
    writel(0x00000000, vic_regs + 0x9e8); // IRQ Mask clear
    wmb();

    return 0;
}

static int clear_vic_status(void __iomem *vic_regs)
{
    u32 val, mask;
    int retry = 5;

    pr_info("Initial VIC status: 0x%08x\n", readl(vic_regs + 0xb4));

    // Write to both status registers we found in control block
    writel(0, vic_regs + 0xb4);  // Main status
    writel(0, vic_regs + 0xb8);  // Possible alternate status
    writel(0, vic_regs + 0xbc);  // Possible alternate status
    wmb();
    udelay(100);

    val = readl(vic_regs + 0xb4);
    pr_info("Final status: 0x%08x\n", val);

    return val ? -EIO : 0;
}


static int readl_poll_timeout(void __iomem *addr, u32 mask, u32 expected,
                            unsigned int timeout_us)
{
    u32 val;
    unsigned long timeout = jiffies + usecs_to_jiffies(timeout_us);

    while (time_before(jiffies, timeout)) {
        val = readl(addr);
        if ((val & mask) == expected)
            return 0;
        cpu_relax();
        udelay(10);
    }

    return -ETIMEDOUT;
}


int destroy_isp_links(struct IMPISPDev *dev)
{
    void __iomem *isp_regs = dev->reg_base;

    // Set bypass mode
    writel(1, isp_regs + 0x164);
    wmb();

    return 0;
}



int tx_isp_create_link(struct IMPISPDev *dev, struct imp_isp_link *link)
{
    struct tx_isp_subdev_pad *src_pad = NULL;
    struct tx_isp_subdev_pad *dst_pad = NULL;
    int ret;

//    if (link->src.mod == IMP_ISP_MOD_CSI && (!dev->csi_dev->sd || !dev->csi_dev->sd->base)) {
//        pr_err("CSI not ready for linking\n");
//        return -EINVAL;
//    }
//
//    if (link->dst.mod == IMP_ISP_MOD_VIC && (!dev->vic_dev->sd || !dev->vic_dev->sd->base)) {
//        pr_err("VIC not ready for linking\n");
//        return -EINVAL;
//    }

    // Convert pad types to tx_isp equivalents
    u32 src_type = (link->src.type == 1) ? TX_ISP_PADTYPE_OUTPUT : TX_ISP_PADTYPE_INPUT;
    u32 dst_type = (link->dst.type == 1) ? TX_ISP_PADTYPE_OUTPUT : TX_ISP_PADTYPE_INPUT;

    // Find source pad
    src_pad = find_pad(dev, link->src.mod, src_type, link->src.pad_id);
    if (!src_pad) {
        pr_err("Failed to find source pad\n");
        return -EINVAL;
    }

    // Find destination pad
    dst_pad = find_pad(dev, link->dst.mod, dst_type, link->dst.pad_id);
    if (!dst_pad) {
        pr_err("Failed to find dest pad\n");
        return -EINVAL;
    }

    // Configure link in hardware
    void __iomem *isp_regs = dev->reg_base;
    void __iomem *link_regs = isp_regs + 0x840;

    switch(link->src.mod) {
    case IMP_ISP_MOD_CSI:
        if (link->dst.mod == IMP_ISP_MOD_VIC) {
            writel(0x00010001, link_regs + 0x0);  // Enable routing
            writel(0x1, link_regs + 0x4);         // Enable link
            wmb();

            src_pad->link.flag |= TX_ISP_LINKFLAG_ENABLED;
            dst_pad->link.flag |= TX_ISP_LINKFLAG_ENABLED;
        }
        break;

    case IMP_ISP_MOD_VIC:
        if (link->dst.mod == IMP_ISP_MOD_DDR) {
            u32 cfg = readl(link_regs);
            writel(cfg | 0x2, link_regs);  // Enable DDR routing
            wmb();

            src_pad->link.flag |= TX_ISP_LINKFLAG_ENABLED;
            dst_pad->link.flag |= TX_ISP_LINKFLAG_ENABLED;
        }
        break;
    }

    pr_info("Created ISP link: %d->%d\n", link->src.mod, link->dst.mod);
    return 0;
}

int tx_isp_destroy_link(struct IMPISPDev *dev, struct imp_isp_link *link)
{
    void __iomem *isp_regs = dev->reg_base;
    u32 reg_val;

    // Set bypass bit, clear processing bit
    reg_val = readl(isp_regs + ISP_BYPASS_REG);
    reg_val |= BIT(0);   // Enable bypass
    reg_val &= ~BIT(1);  // Disable processing
    writel(reg_val, isp_regs + ISP_BYPASS_REG);
    wmb();

    // Disable links and routing
    writel(0, isp_regs + ISP_LINK_ENABLE_REG);
    writel(0, isp_regs + ISP_ROUTE_REG);
    wmb();

    return 0;
}

int setup_isp_links(struct IMPISPDev *dev)
{
    // Destroy any existing links first
    int ret = tx_isp_destroy_link(dev, NULL);
    if (ret)
        return ret;

    // Setup CSI -> VIC link
    struct imp_isp_link link1 = {
        .src = {
            .type = ISP_PAD_SOURCE,
            .mod = IMP_ISP_MOD_CSI,
            .pad_id = 0
        },
        .dst = {
            .type = ISP_PAD_SINK,
            .mod = IMP_ISP_MOD_VIC,
            .pad_id = 0
        }
    };

    ret = tx_isp_create_link(dev, &link1);
    if (ret)
        return ret;

    // Setup VIC -> DDR link
    struct imp_isp_link link2 = {
        .src = {
            .type = ISP_PAD_SOURCE,
            .mod = IMP_ISP_MOD_VIC,
            .pad_id = 0
        },
        .dst = {
            .type = ISP_PAD_SINK,
            .mod = IMP_ISP_MOD_DDR,
            .pad_id = 0
        }
    };

    ret = tx_isp_create_link(dev, &link2);
    if (ret)
        return ret;

    return 0;
}


int init_isp_core(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->reg_base;

    // Core ISP registers from debug dump
    writel(0x00000001, regs + 0x004);
    writel(0x00000001, regs + 0x008);
    writel(0x80700019, regs + 0x00c);  // Control
    writel(0x00000001, regs + 0x010);
    writel(0x00000001, regs + 0x014);
    writel(0x00000001, regs + 0x028);
    writel(0x00400040, regs + 0x02c);
    writel(0x00000001, regs + 0x030);
    wmb();

    // ISP Processing config
    writel(0x00000001, regs + 0x090);
    writel(0x00000001, regs + 0x094);
    writel(0x00030000, regs + 0x098);
    writel(0x58050000, regs + 0x0a8);
    writel(0x58050000, regs + 0x0ac);
    writel(0xffffffff, regs + 0x0b0);  // IRQ Enable
    writel(0x00040000, regs + 0x0c4);
    writel(0x00400040, regs + 0x0c8);
    writel(0x00000100, regs + 0x0cc);
    writel(0x0000000c, regs + 0x0d4);
    writel(0x00ffffff, regs + 0x0d8);
    writel(0x00000100, regs + 0x0e0);
    writel(0x00400040, regs + 0x0e4);
    writel(0xff808000, regs + 0x0f0);
    wmb();

    // VIC Control registers
    writel(0x80007000, regs + 0x110);  // VIC Control
    wmb();

    return 0;
}

//int setup_vic_base(struct IMPISPDev *dev)
//{
//    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
//
//    // Basic VIC setup
//    writel(0x00060003, vic_regs + 0x7000);  // Main control
//    writel(0x00020000, vic_regs + 0x7004);
//    writel(0x00c80000, vic_regs + 0x7008);
//    writel(0x1eff0000, vic_regs + 0x700c);
//    wmb();
//
//    // Configure routing
//    writel(0x0000c83f, vic_regs + 0x7010);
//    writel(0x00001e0a, vic_regs + 0x7014);
//    wmb();
//
//    // Sync parameters
//    writel(0x080f003f, vic_regs + 0x7018);
//    writel(0x141f0000, vic_regs + 0x701c);
//    writel(0x003fff08, vic_regs + 0x7020);
//    writel(0x00133200, vic_regs + 0x7024);
//    writel(0x00010610, vic_regs + 0x7028);
//    writel(0xff00ff00, vic_regs + 0x702c);
//    writel(0x0003ff00, vic_regs + 0x7030);
//    wmb();
//
//    // Timing parameters - blocks of 4 registers
//    writel(0x20202020, vic_regs + 0x7048);
//    writel(0x20202020, vic_regs + 0x704c);
//    writel(0x20202020, vic_regs + 0x7050);
//    writel(0x20202020, vic_regs + 0x7054);
//    writel(0x20202020, vic_regs + 0x7058);
//    writel(0x20202020, vic_regs + 0x705c);
//    writel(0x20202020, vic_regs + 0x7060);
//    writel(0x20202020, vic_regs + 0x7064);
//    wmb();
//
//    // Timing additional
//    writel(0x00380038, vic_regs + 0x7068);
//    writel(0x00380038, vic_regs + 0x706c);
//    writel(0x00380038, vic_regs + 0x7070);
//    writel(0x00380038, vic_regs + 0x7074);
//    wmb();
//
//    // Additional config
//    writel(0x01000000, vic_regs + 0x7080);
//    writel(0x00080000, vic_regs + 0x7088);
//    writel(0x00010001, vic_regs + 0x708c);
//    writel(0x00010001, vic_regs + 0x7810);
//    wmb();
//
//    return 0;
//}

int setup_vic_dma(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    u32 frame_size, stride;
    u32 dma_base;

    // Calculate dimensions
    stride = ALIGN(dev->sensor_width, 32);
    frame_size = (dev->sensor_width << 16) | dev->sensor_height;

    // Size calculations from debug
    u32 base_size = stride * dev->sensor_height;
    u32 meta_size = 23040;
    u32 total_size = base_size + meta_size;

    dma_base = dev->dma_addr;

    // Program DMA registers - removed the 0x7000 offset
    writel(1, vic_regs + 0x908);             // Enable
    wmb();
    writel(frame_size, vic_regs + 0x904);    // Frame size
    writel(stride, vic_regs + 0x910);        // Y stride
    writel(stride, vic_regs + 0x914);        // UV stride
    writel(dma_base, vic_regs + 0x918);      // Base addr
    writel(dma_base + base_size, vic_regs + 0x91c); // Meta addr
    wmb();

    // Control register last
    writel(0x80080020, vic_regs + 0x900);
    wmb();

    return 0;
}
//
//int tx_isp_enable_irq(struct IMPISPDev *dev)
//{
//    void __iomem *regs = dev->reg_base;
//    void __iomem *vic_regs = regs + VIC_BASE;
//
//    // Disable interrupts first
//    writel(0, vic_regs + 0x93c);
//    writel(0, regs + 0xb0);
//    wmb();
//
//    // Clear pending interrupts
//    writel(0xffffffff, vic_regs + 0xb4);
//    writel(0xffffffff, vic_regs + 0xb8);
//    wmb();
//
//    // Enable VIC interrupts with values from debug
//    writel(0x000033fb, vic_regs + 0x93c);
//    writel(0x00000000, vic_regs + 0x9e8);
//    wmb();
//
//    // Configure routing
//    writel(readl(vic_regs + 0x840) | 0x2, vic_regs + 0x840);
//    wmb();
//
//    // Enable ISP core interrupts
//    writel(0xffffffff, regs + 0xb0);
//    wmb();
//
//    return 0;
//}

int configure_vic_for_streaming(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    void __iomem *isp_regs = dev->reg_base;
    int ret;

    pr_info("Configuring VIC for streaming...\n");

    // 1. First disable interrupts and clear pending
    writel(0, vic_regs + 0x93c);            // Disable first
    writel(0xffffffff, vic_regs + 0xb4);    // Clear status
    writel(0xffffffff, vic_regs + 0xb8);    // Clear status2
    wmb();

    // Now follow your exact register programming sequence
    // 2. Enable VIC core
    writel(0x00060003, vic_regs + 0x0);  // VIC enable
    wmb();

    // 3. Initialize ISP core registers
    writel(0x00000001, isp_regs + 0x004);
    writel(0x00000001, isp_regs + 0x008);
    writel(0x80700019, isp_regs + 0x00c);  // ISP Control

    // 4. Initialize ISP core registers
    writel(0x00000001, isp_regs + 0x004);
    writel(0x00000001, isp_regs + 0x008);
    writel(0x80700019, isp_regs + 0x00c);  // ISP Control
    writel(0x00000001, isp_regs + 0x010);
    writel(0x00000001, isp_regs + 0x014);
    writel(0x00000001, isp_regs + 0x028);
    writel(0x00400040, isp_regs + 0x02c);
    writel(0x00000001, isp_regs + 0x030);
    wmb();

    // 5. ISP Processing configuration
    writel(0x00000001, isp_regs + 0x090);
    writel(0x00000001, isp_regs + 0x094);
    writel(0x00030000, isp_regs + 0x098);
    writel(0x58050000, isp_regs + 0x0a8);
    writel(0x58050000, isp_regs + 0x0ac);
    writel(0xffffffff, isp_regs + 0x0b0);  // IRQ Enable
    writel(0x00040000, isp_regs + 0x0c4);
    writel(0x00400040, isp_regs + 0x0c8);
    writel(0x00000100, isp_regs + 0x0cc);
    writel(0x0000000c, isp_regs + 0x0d4);
    writel(0x00ffffff, isp_regs + 0x0d8);
    writel(0x00000100, isp_regs + 0x0e0);
    writel(0x00400040, isp_regs + 0x0e4);
    writel(0xff808000, isp_regs + 0x0f0);
    wmb();

    // 6. VIC Control registers
    writel(0x80007000, isp_regs + 0x110);  // VIC Control
    writel(0x00010001, isp_regs + 0x840);  // VIC Config
    wmb();
    udelay(100);

    // 7. Configure VIC base registers
    writel(0x00020000, vic_regs + 0x4);
    writel(0x00c80000, vic_regs + 0x8);
    writel(0x1eff0000, vic_regs + 0xc);
    writel(0x0000c83f, vic_regs + 0x10);  // Route config
    writel(0x00001e0a, vic_regs + 0x14);  // Route enable
    wmb();

    // 8. VIC Sync parameters
    writel(0x080f003f, vic_regs + 0x18);
    writel(0x141f0000, vic_regs + 0x1c);
    writel(0x003fff08, vic_regs + 0x20);
    writel(0x00133200, vic_regs + 0x24);
    writel(0x00010610, vic_regs + 0x28);
    writel(0xff00ff00, vic_regs + 0x2c);
    writel(0x0003ff00, vic_regs + 0x30);
    wmb();

    // 9. VIC Timing parameters
    writel(0x20202020, vic_regs + 0x48);
    writel(0x20202020, vic_regs + 0x4c);
    writel(0x20202020, vic_regs + 0x50);
    writel(0x20202020, vic_regs + 0x54);
    writel(0x20202020, vic_regs + 0x58);
    writel(0x20202020, vic_regs + 0x5c);
    writel(0x20202020, vic_regs + 0x60);
    writel(0x20202020, vic_regs + 0x64);
    writel(0x00380038, vic_regs + 0x68);
    writel(0x00380038, vic_regs + 0x6c);
    writel(0x00380038, vic_regs + 0x70);
    writel(0x00380038, vic_regs + 0x74);
    wmb();

    // 10. VIC Additional configuration
    writel(0x01000000, vic_regs + 0x80);
    writel(0x00080000, vic_regs + 0x88);
    writel(0x00010001, vic_regs + 0x8c);
    writel(0x00010001, vic_regs + 0x810);
    wmb();

    // 11. Setup DMA last
    ret = setup_vic_dma(dev);
    if (ret)
        return ret;

    // 12. Configure interrupts
    pr_info("VIC IRQ Setup:\n");
    pr_info("  VIC Base=%p ISP Base=%p\n", vic_regs, isp_regs);

    writel(0, vic_regs + 0x93c);            // Disable first
    writel(0xffffffff, vic_regs + 0x7b4);   // Clear status
    writel(0xffffffff, vic_regs + 0x7b8);   // Clear status2
    wmb();

    u32 irq_status;
    void __iomem *irq_enable = vic_regs + 0x93c;
    void __iomem *irq_mask = vic_regs + 0x9e8;

    pr_info("  IRQ Enable addr=%p Mask addr=%p\n", irq_enable, irq_mask);

    writel(0x000033fb, irq_enable);
    irq_status = readl(irq_enable);
    pr_info("  IRQ Enable written: 0x%08x read back: 0x%08x\n",
            0x000033fb, irq_status);

    writel(0x00000000, irq_mask);   // Unmask all
    irq_status = readl(irq_mask);
    pr_info("  IRQ Mask written: 0x%08x read back: 0x%08x\n",
            0x00000000, irq_status);

    // Enable routing
    u32 route_val = readl(vic_regs + 0x840);
    writel(route_val | 0x2, vic_regs + 0x840);
    pr_info("  Route reg: before=0x%08x after=0x%08x\n",
            route_val, readl(vic_regs + 0x840));
    wmb();

    dev->vic_started = 1;
    dev->vic_processing = 1;

    return 0;
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
//
//int init_hw_resources(struct IMPISPDev *dev)
//{
//    void __iomem *base;
//    int ret;
//
//    /* Map ISP registers */
//    base = devm_ioremap(dev->dev, ISP_PHYS_BASE, ISP_REG_SIZE);
//    if (!base) {
//        dev_err(dev->dev, "Failed to map ISP registers\n");
//        return -ENOMEM;
//    }
//    dev->reg_base = base;
//
//    /* Basic hardware init */
//    writel(0, base + ISP_INT_MASK_REG);
//    writel(0xFFFFFFFF, base + ISP_INT_CLEAR_REG);
//    wmb();
//
//    dev_dbg(dev->dev, "Initial control register: 0x%08x\n",
//            readl(base + ISP_CTRL_OFFSET));
//
//    return 0;
//}


// VIC subdev operations
int vic_init(struct tx_isp_subdev *sd, int on)
{
    struct vic_device *vic = sd->host_priv;
    int ret;

    if (on) {
        ret = reset_vic(vic); // Use existing reset_vic function
        if (ret)
            return ret;

        ret = init_vic_control(vic); // Use existing init function
        if (ret)
            return ret;
    }

    return 0;
}

int vic_reset(struct tx_isp_subdev *sd, int on)
{
    if (on)
        return reset_vic(sd->host_priv);
    return 0;
}

int vic_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct vic_device *vic = sd->host_priv;

    if (enable) {
        return configure_vic_for_streaming(vic); // Use existing function
    } else {
        // Disable streaming
        writel(0, vic->regs + 0x7000);
        wmb();
    }

    return 0;
}

int vic_link_stream(struct tx_isp_subdev *sd, int enable)
{
    struct vic_device *vic = sd->host_priv;

    if (enable) {
        return setup_vic_dma(vic); // Use existing DMA setup
    }
    return 0;
}

int vic_link_setup(const struct tx_isp_subdev_pad *local,
                         const struct tx_isp_subdev_pad *remote, u32 flags)
{
    // This can be a simple passthrough for now as link setup
    // is handled by configure_vic_for_streaming
    return 0;
}

irqreturn_t vic_isr(struct tx_isp_subdev *sd, u32 status, bool *handled)
{
    struct vic_device *vic = sd->host_priv;

    // Clear interrupt status
    writel(status, vic->regs + 0xb4);
    writel(status, vic->regs + 0xb8);
    wmb();

    *handled = true;
    return IRQ_HANDLED;
}

irqreturn_t vic_isr_thread(struct tx_isp_subdev *sd, void *data)
{
    struct vic_device *vic = sd->host_priv;
    complete(&vic->frame_complete);
    return IRQ_HANDLED;
}

// CSI subdev operations - wire up to existing functions
int csi_init(struct tx_isp_subdev *sd, int on)
{
    if (on)
        return init_csi_phy(sd->host_priv);
    return 0;
}

int csi_reset(struct tx_isp_subdev *sd, int on)
{
    struct csi_device *csi = sd->host_priv;

    if (on) {
        // Use existing CSI reset sequence from configure_mipi_csi
        writel(0x0, csi->csi_regs + 0x10);  // CSI2_RESETN
        writel(0x0, csi->csi_regs + 0x0c);  // DPHY_RSTZ
        writel(0x0, csi->csi_regs + 0x08);  // PHY_SHUTDOWNZ
        wmb();
        msleep(20);
    }
    return 0;
}

int csi_s_stream(struct tx_isp_subdev *sd, int enable)
{
    if (enable) {
        return configure_csi_streaming(sd->host_priv);
    }
    return 0;
}


static int tx_isp_csi_activate_subdev(struct IMPISPDev *dev)
{
    //spin_lock(&dev->csi_lock);

    if (dev->csi_dev->state == 1) {
        // State 1->2 transition
        dev->csi_dev->state = 2;
    }

    //spin_unlock(&dev->csi_lock);
    return 0;
}

int csi_link_stream(struct tx_isp_subdev *sd, int enable)
{
    if (enable) {
        return tx_isp_csi_activate_subdev(sd->host_priv);
    }
    return 0;
}