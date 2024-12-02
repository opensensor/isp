/*
 * tx-isp-csi.c - TX ISP CSI/MIPI interface driver
 *
 * Contains CSI/MIPI subsystem initialization and control
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/clk.h>

#include "tx-isp.h"


int tx_isp_csi_activate_subdev(struct IMPISPDev *dev)
{
    //spin_lock(&dev->csi_lock);

    if (dev->csi_dev->state == 1) {
        // State 1->2 transition
        dev->csi_dev->state = 2;
    }

    //spin_unlock(&dev->csi_lock);
    return 0;
}


static int enable_csi_clocks(struct IMPISPDev *dev)
{
    void __iomem *cpm_base;
    u32 val;
    int ret;

    pr_info("Enabling CSI clocks...\n");

    // Map CPM registers at 0x10000000
    cpm_base = ioremap(0x10000000, 0x100);
    if (!cpm_base) {
        pr_err("Failed to map CPM registers\n");
        return -ENOMEM;
    }

    // Step 1: Assert CSI reset first
    val = readl(cpm_base + 0xc4);  // SRSR
    val |= 0x100000;  // CSI reset bit
    writel(val, cpm_base + 0xc4);
    wmb();
    udelay(10);

    // Step 2: Enable power domain
    val = readl(cpm_base + 0x28);  // CLKGR1
    val &= ~0x40000;  // Clear power gate bit
    writel(val, cpm_base + 0x28);
    wmb();
    udelay(20);

    // Step 3: Configure CSI clock divider
    writel(0x1, cpm_base + 0x84);  // CSICDR
    wmb();
    udelay(10);

    // Step 4: Enable CSI clock gate
    val = readl(cpm_base + 0x20);  // CLKGR
    val &= ~0x100000;  // Clear CSI clock gate bit
    writel(val, cpm_base + 0x20);
    wmb();
    udelay(10);

    // Step 5: De-assert CSI reset
    val = readl(cpm_base + 0xc4);  // SRSR
    val &= ~0x100000;  // Clear CSI reset bit
    writel(val, cpm_base + 0xc4);
    wmb();
    udelay(50);

    // Step 6: Enable CSI clock through standard API if available
    if (dev->csi_clk) {
        ret = clk_prepare_enable(dev->csi_clk);
        if (ret) {
            pr_err("Failed to enable CSI clock: %d\n", ret);
            iounmap(cpm_base);
            return ret;
        }
    }

    // Debug output
    pr_info("CSI Clock state after setup:\n");
    pr_info("  CLKGR: 0x%08x\n", readl(cpm_base + 0x20));
    pr_info("  CLKGR1: 0x%08x\n", readl(cpm_base + 0x28));
    pr_info("  SRSR: 0x%08x\n", readl(cpm_base + 0xc4));
    pr_info("  CSICDR: 0x%08x\n", readl(cpm_base + 0x84));

    iounmap(cpm_base);
    return 0;
}



#define MIPI_PHY_ADDR   0x10022000
#define ISP_W01_ADDR    0x10023000  // Should be our CSI controller

int init_csi_early(struct IMPISPDev *dev) {
    struct csi_device *csi_dev;
    void __iomem *phy_base;
    void __iomem *csi_base;
    void __iomem *cpm_base;
    void __iomem **csi_regs_ptr;
    u32 val;

    pr_info("Mapping I/O regions:\n");
    pr_info("  MIPI PHY: 0x%08x\n", MIPI_PHY_ADDR);
    pr_info("  ISP W01: 0x%08x\n", ISP_W01_ADDR);

    // Try using request_mem_region first
    struct resource *phy_res, *csi_res;

    phy_res = request_mem_region(MIPI_PHY_ADDR, 0x1000, "mipi-phy");
    if (!phy_res) {
        pr_info("MIPI PHY region already claimed\n");
    }

    csi_res = request_mem_region(ISP_W01_ADDR, 0x1000, "isp-w01");
    if (!csi_res) {
        pr_info("ISP W01 region already claimed\n");
    }

    // Map CPM first for clock setup
    cpm_base = ioremap(CPM_BASE, 0x1000);
    if (!cpm_base) {
        return -ENOMEM;
    }

    pr_info("Initial CPM state:\n");
    pr_info("  CLKGR: 0x%08x\n", readl(cpm_base + CPM_CLKGR));
    pr_info("  CLKGR1: 0x%08x\n", readl(cpm_base + CPM_CLKGR1));

    // Setup clocks and power
    val = readl(cpm_base + CPM_CLKGR);
    val &= ~CPM_CSI_CLK_MASK;  // Enable CSI clock
    writel(val, cpm_base + CPM_CLKGR);
    wmb();

    val = readl(cpm_base + CPM_CLKGR1);
    val &= ~CPM_CSI_PWR_MASK;  // Enable CSI power
    writel(val, cpm_base + CPM_CLKGR1);
    wmb();

    // Configure CSI clock
    writel(0x1, cpm_base + CPM_CSICDR);
    wmb();
    msleep(10);

    // Map MIPI PHY
    phy_base = ioremap(MIPI_PHY_ADDR, 0x1000);
    if (!phy_base) {
        iounmap(cpm_base);
        return -ENOMEM;
    }

    // Map CSI (W01)
    csi_base = ioremap(ISP_W01_ADDR, 0x1000);
    if (!csi_base) {
        iounmap(phy_base);
        iounmap(cpm_base);
        return -ENOMEM;
    }

    // Setup device structure
    csi_dev = kzalloc(sizeof(*csi_dev), GFP_KERNEL);
    if (!csi_dev) {
        iounmap(csi_base);
        iounmap(phy_base);
        iounmap(cpm_base);
        return -ENOMEM;
    }

    // Initialize mutex
    mutex_init(&csi_dev->mutex);

    // Setup registers
    //csi_dev->base = csi_base;
    csi_dev->phy_regs = phy_base;
    dev->csi_dev = csi_dev;

    pr_info("Initial register readings:\n");
    pr_info("  W01 0x00: 0x%08x\n", readl(csi_base + 0x00));
    pr_info("  W01 0x04: 0x%08x\n", readl(csi_base + 0x04));
    pr_info("  W01 0x08: 0x%08x\n", readl(csi_base + 0x08));
    pr_info("  PHY 0x00: 0x%08x\n", readl(phy_base + 0x00));
    pr_info("  PHY 0x04: 0x%08x\n", readl(phy_base + 0x04));

    // First read then try to write test pattern
    writel(0xaaaa5555, csi_base + 0x04);
    writel(0x5555aaaa, phy_base + 0x04);
    wmb();

    pr_info("After test write:\n");
    pr_info("  W01 0x04: 0x%08x\n", readl(csi_base + 0x04));
    pr_info("  PHY 0x04: 0x%08x\n", readl(phy_base + 0x04));

    pr_info("After test write:\n");
    pr_info("  W01 0x04: 0x%08x\n", readl(csi_base + 0x04));
    pr_info("  PHY 0x04: 0x%08x\n", readl(phy_base + 0x04));

    // Store CSI base in device structure
    csi_dev->csi_regs = csi_base;

    // Now we can configure CSI with proper registers
    int ret = configure_mipi_csi(dev);
    if (ret) {
        pr_err("Failed to configure MIPI CSI: %d\n", ret);
        iounmap(csi_base);
        iounmap(phy_base);
        iounmap(cpm_base);
        kfree(csi_dev);
        return ret;
    }

    // Verify configuration
    //dump_csi_reg(dev);

    iounmap(cpm_base);
    return 0;
}


/* CSI initialization */
int tx_isp_csi_probe(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);
    struct csi_device *csi_dev;
    void __iomem *phy_base;
    int ret;

    pr_info("CSI probe starting\n");

    if (!dev || !dev->reg_base) {
        dev_err(&pdev->dev, "No ISP device data\n");
        return -EINVAL;
    }

    // Allocate CSI device structure
    csi_dev = devm_kzalloc(&pdev->dev, sizeof(*csi_dev), GFP_KERNEL);
    if (!csi_dev) {
        dev_err(&pdev->dev, "Failed to allocate CSI device\n");
        return -ENOMEM;
    }

    // Initialize CSI device
    csi_dev->dev = &pdev->dev;
    mutex_init(&csi_dev->mutex);

    // Map CSI registers
    csi_dev->csi_regs = dev->reg_base + 0xb8;  // CSI controller at offset 0xb8
    if (!csi_dev->csi_regs) {
        dev_err(&pdev->dev, "Failed to map CSI registers\n");
        ret = -ENOMEM;
        goto err_free_dev;
    }

    // Store CSI device in main ISP device
    dev->csi_dev = csi_dev;
    dev->csi_dev->state = 1;

    // Map PHY registers
    phy_base = ioremap(0x10022000, 0x1000);
    if (!phy_base) {
        dev_err(&pdev->dev, "Failed to map CSI PHY registers\n");
        ret = -ENOMEM;
        goto err_disable_clocks;
    }

    csi_dev->phy_regs = phy_base;

    init_csi_phy(dev);

    pr_info("CSI probe completed successfully\n");
    return 0;

    err_disable_clocks:
        // Add clock cleanup
    err_unmap_csi:
        dev->csi_dev = NULL;
    err_free_dev:
        devm_kfree(&pdev->dev, csi_dev);
    return ret;
}


/* CSI cleanup */
int tx_isp_csi_remove(struct platform_device *pdev)
{
    struct IMPISPDev *dev = platform_get_drvdata(pdev);
    struct csi_device *csi_dev;

    if (!dev)
        return 0;

    csi_dev = dev->csi_dev;
    if (csi_dev) {
        if (csi_dev->csi_regs)
            iounmap(csi_dev->csi_regs);
        devm_kfree(&pdev->dev, csi_dev);
    }
    dev->csi_dev = NULL;

    return 0;
}


int init_csi_phy(struct IMPISPDev *dev) {
    struct csi_device *csi_dev;
    void __iomem *phy_base;
    void __iomem *csi_base;
    void __iomem *cpm_base;
    void __iomem **csi_regs_ptr;
    u32 val;

    pr_info("Mapping I/O regions:\n");
    pr_info("  MIPI PHY: 0x%08x\n", MIPI_PHY_ADDR);
    pr_info("  ISP W01: 0x%08x\n", ISP_W01_ADDR);

    // Try using request_mem_region first
    struct resource *phy_res, *csi_res;

    phy_res = request_mem_region(MIPI_PHY_ADDR, 0x1000, "mipi-phy");
    if (!phy_res) {
        pr_info("MIPI PHY region already claimed\n");
    }

    csi_res = request_mem_region(ISP_W01_ADDR, 0x1000, "isp-w01");
    if (!csi_res) {
        pr_info("ISP W01 region already claimed\n");
    }

    // Map CPM first for clock setup
    cpm_base = ioremap(CPM_BASE, 0x1000);
    if (!cpm_base) {
        return -ENOMEM;
    }

    pr_info("Initial CPM state:\n");
    pr_info("  CLKGR: 0x%08x\n", readl(cpm_base + CPM_CLKGR));
    pr_info("  CLKGR1: 0x%08x\n", readl(cpm_base + CPM_CLKGR1));

    // Setup clocks and power
    val = readl(cpm_base + CPM_CLKGR);
    val &= ~CPM_CSI_CLK_MASK;  // Enable CSI clock
    writel(val, cpm_base + CPM_CLKGR);
    wmb();

    val = readl(cpm_base + CPM_CLKGR1);
    val &= ~CPM_CSI_PWR_MASK;  // Enable CSI power
    writel(val, cpm_base + CPM_CLKGR1);
    wmb();

    // Configure CSI clock
    writel(0x1, cpm_base + CPM_CSICDR);
    wmb();
    msleep(10);

    // Map MIPI PHY
    phy_base = ioremap(MIPI_PHY_ADDR, 0x1000);
    if (!phy_base) {
        iounmap(cpm_base);
        return -ENOMEM;
    }

    // Map CSI (W01)
    csi_base = ioremap(ISP_W01_ADDR, 0x1000);
    if (!csi_base) {
        iounmap(phy_base);
        iounmap(cpm_base);
        return -ENOMEM;
    }

    // Setup device structure
    csi_dev = kzalloc(sizeof(*csi_dev), GFP_KERNEL);
    if (!csi_dev) {
        iounmap(csi_base);
        iounmap(phy_base);
        iounmap(cpm_base);
        return -ENOMEM;
    }

    // Initialize mutex
    mutex_init(&csi_dev->mutex);

    // Setup registers
    //csi_dev->base = csi_base;
    csi_dev->phy_regs = phy_base;
    dev->csi_dev = csi_dev;

    pr_info("Initial register readings:\n");
    pr_info("  W01 0x00: 0x%08x\n", readl(csi_base + 0x00));
    pr_info("  W01 0x04: 0x%08x\n", readl(csi_base + 0x04));
    pr_info("  W01 0x08: 0x%08x\n", readl(csi_base + 0x08));
    pr_info("  PHY 0x00: 0x%08x\n", readl(phy_base + 0x00));
    pr_info("  PHY 0x04: 0x%08x\n", readl(phy_base + 0x04));

    // First read then try to write test pattern
    writel(0xaaaa5555, csi_base + 0x04);
    writel(0x5555aaaa, phy_base + 0x04);
    wmb();

    pr_info("After test write:\n");
    pr_info("  W01 0x04: 0x%08x\n", readl(csi_base + 0x04));
    pr_info("  PHY 0x04: 0x%08x\n", readl(phy_base + 0x04));

    pr_info("After test write:\n");
    pr_info("  W01 0x04: 0x%08x\n", readl(csi_base + 0x04));
    pr_info("  PHY 0x04: 0x%08x\n", readl(phy_base + 0x04));

    // Store CSI base in device structure
    csi_dev->csi_regs = csi_base;

    // Now we can configure CSI with proper registers
    int ret = configure_mipi_csi(dev);
    if (ret) {
        pr_err("Failed to configure MIPI CSI: %d\n", ret);
        iounmap(csi_base);
        iounmap(phy_base);
        iounmap(cpm_base);
        kfree(csi_dev);
        return ret;
    }

    // Verify configuration
    dump_csi_reg(dev);

    iounmap(cpm_base);
    return 0;
}

int configure_mipi_csi(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->reg_base;
    void __iomem *csi_base = dev->csi_dev->csi_regs;
    u32 val;
    int retry = 5;

    pr_info("T31 CSI: Configuring for SC2336...\n");

    // First clear any stale errors
    writel(0xffffffff, csi_base + 0x20);  // Clear ERR1
    writel(0xffffffff, csi_base + 0x24);  // Clear ERR2
    wmb();
    msleep(1);

    // Configure lanes first
    writel(0x1, csi_base + 0x04);  // N_LANES = 1 for 2 lanes
    wmb();

    // Full reset sequence first
    writel(0x0, regs + ISP_CSI_CTRL);
    writel(0x0, csi_base + 0x10);  // CSI2_RESETN
    writel(0x0, csi_base + 0x0c);  // DPHY_RSTZ
    writel(0x0, csi_base + 0x08);  // PHY_SHUTDOWNZ
    wmb();
    msleep(20);  // Longer reset for T31

    // Check error state after reset
    val = readl(csi_base + 0x20);
    if (val != 0) {
        pr_info("CSI ERR1 after reset: 0x%08x\n", val);
        writel(0xffffffff, csi_base + 0x20);
        wmb();
    }

    // Configure DPHY timing - T31 specific
    writel(0x00000004, regs + ISP_CSI_DPHY_CTRL);  // DPHY clock mode
    writel(0x00000003, regs + ISP_CSI_LANE_CTRL);  // 2 data lanes
    writel(0x0a170201, regs + ISP_CSI_TIMING);     // T31 timing value
    wmb();

    // Configure PHY test registers (T31 specific)
    writel(0x0, csi_base + CSI_PHY_TST_CTRL0);
    writel(0x1, csi_base + CSI_PHY_TST_CTRL1);
    writel(0x0, csi_base + CSI_PHY_TST_CTRL0);
    wmb();
    msleep(1);

    // Set lane configuration for SC2336
    writel(0x1, csi_base + 0x04);     // N_LANES = 1 (2 lanes)
    writel(0x2B, csi_base + 0x18);    // RAW10 format
    wmb();
    msleep(1);

    // Power up sequence with proper timing
    writel(0x1, csi_base + 0x08);  // PHY_SHUTDOWNZ
    wmb();
    usleep_range(100, 150);
    writel(0x1, csi_base + 0x0c);  // DPHY_RSTZ
    wmb();
    usleep_range(100, 150);
    writel(0x1, csi_base + 0x10);  // CSI2_RESETN
    wmb();
    usleep_range(100, 150);

    // Set masks AFTER power up
    writel(0x000f0000, csi_base + 0x28);  // MASK1
    writel(0x01ff0000, csi_base + 0x2c);  // MASK2
    wmb();

    // Clear any errors one more time
    writel(0xffffffff, csi_base + 0x20);  // Clear ERR1
    writel(0xffffffff, csi_base + 0x24);  // Clear ERR2
    wmb();
    msleep(1);

    // Finally enable CSI in ISP
    writel(0x1, regs + ISP_CSI_CTRL);
    wmb();

    // Check final state
    val = readl(csi_base + 0x14);
    pr_info("T31 CSI configured:\n"
            "  PHY state: 0x%08x\n"
            "  ERR1: 0x%08x\n"
            "  ERR2: 0x%08x\n",
            val,
            readl(csi_base + 0x20),
            readl(csi_base + 0x24));

    return 0;
}


int verify_csi_signals(struct IMPISPDev *dev)
{
    void __iomem *csi_regs = dev->csi_dev->csi_regs;
    uint32_t phy_state;
    int timeout = 100;

    if (!csi_regs) {
        pr_err("NULL CSI registers\n");
        return -EINVAL;
    }

    mutex_lock(&dev->csi_dev->mutex);

    // Check PHY state - looking for 0x630 which indicates lanes ready
    phy_state = readl(csi_regs + 0x14); // PHY_STATE register

    pr_info("Initial PHY state: 0x%08x\n", phy_state);

    // Wait for basic PHY ready state (0x630)
    while (timeout-- && (phy_state & 0x630) != 0x630) {
        udelay(100);
        phy_state = readl(csi_regs + 0x14);
    }

    if ((phy_state & 0x630) != 0x630) {
        pr_err("CSI PHY not ready: 0x%08x\n", phy_state);
        mutex_unlock(&dev->csi_dev->mutex);
        return -EIO;
    }

    // Debug output
    pr_info("CSI signals verified:\n");
    pr_info("  PHY state: 0x%08x\n", phy_state);
    pr_info("  Clock lane: %s\n", (phy_state & 0x10) ? "ready" : "not ready");
    pr_info("  Data lanes: %s\n", (phy_state & 0x620) ? "ready" : "not ready");

    mutex_unlock(&dev->csi_dev->mutex);
    return 0;
}

void dump_csi_reg(struct IMPISPDev *dev)
{
    void __iomem *csi_base;

    csi_base = dev->csi_dev->csi_regs;
    if (!csi_base)
        return;

    pr_info("****>>>>> dump csi reg <<<<<****\n");
    pr_info("VERSION = %08x\n", readl(csi_base + 0x00));
    pr_info("N_LANES = %08x\n", readl(csi_base + 0x04));
    pr_info("PHY_SHUTDOWNZ = %08x\n", readl(csi_base + 0x08));
    pr_info("DPHY_RSTZ = %08x\n", readl(csi_base + 0x0c));
    pr_info("CSI2_RESETN = %08x\n", readl(csi_base + 0x10));
    pr_info("PHY_STATE = %08x\n", readl(csi_base + 0x14));
    pr_info("DATA_IDS_1 = %08x\n", readl(csi_base + 0x18));
    pr_info("DATA_IDS_2 = %08x\n", readl(csi_base + 0x1c));
    pr_info("ERR1 = %08x\n", readl(csi_base + 0x20));
    pr_info("ERR2 = %08x\n", readl(csi_base + 0x24));
    pr_info("MASK1 = %08x\n", readl(csi_base + 0x28));
    pr_info("MASK2 = %08x\n", readl(csi_base + 0x2c));
    pr_info("PHY_TST_CTRL0 = %08x\n", readl(csi_base + 0x30));
    pr_info("PHY_TST_CTRL1 = %08x\n", readl(csi_base + 0x34));
}

int csi_core_ops_init(struct IMPISPDev *dev, int enable)
{
    int result = -EINVAL;

    if (!dev || (unsigned long)dev >= 0xfffff001)
        return -EINVAL;

    void __iomem *reg = dev->reg_base + 0xd4;
    if (!reg || (unsigned long)reg >= 0xfffff001)
        return -EINVAL;

    if (readl(reg + 0x128) >= 2) {
        if (enable == 0) {
            pr_info("csi is close!\n");

            void __iomem *reg_b8 = ioremap(readl(reg + 0xb8), 0x20);
            if (!reg_b8)
                return -ENOMEM;

            writel(readl(reg_b8 + 8) & ~1, reg_b8 + 8);
            writel(readl(reg_b8 + 0xc) & ~1, reg_b8 + 0xc);
            writel(readl(reg_b8 + 0x10) & ~1, reg_b8 + 0x10);
            iounmap(reg_b8);

            writel(2, reg + 0x128);
        } else {
            void __iomem *reg_110 = ioremap(readl(reg + 0x110), 0x40);
            if (!reg_110)
                return -ENOMEM;

            int bus_type = readl(reg_110 + 0x14);

            if (bus_type == 1) {
                void __iomem *reg_b8 = ioremap(readl(reg + 0xb8), 0x20);
                if (!reg_b8) {
                    iounmap(reg_110);
                    return -ENOMEM;
                }

                writel(readl(reg_110 + 0x24) - 1, reg_b8 + 4);
                writel(readl(reg_b8 + 8) & ~1, reg_b8 + 8);
                writel(0, reg_b8 + 0xc);
                msleep(1);
                writel(readl(reg_b8 + 0x10) & ~1, reg_b8 + 0x10);
                msleep(1);
                writel(bus_type, reg_b8 + 0xc);
                msleep(1);

                void __iomem *reg_13c = ioremap(readl(reg + 0x13c), 0x300);
                if (!reg_13c) {
                    iounmap(reg_b8);
                    iounmap(reg_110);
                    return -ENOMEM;
                }

                int timing_val;
                if (readl(reg_110 + 0x3c) != 0) {
                    timing_val = readl(reg + 0x13c);
                } else {
                    int val = readl(reg_110 + 0x1c);
                    int timing_code;

                    if (val - 0x50 < 0x1e) {
                        timing_code = readl(reg + 0x13c);
                    } else {
                        if (val - 0x6e >= 0x28) timing_code = 2;
                        else timing_code = 1;
                        if (val - 0x96 >= 0x32) timing_code = 3;
                        if (val - 0xc8 >= 0x32) timing_code = 4;
                        if (val - 0xfa >= 0x32) timing_code = 5;
                        if (val - 0x12c >= 0x64) timing_code = 6;
                        if (val - 0x190 >= 0x64) timing_code = 7;
                        if (val - 0x1f4 >= 0x64) timing_code = 8;
                        if (val - 0x258 >= 0x64) timing_code = 9;
                        if (val - 0x2bc >= 0x64) timing_code = 0xa;
                        if (val - 0x320 >= 0xc8) timing_code = 0xb;

                        writel((readl(reg_13c + 0x160) & 0xfffffff0) | timing_code, reg_13c + 0x160);
                        writel(readl(reg_13c + 0x160), reg_13c + 0x1e0);
                        writel(readl(reg_13c + 0x160), reg_13c + 0x260);
                    }
                }

                writel(0x7d, reg_13c);
                writel(0x3f, reg_13c + 0x128);
                writel(1, reg_b8 + 0x10);

                iounmap(reg_13c);
                iounmap(reg_b8);
                msleep(10);
                writel(3, reg + 0x128);
            } else if (bus_type == 2) {
                void __iomem *reg_b8 = ioremap(readl(reg + 0xb8), 0x20);
                if (!reg_b8) {
                    iounmap(reg_110);
                    return -ENOMEM;
                }

                writel(0, reg_b8 + 0xc);
                writel(1, reg_b8 + 0xc);

                void __iomem *reg_13c = ioremap(readl(reg + 0x13c), 0x300);
                if (!reg_13c) {
                    iounmap(reg_b8);
                    iounmap(reg_110);
                    return -ENOMEM;
                }

                writel(0x7d, reg_13c);
                writel(0x3e, reg_13c + 0x80);
                writel(1, reg_13c + 0x2cc);

                iounmap(reg_13c);
                iounmap(reg_b8);
                writel(3, reg + 0x128);
            } else {
                pr_info("The sensor dbus_type is %d\n", bus_type);
                writel(3, reg + 0x128);
            }
            iounmap(reg_110);
        }
        return 0;
    }
    return result;
}



// Sensor ops implementation
static long csi_sensor_ops_ioctl(struct tx_isp_subdev *sd,
                                unsigned int cmd,
                                void *arg)
{
    // Handle sensor-specific ioctls
    pr_debug("CSI sensor ioctl cmd: 0x%x\n", cmd);

    switch (cmd) {
        case 0x200000e:  // CSI Type 1 check
            pr_info("CSI Type 1 check\n");
            if (readl(sd->base + 0x110 + 0x14) == 1) {
                writel(3, sd->base + 0x128);  // Set state 3
            }
            break;

        case 0x200000f:  // CSI State 4 check
            pr_info("CSI State 4 check\n");
            if (readl(sd->base + 0x110 + 0x14) == 1) {
                writel(4, sd->base + 0x128);  // Set state 4
            }
            break;

        case 0x200000c:  // CSI Init
            pr_info("CSI Init\n");
            return csi_core_ops_init(sd, 1);

        default:
            return -ENOIOCTLCMD;
    }

    return 0;
}

// Now the static structures should work:
static struct tx_isp_subdev_core_ops csi_core_ops = {
    .init = csi_core_ops_init,
};

static struct tx_isp_subdev_sensor_ops csi_sensor_ops = {
    .ioctl = csi_sensor_ops_ioctl,
};

static struct tx_isp_subdev_ops csi_ops = {
    .core = &csi_core_ops,
    .sensor = &csi_sensor_ops,
    .video = NULL,
    .pad = NULL,
};



int csi_set_lanes(struct IMPISPDev *dev, u8 lanes) {
    void __iomem *csi_base = dev->csi_dev->csi_regs;
    u32 val;

    if (!csi_base)
        return -EINVAL;

    //    // Step 1: Disable lanes first
    //    writel(0x0, csi_base + 0x38);  // Clear lane enable
    //    wmb();
    //    udelay(10);

    // Step 2: Set number of lanes (n-1)
    val = readl(csi_base + 0x04);
    val = ((lanes - 1) & 3) | (val & 0xfffffffc);
    writel(val, csi_base + 0x04);
    wmb();
    udelay(10);

    // Step 3: Enable lanes with bitmask
    writel((1 << lanes) - 1, csi_base + 0x38);  // Enable appropriate number of lanes
    wmb();
    udelay(10);

    pr_info("CSI lanes configured: count=%d enable=0x%x\n",
            lanes, readl(csi_base + 0x38));

    return 0;
}

int probe_csi_registers(struct IMPISPDev *dev)
{
    void __iomem *csi_base = dev->csi_dev->csi_regs;
    u32 test_val, read_val;
    int i;

    pr_info("Probing CSI register access:\n");

    // Try each register
    const struct {
        u32 offset;
        const char *name;
    } regs[] = {
        {0x00, "VERSION"},
        {0x04, "N_LANES"},
        {0x08, "PHY_SHUTDOWNZ"},
        {0x0c, "DPHY_RSTZ"},
        {0x10, "CSI2_RESETN"},
        {0x14, "PHY_STATE"},
        {0x18, "DATA_IDS_1"},
        {0x1c, "DATA_IDS_2"},
        {0x20, "ERR1"},
        {0x24, "ERR2"},
        {0x28, "MASK1"},
        {0x2c, "MASK2"},
    };

    // Test each register read/write
    for (i = 0; i < ARRAY_SIZE(regs); i++) {
        // Read original
        read_val = readl(csi_base + regs[i].offset);

        // Try write test pattern
        test_val = 0xAAAAAAAA;
        writel(test_val, csi_base + regs[i].offset);
        wmb();
        read_val = readl(csi_base + regs[i].offset);

        pr_info("  %-12s (0x%02x): wrote 0x%08x read 0x%08x %s\n",
                regs[i].name, regs[i].offset, test_val, read_val,
                (read_val == test_val) ? "OK" : "Failed");
    }

    return 0;
}

// Add PHY setup to separate function that gets called during subdev init
int setup_csi_phy(struct IMPISPDev *dev) {
    void __iomem *phy_base;

    // Skip request_mem_region and directly map the registers
    phy_base = ioremap(0x10022000, 0x1000);
    if (!phy_base) {
        pr_err("Failed to map PHY registers\n");
        return -ENOMEM;
    }

    dev->csi_dev->phy_regs = phy_base;

    pr_info("CSI PHY setup complete\n");
    return 0;
}



void cleanup_csi_phy(struct IMPISPDev *dev)
{
    if (dev->csi_dev->phy_regs) {
        iounmap(dev->csi_dev->phy_regs);
        dev->csi_dev->phy_regs = NULL;
    }

    if (dev->csi_dev->csi_regs) {
        release_mem_region(CSI_PHY_BASE, CSI_PHY_SIZE);
        dev->csi_dev->csi_regs = NULL;
    }
}

int configure_csi_streaming(struct IMPISPDev *dev)
{
  	void __iomem *vic_base = dev->reg_base + VIC_BASE;
    void __iomem *csi_base = dev->csi_dev->csi_regs;
    uint32_t phy_state;
    int ret;
    mutex_lock(&dev->csi_dev->mutex);

    // Full reset first
    writel(0x0, csi_base + 0x10);  // CSI2_RESETN
    writel(0x0, csi_base + 0x0c);  // DPHY_RSTZ
    writel(0x0, csi_base + 0x08);  // PHY_SHUTDOWNZ
    wmb();
    msleep(10);

    // Clear errors
    writel(0xffffffff, csi_base + 0x20);
    writel(0xffffffff, csi_base + 0x24);
    wmb();
    msleep(1);

    // Power up sequence first
    writel(0x1, csi_base + 0x08);  // PHY_SHUTDOWNZ
    wmb();
    msleep(1);
    writel(0x1, csi_base + 0x0c);  // DPHY_RSTZ
    wmb();
    msleep(1);
    writel(0x1, csi_base + 0x10);  // CSI2_RESETN
    wmb();
    msleep(1);

    // Now configure format and lanes
    writel(0x1, csi_base + 0x04);  // N_LANES = 1 (2 lanes)
    wmb();
    msleep(1);
    writel(0x2B, csi_base + 0x18); // RAW10 format
    wmb();
    msleep(1);
	writel(0x3, csi_base + 0x38);  // Enable both lanes
	wmb();
	pr_info("Lane Enable write: wrote 0x3, readback: 0x%08x\n",
        readl(csi_base + 0x38));
    msleep(1);

    // Set masks
    writel(0x000f0000, csi_base + 0x28);
    writel(0x01ff0000, csi_base + 0x2c);
    wmb();

    // Clear any errors one more time
    writel(0xffffffff, csi_base + 0x20);
    writel(0xffffffff, csi_base + 0x24);
    wmb();

    phy_state = readl(csi_base + 0x14);
    pr_info("CSI Final State:\n");
    pr_info("  PHY state: 0x%08x\n", phy_state);
    pr_info("  N_LANES: 0x%08x\n", readl(csi_base + 0x04));
    pr_info("  Lane Enable: 0x%08x\n", readl(csi_base + 0x38));
    pr_info("  Data Format: 0x%08x\n", readl(csi_base + 0x18));
    pr_info("  ERR1: 0x%08x\n", readl(csi_base + 0x20));
    pr_info("  ERR2: 0x%08x\n", readl(csi_base + 0x24));

    mutex_unlock(&dev->csi_dev->mutex);

    if ((phy_state & 0x630) != 0x630) {
        pr_err("CSI PHY not in correct state\n");
        return -EIO;
    }

    return 0;
}



//  TODO
//int init_csi_interface(struct IMPISPDev *dev)
//{
//    void __iomem *csi_base = dev->csi_dev->csi_regs;
//    int ret;
//
//    // Reset state
//    writel(0x0, csi_base + 0x10);  // CSI2_RESETN
//    writel(0x0, csi_base + 0x0c);  // DPHY_RSTZ
//    writel(0x0, csi_base + 0x08);  // PHY_SHUTDOWNZ
//    wmb();
//    msleep(1);
//
//    // Key difference - set lanes first like csi_set_on_lanes()
//    // ((arg2 - 1) & 3) | (current & 0xfffffffc)
//    uint32_t lane_val = readl(csi_base + 0x04);
//    lane_val = ((2 - 1) & 3) | (lane_val & 0xfffffffc);
//    writel(lane_val, csi_base + 0x04);
//    wmb();
//    msleep(1);
//
//    // Configure other registers
//    writel(0x0, csi_base + 0x0c);  // Clear DPHY_RSTZ first
//    msleep(1);
//
//    writel(0x1, csi_base + 0x0c);  // Set DPHY_RSTZ
//    msleep(1);
//
//    // This matches csi_core_ops_init when $s2_1 == 2
//    writel(0x0, csi_base + 0x0c);  // Clear again
//    writel(0x1, csi_base + 0x0c);  // And set
//    writel(0x7d, dev->reg_base + 0x13c);  // Magic value
//    writel(0x3e, dev->reg_base + 0x13c + 0x80);
//    writel(0x1, dev->reg_base + 0x13c + 0x2cc);
//    msleep(10);
//
//    // Enable CSI
//    writel(0x1, csi_base + 0x10);  // CSI2_RESETN
//    wmb();
//    msleep(1);
//
//    // Clear errors
//    writel(0xffffffff, csi_base + 0x20);
//    writel(0xffffffff, csi_base + 0x24);
//    writel(0x000f0000, csi_base + 0x28);
//    writel(0x01ff0000, csi_base + 0x2c);
//    wmb();
//
//    return 0;
//}


