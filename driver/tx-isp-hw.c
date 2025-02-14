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


#include <linux/types.h>
#include <linux/interrupt.h>

// Error codes
#define VIC_SUCCESS          0
#define VIC_ERR_INVALID     -22  // -EINVAL
#define VIC_ERR_BUSY        -16  // -EBUSY

// VIC States
#define VIC_STATE_INIT      1
#define VIC_STATE_READY     2
#define VIC_STATE_RUNNING   3
#define VIC_STATE_STOPPING  4

// Register offsets
#define VIC_ISR         0x00    // Interrupt Status Register
#define VIC_IMR         0x04    // Interrupt Mask Register
#define VIC_IMSR        0x08    // Interrupt Mask Set Register
#define VIC_IMCR        0x0C    // Interrupt Mask Clear Register
#define VIC_IPR         0x10    // Interrupt Pending Register

// VIC interrupt bit
#define VIC_INT_BIT     30      // VIC interrupt is bit 30 in ISR/IMR


// Global state variable
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

//
//int tx_isp_create_link(struct IMPISPDev *dev, struct imp_isp_link *link)
//{
//    struct tx_isp_subdev_pad *src_pad = NULL;
//    struct tx_isp_subdev_pad *dst_pad = NULL;
//    int ret;
//
////    if (link->src.mod == IMP_ISP_MOD_CSI && (!dev->csi_dev->sd || !dev->csi_dev->sd->base)) {
////        pr_err("CSI not ready for linking\n");
////        return -EINVAL;
////    }
////
////    if (link->dst.mod == IMP_ISP_MOD_VIC && (!dev->vic_dev->sd || !dev->vic_dev->sd->base)) {
////        pr_err("VIC not ready for linking\n");
////        return -EINVAL;
////    }
//
//    // Convert pad types to tx_isp equivalents
//    u32 src_type = (link->src.type == 1) ? TX_ISP_PADTYPE_OUTPUT : TX_ISP_PADTYPE_INPUT;
//    u32 dst_type = (link->dst.type == 1) ? TX_ISP_PADTYPE_OUTPUT : TX_ISP_PADTYPE_INPUT;
//
//    // Find source pad
//    src_pad = find_pad(dev, link->src.mod, src_type, link->src.pad_id);
//    if (!src_pad) {
//        pr_err("Failed to find source pad\n");
//        return -EINVAL;
//    }
//
//    // Find destination pad
//    dst_pad = find_pad(dev, link->dst.mod, dst_type, link->dst.pad_id);
//    if (!dst_pad) {
//        pr_err("Failed to find dest pad\n");
//        return -EINVAL;
//    }
//
//    // Configure link in hardware
//    void __iomem *isp_regs = dev->reg_base;
//    void __iomem *link_regs = isp_regs + 0x840;
//
//    switch(link->src.mod) {
//    case IMP_ISP_MOD_CSI:
//        if (link->dst.mod == IMP_ISP_MOD_VIC) {
//            writel(0x00010001, link_regs + 0x0);  // Enable routing
//            writel(0x1, link_regs + 0x4);         // Enable link
//            wmb();
//
//            src_pad->link.flag |= TX_ISP_LINKFLAG_ENABLED;
//            dst_pad->link.flag |= TX_ISP_LINKFLAG_ENABLED;
//        }
//        break;
//
//    case IMP_ISP_MOD_VIC:
//        if (link->dst.mod == IMP_ISP_MOD_DDR) {
//            u32 cfg = readl(link_regs);
//            writel(cfg | 0x2, link_regs);  // Enable DDR routing
//            wmb();
//
//            src_pad->link.flag |= TX_ISP_LINKFLAG_ENABLED;
//            dst_pad->link.flag |= TX_ISP_LINKFLAG_ENABLED;
//        }
//        break;
//    }
//
//    pr_info("Created ISP link: %d->%d\n", link->src.mod, link->dst.mod);
//    return 0;
//}
//
//int tx_isp_destroy_link(struct IMPISPDev *dev, struct imp_isp_link *link)
//{
//    void __iomem *isp_regs = dev->reg_base;
//    u32 reg_val;
//
//    // Set bypass bit, clear processing bit
//    reg_val = readl(isp_regs + ISP_BYPASS_REG);
//    reg_val |= BIT(0);   // Enable bypass
//    reg_val &= ~BIT(1);  // Disable processing
//    writel(reg_val, isp_regs + ISP_BYPASS_REG);
//    wmb();
//
//    // Disable links and routing
//    writel(0, isp_regs + ISP_LINK_ENABLE_REG);
//    writel(0, isp_regs + ISP_ROUTE_REG);
//    wmb();
//
//    return 0;
//}
//
//int setup_isp_links(struct IMPISPDev *dev)
//{
//    // Destroy any existing links first
//    int ret = tx_isp_destroy_link(dev, NULL);
//    if (ret)
//        return ret;
//
//    // Setup CSI -> VIC link
//    struct imp_isp_link link1 = {
//        .src = {
//            .type = ISP_PAD_SOURCE,
//            .mod = IMP_ISP_MOD_CSI,
//            .pad_id = 0
//        },
//        .dst = {
//            .type = ISP_PAD_SINK,
//            .mod = IMP_ISP_MOD_VIC,
//            .pad_id = 0
//        }
//    };
//
//    ret = tx_isp_create_link(dev, &link1);
//    if (ret)
//        return ret;
//
//    // Setup VIC -> DDR link
//    struct imp_isp_link link2 = {
//        .src = {
//            .type = ISP_PAD_SOURCE,
//            .mod = IMP_ISP_MOD_VIC,
//            .pad_id = 0
//        },
//        .dst = {
//            .type = ISP_PAD_SINK,
//            .mod = IMP_ISP_MOD_DDR,
//            .pad_id = 0
//        }
//    };
//
//    ret = tx_isp_create_link(dev, &link2);
//    if (ret)
//        return ret;
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


void dump_irq_info(void) {
    void __iomem *intc_base = ioremap(0x10001000, 0x1000);
    pr_info("ISR: 0x%08x\n", readl(intc_base + 0x00));
    pr_info("IMR: 0x%08x\n", readl(intc_base + 0x04));
    pr_info("IPR: 0x%08x\n", readl(intc_base + 0x10));
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

void configure_vic_interrupts(struct IMPISPDev *isp)
{
    void __iomem *vic_regs = isp->reg_base + VIC_BASE;
    void __iomem *vic_cfg = ioremap(0x10023000, 0x1000);
    void __iomem *isp_ctrl = ioremap(0x13300000, 0x10000);
    void __iomem *intc_base = ioremap(0x10001000, 0x100);

    if (!vic_cfg || !isp_ctrl || !intc_base) {
        dev_err(isp->dev, "Failed to map registers\n");
        goto cleanup;
    }

    // Configure VIC frame control (from existing code)
    writel(0x010123ce, vic_regs + 0x080);
    writel(0x000203ca, vic_regs + 0x084);
    writel(0x00060003, vic_regs + 0x088);
    writel(0x00000010, vic_regs + 0x08c);
    wmb();

    // Configure VIC IRQ routing (from existing code)
    writel(0x00000001, vic_regs + 0x810);
    wmb();

    // VIC Configuration registers (from OEM analysis)
    writel(0x1ffffff3, vic_cfg + 0x28);   // IMSR1
    writel(0x00ffff33, vic_cfg + 0x2C);   // IMCR1
    wmb();

    // ISP Control registers (from OEM analysis)
    writel(0x07800438, isp_ctrl + 0x04);  // IMR
    writel(0xb5742249, isp_ctrl + 0x0C);  // IMCR
    wmb();

    // Top-level interrupt controller (from existing code)
    u32 intc_mask = readl(intc_base + 0x04);
    intc_mask &= ~((1 << 37) | (1 << 38));
    writel(intc_mask, intc_base + 0x04);
    wmb();

    cleanup:
        if (vic_cfg)
            iounmap(vic_cfg);
    if (isp_ctrl)
        iounmap(isp_ctrl);
    if (intc_base)
        iounmap(intc_base);
}

void dump_vic_registers(void)
{
    void __iomem *vic_base, *isp_base, *tuning_base;
    static const struct {
        unsigned int offset;
        const char *name;
    } int_regs[] = {
        { 0x00, "ISR" },
        { 0x04, "IMR" },
        { 0x08, "IMSR" },
        { 0x0C, "IMCR" },
        { 0x10, "IPR" },
        { 0x20, "ISR1" },
        { 0x24, "IMR1" },
        { 0x28, "IMSR1" },
        { 0x2C, "IMCR1" },
        { 0x30, "IPR1" }
    };

    // Map all three regions
    vic_base = ioremap(0x10023000, 0x1000);    // VIC config
    isp_base = ioremap(0x13300000, 0x10000);   // Main ISP control
    tuning_base = ioremap(0x133e0000, 0x10000);// Image tuning params

    if (!vic_base || !isp_base || !tuning_base) {
        printk("Failed to map one or more ISP regions\n");
        goto cleanup;
    }

    printk("\nVIC Configuration (0x10023000):\n");
    printk("============================\n");
    for (int i = 0; i < ARRAY_SIZE(int_regs); i++) {
        u32 val = readl(vic_base + int_regs[i].offset);
        printk("%s (0x%02x): 0x%08x\n",
               int_regs[i].name, int_regs[i].offset, val);
    }

    printk("\nISP Control (0x13300000):\n");
    printk("===========================\n");
    for (int i = 0; i < ARRAY_SIZE(int_regs); i++) {
        u32 val = readl(isp_base + int_regs[i].offset);
        printk("%s (0x%02x): 0x%08x\n",
               int_regs[i].name, int_regs[i].offset, val);
    }

    printk("\nImage Tuning (0x133e0000):\n");
    printk("============================\n");
    for (int i = 0; i < ARRAY_SIZE(int_regs); i++) {
        u32 val = readl(tuning_base + int_regs[i].offset);
        printk("%s (0x%02x): 0x%08x\n",
               int_regs[i].name, int_regs[i].offset, val);
    }

    cleanup:
        if (vic_base)
            iounmap(vic_base);
    if (isp_base)
        iounmap(isp_base);
    if (tuning_base)
        iounmap(tuning_base);
}


void report_vic_interrupts()
{
    void __iomem *vic_base = ioremap(0x10023000, 0x1000);
    void __iomem *intc_base = ioremap(0x10001000, 0x100);
    u32 imr, imcr_val;

    if (!vic_base || !intc_base) {
        pr_err("Failed to map registers\n");
        goto out;
    }
    // 3. Read and verify IMR state
    imr = readl(intc_base + 0x04);
    pr_info("Existing IMR: 0x%08x\n", imr);

    // 4. Verify configuration
    pr_info("Configuration status:\n");
    pr_info("  ISR: 0x%08x\n", readl(intc_base + 0x00));
    pr_info("  IMR: 0x%08x\n", readl(intc_base + 0x04));
    pr_info("  IPR: 0x%08x\n", readl(intc_base + 0x10));
    pr_info("VIC Status: 0x%08x\n", readl(vic_base + 0x00));

    // 5. Read pre/post operational states
    pr_info("Pre-operational state: 0x%08x\n",
             readl(vic_base + 0x870));
    pr_info("Post-operational state: 0x%08x\n",
             readl(vic_base + 0x880));

out:
    if (intc_base)
        iounmap(intc_base);
    if (vic_base)
        iounmap(vic_base);
}

static int configure_vic_gains(struct IMPISPDev *isp) {
    void __iomem *vic_regs = isp->reg_base + VIC_BASE;

    // Initial blc/gain configuration
    writel(0x00000000, vic_regs + 0x1008);
    writel(0x00000000, vic_regs + 0x1000);
    writel(0x00000000, vic_regs + 0x1004);
    writel(0x00000000, vic_regs + 0x100C);
    writel(0x00000000, vic_regs + 0x1010);

    // BLC again interpolation
    writel(0x00000001, vic_regs + 0x1014);
    writel(0x00000000, vic_regs + 0x1018);
    writel(0x00000000, vic_regs + 0x101C);
    writel(0x00000000, vic_regs + 0x1020);

    return 0;
}

int vic_mdma_enable(struct IMPISPDev *isp)
{
    void __iomem *vic_regs = isp->reg_base + VIC_BASE;
    struct isp_channel *chn = &isp->channels[0];
    uint32_t width = ALIGN(chn->width, 32);
    uint32_t height = ALIGN(chn->height, 32);

    // Begin with basic DMA setup
    writel(1, vic_regs + 0x308);
    wmb();

    // Set dimensions
    writel((width << 16) | height, vic_regs + 0x304);
    writel(width, vic_regs + 0x310);
    writel(width, vic_regs + 0x314);
    wmb();

    // Write parameter configurations
    writel(0x80000020, vic_regs + 0x300);
    writel(0x00000001, vic_regs + 0x30ac);  // Trigger refresh
    wmb();

    return 0;
}

void configure_vic_buffer_chain(struct IMPISPDev *isp)
{
    void __iomem *isp_regs = ioremap(0x13309880, 0x1000);
    void __iomem *vic_regs = ioremap(0x10023000, 0x1000);
    struct isp_channel *chn = &isp->channels[0];
    uint32_t buffer_start = ALIGN(isp->dma_addr + 0x1094d4, 32);

    // Calculate buffer offsets using same logic
    uint32_t width = ALIGN(chn->width, 32);
    uint32_t height = ALIGN(chn->height, 32);
    uint32_t raw10_stride = ALIGN((width * 10) / 8, 32);
    uint32_t raw10_size = ALIGN(raw10_stride * height, 4096);
    uint32_t y_stride = ALIGN(width, 32);
    uint32_t y_size = ALIGN(y_stride * height, 4096);
    uint32_t uv_size = ALIGN((y_stride * height) / 2, 4096);

    // Frame control metadata
    writel(0x20151120, vic_regs + 0x800);
    writel(0x00000110, vic_regs + 0x804);
    writel(0x00000000, vic_regs + 0x808);
    writel(0x20151120, vic_regs + 0x80c);
    wmb();

    // Frame sync and control - KEEP WORKING VALUES
//    writel(0x11111111, vic_regs + 0x810);
//    writel(0x00f13101, vic_regs + 0x814);
//    writel(0x00000000, vic_regs + 0x818);
//    writel(0x20151120, vic_regs + 0x81c);
//    wmb();

    // Set up configuration blocks (0x820-0x860)
    writel(0x06300000, vic_regs + 0x820);
    writel(0x00000780, vic_regs + 0x824);
    writel(0x064fa400, vic_regs + 0x828);
    writel(0x00000780, vic_regs + 0x82c);
    wmb();

    writel(0x065f7600, vic_regs + 0x830);
    writel(0x00000040, vic_regs + 0x834);
    writel(0x00000000, vic_regs + 0x838);
    writel(0x00000001, vic_regs + 0x83c);
    wmb();

    writel(0x065f8740, vic_regs + 0x840);
    writel(0x00000040, vic_regs + 0x844);
    writel(0x065f9880, vic_regs + 0x848);
    writel(0x00000040, vic_regs + 0x84c);
    wmb();

    writel(0x065fa9c0, vic_regs + 0x850);
    writel(0x00000040, vic_regs + 0x854);
    writel(0x065fbb00, vic_regs + 0x858);
    writel(0x000003c0, vic_regs + 0x85c);
    wmb();

    writel(0x066f8d00, vic_regs + 0x860);
    writel(0x000003c0, vic_regs + 0x864);
    writel(0x06777600, vic_regs + 0x868);
    writel(0x00000040, vic_regs + 0x86c);
    wmb();

//    // Processing stage configuration blocks
//    writel(0x00000002, vic_regs + 0x870);  // Control flags (unchanged)
//    writel(0x78441010, vic_regs + 0x874);  // OEM timing parameters
//    writel(0x000014f1, vic_regs + 0x878);  // Status config (unchanged)
//    writel(0xff0200a4, vic_regs + 0x87c);  // OEM control masks
//    wmb();
//
//    // Second processing stage
//    writel(0x00080000, vic_regs + 0x880);  // OEM offset
//    writel(0x04300780, vic_regs + 0x884);  // OEM dimensions
//    writel(0x00000002, vic_regs + 0x888);  // Control flags (unchanged)
//    writel(0x78431010, vic_regs + 0x88c);  // OEM timing
//    wmb();

    // Third processing stage
    writel(0x000014f0, vic_regs + 0x890);  // Config ID (unchanged)
    writel(0xff0200a4, vic_regs + 0x894);  // OEM control mask
    writel(0x00000008, vic_regs + 0x898);  // OEM value
    writel(0x04380778, vic_regs + 0x89c);  // OEM dimensions
    wmb();

    // Fourth processing stage
    writel(0x00000003, vic_regs + 0x8a0);  // Control flags (unchanged)
    writel(0x78441010, vic_regs + 0x8a4);  // OEM timing
    writel(0x000014f0, vic_regs + 0x8a8);  // Config ID (unchanged)
    writel(0xff0200a4, vic_regs + 0x8ac);  // OEM control mask
     wmb();

    // Final stage
    writel(0x00080008, vic_regs + 0x8b0);  // OEM offsets
    writel(0x04300778, vic_regs + 0x8b4);  // OEM dimensions
    writel(0x00000001, vic_regs + 0x8b8);  // OEM control flag
    writel(0x78431010, vic_regs + 0x8bc);  // OEM timing
    wmb();

    // New: Calibration data setup (0x8d0-0x900)
    writel(0x00240020, vic_regs + 0x8d0);
    writel(0x00070006, vic_regs + 0x8d4);
    writel(0x00090008, vic_regs + 0x8d8);
    writel(0x00000000, vic_regs + 0x8dc);

    writel(0x0a0a0a0a, vic_regs + 0x8e0);
    writel(0x00008c8c, vic_regs + 0x8e4);
    writel(0x00000100, vic_regs + 0x8e8);
    writel(0x80808080, vic_regs + 0x8ec);

    writel(0x80808080, vic_regs + 0x8f0);
    writel(0x80807e7c, vic_regs + 0x8f4);
    writel(0x80808080, vic_regs + 0x8f8);
    writel(0x00000104, vic_regs + 0x8fc);

    // First row of 0x900
    writel(0x80808080, vic_regs + 0x900);
    writel(0x80808080, vic_regs + 0x904);
    writel(0x80808080, vic_regs + 0x908);
    writel(0x80808080, vic_regs + 0x90c);
    wmb();

    // DMA Control
    writel(0x80808080, vic_regs + 0x940);
    writel(0xff023054, vic_regs + 0x944);
    writel(0xc0b38005, vic_regs + 0x948);
    writel(0xe0dbd5cd, vic_regs + 0x94c);
    wmb();

    // LUT/Calibration data (0x940-0x990)
    for (int i = 0; i < 4; i++) {
        writel(0xf5f0ebe6, vic_regs + 0x950 + i*16);
        writel(0xfafafafa, vic_regs + 0x954 + i*16);
        writel(0xc0b38005, vic_regs + 0x958 + i*16);
        writel(0xe0dbd5cd, vic_regs + 0x95c + i*16);
    }
    wmb();

    // Configuration values
    writel(0xf5f0ebe6, vic_regs + 0x990);
    writel(0xfafafafa, vic_regs + 0x994);
    writel(0x00141402, vic_regs + 0x998);
    writel(0x00d214e1, vic_regs + 0x99c);
    wmb();

   // Processing tables (0x9b0-0x9f0)
    writel(0x00000000, vic_regs + 0x9b0);
    writel(0x00000000, vic_regs + 0x9b4);
    writel(0x97979797, vic_regs + 0x9b8);
    writel(0x97979797, vic_regs + 0x9bc);
    writel(0x65979797, vic_regs + 0x9c0);
    writel(0x00000133, vic_regs + 0x9c4);
    writel(0x97979797, vic_regs + 0x9c8);
    writel(0x97979797, vic_regs + 0x9cc);
    writel(0x65979797, vic_regs + 0x9d0);
    writel(0x00010133, vic_regs + 0x9d4);
    writel(0x00281e14, vic_regs + 0x9d8);
    writel(0x000000dc, vic_regs + 0x9dc);
    writel(0x00000000, vic_regs + 0x9e0);
    writel(0x00281e14, vic_regs + 0x9e4);
    writel(0x000000dc, vic_regs + 0x9e8);
    writel(0x00000000, vic_regs + 0x9ec);
    writel(0x00000000, vic_regs + 0x9f0);
    writel(0x00000000, vic_regs + 0x9f4);
    writel(0x00000000, vic_regs + 0x9f8);
    writel(0x00000000, vic_regs + 0x9fc);
    wmb();

    // Image processing settings (0xa60-0xaf0)
    writel(0x00000000, vic_regs + 0xa60);
    writel(0xffff3141, vic_regs + 0xa64);
    writel(0x00141400, vic_regs + 0xa68);
    writel(0x05c80aff, vic_regs + 0xa6c);
    writel(0x000000ff, vic_regs + 0xa70);
    writel(0x09091480, vic_regs + 0xa74);
    writel(0x00000000, vic_regs + 0xa78);
    writel(0x00000000, vic_regs + 0xa7c);
    wmb();

    writel(0x0017ffa1, vic_regs + 0xa80);
    wmb();
    writel(0x00060000, vic_regs + 0xa84);
    wmb();
    writel(0x021003c0, vic_regs + 0xa88);
    wmb();
    writel(0x00000000, vic_regs + 0xa8c);
    wmb();

    writel(0xffffffff, vic_regs + 0xa90);
    writel(0xffffffff, vic_regs + 0xa94);
    writel(0xffffffff, vic_regs + 0xa98);
    writel(0x00ffffff, vic_regs + 0xa9c);
    writel(0xffffffff, vic_regs + 0xaa0);
    writel(0xffffffff, vic_regs + 0xaa4);
    writel(0xffffffff, vic_regs + 0xaa8);
    writel(0x00ffffff, vic_regs + 0xaac);
    writel(0x001e140a, vic_regs + 0xab0);
    writel(0x000000c8, vic_regs + 0xab4);
    writel(0x00000000, vic_regs + 0xab8);
    writel(0x001e140a, vic_regs + 0xabc);
    wmb();

    writel(0x000000c8, vic_regs + 0xac0);
    writel(0x00000001, vic_regs + 0xac4);
    writel(0x00000000, vic_regs + 0xac8);
    writel(0x00000000, vic_regs + 0xacc);
    writel(0x00000000, vic_regs + 0xad0);
    writel(0x00000000, vic_regs + 0xad4);
    writel(0x1e140a80, vic_regs + 0xad8);
    writel(0x00000000, vic_regs + 0xadc);
    writel(0x00000000, vic_regs + 0xae0);
    writel(0x00000000, vic_regs + 0xae4);
    writel(0x00000000, vic_regs + 0xae8);
    writel(0x00200040, vic_regs + 0xaec);
    writel(0x00e8d7b4, vic_regs + 0xaf0);
    writel(0x00006464, vic_regs + 0xaf4);
    writel(0x00000000, vic_regs + 0xaf8);
    writel(0x00001410, vic_regs + 0xafc);
    wmb();

    iounmap(isp_regs);
    iounmap(vic_regs);
}

void configure_vic_buffer_chain2(struct IMPISPDev *dev)
{
    void __iomem *isp_regs = ioremap(0x13309880, 0x1000);
    void __iomem *vic_regs = ioremap(0x10023000, 0x1000);
    // Initial status/IRQ config
    writel(0x11111111, vic_regs + 0x810);
    writel(0x00f11101, vic_regs + 0x814);
    writel(0x00000000, vic_regs + 0x818);
    writel(0x20151120, vic_regs + 0x81c);

    // Base buffer config - using OEM offsets
    uint32_t base_addr = 0x02a80000;  // Match OEM base
    writel(base_addr, vic_regs + 0x820);
    writel(0x780, vic_regs + 0x824);
    writel(0x02c7a400, vic_regs + 0x828);
    writel(0x780, vic_regs + 0x82c);
    writel(0x02d77600, vic_regs + 0x830);
    writel(0x40, vic_regs + 0x834);
    writel(0x0, vic_regs + 0x838);
    writel(0x1, vic_regs + 0x83c);

    // Buffer chain - exact OEM addresses
    writel(0x02d78740, vic_regs + 0x840);
    writel(0x40, vic_regs + 0x844);
    writel(0x02d79880, vic_regs + 0x848);
    writel(0x40, vic_regs + 0x84c);
    writel(0x02d7a9c0, vic_regs + 0x850);
    writel(0x40, vic_regs + 0x854);
    writel(0x02d7bb00, vic_regs + 0x858);
    writel(0x3c0, vic_regs + 0x85c);
    writel(0x02e78d00, vic_regs + 0x860);
    writel(0x3c0, vic_regs + 0x864);
    writel(0x02ef7600, vic_regs + 0x868);
    writel(0x40, vic_regs + 0x86c);

    // Configuration data from 0x870-0x900
    writel(0x00000003, vic_regs + 0x870);
    writel(0x45271c1c, vic_regs + 0x874);
    writel(0x000014f1, vic_regs + 0x878);
    writel(0xff0800e4, vic_regs + 0x87c);
    writel(0x000e0000, vic_regs + 0x880);
    writel(0x042a0780, vic_regs + 0x884);
    writel(0x00000003, vic_regs + 0x888);
    writel(0x45271c1c, vic_regs + 0x88c);
    writel(0x000014f0, vic_regs + 0x890);
    writel(0xff0800e4, vic_regs + 0x894);
    writel(0x0000000e, vic_regs + 0x898);
    writel(0x04380772, vic_regs + 0x89c);
    writel(0x00000003, vic_regs + 0x8a0);
    writel(0x45271c1c, vic_regs + 0x8a4);
    writel(0x000014f0, vic_regs + 0x8a8);
    writel(0xff0800e4, vic_regs + 0x8ac);
    writel(0x000e000e, vic_regs + 0x8b0);
    writel(0x042a0772, vic_regs + 0x8b4);
    writel(0x00000003, vic_regs + 0x8b8);
    writel(0x45271c1c, vic_regs + 0x8bc);
    writel(0x000014f1, vic_regs + 0x8c0);
    writel(0xff0800e4, vic_regs + 0x8c4);
    writel(0x00000000, vic_regs + 0x8c8);
    writel(0x001c0018, vic_regs + 0x8cc);
    writel(0x00240020, vic_regs + 0x8d0);
    writel(0x00070006, vic_regs + 0x8d4);
    writel(0x00090008, vic_regs + 0x8d8);
    writel(0x00000000, vic_regs + 0x8dc);
    writel(0x0a0a0a0a, vic_regs + 0x8e0);
    writel(0x0000a8a8, vic_regs + 0x8e4);
    writel(0x00000100, vic_regs + 0x8e8);
    writel(0x80808080, vic_regs + 0x8ec);
    writel(0x80808080, vic_regs + 0x8f0);
    writel(0x80808080, vic_regs + 0x8f4);
    writel(0x80808080, vic_regs + 0x8f8);
    writel(0x00000104, vic_regs + 0x8fc);

    // DMA Control
    writel(0x80808080, vic_regs + 0x900);
    writel(0x80808080, vic_regs + 0x904);
    writel(0x80808080, vic_regs + 0x908);
    writel(0x80808080, vic_regs + 0x90c);
    wmb();

    // Extended DMA/Control section
    writel(0x00000106, vic_regs + 0x910);
    writel(0x80808080, vic_regs + 0x914);
    writel(0x80808080, vic_regs + 0x918);
    writel(0x80808080, vic_regs + 0x91c);
    writel(0x80808080, vic_regs + 0x920);
    writel(0x00000140, vic_regs + 0x924);
    writel(0x80808080, vic_regs + 0x928);
    writel(0x80808080, vic_regs + 0x92c);
    writel(0x80808080, vic_regs + 0x930);
    writel(0x80808080, vic_regs + 0x934);
    writel(0x00000001, vic_regs + 0x938);
    writel(0x80808080, vic_regs + 0x93c);

    // Processing configuration
    writel(0x00000080, vic_regs + 0x940);
    writel(0xff083074, vic_regs + 0x944);
    writel(0xc8b4a000, vic_regs + 0x948);
    writel(0xfafaf0dc, vic_regs + 0x94c);

    // Processing blocks
    for (int i = 0; i < 5; i++) {
        writel(0xfafafafa, vic_regs + 0x950 + i*0x10);
        writel(0xfafafafa, vic_regs + 0x954 + i*0x10);
        writel(0xc8b4a000, vic_regs + 0x958 + i*0x10);
        writel(0xfafaf0dc, vic_regs + 0x95c + i*0x10);
    }


    iounmap(isp_regs);
    iounmap(vic_regs);
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


    // VIC Control registers
//    writel0x58050000(0x80007000, regs + 0x110);  // VIC Control
//    wmb();
    pr_info("ISP Processing Configuration complete\n");
}

static int configure_vic_core(struct IMPISPDev *isp)
{
    void __iomem *vic_regs = isp->reg_base + VIC_BASE;

    // Core register base config to match working OEM dump
    writel(0x001f0003, vic_regs + 0x000);  // Control
    writel(0x00040001, vic_regs + 0x004);  // Config
    writel(0x010a0301, vic_regs + 0x008);  // Corrected
    writel(0x0aff0a00, vic_regs + 0x00c);  // Corrected
    writel(0x01003232, vic_regs + 0x010);  // MIPI timing
    wmb();

    // Extra registers from OEM dump
    writel(0x003fff08, vic_regs + 0x020);
    writel(0x00133200, vic_regs + 0x024);
    writel(0x00010610, vic_regs + 0x028);
    writel(0xff00ff00, vic_regs + 0x02c);
    wmb();

    return 0;
}

static int configure_vic_processing(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;

    // Process blocks configuration (0x990-0x9FC)
    writel(0xfafafafa, vic_regs + 0x990);
    writel(0xfafafafa, vic_regs + 0x994);
    writel(0x00141402, vic_regs + 0x998);
    writel(0x0ef000ff, vic_regs + 0x99c);
    writel(0x000000ff, vic_regs + 0x9a0);
    writel(0x00000508, vic_regs + 0x9a4);
    writel(0x00000000, vic_regs + 0x9a8);
    writel(0x00000508, vic_regs + 0x9ac);
    wmb();

    // Processing configuration (0x9b0-0x9fc)
	// 0x9b0-0x9d0 block (note specific pattern in OEM)
    writel(0x00000000, vic_regs + 0x9b0);
    writel(0x00000000, vic_regs + 0x9b4);
    writel(0xc4c4c4c4, vic_regs + 0x9b8);
    writel(0xc4c4c4c4, vic_regs + 0x9bc);
    writel(0x8fc4c4c4, vic_regs + 0x9c0);
    writel(0x00002e5d, vic_regs + 0x9c4);
    writel(0xc4c4c4c4, vic_regs + 0x9c8);
    writel(0xc4c4c4c4, vic_regs + 0x9cc);
    writel(0x8fc4c4c4, vic_regs + 0x9d0);
    writel(0x00002e5d, vic_regs + 0x9d4);
    writel(0x00281e14, vic_regs + 0x9d8);
    writel(0x000000dc, vic_regs + 0x9dc);
    writel(0x00000000, vic_regs + 0x9e0);
    writel(0x00281e14, vic_regs + 0x9e4);
    writel(0x000000dc, vic_regs + 0x9e8);
    writel(0x00000000, vic_regs + 0x9ec);
    writel(0x42424242, vic_regs + 0x9f0);
    writel(0x42424242, vic_regs + 0x9f4);
    writel(0x42424242, vic_regs + 0x9f8);
    writel(0x00002142, vic_regs + 0x9fc);
    wmb();

    // Processing control (0xa00-0xa74)
    // 0xa00-0xa40 block (correcting pattern)
    writel(0x00004300, vic_regs + 0xa00);
    writel(0x00008080, vic_regs + 0xa04);
    writel(0x00000000, vic_regs + 0xa08);
    writel(0x80808080, vic_regs + 0xa0c);

    // Modified loop for 0xa10-0xa40 to match OEM pattern exactly
    writel(0x80808080, vic_regs + 0xa10);
    writel(0x80808080, vic_regs + 0xa14);
    writel(0x80808080, vic_regs + 0xa18);
    writel(0x00000000, vic_regs + 0xa1c);

    writel(0x80808080, vic_regs + 0xa20);
    writel(0x80808080, vic_regs + 0xa24);
    writel(0x80808080, vic_regs + 0xa28);
    writel(0x80808080, vic_regs + 0xa2c);

    writel(0x00000000, vic_regs + 0xa30);
    writel(0x80808080, vic_regs + 0xa34);
    writel(0x80808080, vic_regs + 0xa38);
    writel(0x80808080, vic_regs + 0xa3c);

    writel(0x80808080, vic_regs + 0xa40);
    writel(0x00000000, vic_regs + 0xa44);
    writel(0x80808080, vic_regs + 0xa48);
    writel(0x80808080, vic_regs + 0xa4c);

    wmb();

    // Final configuration blocks
    writel(0x80808080, vic_regs + 0xa50);
    writel(0x80808080, vic_regs + 0xa54);
    writel(0x00000001, vic_regs + 0xa58);
    writel(0x00808080, vic_regs + 0xa5c);
    writel(0x00000000, vic_regs + 0xa60);
    writel(0xff603141, vic_regs + 0xa64);
    writel(0x00141400, vic_regs + 0xa68);
    writel(0x12e40aff, vic_regs + 0xa6c);
    writel(0x000000ff, vic_regs + 0xa70);
    writel(0x09091480, vic_regs + 0xa74);
	wmb();

    return 0;
}

static int configure_vic_streaming(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    void __iomem *vic_control = dev->reg_base + 0xb8;
    int timeout, ret;

    pr_info("Configuring VIC streaming\n");

    // Frame/interrupt control - gentler values
    writel(0x01000000, vic_regs + 0x080);  // Basic frame control
    writel(0x00000000, vic_regs + 0x084);  // Clear interrupt config
    writel(0x00080000, vic_regs + 0x088);  // Basic interrupt enable
    writel(0x00010001, vic_regs + 0x08c);  // Frame sync control
    wmb();

    // Timing configuration
    writel(0x001e140a, vic_regs + 0xab0);
    writel(0x000000c8, vic_regs + 0xab4);
    writel(0x00000000, vic_regs + 0xab8);
    writel(0x001e140a, vic_regs + 0xabc);
    writel(0x000000c8, vic_regs + 0xac0);
    writel(0x00000001, vic_regs + 0xac4);
    wmb();

    // State transition sequence
    writel(4, vic_control); // -> Operational
    wmb();

    // Wait for ready state
    timeout = 1000;
    while ((readl(vic_control) & 0x7) != 0 && timeout--) {
        if (timeout % 100 == 0) {
            dev_info(dev->dev, "Waiting for ready... state=0x%08x\n",
                    readl(vic_control));
            // Debug read the interrupt status
            dev_info(dev->dev, "IRQ Status: 0x%08x\n",
                    readl(vic_regs + 0xb4));
        }
        udelay(1);
    }

    if (timeout <= 0) {
        dev_err(dev->dev, "Timeout waiting for ready state\n");
        return -ETIMEDOUT;
    }

    // Final control values
    writel(0x00004040, vic_regs + 0xb00);
    wmb();
    writel(0x20151120, vic_regs + 0xb04);
    wmb();
    writel(0x20151120, vic_regs + 0xb08);
    wmb();
    writel(0x20151120, vic_regs + 0xb0c);
    wmb();

    // Enter running state
    writel(1, vic_control);
    wmb();

    return 0;
}

static int configure_vic_sensor_type(struct IMPISPDev *isp)
{
    void __iomem *vic_regs = isp->reg_base + VIC_BASE;
    u32 mipi_mode;

    dev_info(isp->dev, "Setting sensor type - before: 0x%08x\n",
         readl(vic_regs + 0x014));

    if (isp->sensor_type == SENSOR_TYPE_MIPI_OTHER) {
        dev_info(isp->dev, "sensor type is OTHER_MIPI!\n");
        // Set sensor type register directly to match OEM
        writel(0x00000a02, vic_regs + 0x014);  // SensorType
        writel(0x01003232, vic_regs + 0x010);  // MIPI
        writel(0x0a08003f, vic_regs + 0x018);  // SensorMode
        mipi_mode = 0xa000a;
        dev_info(isp->dev, "After writing 0x300: 0x%08x\n",
        readl(vic_regs + 0x014));
    } else if (isp->sensor_type == SENSOR_TYPE_MIPI_SONY) {
        dev_info(isp->dev, "sensor type is SONY_MIPI!\n");
        mipi_mode = 0x100010;
        writel(0x20000, vic_regs + 0x10);
    } else {
        dev_err(isp->dev, "Unsupported sensor type\n");
        return -EINVAL;
    }

    // Write MIPI mode
    writel(mipi_mode, vic_regs + 0x1a4);
    wmb();

    dev_info(isp->dev, "After writing 0x300: 0x%08x\n",
         readl(vic_regs + 0x014));

    msleep(1000);

    dev_info(isp->dev, "After writing 0x300: 0x%08x\n",
         readl(vic_regs + 0x014));

    msleep(1000);

    dev_info(isp->dev, "After writing 0x300: 0x%08x\n",
         readl(vic_regs + 0x014));

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

void tx_vic_enable_irq(struct IMPISPDev *dev)
{
    void __iomem *vic_base = ioremap(0x10023000, 0x1000);    // Interrupt controller
    void __iomem *isp_ctrl = ioremap(0x13300000, 0x10000);
    void __iomem *tuning = ioremap(0x133e0000, 0x10000);
    unsigned long flags;

    if (!vic_base || !isp_ctrl || !tuning) {
        dev_err(dev->dev, "Failed to map registers\n");
        goto cleanup;
    }

    spin_lock_irqsave(&dev->vic_dev->lock, flags);
    if (!dev->vic_dev->irq_enabled) {
        // VIC setup (same as before)
        writel(0x0, vic_base + 0x04);
        writel(0x0, vic_base + 0x08);
        writel(0x0, vic_base + 0x0c);
        wmb();
        writel(0x00000001, vic_base + 0x04);
        wmb();
        writel(0x00000001, vic_base + 0x0c);
        wmb();

        // ISP Control (same as before)
        writel(0x07800438, isp_ctrl + 0x04);
        writel(0xb5742249, isp_ctrl + 0x0c);
        wmb();
        writel(0x00000000, isp_ctrl + 0x08);
        wmb();
        writel(0x8fffffff, isp_ctrl + 0x30);
        wmb();

        // Configure Image Tuning interrupts
        writel(0x07800438, tuning + 0x04);  // IMR - Same as ISP Control
        writel(0x00000002, tuning + 0x0c);  // IMCR
        wmb();
        writel(0x00000000, tuning + 0x08);  // IMSR
        wmb();
        writel(0x00000001, tuning + 0x00);  // Set ISR bit explicitly
        wmb();

        dev->vic_dev->irq_enabled = 1;
    }
    spin_unlock_irqrestore(&dev->vic_dev->lock, flags);

    cleanup:
        if (vic_base)
            iounmap(vic_base);
    if (isp_ctrl)
        iounmap(isp_ctrl);
    if (tuning)
        iounmap(tuning);
}

void tx_vic_disable_irq(struct IMPISPDev *dev)
{
    void __iomem *base = dev->reg_base;
    void __iomem *isp_ctrl = ioremap(0x13300000, 0x10000);
    unsigned long flags;

    if (!base || !isp_ctrl) {
        dev_err(dev->dev, "Failed to map registers\n");
        goto cleanup;
    }

    spin_lock_irqsave(&dev->vic_dev->lock, flags);
    if (dev->vic_dev->irq_enabled) {

        // Disable ISP Control interrupts
        writel(0x00000000, isp_ctrl + 0x04);  // Clear IMR
        wmb();

        dev->vic_dev->irq_enabled = 0;
    }
    spin_unlock_irqrestore(&dev->vic_dev->lock, flags);

    cleanup:
        if (isp_ctrl)
            iounmap(isp_ctrl);
}

static int tx_isp_vic_start(struct IMPISPDev *dev)
{
   void __iomem *vic_base = ioremap(0x10023000, 0x1000);
   void __iomem *vic_control = dev->reg_base + 0xb8;
   void __iomem *isp_ctrl = ioremap(0x13300000, 0x10000);
   void __iomem *tuning = ioremap(0x133e0000, 0x10000);
   void *y_virt = NULL, *uv_virt = NULL;
   dma_addr_t y_phys = 0, uv_phys = 0;
   int ret = 0;

   size_t y_size = dev->sensor_width * dev->sensor_height;
   size_t uv_size = y_size / 2;

   // Save original states before we do anything
   u32 original_phy_state = 0;
   u32 original_isr = 0;
   u32 original_imr = 0;
   if (dev->csi_dev && dev->csi_dev->csi_regs) {
       original_phy_state = readl(dev->csi_dev->csi_regs + 0x14);
       pr_info("Preserving initial PHY state: 0x%08x\n", original_phy_state);
   }
   if (isp_ctrl) {
       original_isr = readl(isp_ctrl + 0x000);
       original_imr = readl(isp_ctrl + 0x04);
   }

   pr_info("VIC: Allocating DMA buffers: Y=%zux%zu UV=%zu\n",
           dev->sensor_width, dev->sensor_height, uv_size);

   if (!vic_base || !isp_ctrl || !tuning) {
       dev_err(dev->dev, "Failed to map registers\n");
       ret = -ENOMEM;
       goto cleanup;
   }

   // Configure VIC interrupts (keeping existing bits)
   writel(readl(vic_base + 0x04) | 0x00000001, vic_base + 0x04);  // IMR
   writel(readl(vic_base + 0x0c) | 0x00000001, vic_base + 0x0c);  // IMCR
   wmb();

   // DMA buffer allocation
   y_virt = dma_alloc_coherent(dev->dev, y_size, &y_phys, GFP_KERNEL);
   if (!y_virt) {
       dev_err(dev->dev, "Failed to allocate Y plane DMA buffer\n");
       ret = -ENOMEM;
       goto cleanup;
   }
   pr_info("VIC: Allocated Y plane DMA buffer: virt=%p phys=0x%llx\n",
           y_virt, (unsigned long long)y_phys);

   uv_virt = dma_alloc_coherent(dev->dev, uv_size, &uv_phys, GFP_KERNEL);
   if (!uv_virt) {
       dev_err(dev->dev, "Failed to allocate UV plane DMA buffer\n");
       ret = -ENOMEM;
       goto cleanup;
   }
   pr_info("VIC: Allocated UV plane DMA buffer: virt=%p phys=0x%llx\n",
           uv_virt, (unsigned long long)uv_phys);

   // Set ISP Control registers while preserving PHY-related bits
   writel((original_isr & 0xFF000000) | 0x54560031, isp_ctrl + 0x000);  // Keep high bits
   wmb();
   writel(original_imr | 0x07800438, isp_ctrl + 0x04);   // Only set needed bits
   writel(0xb5742249, isp_ctrl + 0x0c);   // IMCR
   writel(0x00400040, isp_ctrl + 0x2c);   // IMCR1
   wmb();

   // Basic ISP config - be careful with register that might affect PHY
   writel(readl(isp_ctrl + 0x1a4) | 0xa000a, isp_ctrl + 0x1a4);
   wmb();

   // Frame buffer setup for NV12
   uint32_t mult = 8;  // Multiplier for buffer size
   uint32_t frame_size = mult * (dev->sensor_width * dev->sensor_height * 3/2);
   uint32_t header_size = (frame_size >> 5) + ((frame_size & 0x1f) ? 1 : 0);
   writel(header_size, isp_ctrl + 0x100);  // Set header size
   wmb();

   // Set frame header registers
   writel(0x072d7001, isp_ctrl + 0x104);
   writel(0x00041300, isp_ctrl + 0x108);
   wmb();

   // Configure DMA control and pattern registers
   writel(readl(isp_ctrl + 0x004) | 0x00000004, isp_ctrl + 0x004);  // Set bits without clearing
   wmb();

   // Write DMA addresses
   writel(y_phys & 0xFFFFFFF8, isp_ctrl + 0x008);    // Y address
   writel(uv_phys & 0xFFFFFFF8, isp_ctrl + 0x00c);   // UV address
   wmb();

   // Set DMA pattern registers
   writel(0x01000008, isp_ctrl + 0x010);  // DMA pattern 1
   writel(0x01010001, isp_ctrl + 0x01c);  // DMA pattern 2
   writel(0x01010001, isp_ctrl + 0x028);  // DMA pattern 3
   wmb();

   pr_info("VIC: DMA setup sequence - Writing addresses and patterns\n");
   pr_info("VIC: Initial Y addr=0x%x UV addr=0x%x\n",
           readl(isp_ctrl + 0x008), readl(isp_ctrl + 0x00c));

   // Double-check the addresses are set
   writel(y_phys & 0xFFFFFFF8, isp_ctrl + 0x008);
   wmb();
   writel(uv_phys & 0xFFFFFFF8, isp_ctrl + 0x00c);
   wmb();

   // Mode transitions - be careful with these
   writel(2, vic_control + 0x00);
   wmb();

   int timeout = 1000;
   while (readl(vic_control + 0x10) & 1) {
       if (timeout-- == 0) {
           dev_err(dev->dev, "Timeout waiting for mode 2\n");
           ret = -ETIMEDOUT;
           goto cleanup;
       }
       cpu_relax();
   }

   writel(4, vic_control + 0x00);
   wmb();

   timeout = 1000;
   while (readl(vic_control + 0x00) != 0 && timeout--) {
       cpu_relax();
   }

   if (timeout <= 0) {
       dev_err(dev->dev, "Timeout waiting for mode transition\n");
       ret = -ETIMEDOUT;
       goto cleanup;
   }

   writel(1, vic_control + 0x00);
   wmb();

   // Verify PHY state is preserved
   if (dev->csi_dev && dev->csi_dev->csi_regs) {
       u32 final_phy_state = readl(dev->csi_dev->csi_regs + 0x14);
       pr_info("Final PHY state: 0x%08x (original: 0x%08x)\n",
               final_phy_state, original_phy_state);
   }

   // Store allocations in device structure
   dev->y_virt = y_virt;
   dev->y_phys = y_phys;
   dev->uv_virt = uv_virt;
   dev->uv_phys = uv_phys;

cleanup:
   if (ret < 0) {
       if (y_virt)
           dma_free_coherent(dev->dev, y_size, y_virt, y_phys);
       if (uv_virt)
           dma_free_coherent(dev->dev, uv_size, uv_virt, uv_phys);
   }

   if (vic_base)
       iounmap(vic_base);
   if (isp_ctrl)
       iounmap(isp_ctrl);
   if (tuning)
       iounmap(tuning);

   return ret;
}

int vic_core_ops_init(struct IMPISPDev *dev, int enable)
{
    struct vic_device *vic = dev->vic_dev;

    if (enable == 0) {
        if (vic->state != VIC_STATE_READY) {
            tx_vic_disable_irq(dev);
            vic->state = VIC_STATE_READY;
        }
    } else {
        if (vic->state != VIC_STATE_STOPPING) {
            tx_vic_enable_irq(dev);
            vic->state = VIC_STATE_STOPPING;
        }
    }

    return 0;
}

int vic_core_s_stream(struct IMPISPDev *dev, int enable)
{
    struct vic_device *vic = dev->vic_dev;
    int ret = -EINVAL;

    if (enable == 0) {
        if (vic->state == VIC_STATE_RUNNING) {
            vic->state = VIC_STATE_STOPPING;
            ret = 0;
        }
    } else {
        if (vic->state != VIC_STATE_RUNNING) {
            ret = tx_isp_vic_start(dev);
            if (ret == 0) {
                vic->state = VIC_STATE_RUNNING;
            }
        }
    }

    dump_vic_registers();

    return ret;
}

/**
 * tx_isp_vic_slake_subdev - Shutdown VIC subdevice
 * @param dev: Pointer to parent device structure
 * @return 0 on success, error code on failure
 */
int32_t tx_isp_vic_slake_subdev(struct IMPISPDev *dev)
{
    struct vic_device *vic = dev->vic_dev;

    // Stop streaming if active
    if (vic->state == VIC_STATE_RUNNING) {
        vic_core_s_stream(dev, 0);

        // Wait for state transition
        if (vic->state != VIC_STATE_STOPPING) {
            return VIC_ERR_BUSY;
        }
    }

    // Reset device if needed
    if (vic->state == VIC_STATE_STOPPING) {
        vic_core_ops_init(dev, 0);
    }

    // Lock state transition
    mutex_lock(&vic->state_lock);

    if (vic->state == VIC_STATE_READY) {
        vic->state = VIC_STATE_INIT;
    }

    mutex_unlock(&vic->state_lock);

    return VIC_SUCCESS;
}