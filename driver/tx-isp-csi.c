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





#define MIPI_PHY_ADDR   0x10022000
int init_csi_early(struct IMPISPDev *dev)
{
    struct csi_device *csi_dev = dev->csi_dev;
    void __iomem *phy_base = csi_dev->phy_regs;
    void __iomem *csi_base = csi_dev->csi_regs;
    u32 val;

    // Configure PHY - no clock management here
    writel(0x00000004, phy_base + 0x00);  // PHY enable
    writel(0x00000003, phy_base + 0x04);  // 2 lanes
    wmb();
    msleep(1);

    // Setup error monitoring
    writel(0x000FFFFF, csi_base + 0x28); // MASK1 - all errors visible
    writel(0x00FFFFFF, csi_base + 0x2c); // MASK2 - all errors visible
    wmb();

    // Clear any pending errors
    writel(0xFFFFFFFF, csi_base + 0x30); // Clear ERR1
    writel(0xFFFFFFFF, csi_base + 0x34); // Clear ERR2
    wmb();
    msleep(1);

    // Power up PHY
    writel(0x1, csi_base + 0x08);  // PHY_SHUTDOWNZ
    wmb();
    msleep(10);

    writel(0x1, csi_base + 0x0c);  // DPHY_RSTZ
    wmb();
    msleep(10);

    pr_info("PHY setup complete:\n");
    pr_info("  PHY_CTL: 0x%08x\n", readl(phy_base + 0x00));
    pr_info("  PHY_CFG: 0x%08x\n", readl(phy_base + 0x04));
    pr_info("  Errors: 0x%08x/0x%08x\n",
            readl(csi_base + 0x30),
            readl(csi_base + 0x34));

    return 0;
}

//
//int init_csi_phy(struct IMPISPDev *dev) {
//    struct csi_device *csi_dev;
//    void __iomem *phy_base;
//    void __iomem *csi_base;
//    void __iomem *cpm_base;
//    void __iomem **csi_regs_ptr;
//    u32 val;
//
//    pr_info("Mapping I/O regions:\n");
//    pr_info("  MIPI PHY: 0x%08x\n", MIPI_PHY_ADDR);
//    pr_info("  ISP W01: 0x%08x\n", ISP_W01_ADDR);
//
//    // Try using request_mem_region first
//    struct resource *phy_res, *csi_res;
//
//    phy_res = request_mem_region(MIPI_PHY_ADDR, 0x1000, "mipi-phy");
//    if (!phy_res) {
//        pr_info("MIPI PHY region already claimed\n");
//    }
//
//    csi_res = request_mem_region(ISP_W01_ADDR, 0x1000, "isp-w01");
//    if (!csi_res) {
//        pr_info("ISP W01 region already claimed\n");
//    }
//
//    // Map CPM first for clock setup
//    cpm_base = ioremap(CPM_BASE, 0x1000);
//    if (!cpm_base) {
//        return -ENOMEM;
//    }
//
//    pr_info("Initial CPM state:\n");
//    pr_info("  CLKGR: 0x%08x\n", readl(cpm_base + CPM_CLKGR));
//    pr_info("  CLKGR1: 0x%08x\n", readl(cpm_base + CPM_CLKGR1));
//
//    // Setup clocks and power
//    val = readl(cpm_base + CPM_CLKGR);
//    val &= ~CPM_CSI_CLK_MASK;  // Enable CSI clock
//    writel(val, cpm_base + CPM_CLKGR);
//    wmb();
//
//    val = readl(cpm_base + CPM_CLKGR1);
//    val &= ~CPM_CSI_PWR_MASK;  // Enable CSI power
//    writel(val, cpm_base + CPM_CLKGR1);
//    wmb();
//
//    // Configure CSI clock
//    writel(0x1, cpm_base + CPM_CSICDR);
//    wmb();
//    msleep(10);
//
//    // Setup registers
//    //csi_dev->base = csi_base;
//    csi_dev->phy_regs = phy_base;
//    dev->csi_dev = csi_dev;
//
//    pr_info("Initial register readings:\n");
//    pr_info("  W01 0x00: 0x%08x\n", readl(csi_base + 0x00));
//    pr_info("  W01 0x04: 0x%08x\n", readl(csi_base + 0x04));
//    pr_info("  W01 0x08: 0x%08x\n", readl(csi_base + 0x08));
//    pr_info("  PHY 0x00: 0x%08x\n", readl(phy_base + 0x00));
//    pr_info("  PHY 0x04: 0x%08x\n", readl(phy_base + 0x04));
//
//    // First read then try to write test pattern
//    writel(0xaaaa5555, csi_base + 0x04);
//    writel(0x5555aaaa, phy_base + 0x04);
//    wmb();
//
//    pr_info("After test write:\n");
//    pr_info("  W01 0x04: 0x%08x\n", readl(csi_base + 0x04));
//    pr_info("  PHY 0x04: 0x%08x\n", readl(phy_base + 0x04));
//
//    pr_info("After test write:\n");
//    pr_info("  W01 0x04: 0x%08x\n", readl(csi_base + 0x04));
//    pr_info("  PHY 0x04: 0x%08x\n", readl(phy_base + 0x04));
//
//    // Store CSI base in device structure
//    csi_dev->csi_regs = csi_base;
//
//    // Now we can configure CSI with proper registers
//    int ret = configure_mipi_csi(dev);
//    if (ret) {
//        pr_err("Failed to configure MIPI CSI: %d\n", ret);
//        iounmap(csi_base);
//        iounmap(phy_base);
//        iounmap(cpm_base);
//        kfree(csi_dev);
//        return ret;
//    }
//
//    // Verify configuration
//    dump_csi_reg(dev);
//
//    iounmap(cpm_base);
//    return 0;
//}

// Register definitions
#define CSI_ERR1            0x30    // Error 1 status
#define CSI_ERR2            0x34    // Error 2 status
#define CSI_MASK1           0x28    // Interrupt mask 1
#define CSI_MASK2           0x2c    // Interrupt mask 2
#define CSI_PHY_STATE       0x14    // PHY state register
#define CSI_N_LANES         0x04    // Lane configuration
#define CSI_DATA_FORMAT     0x18    // Data format control
#define CSI_PHY_SHUTDOWNZ   0x08    // PHY shutdown control
#define CSI_DPHY_RSTZ       0x0c    // DPHY reset control
#define CSI_CSI2_RESETN     0x10    // CSI2 reset control

int configure_mipi_csi(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->reg_base;
    void __iomem *csi_base = dev->csi_dev->csi_regs;
    u32 val;
    int retry = 5;

    pr_info("T31 CSI: Configuring Sensor...\n");

    // Step 1: Initial error state check
    pr_info("CSI pre-config state:\n");
    pr_info("  ERR1: 0x%08x\n", readl(csi_base + CSI_ERR1));
    pr_info("  ERR2: 0x%08x\n", readl(csi_base + CSI_ERR2));

    // Step 2: Full reset sequence
    writel(0x0, regs + ISP_CSI_CTRL);
    writel(0x0, csi_base + CSI_CSI2_RESETN);
    writel(0x0, csi_base + CSI_DPHY_RSTZ);
    writel(0x0, csi_base + CSI_PHY_SHUTDOWNZ);
    wmb();
    msleep(20);

    // Step 3: Enable error detection during init
    writel(0x000FFFFF, csi_base + CSI_MASK1);  // Enable all error detection
    writel(0x00FFFFFF, csi_base + CSI_MASK2);  // Enable all error detection
    wmb();

    // Step 4: Clear any pending errors
    writel(0xFFFFFFFF, csi_base + CSI_ERR1);
    writel(0xFFFFFFFF, csi_base + CSI_ERR2);
    wmb();
    msleep(1);

    // Step 5: Configure lanes and format
    writel(0x1, csi_base + CSI_N_LANES);     // N_LANES = 1 (2 lanes)
    writel(0x2B, csi_base + CSI_DATA_FORMAT); // RAW10 format
    wmb();
    msleep(1);

    // Step 6: Configure DPHY timing
    writel(0x00000004, regs + 0x40d8);  // ISP_CSI_DPHY_CTRL
    writel(0x00000003, regs + 0x40dc);  // ISP_CSI_LANE_CTRL
    writel(0x0a170201, regs + 0x40e0);  // ISP_CSI_TIMING
    wmb();
    msleep(1);

    // Step 7: Power up sequence with proper delays
    writel(0x1, csi_base + CSI_PHY_SHUTDOWNZ);
    wmb();
    msleep(1);

    writel(0x1, csi_base + CSI_DPHY_RSTZ);
    wmb();
    msleep(1);

    writel(0x1, csi_base + CSI_CSI2_RESETN);  // Enable CSI2
    wmb();
    msleep(1);

    // Step 8: Set operational interrupt masks
    writel(0x000F0000, csi_base + CSI_MASK1);  // Operational mask
    writel(0x00FF0000, csi_base + CSI_MASK2);  // Operational mask
    wmb();
    msleep(1);

    // Final state verification
    val = readl(csi_base + CSI_PHY_STATE);
    pr_info("T31 CSI configuration complete:\n"
            "  PHY state: 0x%08x\n"
            "  ERR1: 0x%08x\n"
            "  ERR2: 0x%08x\n"
            "  MASK1: 0x%08x\n"
            "  MASK2: 0x%08x\n",
            val,
            readl(csi_base + CSI_ERR1),
            readl(csi_base + CSI_ERR2),
            readl(csi_base + CSI_MASK1),
            readl(csi_base + CSI_MASK2));

    return ((val & 0x630) == 0x630) ? 0 : -EIO;
}



int csi_reset(struct tx_isp_subdev *sd)
{
    struct csi_device *csi = tx_isp_get_subdevdata(sd);

    mutex_lock(&csi->mutex);
    csi_hw_init(csi);
    mutex_unlock(&csi->mutex);

    return 0;
}

extern struct IMPISPDev *ourISPdev;

/* Subdev core ops */
int csi_init(struct tx_isp_subdev *sd)
{
    struct csi_device *csi;
    int ret;

    pr_info("CSI init called\n");

    if (!sd) {
        pr_err("NULL subdev in csi_init\n");
        return -EINVAL;
    }

    pr_info("CSI init called II\n");

    csi = ourISPdev->csi_dev;
    pr_info("CSI init called III\n");

    pr_info("CSI device checks: dev=%p clk=%p state=%d mutex=%p\n",
            csi->dev, csi->clk, csi->state, &csi->mutex);

    pr_info("Original CSI device from probe: %p\n", ourISPdev->csi_dev);
    pr_info("CSI device from subdev: %p\n", csi);

    pr_info("CSI init called IV\n");

    mutex_lock(&csi->mutex);

    if (csi->state != TX_ISP_MODULE_SLAKE) {
        pr_err("Invalid CSI state %d, expected %d\n",
               csi->state, TX_ISP_MODULE_SLAKE);
        ret = -EINVAL;
        goto unlock;
    }

    /* Enable clock */
//    ret = clk_prepare_enable(csi->clk);
//    if (ret) {
//        dev_err(csi->dev, "Failed to enable clock\n");
//        //goto unlock;
//    }

    csi->state = TX_ISP_MODULE_INIT;

    unlock:
        mutex_unlock(&csi->mutex);
    return ret;
}

int complete_csi_init(struct IMPISPDev *dev)
{
    void __iomem *regs = dev->reg_base;
    void __iomem *csi_base = dev->csi_dev->csi_regs;
    u32 val;

    // Clear any accumulated errors first
    writel(0xffffffff, csi_base + 0x20);  // Clear ERR1
    writel(0xffffffff, csi_base + 0x24);  // Clear ERR2
    wmb();

    // Set proper masks before enabling
    writel(0x000f0000, csi_base + 0x28);  // MASK1
    writel(0x01ff0000, csi_base + 0x2c);  // MASK2
    wmb();

    // Now enable CSI2
    writel(0x1, csi_base + 0x10);  // CSI2_RESETN
    wmb();
    usleep_range(100, 150);

    // Finally enable CSI in ISP
    writel(0x1, regs + ISP_CSI_CTRL);
    wmb();

    val = readl(csi_base + 0x14);
    pr_info("T31 CSI fully configured:\n"
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
//
//void dump_csi_reg(struct IMPISPDev *dev)
//{
//    void __iomem *csi_base;
//
//    csi_base = dev->csi_dev->csi_regs;
//    if (!csi_base)
//        return;
//
//    pr_info("****>>>>> dump csi reg <<<<<****\n");
//    pr_info("VERSION = %08x\n", readl(csi_base + 0x00));
//    pr_info("N_LANES = %08x\n", readl(csi_base + 0x04));
//    pr_info("PHY_SHUTDOWNZ = %08x\n", readl(csi_base + 0x08));
//    pr_info("DPHY_RSTZ = %08x\n", readl(csi_base + 0x0c));
//    pr_info("CSI2_RESETN = %08x\n", readl(csi_base + 0x10));
//    pr_info("PHY_STATE = %08x\n", readl(csi_base + 0x14));
//    pr_info("DATA_IDS_1 = %08x\n", readl(csi_base + 0x18));
//    pr_info("DATA_IDS_2 = %08x\n", readl(csi_base + 0x1c));
//    pr_info("ERR1 = %08x\n", readl(csi_base + 0x20));
//    pr_info("ERR2 = %08x\n", readl(csi_base + 0x24));
//    pr_info("MASK1 = %08x\n", readl(csi_base + 0x28));
//    pr_info("MASK2 = %08x\n", readl(csi_base + 0x2c));
//    pr_info("PHY_TST_CTRL0 = %08x\n", readl(csi_base + 0x30));
//    pr_info("PHY_TST_CTRL1 = %08x\n", readl(csi_base + 0x34));
//}

void dump_csi_registers_detailed()
{
    u32 phy_state;
    void __iomem *csi_base;

    csi_base = ioremap(0x10022000, 0x1000);
    if (!csi_base) {
        pr_err("Failed to map CSI registers\n");
        return;
    }

    phy_state = readl(csi_base + 0x14);

    pr_info("****>>>>> dump csi reg <<<<<****\n");
    pr_info("VERSION:       0x%08x\n", readl(csi_base + 0x00));
    pr_info("N_LANES:       0x%08x (%d lanes)\n",
            readl(csi_base + 0x04),
            (readl(csi_base + 0x04) & 0x3) + 1);
    pr_info("PHY_SHUTDOWNZ: 0x%08x (%s)\n",
            readl(csi_base + 0x08),
            readl(csi_base + 0x08) ? "enabled" : "shutdown");
    pr_info("DPHY_RSTZ:     0x%08x (%s)\n",
            readl(csi_base + 0x0c),
            readl(csi_base + 0x0c) ? "running" : "reset");
    pr_info("CSI2_RESETN:   0x%08x (%s)\n",
            readl(csi_base + 0x10),
            readl(csi_base + 0x10) ? "running" : "reset");
    pr_info("PHY_STATE:     0x%08x (status:%s%s%s)\n",
            phy_state,
            phy_state & BIT(10) ? " rxclkactivehs" : "",
            phy_state & BIT(5) ? " rxulpsclknot" : "",
            phy_state & BIT(4) ? " stopstate" : "");
    pr_info("DATA_IDS_1:    0x%08x\n", readl(csi_base + 0x18));
    pr_info("DATA_IDS_2:    0x%08x\n", readl(csi_base + 0x1c));
    pr_info("ERR1:          0x%08x\n", readl(csi_base + 0x20));
    pr_info("ERR2:          0x%08x\n", readl(csi_base + 0x24));
    pr_info("MASK1:         0x%08x\n", readl(csi_base + 0x28));
    pr_info("MASK2:         0x%08x\n", readl(csi_base + 0x2c));
    pr_info("PHY_TST_CTRL0: 0x%08x\n", readl(csi_base + 0x30));
    pr_info("PHY_TST_CTRL1: 0x%08x\n", readl(csi_base + 0x34));

    iounmap(csi_base);
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
int setup_csi_power_domain(struct IMPISPDev *dev)
{
    void __iomem *cpm_base;
    u32 val;

    cpm_base = ioremap(CPM_BASE, 0x100);
    if (!cpm_base)
        return -ENOMEM;

    pr_info("Initial CPM state:\n");
    pr_info("  CLKGR: 0x%08x\n", readl(cpm_base + CPM_CLKGR));
    pr_info("  CLKGR1: 0x%08x\n", readl(cpm_base + CPM_CLKGR1));
    pr_info("  CSICDR: 0x%08x\n", readl(cpm_base + CPM_CSICDR));

    // 1. Start with everything disabled
    val = readl(cpm_base + CPM_CLKGR);
    val |= CPM_CSI_CLK_MASK;  // Disable clock
    writel(val, cpm_base + CPM_CLKGR);
    wmb();

    val = readl(cpm_base + CPM_CLKGR1);
    val |= CPM_CSI_PWR_MASK;  // Disable power
    writel(val, cpm_base + CPM_CLKGR1);
    wmb();
    msleep(10);

    pr_info("After disable:\n");
    pr_info("  CLKGR: 0x%08x\n", readl(cpm_base + CPM_CLKGR));
    pr_info("  CLKGR1: 0x%08x\n", readl(cpm_base + CPM_CLKGR1));

    // 2. Enable power domain first
    val = readl(cpm_base + CPM_CLKGR1);
    val &= ~CPM_CSI_PWR_MASK;
    writel(val, cpm_base + CPM_CLKGR1);
    wmb();
    msleep(10);  // Wait for power domain to stabilize

    pr_info("After power enable:\n");
    pr_info("  CLKGR1: 0x%08x\n", readl(cpm_base + CPM_CLKGR1));

    // 3. Configure clock divider while gate is still disabled
    writel(0x1, cpm_base + CPM_CSICDR);
    wmb();
    msleep(1);

    // 4. Enable clock gate after divider is set
    val = readl(cpm_base + CPM_CLKGR);
    val &= ~CPM_CSI_CLK_MASK;
    writel(val, cpm_base + CPM_CLKGR);
    wmb();
    msleep(10);  // Wait for clocks to stabilize

    pr_info("Final power/clock status:\n");
    pr_info("  CLKGR: 0x%08x\n", readl(cpm_base + CPM_CLKGR));
    pr_info("  CLKGR1: 0x%08x\n", readl(cpm_base + CPM_CLKGR1));
    pr_info("  CSICDR: 0x%08x\n", readl(cpm_base + CPM_CSICDR));

    // Verify our writes took effect
    if ((readl(cpm_base + CPM_CLKGR) & CPM_CSI_CLK_MASK) ||
        (readl(cpm_base + CPM_CLKGR1) & CPM_CSI_PWR_MASK)) {
        pr_err("Failed to enable CSI power/clocks\n");
        iounmap(cpm_base);
        return -EIO;
    }

    iounmap(cpm_base);
    return 0;
}

int setup_csi_phy(struct IMPISPDev *dev)
{
    struct csi_device *csi_dev = dev->csi_dev;
    void __iomem *phy_base = csi_dev->phy_regs;
    void __iomem *csi_base = csi_dev->csi_regs;
    u32 val;

    // 1. Start with PHY in reset
    writel(0, phy_base + 0x00);  // PHY reset
    writel(0, phy_base + 0x04);  // Clear PHY config
    writel(0, csi_base + 0x08);  // PHY_SHUTDOWNZ
    writel(0, csi_base + 0x0c);  // DPHY_RSTZ
    wmb();
    msleep(10);

    // 2. Basic PHY configuration while still in reset
    writel(0x00000004, phy_base + 0x00);  // PHY enable
    wmb();
    msleep(1);

    val = readl(phy_base + 0x00);
    if (val != 0x4) {
        pr_err("PHY enable failed: got 0x%08x\n", val);
        return -EIO;
    }

    writel(0x00000003, phy_base + 0x04);  // 2 lanes
    wmb();
    msleep(1);

    // 3. Power up sequence
    writel(0x1, csi_base + 0x08);  // PHY_SHUTDOWNZ first
    wmb();
    msleep(10);

    writel(0x1, csi_base + 0x0c);  // DPHY_RSTZ next
    wmb();
    msleep(10);

    pr_info("PHY setup complete:\n");
    pr_info("  PHY_CTL: 0x%08x\n", readl(phy_base + 0x00));
    pr_info("  PHY_CFG: 0x%08x\n", readl(phy_base + 0x04));

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


// Called during probe
int tx_isp_csi_probe_init(struct IMPISPDev *dev)
{
    void __iomem *csi_base = dev->csi_dev->csi_regs;
    uint32_t val;

    // Start with all interrupts/errors enabled for debugging
    writel(0x000FFFFF, csi_base + 0x28); // MASK1 - enable all error bits
    writel(0x00FFFFFF, csi_base + 0x2c); // MASK2 - enable all error bits
    wmb();

    // Clear any startup errors
    writel(0xFFFFFFFF, csi_base + 0x30); // Clear ERR1
    writel(0xFFFFFFFF, csi_base + 0x34); // Clear ERR2
    wmb();

    // Initial reset sequence
    writel(0, csi_base + 0x08); // Clear PHY_SHUTDOWNZ
    writel(0, csi_base + 0x0c); // Clear DPHY_RSTZ
    writel(0, csi_base + 0x10); // Clear CSI2_RESETN
    wmb();
    msleep(1);

    // Base initialization
    writel(1, csi_base + 0x08); // Set PHY_SHUTDOWNZ
    wmb();
    msleep(1);

    writel(1, csi_base + 0x0c); // Set DPHY_RSTZ
    wmb();
    msleep(1);

    writel(1, csi_base + 0x10); // Set CSI2_RESETN
    wmb();
    msleep(1);

    // Set initial lane configuration
    val = readl(csi_base + 0x04); // N_LANES
    val = (1 & 3) | (val & 0xfffffffc); // Start with 2 lanes
    writel(val, csi_base + 0x04);
    wmb();

    pr_info("CSI probe initialization complete:\n");
    pr_info("  PHY_SHUTDOWNZ: 0x%08x\n", readl(csi_base + 0x08));
    pr_info("  DPHY_RSTZ: 0x%08x\n", readl(csi_base + 0x0c));
    pr_info("  CSI2_RESETN: 0x%08x\n", readl(csi_base + 0x10));
    pr_info("  N_LANES: 0x%08x\n", readl(csi_base + 0x04));
    pr_info("  ERR1: 0x%08x\n", readl(csi_base + 0x30));
    pr_info("  ERR2: 0x%08x\n", readl(csi_base + 0x34));

    return 0;
}

// Modified stream configuration function
int configure_csi_streaming(struct IMPISPDev *dev)
{
    void __iomem *csi_base = dev->csi_dev->csi_regs;
    uint32_t phy_state, val;
    int ret;

    mutex_lock(&dev->csi_dev->mutex);

    pr_info("CSI pre-stream state:\n");
    pr_info("  ERR1: 0x%08x\n", readl(csi_base + 0x30));
    pr_info("  ERR2: 0x%08x\n", readl(csi_base + 0x34));

    // Clear any accumulated errors
    writel(0xFFFFFFFF, csi_base + 0x30);
    writel(0xFFFFFFFF, csi_base + 0x34);
    wmb();

    // Update lane configuration if needed
    uint32_t lanes = 2;
    val = readl(csi_base + 0x04);
    val = ((lanes - 1) & 3) | (val & 0xfffffffc);
    writel(val, csi_base + 0x04);
    wmb();

    // Format configuration
    writel(0x2B, csi_base + 0x18); // RAW10
    wmb();
    msleep(1);

    // Set up PHY timing registers
    writel(0x7d, csi_base + 0x13c);         // PHY timing
    writel(0x3f, csi_base + 0x13c + 0x128); // Additional timing
    wmb();
    msleep(1);

    // Set final operational masks
    writel(0x000F0000, csi_base + 0x28); // MASK1
    writel(0x00FF0000, csi_base + 0x2c); // MASK2
    wmb();
    msleep(1);

    // Final state check
    phy_state = readl(csi_base + 0x14);

    pr_info("CSI stream configuration complete:\n");
    pr_info("  PHY state: 0x%08x\n", phy_state);
    pr_info("  ERR1: 0x%08x\n", readl(csi_base + 0x30));
    pr_info("  ERR2: 0x%08x\n", readl(csi_base + 0x34));
    pr_info("  MASK1: 0x%08x\n", readl(csi_base + 0x28));
    pr_info("  MASK2: 0x%08x\n", readl(csi_base + 0x2c));

    mutex_unlock(&dev->csi_dev->mutex);

    return ((phy_state & 0x630) == 0x630) ? 0 : -EIO;
}



/// REVISED ATTEMPT BELOW

int csi_hw_init(struct csi_device *csi)
{
    /* Clear any pending errors first */
    readl(csi->csi_regs + 0x20); /* ERR1 */
    readl(csi->csi_regs + 0x24); /* ERR2 */

    /* Disable interrupts initially */
    writel(0x0, csi->csi_regs + 0x28); /* MASK1 */
    writel(0x0, csi->csi_regs + 0x2C); /* MASK2 */

    /* Full reset sequence */
    writel(0x0, csi->csi_regs + 0x10); /* CSI2_RESETN */
    writel(0x0, csi->csi_regs + 0x0C); /* DPHY_RSTZ */
    writel(0x0, csi->csi_regs + 0x08); /* PHY_SHUTDOWNZ */

    usleep_range(10000, 15000); /* Give PHY time to reset */

    return 0;
}

int csi_power_on(struct csi_device *csi)
{
    int ret;

    mutex_lock(&csi->mutex);

    if (csi->state != 1) {  /* Check if not in init state */
        mutex_unlock(&csi->mutex);
        return 0;
    }

    /* Enable clock */
    ret = clk_prepare_enable(csi->clk);
    if (ret) {
        dev_err(csi->dev, "Failed to enable clock\n");
        goto unlock;
    }

    /* Basic hardware init */
    ret = csi_hw_init(csi);
    if (ret) {
        dev_err(csi->dev, "Failed to initialize hardware\n");
        goto disable_clk;
    }

    csi->state = 2;  /* Set to enabled state */
    mutex_unlock(&csi->mutex);
    return 0;

disable_clk:
    clk_disable_unprepare(csi->clk);
unlock:
    mutex_unlock(&csi->mutex);
    return ret;
}

int csi_start_stream(struct csi_device *csi)
{
    unsigned long flags;
    int ret = 0;

    mutex_lock(&csi->mutex);

    if (csi->state != 2) {  /* Must be in enabled state */
        ret = -EINVAL;
        goto unlock;
    }

    spin_lock_irqsave(&csi->lock, flags);

    /* Set lane configuration before enabling */
    writel((csi->module_info->offset_2 - 1) & 3, csi->csi_regs + 0x04);  /* N_LANES */

    /* Enable sequence: PHY -> DPHY -> CSI2 */
    writel(0x0, csi->csi_regs + 0x08);  /* PHY_SHUTDOWNZ */
    usleep_range(1000, 1500);

    writel(0x1, csi->csi_regs + 0x0C);  /* DPHY_RSTZ */
    usleep_range(1000, 1500);

    writel(0x1, csi->csi_regs + 0x10);  /* CSI2_RESETN */
    usleep_range(1000, 1500);

    spin_unlock_irqrestore(&csi->lock, flags);

unlock:
    mutex_unlock(&csi->mutex);
    return ret;
}

int csi_stop_stream(struct csi_device *csi)
{
    unsigned long flags;

    mutex_lock(&csi->mutex);

    spin_lock_irqsave(&csi->lock, flags);

    /* Disable sequence: CSI2 -> DPHY -> PHY */
    writel(0x0, csi->csi_regs + 0x10);  /* CSI2_RESETN */
    usleep_range(1000, 1500);

    writel(0x0, csi->csi_regs + 0x0C);  /* DPHY_RSTZ */
    usleep_range(1000, 1500);

    writel(0x0, csi->csi_regs + 0x08);  /* PHY_SHUTDOWNZ */

    spin_unlock_irqrestore(&csi->lock, flags);
    mutex_unlock(&csi->mutex);
    return 0;
}

/* Debug helper */
void csi_dump_registers(struct csi_device *csi)
{
    dev_info(csi->dev, "CSI Registers:");
    dev_info(csi->dev, "VERSION = 0x%08x", readl(csi->csi_regs + 0x00));
    dev_info(csi->dev, "N_LANES = 0x%08x", readl(csi->csi_regs + 0x04));
    dev_info(csi->dev, "PHY_SHUTDOWNZ = 0x%08x", readl(csi->csi_regs + 0x08));
    dev_info(csi->dev, "DPHY_RSTZ = 0x%08x", readl(csi->csi_regs + 0x0C));
    dev_info(csi->dev, "CSI2_RESETN = 0x%08x", readl(csi->csi_regs + 0x10));
    dev_info(csi->dev, "PHY_STATE = 0x%08x", readl(csi->csi_regs + 0x14));
    dev_info(csi->dev, "ERR1 = 0x%08x", readl(csi->csi_regs + 0x20));
    dev_info(csi->dev, "ERR2 = 0x%08x", readl(csi->csi_regs + 0x24));
}


// CSI streaming control - must be non-static since declared in header
int csi_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct csi_device *csi = tx_isp_get_subdevdata(sd);
    unsigned long flags;
    int ret = 0;

    pr_info("CSI stream %s\n", enable ? "on" : "off");

    mutex_lock(&csi->mutex);

    if (enable && csi->state == TX_ISP_MODULE_INIT) {
        spin_lock_irqsave(&csi->lock, flags);

        /* Configure lanes based on sensor settings */
        writel((csi->module_info->offset_2 - 1) & 3, csi->csi_regs + 0x04);

        /* Enable sequence */
        writel(0x1, csi->csi_regs + 0x08);
        usleep_range(1000, 1500);

        writel(0x1, csi->csi_regs + 0x0C);
        usleep_range(1000, 1500);

        writel(0x1, csi->csi_regs + 0x10);
        usleep_range(1000, 1500);

        spin_unlock_irqrestore(&csi->lock, flags);

        csi->state = TX_ISP_MODULE_RUNNING;
    } else if (!enable && csi->state == TX_ISP_MODULE_RUNNING) {
        spin_lock_irqsave(&csi->lock, flags);

        /* Disable sequence */
        writel(0x0, csi->csi_regs + 0x10);
        usleep_range(1000, 1500);

        writel(0x0, csi->csi_regs + 0x0C);
        usleep_range(1000, 1500);

        writel(0x0, csi->csi_regs + 0x08);

        spin_unlock_irqrestore(&csi->lock, flags);

        csi->state = TX_ISP_MODULE_INIT;
    }

    mutex_unlock(&csi->mutex);
    return ret;
}

int csi_link_stream(struct tx_isp_subdev *sd, int enable)
{
    pr_info("CSI link stream %s\n", enable ? "on" : "off");
    /* Handle any link-specific streaming requirements */
    return 0;
}

int verify_mipi_timing_and_phy(struct csi_device *csi_dev, struct i2c_client *sensor)
{
    struct {
        const char *name;
        u32 host_reg_offset;
        u8 sensor_reg;
        u8 host_value;
        u8 sensor_value;
    } timing_params[] = {
        { "Timing Register 1",   0x20, 0x3306, 0, 0 },
        { "Timing Register 2",   0x24, 0x3307, 0, 0 },
        { "Clock Timing",        0x28, 0x330c, 0, 0 },
        { "Term Control",        0x2c, 0x330a, 0, 0 },
        { "Term Timing",         0x30, 0x330b, 0, 0 },
        { "MIPI Control 1",      0x34, 0x36e9, 0, 0 },
        { "MIPI Control 2",      0x38, 0x37f9, 0, 0 }
    };
    int i, ret, mismatches = 0;
    u32 phy_state, prev_state = 0;
    int state_changes = 0;
    const int NUM_SAMPLES = 10;

    dev_info(csi_dev->dev, "=== MIPI Timing and PHY State Verification ===\n");

    // Monitor PHY state stability
    dev_info(csi_dev->dev, "Monitoring PHY state stability...\n");
    for (i = 0; i < NUM_SAMPLES; i++) {
        phy_state = readl(csi_dev->csi_regs + 0x14);  // PHY_STATE register
        if (prev_state && phy_state != prev_state) {
            dev_warn(csi_dev->dev, "PHY state changed: 0x%08x -> 0x%08x\n",
                    prev_state, phy_state);
            state_changes++;
        }
        prev_state = phy_state;
        msleep(20);  // Sample every 20ms
    }

    if (state_changes > 0) {
        dev_warn(csi_dev->dev, "PHY state unstable - %d changes detected\n",
                state_changes);
    }

    // Check error registers
    u32 err1 = readl(csi_dev->csi_regs + 0x20);  // ERR1 register
    u32 err2 = readl(csi_dev->csi_regs + 0x24);  // ERR2 register

    dev_info(csi_dev->dev, "\n=== CSI Error Analysis ===\n");
    if (err1) {
        dev_warn(csi_dev->dev, "ERR1: 0x%08x\n", err1);
        if (err1 & BIT(4)) dev_warn(csi_dev->dev, "  - CRC error detected\n");
        if (err1 & BIT(8)) dev_warn(csi_dev->dev, "  - Bit 8 error (undefined)\n");
        if (err1 & BIT(28)) dev_warn(csi_dev->dev, "  - Bit 28 error (undefined)\n");
    }

    if (err2) {
        dev_warn(csi_dev->dev, "ERR2: 0x%08x\n", err2);
    }

    // Verify timing parameters
    dev_info(csi_dev->dev, "\n=== Timing Parameter Verification ===\n");
    for (i = 0; i < ARRAY_SIZE(timing_params); i++) {
        timing_params[i].host_value = readl(csi_dev->phy_regs +
                                          timing_params[i].host_reg_offset) & 0xFF;

        ret = i2c_smbus_read_byte_data(sensor, timing_params[i].sensor_reg);
        if (ret < 0) {
            dev_err(csi_dev->dev, "Failed to read sensor reg 0x%02x: %d\n",
                    timing_params[i].sensor_reg, ret);
            continue;
        }
        timing_params[i].sensor_value = ret & 0xFF;

        if (timing_params[i].host_value != timing_params[i].sensor_value) {
            dev_warn(csi_dev->dev, "%s MISMATCH:\n", timing_params[i].name);
            dev_warn(csi_dev->dev, "  Host (0x%02x): 0x%02x\n",
                    timing_params[i].host_reg_offset,
                    timing_params[i].host_value);
            dev_warn(csi_dev->dev, "  Sensor (0x%02x): 0x%02x\n",
                    timing_params[i].sensor_reg,
                    timing_params[i].sensor_value);
            mismatches++;
        }
    }

    // Check data format configuration
    dev_info(csi_dev->dev, "\n=== Data Format Configuration ===\n");
    u32 data_type = readl(csi_dev->csi_regs + 0x18) & 0x3F;
    u8 sensor_format = i2c_smbus_read_byte_data(sensor, 0x3f01);  // Format register

    dev_info(csi_dev->dev, "Data type - Host: 0x%02x, Sensor: 0x%02x (RAW10=0x2A)\n",
             data_type, sensor_format);

    return (mismatches > 0 || state_changes > 0) ? -EINVAL : 0;
}


static inline bool sample_phy_state(struct csi_device *csi_dev, u32 desired_state, int samples_needed)
{
    int stable_count = 0;
    int max_tries = samples_needed + 5;  // Allow a few extra attempts

    while (max_tries--) {
        u32 state = readl(csi_dev->csi_regs + 0x14);  // PHY_STATE

        if (state == desired_state) {
            stable_count++;
            if (stable_count >= samples_needed) {
                return true;
            }
        } else {
            stable_count = 0;  // Reset on any unstable sample
        }
        udelay(10);  // Short delay between samples
    }

    return false;
}

int init_csi_phy(struct csi_device *csi_dev)
{
    u32 reg, rate_cfg;

    if (!csi_dev || !csi_dev->csi_regs || !csi_dev->phy_regs) {
        pr_err("CSI device or register spaces not properly initialized\n");
        return -EINVAL;
    }

    // Get clock rate safely
    if (!csi_dev->clk) {
        dev_err(csi_dev->dev, "CSI clock not initialized\n");
        return -EINVAL;
    }
    unsigned long csi_rate = clk_get_rate(csi_dev->clk);

    // Configure for 405Mbps
    rate_cfg = 6;  // Lower rate setting

    dev_info(csi_dev->dev, "Initializing CSI PHY with clock rate %lu Hz (rate_cfg: %u)",
             csi_rate, rate_cfg);

    // 1. Reset sequence with safety checks
    writel(0, csi_dev->csi_regs + 0x08);  // PHY_SHUTDOWNZ = 0
    writel(0, csi_dev->csi_regs + 0x0c);  // DPHY_RSTZ = 0
    writel(0, csi_dev->csi_regs + 0x10);  // CSI2_RESETN = 0
    wmb();
    msleep(10);

    // 2. Initial PHY configuration
    reg = readl(csi_dev->phy_regs + 0x000);
    writel((reg & ~0xFF) | 0x7d, csi_dev->phy_regs + 0x000);  // PHY control

    reg = readl(csi_dev->phy_regs + 0x128);
    writel((reg & ~0xFF) | 0x3f, csi_dev->phy_regs + 0x128);  // PHY config
    wmb();
    msleep(1);

    // 3. PHY Rate Configuration - protecting register bits
    reg = readl(csi_dev->phy_regs + 0x160);
    writel((reg & ~0xF) | rate_cfg, csi_dev->phy_regs + 0x160);

    reg = readl(csi_dev->phy_regs + 0x1e0);
    writel((reg & ~0xF) | rate_cfg, csi_dev->phy_regs + 0x1e0);

    reg = readl(csi_dev->phy_regs + 0x260);
    writel((reg & ~0xF) | rate_cfg, csi_dev->phy_regs + 0x260);
    wmb();
    msleep(1);

    // 4. Configure lanes with safety masks
    reg = readl(csi_dev->csi_regs + 0x04);
    writel((reg & ~0x3) | 0x1, csi_dev->csi_regs + 0x04);  // 2 lanes
    wmb();

    reg = readl(csi_dev->phy_regs + 0x12C);
    writel((reg & ~0xFF) | 0x03, csi_dev->phy_regs + 0x12C);  // Enable lanes 0 & 1

    reg = readl(csi_dev->phy_regs + 0x130);
    writel((reg & ~0xFF) | 0x02, csi_dev->phy_regs + 0x130);  // Configure lane roles
    wmb();
    msleep(1);

    // 5. Bring up sequence
    writel(1, csi_dev->csi_regs + 0x08);  // PHY_SHUTDOWNZ = 1
    wmb();
    msleep(1);

    writel(1, csi_dev->csi_regs + 0x0c);  // DPHY_RSTZ = 1
    wmb();
    msleep(1);

    writel(1, csi_dev->csi_regs + 0x10);  // CSI2_RESETN = 1
    wmb();
    msleep(10);

    // 6. Verify final state
    u32 phy_state = readl(csi_dev->csi_regs + 0x14);
    if (phy_state != 0x300) {
        dev_warn(csi_dev->dev, "PHY in unexpected state: 0x%08x\n", phy_state);
    }

    // Log configuration - with NULL checks on dev
    if (csi_dev && csi_dev->dev) {
        dev_info(csi_dev->dev, "CSI PHY Register state after initialization:");
        dev_info(csi_dev->dev, "PHY_CTRL (0x000):     0x%08x",
                 readl(csi_dev->phy_regs + 0x000));
        dev_info(csi_dev->dev, "PHY_CFG (0x128):      0x%08x",
                 readl(csi_dev->phy_regs + 0x128));
        dev_info(csi_dev->dev, "PHY_STATE:            0x%08x", phy_state);
    }

    return 0;
}

static int configure_phy_rate(struct csi_device *csi_dev)
{
    void __iomem *phy_regs = csi_dev->phy_regs;
    u32 rate_cfg;
    u32 clock_mhz;

    if (!csi_dev->clk) {
        pr_err("CSI clock not initialized\n");
        return -EINVAL;
    }

    clock_mhz = clk_get_rate(csi_dev->clk) / 1000000;

    // Rate configuration logic from trace analysis
    if (clock_mhz < 80)         rate_cfg = 0;
    else if (clock_mhz < 110)   rate_cfg = 1;
    else if (clock_mhz < 150)   rate_cfg = 2;
    else if (clock_mhz < 200)   rate_cfg = 3;
    else if (clock_mhz < 250)   rate_cfg = 4;
    else if (clock_mhz < 300)   rate_cfg = 5;
    else if (clock_mhz < 400)   rate_cfg = 6;
    else if (clock_mhz < 500)   rate_cfg = 7;
    else if (clock_mhz < 600)   rate_cfg = 8;
    else if (clock_mhz < 700)   rate_cfg = 9;
    else if (clock_mhz < 800)   rate_cfg = 0xa;
    else                        rate_cfg = 0xb;

    // Apply rate configuration to all PHY rate registers
    writel((readl(phy_regs + 0x160) & ~0xF) | rate_cfg, phy_regs + 0x160);
    writel((readl(phy_regs + 0x1e0) & ~0xF) | rate_cfg, phy_regs + 0x1e0);
    writel((readl(phy_regs + 0x260) & ~0xF) | rate_cfg, phy_regs + 0x260);
    wmb();

    return 0;
}


#define CSI_MASK1_IPU   0x000FF113  // Keep IPU bits in higher part
#define CSI_MASK2_IPU   0x0000FF33  // Keep format related bits
int tx_isp_csi_s_stream(int enable)
{
    void __iomem *csi_base = ioremap(0x10022000, 0x1000);
    void __iomem *phy_base;
    int ret = 0;

    if (!csi_base) {
        pr_err("Failed to map CSI registers\n");
        return -ENOMEM;
    }

    // Map PHY registers separately - trace shows these are accessed
    phy_base = ioremap(0x10022800, 0x1000);  // PHY offset from trace
    if (!phy_base) {
        pr_err("Failed to map PHY registers\n");
        ret = -ENOMEM;
        goto cleanup_csi;
    }

    if (enable) {
        // 1. Initial shutdown sequence
        writel(0, csi_base + 0x08);  // PHY_SHUTDOWNZ off
        writel(0, csi_base + 0x0c);  // DPHY_RSTZ off
        writel(0, csi_base + 0x10);  // CSI2_RESETN off
        wmb();
        msleep(10);  // Important delay from trace

        // 2. PHY configuration - matches trace sequence
        writel(0x3f00, csi_base + 0x9804);  // PHY control mode
        writel(0x7800438, csi_base + 0x9864);  // PHY config
        writel(0xc0000000, csi_base + 0x987c);  // State control
        wmb();

        // 3. Configure lanes - trace shows 2 lanes
        writel(1, csi_base + 0x04);  // Set 2 lanes
        wmb();

        // 4. PHY timing configuration from trace
        writel(0x7d, phy_base + 0x000);  // PHY timing control
        writel(0x3f, phy_base + 0x128);  // PHY timing config
        wmb();
        msleep(1);

        // 5. Color matrix configuration - missing from original
        writel(0x40404040, phy_base + 0xb018);  // Color matrix 1
        writel(0x40404040, phy_base + 0xb01c);  // Color matrix 2
        writel(0x40404040, phy_base + 0xb020);  // Color matrix 3
        wmb();

        // 6. Color processing parameters - from trace
        writel(0x82b9, phy_base + 0xb080);  // Color processing
        writel(0x271b, phy_base + 0xb084);  // Color parameters
        wmb();

        // 7. Bring up sequence with proper delays
        writel(1, csi_base + 0x08);  // PHY_SHUTDOWNZ = 1
        wmb();
        msleep(1);

        writel(1, csi_base + 0x0c);  // DPHY_RSTZ = 1
        wmb();
        msleep(1);

        writel(1, csi_base + 0x10);  // CSI2_RESETN = 1
        wmb();
        msleep(10);

        // 8. Verify PHY state
        u32 phy_state = readl(csi_base + 0x14);
        if (phy_state != 0x300) {
            pr_warn("PHY in unexpected state: 0x%08x\n", phy_state);
        }

    } else {
        // Power down sequence matching trace
        writel(readl(csi_base + 0x08) & ~0x1, csi_base + 0x08);
        writel(readl(csi_base + 0x0c) & ~0x1, csi_base + 0x0c);
        writel(readl(csi_base + 0x10) & ~0x1, csi_base + 0x10);
        wmb();
    }

cleanup_phy:
    if (phy_base)
        iounmap(phy_base);
cleanup_csi:
    if (csi_base)
        iounmap(csi_base);
    return ret;
}
