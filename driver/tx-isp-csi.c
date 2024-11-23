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

    // Enable CSI clocks
    ret = enable_csi_clocks(dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to enable CSI clocks\n");
        goto err_unmap_csi;
    }

    // Map PHY registers
    phy_base = ioremap(0x10022000, 0x1000);
    if (!phy_base) {
        dev_err(&pdev->dev, "Failed to map CSI PHY registers\n");
        ret = -ENOMEM;
        goto err_disable_clocks;
    }

    csi_dev->phy_regs = phy_base;

    // Initialize PHY
    writel(0x1, phy_base + 0x0);  // PHY enable
    writel(0x1, phy_base + 0x4);  // Reset PHY
    udelay(10);
    writel(0x0, phy_base + 0x4);  // De-assert reset
    udelay(10);

    // Enable CSI-2 interface
    writel(0x1, phy_base + 0x8);  // Enable CSI-2
    writel(0x1, phy_base + 0xc);  // Enable data lanes
    wmb();

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


int init_csi_phy(struct IMPISPDev *dev)
{
    void __iomem *csi_phy;
    int ret;

    // Map PHY registers
    // Instead of request_mem_region, directly map the PHY registers
    // since they're probably already claimed by the platform
    csi_phy = ioremap(CSI_PHY_BASE, CSI_PHY_SIZE);
    if (!csi_phy) {
        pr_err("Failed to map CSI PHY registers\n");
        return -ENOMEM;
    }

    // Store PHY info
    dev->csi_dev->phy_regs = csi_phy;

    // Initialize PHY before CSI
    writel(0x0, csi_phy + 0x0);  // Reset PHY
    wmb();
    msleep(1);

    // Configure PHY for MIPI
    writel(0x1, csi_phy + 0x4);  // Enable PHY
    writel(0x0, csi_phy + 0x8);  // Clear status
    wmb();
    msleep(1);

    // Now initialize CSI
    void __iomem *csi_base = dev->csi_dev->csi_regs;

    // Full reset first
    writel(0x0, csi_base + 0x10);  // CSI2_RESETN
    writel(0x0, csi_base + 0x0c);  // DPHY_RSTZ
    writel(0x0, csi_base + 0x08);  // PHY_SHUTDOWNZ
    wmb();
    msleep(1);

    // Configure CSI registers
    writel(0x0, csi_base + 0x04);   // N_LANES = 0
    writel(0x1, csi_base + 0x18);   // DATA_IDS_1 = 0x1
    writel(0xc, csi_base + 0x1c);   // DATA_IDS_2 = 0xc
    wmb();
    msleep(1);

    // Enable sequence
    writel(0x1, csi_base + 0x08);   // PHY_SHUTDOWNZ
    wmb();
    msleep(1);
    writel(0x1, csi_base + 0x0c);   // DPHY_RSTZ
    wmb();
    msleep(1);
    writel(0x1, csi_base + 0x10);   // CSI2_RESETN
    wmb();
    msleep(1);

    // Set masks
    writel(0x000f0000, csi_base + 0x28);  // MASK1
    writel(0x01ff0000, csi_base + 0x2c);  // MASK2
    wmb();

    // Clear initial errors
    writel(0xffffffff, csi_base + 0x20);
    writel(0xffffffff, csi_base + 0x24);
    wmb();

    pr_info("CSI/MIPI PHY initialized\n");
    return 0;
}

int configure_mipi_csi(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->reg_base;
    void __iomem *csi_base = dev->csi_dev->csi_regs;
    u32 val;

    pr_info("TX-ISP CSI: Configuring...\n");

    /* First disable everything */
    writel(0x0, csi_base + 0x08);  /* PHY_SHUTDOWNZ = 0 */
    writel(0x0, csi_base + 0x0c);  /* DPHY_RSTZ = 0 */
    writel(0x0, csi_base + 0x10);  /* CSI2_RESETN = 0 */
    wmb();
    msleep(1);

    /* Basic configuration */
    writel(0x1, csi_base + 0x04);  /* N_LANES = 1 (2 lanes) */
    wmb();
    msleep(1);

    /* Power up sequence with 1ms delays between each */
    writel(0x1, csi_base + 0x08);  /* PHY_SHUTDOWNZ = 1 */
    wmb();
    msleep(1);

    writel(0x1, csi_base + 0x0c);  /* DPHY_RSTZ = 1 */
    wmb();
    msleep(1);

    writel(0x1, csi_base + 0x10);  /* CSI2_RESETN = 1 */
    wmb();
    msleep(1);

    /* Configure data format */
    writel(0x2b, csi_base + 0x18);  /* DATA_IDS_1 = 0x2b (RAW10) */
    wmb();

    /* Configure error masks */
    writel(0x000f0000, csi_base + 0x28);  /* MASK1 */
    writel(0x00ff0000, csi_base + 0x2c);  /* MASK2 */
    wmb();

    /* Clear any errors */
    writel(0xffffffff, csi_base + 0x20);  /* Clear ERR1 */
    writel(0xffffffff, csi_base + 0x24);  /* Clear ERR2 */
    wmb();

    /* Verify PHY state */
    val = readl(csi_base + 0x14);  /* PHY_STATE */
    pr_info("CSI configured:\n"
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
    void __iomem *csi_base = dev->csi_dev->csi_regs;
    pr_info("CSI init (enable=%d)\n", enable);

    if (enable) {
        // First configure lanes based on sensor info
        u32 num_lanes = 2; // TODO configure based on sensor info the number of lanes
        writel((num_lanes - 1) & 0x3, csi_base + 0x04);
        wmb();
        msleep(1);

        // Power down sequence
        writel(readl(csi_base + 0x08) & ~0x1, csi_base + 0x08); // PHY_SHUTDOWNZ low
        writel(0x0, csi_base + 0x0c); // DPHY_RSTZ low
        wmb();
        msleep(1);

        writel(readl(csi_base + 0x10) & ~0x1, csi_base + 0x10); // CSI2_RESETN low
        wmb();
        msleep(1);

        // Power up sequence
        writel(readl(csi_base + 0x0c) | 0x1, csi_base + 0x0c); // DPHY_RSTZ high
        wmb();
        msleep(1);

        writel(readl(csi_base + 0x10) | 0x1, csi_base + 0x10); // CSI2_RESETN high
        wmb();
        msleep(10);

    } else {
        pr_info("Disabling CSI...\n");
        writel(readl(csi_base + 0x08) & ~0x1, csi_base + 0x08);
        writel(readl(csi_base + 0x0c) & ~0x1, csi_base + 0x0c);
        writel(readl(csi_base + 0x10) & ~0x1, csi_base + 0x10);
    }

    return 0;
}

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

    if (dev->csi_dev->state < 1 || dev->csi_dev->state > 2) {
        pr_err("Invalid CSI state for stream start: %d\n", dev->csi_dev->state);
        return -EINVAL;
    }

    if (dev->csi_dev->state == 1) {
        ret = tx_isp_csi_activate_subdev(dev);
        if (ret) {
            pr_err("Failed to activate CSI: %d\n", ret);
            return ret;
        }
    }

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


