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
int tx_isp_irq_init(struct IMPISPDev *dev, void (*handler)(void *),
                    void (*disable)(void *), void *priv)
{
    void __iomem *regs = dev->reg_base;
    void __iomem *intc;
    u32 val;

    pr_info("Starting ISP IRQ init\n");

    // Map INTC registers
    intc = ioremap(T31_INTC_BASE, 0x100);
    if (!intc)
        return -ENOMEM;

    // First disable everything
    writel(0, regs + ISP_INT_MASK_OFF);
    writel(~0, regs + ISP_INT_CLR_OFF);
    wmb();

    // Clear pending at INTC level
    val = readl(intc + INTC_IMR_OFF);
    val &= ~(BIT(37) | BIT(38)); // Clear ISP and VIC mask bits
    writel(val, intc + INTC_IMR_OFF);
    wmb();

    // Enable I2C -> ISP -> VIC interrupt path
    writel(0xffffffff, regs + ISP_INT_MASK_OFF); // Enable all ISP interrupts
    wmb();

    spin_lock_init(&dev->lock);
    dev->irq_enabled = 0;
    dev->irq_handler = handler;
    dev->irq_disable = disable;
    dev->irq_priv = priv;

    dump_isd = dev;

    pr_info("ISP IRQ initialized successfully\n");
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


static void configure_vic_mipi_mode(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    uint32_t mipi_mode;

    // MIPI basic setup matches registers from working state
    writel(0x01004632, vic_regs + 0x010);  // MIPI timing
    writel(0x00000a02, vic_regs + 0x014);  // Sensor type
    writel(0x0a08003f, vic_regs + 0x018);  // Mode configuration

    // Mode B configuration
    mipi_mode = 0x100010;
    writel(mipi_mode, vic_regs + 0x1a4);
    writel(0x4210, vic_regs + 0x1ac);
    writel(0x10, vic_regs + 0x1b0);
    writel(0x0, vic_regs + 0x1b4);
    wmb();
}

int init_vic_control(struct IMPISPDev *dev)
{
    void __iomem *vic_regs;

    if (!dev || !dev->reg_base) {
        pr_err("Invalid device state in init_vic_control\n");
        return -EINVAL;
    }

    vic_regs = dev->reg_base + VIC_BASE;
    pr_info("VIC init check: dev=%p reg_base=%p vic_dev=%p\n",
            dev, dev->reg_base, dev->vic_dev);

    if (!dev->vic_dev) {
        pr_err("VIC device not initialized\n");
        return -EINVAL;
    }

    // Reinitialize the mutex
    mutex_init(&dev->vic_dev->state_lock);
    pr_info("Mutex reinitialized\n");

    // First enable VIC core
    writel(0x00060003, vic_regs + 0x0);
    wmb();

    // Configure MIPI mode
    configure_vic_mipi_mode(dev);
    wmb();

    // Then setup IRQs
    writel(0x000033fb, vic_regs + 0x93c); // IRQ Enable with OEM value
    writel(0x00000000, vic_regs + 0x9e8); // IRQ Mask clear
    wmb();

    pr_info("VIC About to lock (state_lock=%p), count=%d\n",
            &dev->vic_dev->state_lock,
            dev->vic_dev->state_lock.count.counter);

    if (!mutex_trylock(&dev->vic_dev->state_lock)) {
        pr_err("Mutex is locked by someone else!\n");
        return -EBUSY;
    }

    pr_info("Got lock successfully\n");
    dev->vic_dev->state = TX_ISP_MODULE_READY;
    mutex_unlock(&dev->vic_dev->state_lock);

    pr_info("VIC Unlocked\n");
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
    dma_addr_t dma_base = 0x02a80000;

    // Set buffer indices like OEM
    dev->vic_dev->ch0_buf_idx = 4;
    dev->vic_dev->ch0_sub_get_num = 4;

    // 1. First enable DMA core like OEM
    writel(1, vic_regs + 0x308);
    wmb();

    // 2. Set dimensions exactly as OEM does
    u32 width = dev->width;
    u32 height = dev->height;
    u32 dim_val = (width << 16) | height;
    writel(dim_val, vic_regs + 0x304);

    // 3. Set stride in both registers
    u32 stride = width * 2;
    writel(stride, vic_regs + 0x310);
    writel(stride, vic_regs + 0x314);
    wmb();

    // 4. Calculate buffer info
    u32 frame_size = stride * height;
    u32 buffer_offset = frame_size;

    pr_info("VIC DMA: dim=%08x stride=%d base=%08x size=%d offset=%d\n",
            dim_val, stride, (u32)dma_base, frame_size, buffer_offset);

    // 5. Configure DMA buffer addresses in OEM order
    writel(dma_base, vic_regs + 0x318);  // First buffer exact
    writel(dma_base + buffer_offset, vic_regs + 0x31c);
    writel(dma_base + (buffer_offset * 2), vic_regs + 0x320);
    writel(dma_base + (buffer_offset * 3), vic_regs + 0x324);
    wmb();

    if (readl(vic_regs + 0x318) != dma_base) {
        pr_err("DMA base addr mismatch: wrote %08x read %08x\n",
               (u32)dma_base, readl(vic_regs + 0x318));
        return -EIO;
    }

    // 6. Configure DMA control after addresses set
    writel(0x80040020, vic_regs + 0x900);  // 4 buffers like OEM
    writel(0x003c0438, vic_regs + 0x904);
    writel(0x00000001, vic_regs + 0x908);  // Frame sync
    wmb();

    pr_info("VIC DMA Status: 0x%08x\n", readl(vic_regs + 0x84));

    return 0;
}

void dump_vic_registers(struct IMPISPDev *dev) {
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;

    // Dump key control/status registers
    pr_info("\nVIC Register Space Dump:\n");
    pr_info("Control (0x0):     0x%08x\n", readl(vic_regs + 0x0));
    pr_info("Config (0x4):      0x%08x\n", readl(vic_regs + 0x4));
    pr_info("Status (0xb4):     0x%08x\n", readl(vic_regs + 0xb4));
    pr_info("Status2 (0xb8):    0x%08x\n", readl(vic_regs + 0xb8));
    pr_info("DMA Status (0x84): 0x%08x\n", readl(vic_regs + 0x84));

    // Dump full register range from 0x900-0x9ff
    pr_info("\nVIC 0x900-0x9ff Register Range:\n");
    for (int offset = 0x900; offset < 0xF00; offset += 4) {
        u32 val = readl(vic_regs + offset);
        if (val != 0 && val != 0x20151120) {
            pr_info("  0x%03x: 0x%08x\n", offset, val);
        }
    }
}

// Test different access patterns to locate registers
void probe_vic_registers(struct IMPISPDev *dev) {
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    u32 test_patterns[] = {0xAAAAAAAA, 0x55555555};

    pr_info("Probing VIC register access patterns:\n");

    // Test direct access
    for (int i = 0; i < ARRAY_SIZE(test_patterns); i++) {
        writel(test_patterns[i], vic_regs + 0x1a4);
        pr_info("Direct 0x1a4 write %08x, readback: %08x\n",
                test_patterns[i], readl(vic_regs + 0x1a4));
    }
}

void dump_vic_registers_detailed(void)
{
    void __iomem *vic_regs = ioremap(0x13300000 + 0x7000, 0x2000);  // ISP_BASE + VIC_BASE
    if (!vic_regs) {
        pr_err("Failed to map VIC registers for debug dump\n");
        return;
    }

    // Core registers (0x000-0x0FF)
    pr_info("=== VIC Core Registers ===\n");
    pr_info("0x000 (Control):     %08x\n", readl(vic_regs + 0x000));
    pr_info("0x004 (Config):      %08x\n", readl(vic_regs + 0x004));
    pr_info("0x010 (MIPI):        %08x\n", readl(vic_regs + 0x010));
    pr_info("0x014 (SensorType):  %08x\n", readl(vic_regs + 0x014));
    pr_info("0x018 (SensorMode):  %08x\n", readl(vic_regs + 0x018));

    // Same format as before but keep going with more ranges...
    pr_info("\n=== Extended Register Ranges ===\n");
    for (int i = 0x0; i <= 0x1000; i += 16) {
        pr_info("%03x: %08x %08x %08x %08x\n",
            i,
            readl(vic_regs + i),
            readl(vic_regs + i + 4),
            readl(vic_regs + i + 8),
            readl(vic_regs + i + 12)
        );
    }

    iounmap(vic_regs);
}


#define VIC_STATE_IDLE       1
#define VIC_STATE_CONFIGURED 2
#define VIC_STATE_INITIALIZED 3
#define VIC_STATE_STREAMING  4

struct vic_dev {
    struct mutex state_lock;
    int state;
    void __iomem *reg_base;
    struct device *dev;
    /* existing fields... */
};

// Core state management functions
static int vic_core_set_state(struct vic_dev *vic, int new_state)
{
    int ret = 0;
    mutex_lock(&vic->state_lock);

    switch(new_state) {
        case VIC_STATE_IDLE:
            if (vic->state != VIC_STATE_CONFIGURED) {
                ret = -EINVAL;
                goto out;
            }
        break;

        case VIC_STATE_CONFIGURED:
            if (vic->state != VIC_STATE_IDLE && vic->state != VIC_STATE_INITIALIZED) {
                ret = -EINVAL;
                goto out;
            }
        tx_vic_disable_irq(vic->reg_base);
        break;

        case VIC_STATE_INITIALIZED:
            if (vic->state != VIC_STATE_CONFIGURED) {
                ret = -EINVAL;
                goto out;
            }
        tx_vic_enable_irq(vic->reg_base);
        break;

        case VIC_STATE_STREAMING:
            if (vic->state != VIC_STATE_INITIALIZED) {
                ret = -EINVAL;
                goto out;
            }
        break;

        default:
            ret = -EINVAL;
        goto out;
    }

    vic->state = new_state;

    out:
        mutex_unlock(&vic->state_lock);
    return ret;
}


void dump_irq_info(void) {
    void __iomem *intc_base = ioremap(0x10001000, 0x1000);
    pr_info("ISR: 0x%08x\n", readl(intc_base + 0x00));
    pr_info("IMR: 0x%08x\n", readl(intc_base + 0x04));
    pr_info("IPR: 0x%08x\n", readl(intc_base + 0x10));
}

void tx_vic_disable_irq(void __iomem *intc_base) {
    u32 mask = readl(intc_base + ICMRn);
    // Mask VIC and ISP (bits 30 and 31)
    mask |= (BIT(30) | BIT(31));
    writel(mask, intc_base + ICMRn);
    wmb();
}

void tx_vic_enable_irq(void __iomem *intc_base) {
    u32 orig_mask = readl(intc_base + ICMRn);
    pr_info("Original INTC mask: 0x%08x\n", orig_mask);

    // Clear only VIC and ISP bits while preserving others
    u32 new_mask = orig_mask & ~(BIT(30) | BIT(31));
    writel(new_mask, intc_base + ICMRn);
    wmb();

    pr_info("Updated INTC mask: 0x%08x\n", readl(intc_base + ICMRn));
}

// Structure for frame metadata
struct vic_frame_metadata {
    uint32_t magic;          // 0x80
    uint32_t validation1;    // 0xe1
    uint32_t width_check;    // aligned_width/2
    uint32_t validation2;    // 0x147
    uint32_t fps_num;        // FPS numerator
    uint32_t fps_den;        // FPS denominator
    uint32_t width;          // Frame width
    uint32_t height;         // Frame height
} __attribute__((packed));


void configure_vic_interrupts(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;

    // First clear any pending interrupts
    writel(0xffffffff, vic_regs + 0xb4);
    writel(0xffffffff, vic_regs + 0xb8);
    wmb();

    // Core interrupt setup

    // Write and verify each core interrupt register
    writel(0x01000000, vic_regs + 0x080);
    u32 val = readl(vic_regs + 0x080);
    dev_info(dev->dev, "0x080 wrote 0x01000000, read 0x%08x\n", val);

    writel(0x00000000, vic_regs + 0x084);
    val = readl(vic_regs + 0x084);
    dev_info(dev->dev, "0x084 wrote 0x00000000, read 0x%08x\n", val);

    writel(0x00080000, vic_regs + 0x088);
    writel(0x00010001, vic_regs + 0x08c);
    wmb();

    // Status registers
    writel(0x00010001, vic_regs + 0x810);
    writel(0x00000000, vic_regs + 0x814);
    writel(0x00000000, vic_regs + 0x818);
    writel(0x20151120, vic_regs + 0x81c);
    wmb();

    // Frame routing registers from working state
    writel(0x02a80042, vic_regs + 0x840);
    writel(0x00000040, vic_regs + 0x844);
    writel(0x02c7a482, vic_regs + 0x848);
    writel(0x00000040, vic_regs + 0x84c);
    wmb();

    // DMA and control
    writel(0x808080a0, vic_regs + 0x900);
    writel(0x00000140, vic_regs + 0x920);
    writel(0x00000001, vic_regs + 0x92c);
    wmb();

    dev_info(dev->dev, "VIC interrupt configuration complete\n");
}

void configure_vic_buffer_chain(struct IMPISPDev *dev, uint32_t mipi_mode)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    uint32_t sensor_mode = readl(vic_regs + 0x018);

    // Fixed metadata requirements
    const uint32_t META_SIZE = 0x40;
    const uint32_t META_OFFSET = 0x42;

    // Use Mode B stride when sensor mode is 0x0a08003f
    uint32_t FRAME_STRIDE = (sensor_mode == 0x0a08003f) ? 0x780 : 0xf00;
    uint32_t frame_size = FRAME_STRIDE * dev->height;
    uint32_t base_addr = ALIGN(dev->dma_addr, 32);

    // Standard init sequence (common to both modes)
    writel(0x20151120, vic_regs + 0x800);
    writel(0x00000110, vic_regs + 0x804);
    writel(0x00000000, vic_regs + 0x808);
    writel(0x20151120, vic_regs + 0x80c);
    writel(0x00010001, vic_regs + 0x810);
    writel(0x00000000, vic_regs + 0x814);
    writel(0x00000000, vic_regs + 0x818);
    writel(0x20151120, vic_regs + 0x81c);

    // Frame buffer chain setup
    writel(base_addr, vic_regs + 0x820);
    writel(FRAME_STRIDE, vic_regs + 0x824);
    writel(FRAME_STRIDE, vic_regs + 0x828);
    writel(FRAME_STRIDE, vic_regs + 0x82c);
    writel(frame_size, vic_regs + 0x830);
    writel(META_SIZE, vic_regs + 0x834);
    writel(0x0, vic_regs + 0x838);
    writel(0x1, vic_regs + 0x83c);

    // Frame buffer pointers
    uint32_t frame1_addr = base_addr + META_OFFSET;
    uint32_t frame2_addr = frame1_addr + frame_size + META_SIZE;
    uint32_t frame3_addr = frame2_addr + frame_size + META_SIZE;

    writel(frame1_addr, vic_regs + 0x840);
    writel(META_SIZE, vic_regs + 0x844);
    writel(frame2_addr, vic_regs + 0x848);
    writel(META_SIZE, vic_regs + 0x84c);
    writel(frame3_addr, vic_regs + 0x850);
    writel(META_SIZE, vic_regs + 0x854);
    writel(frame_size, vic_regs + 0x858);
    writel(0x3c0, vic_regs + 0x85c);

    // DMA Control - Common for both modes
    writel(0x808080a0, vic_regs + 0x900);
    writel(0x80808080, vic_regs + 0x904);
    writel(0x80808080, vic_regs + 0x908);
    writel(0x80808080, vic_regs + 0x90c);

    // Control sequence - Common for both modes
    writel(0x00000140, vic_regs + 0x920);
    writel(0x00000000, vic_regs + 0x924);
    writel(0x00000000, vic_regs + 0x928);
    writel(0x00000001, vic_regs + 0x92c);
    wmb();

    dev_info(dev->dev, "VIC buffer chain configured for MIPI mode 0x%x:\n"
            "  Base addr: %08x\n"
            "  Frame size: %08x (stride: %08x)\n"
            "  Frames: %08x %08x %08x\n",
            mipi_mode, base_addr, frame_size, FRAME_STRIDE,
            frame1_addr, frame2_addr, frame3_addr);
}

void configure_isp_processing_regs(struct IMPISPDev *dev) {
    void __iomem *isp_regs = dev->reg_base;


    // 5. ISP Core Configuration
    writel(0x00000001, isp_regs + 0x004);
    writel(0x00000001, isp_regs + 0x008);
    writel(0x80700019, isp_regs + 0x00c);  // ISP Control
    writel(0x00000001, isp_regs + 0x010);
    writel(0x00000001, isp_regs + 0x014);
    writel(0x00000001, isp_regs + 0x028);
    writel(0x00400040, isp_regs + 0x02c);
    writel(0x00000001, isp_regs + 0x030);
    wmb();

    // 6. ISP Processing Configuration
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
    pr_info("ISP Processing Configuration complete\n");
}

int configure_vic_for_streaming(struct IMPISPDev *dev)
{
    void __iomem *isp_regs = dev->reg_base;
    void __iomem *intc_base = ioremap(T31_INTC_BASE, 0x100);
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    void __iomem *vic_control = (void __iomem *)(dev->reg_base + 0xb8);
    void __iomem *gpioz_base = ioremap(0x10017000, 0x1000); // GPIO Port Z
    u32 val;
    int ret, timeout;

    if (!intc_base || !gpioz_base) {
        dev_err(dev->dev, "Failed to map registers\n");
        ret = -ENOMEM;
        goto err_unmap;
    }

    dev_info(dev->dev, "Configuring VIC for streaming...\n");

    dev_info(dev->dev, "sensor type is OTHER_MIPI!\n");
    u32 shadow_val = 0xa000a;

    // 1. Configure GPIO/INTC first as before
    writel(VIC_INT, gpioz_base + PZINTS);
    writel(0x00000000, gpioz_base + PZMSKS);
    writel(VIC_INT, gpioz_base + PZPAT1S);
    writel(0x00000000, gpioz_base + PZPAT0S);
    writel(0x1, gpioz_base + PZGID2LD);

    tx_vic_enable_irq(intc_base);

    u32 mode_val = readl(vic_regs + 0x7c);
    dev_info(dev->dev, "Mode value at 0x7c: 0x%08x\n", mode_val);
    int mode_reg = 8;  // Default
    if (mode_val != 0)
        mode_reg = 0xa;
    if (mode_val != 1)
        mode_reg = 0xc;
    if (mode_val != 2)
        mode_reg = 0x10;
    if (mode_val != 7)
        mode_reg = 0;

    // 2. OEM sequence for shadow/control registers
    dev_info(dev->dev, "sensor type is OTHER_MIPI!\n");
    uint32_t mipi_mode = 0xa000a;  // Mode we're setting
    writel(mipi_mode, vic_regs + 0x1a4);
    wmb();

    // 1. Set initial control state
    writel(2, vic_control + 0xc);
    wmb();

    // 2. Core register base config
    writel(0x030000, vic_regs + 0x000);
    writel(0x070000, vic_regs + 0x004);
    writel(0x010a0000, vic_regs + 0x008);
    writel(0x0aff0700, vic_regs + 0x00c);
    writel(0x01004632, vic_regs + 0x010);
    wmb();

    // 4. Additional shadow register setup from frame mode
    writel(0x4440, vic_regs + 0x1ac);
    writel(0x10, vic_regs + 0x1b0);
    writel(0x0, vic_regs + 0x1b4);

    dump_vic_registers_detailed();

    // 5. Enter config state
    writel(2, vic_control);
    wmb();
    udelay(1);

    // 6. Configure ISP core and processing
    configure_isp_processing_regs(dev);

    // 7. Configure VIC interrupts
    configure_vic_interrupts(dev);

    // 8. Timing parameters
    writel(0x20202020, vic_regs + 0x060);
    writel(0x20202020, vic_regs + 0x064);
    writel(0x001e001e, vic_regs + 0x068);
    writel(0x001e001e, vic_regs + 0x06c);
    writel(0x001e001e, vic_regs + 0x070);
    writel(0x001e001e, vic_regs + 0x074);
    writel(0x00010103, vic_regs + 0x078);
    writel(0x00010103, vic_regs + 0x07c);
    wmb();

    writel(0x003f003f, vic_regs + 0x034);
    writel(0x0a140000, vic_regs + 0x01c);
    wmb();

    // 9. Configure buffer chain
    configure_vic_buffer_chain(dev, mipi_mode);

    // Keep the basic setup the same, but modify these values:

    // Before writes
    dev_info(dev->dev, "Before interrupt block - 0x080: 0x%08x\n", readl(vic_regs + 0x080));

    // Frame/interrupt control block to match OEM exactly
    writel(0x012975c5, vic_regs + 0x080);
    wmb();
    writel(0x0052a5c1, vic_regs + 0x084);
    wmb();
    writel(0x00060003, vic_regs + 0x088);
    wmb();
    writel(0x00000010, vic_regs + 0x08c);
    wmb();

    // After writes
    dev_info(dev->dev, "After interrupt block - 0x080: 0x%08x\n", readl(vic_regs + 0x080));

    // Set sensor type directly
    writel(0x00000a02, vic_regs + 0x014);
    wmb();

    // Set mode directly
    writel(0x0a08003f, vic_regs + 0x018);
    wmb();

    dev_info(dev->dev, "After both direct writes - type: 0x%08x mode: 0x%08x\n",
             readl(vic_regs + 0x014),
             readl(vic_regs + 0x018));

    // 11. Enable DMA interrupts and routing
    val = readl(vic_regs + 0x900);
    val |= 0x80000020;
    writel(val, vic_regs + 0x900);

    val = readl(vic_regs + 0x840);
    writel(val | VIC_ROUTE_IRQ, vic_regs + 0x840);

    wmb();

    // 12. State transitions (OEM sequence)
    writel(4, vic_control); // -> Operational
    wmb();

    // 13. Wait for ready state
    timeout = 1000;
    while ((readl(vic_control) & 0x7) != 0 && timeout--) {
        if (timeout % 100 == 0)
            dev_info(dev->dev, "Waiting for ready... state=0x%08x\n", readl(vic_control));
        udelay(1);
    }

    if (timeout <= 0) {
        dev_err(dev->dev, "Timeout waiting for ready state\n");
        ret = -ETIMEDOUT;
        goto err_unmap;
    }

    // 14. Enter running state
    writel(1, vic_control);
    wmb();

    dev_info(dev->dev, "VIC configuration complete and running\n");

    // 15. Debug register dumps
    dev_info(dev->dev, "VIC Final Config:\n");
    dev_info(dev->dev, "  Control: 0x%08x\n", readl(vic_regs + 0x000));
    dev_info(dev->dev, "  Status: 0x%08x\n", readl(vic_regs + 0xb4));
    dev_info(dev->dev, "  DMA Control: 0x%08x\n", readl(vic_regs + 0x900));
    dev_info(dev->dev, "  IRQ Enable: 0x%08x\n", readl(vic_regs + 0x93c));

    dump_vic_registers_detailed();

    // 16. Final state update and cleanup
    mutex_lock(&dev->vic_dev->state_lock);
    dev->vic_dev->state = VIC_STATE_STREAMING;
    mutex_unlock(&dev->vic_dev->state_lock);

    ret = 0;

    err_unmap:
       if (intc_base)
           iounmap(intc_base);
    if (gpioz_base)
        iounmap(gpioz_base);

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

// VIC subdev operations
int vic_init(struct tx_isp_subdev *sd, int on)
{
    struct vic_device *vic = sd->host_priv;
    int ret;

    pr_info("VIC init %s\n", on ? "on" : "off");

    return 0;
}

int vic_reset(struct tx_isp_subdev *sd, int on)
{
    if (on)
        return reset_vic(sd->host_priv);
    return 0;
}


int vic_link_stream(struct tx_isp_subdev *sd, int enable)
{
    struct vic_device *vic = sd->host_priv;
    pr_info("VIC link stream %s\n", enable ? "on" : "off");

//    if (enable) {
//        return setup_vic_dma(vic); // Use existing DMA setup
//    }
    return 0;
}

int vic_link_setup(const struct tx_isp_subdev_pad *local,
                         const struct tx_isp_subdev_pad *remote, u32 flags)
{
    pr_info("VIC link setup\n");
    // This can be a simple passthrough for now as link setup
    // is handled by configure_vic_for_streaming
    return 0;
}


irqreturn_t vic_isr(struct tx_isp_subdev *sd, u32 status, bool *handled)
{
    struct vic_device *vic = tx_isp_get_subdevdata(sd);
    void __iomem *regs = vic->regs;
    u32 irq_status;

    pr_info("VIC ISR triggered:\n");

    irq_status = readl(regs + 0xb4);
    if (!irq_status)
        return IRQ_NONE;

    pr_info("VIC ISR: status=0x%08x\n", irq_status);

    // Clear handled interrupts
    writel(irq_status & 0x33fb, regs + 0xb4);
    wmb();

    *handled = true;
    return IRQ_HANDLED;
}

irqreturn_t vic_isr_thread(struct tx_isp_subdev *sd, void *data)
{
    struct vic_device *vic = tx_isp_get_subdevdata(sd);
    pr_info("VIC ISR thread triggered:\n");

    // Handle frame completion etc
    return IRQ_HANDLED;
}


static inline struct csi_device *get_csi_dev(struct tx_isp_subdev *sd)
{
    return container_of(sd, struct csi_device, sd);
}

int tx_isp_vic_start(struct IMPISPDev *dev)
{
    struct vic_device *vic_dev = dev->vic_dev;
    int ret;

    // Check sensor type/bus type first (OEM check for MIPI vs BT656 etc)
    if (dev->sensor_interface_type != 1) { // MIPI Type
        pr_err("Unsupported sensor bus type: %d\n", dev->sensor_interface_type);
        return -EINVAL;
    }

    pr_info("sensor type is SONY_MIPI!\n");
    writel(0x20000, vic_dev->regs + 0x10); // From decompiled MIPI path

    // Configure pipeline mode
    writel(0x100010, vic_dev->regs + 0x1a4);

    // Configure format and resolution
    writel((dev->width << 16) | dev->height, vic_dev->regs + 0x04);

    // State machine transitions
    writel(2, vic_dev->regs + 0x00); // Enter config mode
    wmb();

    writel(4, vic_dev->regs + 0x00); // Enter operational mode
    wmb();

    // Wait for ready state (state == 0)
    int timeout = 1000;
    while (readl(vic_dev->regs) != 0 && timeout--) {
        if (timeout % 100 == 0) {
            pr_info("Waiting for ready... state=0x%08x\n",
                    readl(vic_dev->regs));
        }
        udelay(1);
    }

    if (timeout <= 0) {
        pr_err("Timeout waiting for VIC ready\n");
        return -ETIMEDOUT;
    }

    // Final state transition
    writel(1, vic_dev->regs + 0x00);  // Enter active state
    wmb();

    // Mark VIC as started
    vic_dev->started = 1;

    // Set the processing flag
    vic_dev->processing = 1;

    return 0;
}

int vic_core_s_stream(struct vic_dev *vic, int enable)
{
    int ret = 0;

    if (!vic)
        return -EINVAL;

    if (!enable) {
        if (vic->state == VIC_STATE_STREAMING) {
            ret = vic_core_set_state(vic, VIC_STATE_INITIALIZED);
        }
        return ret;
    }

    if (vic->state == VIC_STATE_STREAMING)
        return 0;

    // Disable interrupts during configuration
    tx_vic_disable_irq(vic->reg_base);

    // Start the VIC hardware
    ret = configure_vic_for_streaming(vic);
    if (ret)
        goto err_restore_irq;

    ret = vic_core_set_state(vic, VIC_STATE_STREAMING);
    if (ret)
        goto err_restore_irq;

    tx_vic_enable_irq(vic->reg_base);
    return 0;

    err_restore_irq:
        tx_vic_enable_irq(vic->reg_base);
    return ret;
}

int vic_core_ops_init(struct vic_dev *vic, int enable)
{
    int ret;

    if (!vic)
        return -EINVAL;

    if (!enable) {
        // Shutdown sequence
        if (vic->state != VIC_STATE_INITIALIZED)
            return 0;

        tx_vic_disable_irq(vic->reg_base);
        ret = vic_core_set_state(vic, VIC_STATE_CONFIGURED);

    } else {
        // Initialization sequence
        if (vic->state != VIC_STATE_CONFIGURED)
            return 0;

        tx_vic_enable_irq(vic->reg_base);
        ret = vic_core_set_state(vic, VIC_STATE_INITIALIZED);
    }

    return ret;
}

int tx_isp_vic_slake_subdev(struct IMPISPDev *dev)
{
    struct vic_device *vic_dev = dev->vic_dev;
    int ret;

    // Take the state machine mutex
    mutex_lock(&vic_dev->state_lock);

    // Handle transitioning from streaming
    if (vic_dev->state == TX_ISP_MODULE_STREAMING) {
        ret = vic_core_s_stream(dev, 0);
        if (ret) {
            mutex_unlock(&vic_dev->state_lock);
            return ret;
        }
    }

    // If we're in stopping state, initialize
    if (vic_dev->state == TX_ISP_MODULE_STOPPING) {
        ret = vic_core_ops_init(dev, 0);
        if (ret) {
            mutex_unlock(&vic_dev->state_lock);
            return ret;
        }
    }

    // Final transition to init if in ready state
    if (vic_dev->state == TX_ISP_MODULE_READY) {
        vic_dev->state = TX_ISP_MODULE_INIT;
    }

    mutex_unlock(&vic_dev->state_lock);
    return 0;
}