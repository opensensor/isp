#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/delay.h>   // For msleep

#include "tx-isp.h"
#include "tx-isp-main.h"
#include "tx-isp-hw.h"

/* Hardware initialization and management */
int configure_isp_clocks(struct IMPISPDev *dev)
{
    int ret;

    pr_info("Configuring ISP clocks using standard API\n");

    // Step 1: Get CSI clock
    dev->csi_clk = clk_get(dev->dev, "csi");
    if (!IS_ERR(dev->csi_clk)) {
        ret = clk_prepare_enable(dev->csi_clk);
        if (ret) {
            pr_err("Failed to enable CSI clock\n");
            goto err_put_csi;
        }
    }

    // Step 2: Get IPU clock
    dev->ipu_clk = clk_get(dev->dev, "ipu");
    if (IS_ERR(dev->ipu_clk)) {
        pr_warn("IPU clock not available\n");
        dev->ipu_clk = NULL;
    }

    // Step 3: Get ISP core clock
    dev->isp_clk = clk_get(dev->dev, "isp");
    if (IS_ERR(dev->isp_clk)) {
        ret = PTR_ERR(dev->isp_clk);
        pr_err("Failed to get ISP clock: %d\n", ret);
        dev->isp_clk = NULL;
        return ret;
    }

    // Step 4: Get CGU ISP clock
    dev->cgu_isp = clk_get(dev->dev, "cgu_isp");
    if (IS_ERR(dev->cgu_isp)) {
        ret = PTR_ERR(dev->cgu_isp);
        pr_err("Failed to get CGU ISP clock: %d\n", ret);
        dev->cgu_isp = NULL;
        goto err_put_isp;
    }

    // Step 5: Enable CGU ISP clock first
    ret = clk_set_rate(dev->cgu_isp, T31_CGU_ISP_FREQ);
    if (ret) {
        pr_err("Failed to set CGU ISP clock rate: %d\n", ret);
        goto err_put_cgu;
    }
    ret = clk_prepare_enable(dev->cgu_isp);
    if (ret) {
        pr_err("Failed to enable CGU ISP clock: %d\n", ret);
        goto err_put_cgu;
    }

    // Step 6: Enable ISP core clock
    ret = clk_prepare_enable(dev->isp_clk);
    if (ret) {
        pr_err("Failed to enable ISP clock: %d\n", ret);
        goto err_disable_cgu;
    }

    // Step 7: Enable IPU clock if available
    if (dev->ipu_clk) {
        ret = clk_prepare_enable(dev->ipu_clk);
        if (ret) {
            pr_warn("Failed to enable IPU clock\n");
        }
    }

    // Step 8: Enable additional peripheral clocks (e.g., LCD, AES)
    struct clk *lcd_clk = clk_get(dev->dev, "lcd");
    if (!IS_ERR(lcd_clk)) {
        clk_prepare_enable(lcd_clk);
        clk_set_rate(lcd_clk, 250000000);
    }

    struct clk *aes_clk = clk_get(dev->dev, "aes");
    if (!IS_ERR(aes_clk)) {
        clk_prepare_enable(aes_clk);
        clk_set_rate(aes_clk, 250000000);
    }

    // Step 9: Verify clock rates
    pr_info("Clock rates after configuration:\n");
    pr_info("  CSI: %lu Hz\n", dev->csi_clk ? clk_get_rate(dev->csi_clk) : 0);
    pr_info("  ISP Core: %lu Hz\n", clk_get_rate(dev->isp_clk));
    pr_info("  CGU ISP: %lu Hz\n", clk_get_rate(dev->cgu_isp));
    pr_info("  IPU: %lu Hz\n", dev->ipu_clk ? clk_get_rate(dev->ipu_clk) : 0);

    return 0;

err_disable_cgu:
    clk_disable_unprepare(dev->cgu_isp);
err_put_cgu:
    clk_put(dev->cgu_isp);
    dev->cgu_isp = NULL;
err_put_isp:
    clk_put(dev->isp_clk);
    dev->isp_clk = NULL;
err_put_csi:
    if (dev->csi_clk) {
        clk_disable_unprepare(dev->csi_clk);
        clk_put(dev->csi_clk);
        dev->csi_clk = NULL;
    }
    return ret;
}

void cleanup_isp_clocks(struct IMPISPDev *dev)
{
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

/* VIC control functions */
int init_vic_control(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->reg_base + VIC_BASE_OFFSET;
    int ret;

    pr_info("Initializing VIC control...\n");

    // 1. Full reset/disable sequence
    writel(0, vic_regs + VIC_DMA_CTRL);
    writel(0, vic_regs + VIC_CTRL);
    wmb();
    udelay(100);

    // 2. Reset VIC
    writel(0x4, vic_regs + VIC_DMA_CTRL);  // Reset command
    wmb();
    udelay(100);
    while(readl(vic_regs + VIC_DMA_CTRL) & 0x4) {
        udelay(10);
    }

    // 3. Initial mode setup - match streaming enable sequence
    writel(2, vic_regs + 0xc);  // Mode = 2
    writel(0x800c0000, vic_regs + 0x10);  // Magic control
    wmb();

    // 4. Poll until VIC status is 0
    uint32_t status;
    do {
        status = readl(vic_regs + VIC_STATUS);
    } while (status != 0);

    // 5. Configure VIC with correct magic values
    writel(0x100010, vic_regs + 0x1a4);
    writel(0x4210, vic_regs + 0x1ac);
    writel(0x10, vic_regs + 0x1b0);
    writel(0, vic_regs + 0x1b4);
    wmb();

    // 6. Control sequence - THIS IS KEY
    writel(2, vic_regs + VIC_CTRL);
    wmb();
    writel(1, vic_regs + VIC_CTRL);  // Back to 1 as per OEM
    wmb();

    // Replace direct register_vic_irq call with tx_isp_request_irq
    // Request both IRQs
//    ret = tx_isp_request_irq(36, dev->irq_data); // VIC IRQ
//    if (ret) {
//        pr_err("Failed to setup VIC IRQ: %d\n", ret);
//        return ret;
//    }
//
//    ret = tx_isp_request_irq(37, dev->irq_data); // ISP IRQ
//    if (ret) {
//        pr_err("Failed to setup ISP IRQ: %d\n", ret);
//        free_irq(36, dev->irq_data);
//        return ret;
//    }


    // 8. Now enable interrupts
    writel(0x1, vic_regs + VIC_CTRL);       // Enable VIC
    writel(0x1, vic_regs + VIC_FRAME_CTRL); // Enable frame control
    writel(0x3, vic_regs + VIC_IRQ_EN);     // Enable both IRQs
    writel(~0x33fb, vic_regs + VIC_MASK);   // Unmask interrupts - inverted!
    wmb();

    // Debug full state
    pr_info("VIC Configuration:\n");
    pr_info("  Control:     0x%08x\n", readl(vic_regs + VIC_CTRL));
    pr_info("  IRQ Enable:  0x%08x\n", readl(vic_regs + VIC_IRQ_EN));
    pr_info("  Frame Ctrl:  0x%08x\n", readl(vic_regs + VIC_FRAME_CTRL));
    pr_info("  Int Mask:    0x%08x\n", readl(vic_regs + VIC_MASK));
    pr_info("  IRQ Status:  0x%08x\n", readl(vic_regs + 0x1e0));
    pr_info("  Status:      0x%08x\n", readl(vic_regs + VIC_STATUS));

    return 0;
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

void dump_vic_state(struct IMPISPDev *dev)
{
    void __iomem *vic_regs = dev->vic_regs + 0x300;

    pr_info("VIC State:\n");
    pr_info("  Control: 0x%08x\n", readl(vic_regs + 0x140));
    pr_info("  DMA ctrl: 0x%08x\n", readl(vic_regs + 0x300));
    pr_info("  Frame ctrl: 0x%08x\n", readl(vic_regs + 0x308));
    pr_info("  IRQ status: 0x%08x\n", readl(vic_regs + 0x1e0));
    pr_info("  IRQ mask: 0x%08x\n", readl(vic_regs + 0x1e8));
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
    struct isp_framesource_state *fs = &dev->frame_sources[0];
    struct frame_source_channel *fc;
    void __iomem *regs = dev->reg_base;
    uint32_t buffer_start, buffer_size;
    uint32_t input_offset, y_offset, uv_offset;

    if (!dev || !fs || !fs->fc) {
        pr_err("Invalid device state\n");
        return -EINVAL;
    }

    fc = fs->fc;

    // Calculate aligned dimensions - match OEM alignment
    uint32_t width = ALIGN(fs->width, 32);
    uint32_t height = ALIGN(fs->height, 32);

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
    fc->dma_addr = buffer_start;
    fc->buf_size = buffer_size;
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

int configure_streaming_hardware(struct IMPISPDev *dev)
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
//    writel(ALIGN(fc->dma_addr, 8), regs + ISP_BUF0_OFFSET);
//    writel(y_stride, regs + ISP_BUF0_OFFSET + 0x4);
//
//    // UV plane
//    writel(ALIGN(fc->dma_addr + y_size, 8), regs + ISP_BUF0_OFFSET + 0x8);
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
//
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
    pr_info("  VIC ctrl: 0x%08x\n", readl(vic_regs + VIC_CTRL));
    pr_info("  VIC IRQ: 0x%08x\n", readl(vic_regs + VIC_IRQ_EN));
    pr_info("  VIC status: 0x%08x\n", readl(vic_regs + VIC_STATUS));
    pr_info("  VIC frame: 0x%08x\n", readl(vic_regs + VIC_FRAME_CTRL));
    pr_info("  VIC DMA: 0x%08x\n", readl(vic_regs + VIC_DMA_CTRL));
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
    dev->dma_addr = aligned_base;
    dev->dma_size = aligned_size;

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
             (uint32_t)dev->dma_addr,
             dev->dma_buf,
             dev->dma_size,
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
    if (dev->dma_addr) {
        devm_release_mem_region(dev->dev, dev->dma_addr, dev->dma_size);
        dev->dma_addr = 0;
    }

    /* Clear memory info */
    dev->param_virt = NULL;
    dev->frame_buf_offset = 0;
    dev->dma_size = 0;
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
