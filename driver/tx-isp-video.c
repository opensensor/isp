#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>

#include "tx-isp-main.h"
#include "tx-isp.h"
#include "tx-isp-hw.h"


int enable_isp_streaming(struct IMPISPDev *dev, struct file *file, int channel, bool enable)
{
    struct isp_framesource_state *fs;
    struct frame_source_channel *fc;
    void __iomem *isp_regs = dev->reg_base + ISP_BASE;
    void __iomem *vic_regs = dev->reg_base + VIC_BASE;
    void __iomem *csi_regs;
    unsigned long flags;
    int ret = 0;
    uint32_t status;
    int timeout;

    if (!dev || !dev->reg_base || !dev->csi_dev) {
        pr_err("Invalid device structure\n");
        return -EINVAL;
    }

    csi_regs = dev->csi_dev->csi_regs;
    if (!csi_regs) {
        pr_err("Invalid CSI registers\n");
        return -EINVAL;
    }

    fs = &dev->frame_sources[channel];
    if (!fs || !fs->is_open) {
        pr_err("Invalid channel state\n");
        return -EINVAL;
    }
    fc = fs->fc;

    pr_info("Starting stream setup:\n");
    pr_info("  ISP registers: %p\n", dev->reg_base);
    pr_info("\n=== ISP Stream %s Debug ===\n", enable ? "Enable" : "Disable");
    pr_info("Initial IRQ State:\n");
    pr_info("  IRQ Number: %d\n", dev->irq);
    //pr_info("  IRQ Enabled: %d\n", dev->irq_enabled);
//    pr_info("  Has Handler: %s\n",
//        dev->irq_data && dev->irq_data->handler_function ? "yes" : "no");
    if (enable) {
        // 1. CSI Configuration first
        mutex_lock(&dev->csi_dev->mutex);
        pr_info("CSI base: %p\n", csi_regs);
        pr_info("CSI mutex locked\n");

        // Clear errors first
        writel(0xffffffff, csi_regs + 0x20); // ERR1
        writel(0xffffffff, csi_regs + 0x24); // ERR2
        wmb();

        // Full reset sequence
        writel(0x0, csi_regs + 0x10);  // CSI2_RESETN
        writel(0x0, csi_regs + 0x0c);  // DPHY_RSTZ
        writel(0x0, csi_regs + 0x08);  // PHY_SHUTDOWNZ
        wmb();
        udelay(100);

        // Configure lanes
        uint32_t lanes_val = readl(csi_regs + 0x04);
        lanes_val = ((2 - 1) & 3) | (lanes_val & 0xfffffffc);
        writel(lanes_val, csi_regs + 0x04);  // N_LANES setup
        writel(0x2B, csi_regs + 0x18);       // DATA_IDS_1 = RAW10
        writel(0x3, csi_regs + 0x38);        // Enable both lanes
        wmb();
        udelay(100);

        // Power up sequence
        writel(0x1, csi_regs + 0x08);  // PHY_SHUTDOWNZ
        wmb();
        udelay(100);

        writel(0x1, csi_regs + 0x0c);  // DPHY_RSTZ
        wmb();
        udelay(100);

        writel(0x1, csi_regs + 0x10);  // CSI2_RESETN
        wmb();
        udelay(100);

        // Set masks
        writel(0x000f0000, csi_regs + 0x28);  // MASK1
        writel(0x01ff0000, csi_regs + 0x2c);  // MASK2
        wmb();

        mutex_unlock(&dev->csi_dev->mutex);

        // Verify CSI signals after full setup
        ret = verify_csi_signals(dev);
        if (ret) {
            pr_err("CSI signal verification failed\n");
            return ret;
        }

        // Reset VIC before configuration
        ret = reset_vic(dev);
        if (ret) {
            return ret;
        }
        pr_info("\n=== Post-Reset VIC State ===\n");
        pr_info("  Status: 0x%08x\n", readl(vic_regs + VIC_STATUS));
        pr_info("  Control: 0x%08x\n", readl(vic_regs + VIC_CTRL));
        pr_info("  IRQ Enable: 0x%08x\n", readl(vic_regs + VIC_IRQ_EN));
        pr_info("  IRQ Mask: 0x%08x\n", readl(vic_regs + VIC_MASK));
        pr_info("  Frame Control: 0x%08x\n", readl(vic_regs + VIC_FRAME_CTRL));
        spin_lock_irqsave(&dev->vic_lock, flags);

        // Clear any pending interrupts
        writel(0xFFFFFFFF, vic_regs + VIC_STATUS);
        wmb();

        // Initial mode setup
        writel(2, vic_regs + 0xc);  // Mode = 2
        writel(0x800c0000, vic_regs + 0x10);  // Magic control
        wmb();
        udelay(100);

        // Poll until VIC status is 0 or 0x707 (which seems to be valid)
        timeout = 1000;
        do {
            status = readl(vic_regs + VIC_STATUS);
            pr_info("Polling VIC status: 0x%08x\n", status);

            // Break if we see either 0 or the magic 0x707
            if (status == 0 || status == 0x707)
                break;

            if (timeout-- <= 0) {
                pr_err("VIC status timeout: 0x%08x\n", status);
                spin_unlock_irqrestore(&dev->vic_lock, flags);
                return -ETIMEDOUT;
            }
            udelay(1);
        } while (1);

        // If we got 0x707, clear it
        if (status == 0x707) {
            writel(0x707, vic_regs + VIC_STATUS);
            wmb();
            udelay(100);
        }

        writel(1, vic_regs + 0x4);           // Control back to 1
		writel(0x33fb, vic_regs + 0xc);      // IRQ Enable with magic value
		writel(0x33fb, vic_regs + 0x8);      // Full mask
		writel(0x3, vic_regs + 0x10);        // Both route bits
		wmb();

        // Configure VIC with magic values
        writel(0x100010, vic_regs + 0x1a4);
        writel(0x4210, vic_regs + 0x1ac);
        writel(0x10, vic_regs + 0x1b0);
        writel(0, vic_regs + 0x1b4);
        wmb();
        udelay(100);

        // Frame format should be 0 for RAW/NV12
        writel(0, vic_regs + 0x14);
        wmb();
        udelay(100);

        // Control sequence - 2->1 like before
        writel(2, vic_regs + VIC_CTRL);
        wmb();
        udelay(100);

        writel(1, vic_regs + VIC_CTRL);
        wmb();
        udelay(100);

        // Frame configuration
        writel((fs->width << 16) | fs->height, vic_regs + VIC_FRAME_SIZE);
        writel((fs->width << 1) + fs->width, vic_regs + 0x18);
        wmb();
        udelay(100);

        // Set status and enable
        writel(1, vic_regs + VIC_STATUS);
        writel(1, vic_regs + VIC_FRAME_CTRL);
        writel(1, vic_regs + VIC_IRQ_EN);
        writel(0xFFFFFFFF, vic_regs + VIC_MASK);
        wmb();
        udelay(100);

        // DMA setup
        if (fs->dma_addr & 0x1F) {
            pr_warn("DMA address not 32-byte aligned\n");
        }
        writel(ALIGN(fs->dma_addr, 32), vic_regs + VIC_DMA_CTRL);
        writel(0x20151120, vic_regs + VIC_DMA_CTRL);
        wmb();
        udelay(100);

        // Before enabling IRQs:
        pr_info("\n=== Pre-IRQ Enable State ===\n");
        pr_info("  VIC Status: 0x%08x\n", readl(vic_regs + VIC_STATUS));
        pr_info("  VIC Control: 0x%08x\n", readl(vic_regs + VIC_CTRL));
        pr_info("  VIC Frame Control: 0x%08x\n", readl(vic_regs + VIC_FRAME_CTRL));
        pr_info("  VIC IRQ Enable: 0x%08x\n", readl(vic_regs + VIC_IRQ_EN));
        pr_info("  VIC IRQ Mask: 0x%08x\n", readl(vic_regs + VIC_MASK));
        pr_info("  VIC DMA Control: 0x%08x\n", readl(vic_regs + VIC_DMA_CTRL));
        pr_info("  ISP Stream Control: 0x%08x\n", readl(isp_regs + ISP_STREAM_CTRL));
    	// 5. Re-verify and set route again
    	writel(0x1, vic_regs + 0x10);
    	wmb();
        // Enable IRQs after full configuration
        tx_isp_enable_irq(dev);

        // After enabling IRQs:
        pr_info("\n=== Post-IRQ Enable State ===\n");
        pr_info("  VIC Status: 0x%08x\n", readl(vic_regs + VIC_STATUS));
        pr_info("  VIC IRQ Enable: 0x%08x\n", readl(vic_regs + VIC_IRQ_EN));
        pr_info("  VIC IRQ Mask: 0x%08x\n", readl(vic_regs + VIC_MASK));
        // Start ISP streaming
        writel(0x1, isp_regs + ISP_STREAM_START);
        wmb();
        udelay(100);
        writel(0x1, isp_regs + ISP_STREAM_CTRL);
        wmb();

        pr_info("Final VIC State:\n");
        pr_info("  Control: 0x%08x\n", readl(vic_regs + VIC_CTRL));
        pr_info("  Status: 0x%08x\n", readl(vic_regs + VIC_STATUS));
        pr_info("  IRQ Enable: 0x%08x\n", readl(vic_regs + VIC_IRQ_EN));
        pr_info("  Frame Control: 0x%08x\n", readl(vic_regs + VIC_FRAME_CTRL));

        // After starting stream:
        pr_info("\n=== Stream Start State ===\n");
        pr_info("  ISP Stream Control: 0x%08x\n", readl(isp_regs + ISP_STREAM_CTRL));
        pr_info("  ISP Stream Start: 0x%08x\n", readl(isp_regs + ISP_STREAM_START));
        pr_info("  VIC Frame Control: 0x%08x\n", readl(vic_regs + VIC_FRAME_CTRL));
        pr_info("  VIC DMA Control: 0x%08x\n", readl(vic_regs + VIC_DMA_CTRL));

        // Add extended wait and check
        pr_info("\n=== IRQ State Check (1ms) ===\n");
        udelay(1000);
        pr_info("  VIC Status: 0x%08x\n", readl(vic_regs + VIC_STATUS));
        pr_info("  VIC IRQ Enable: 0x%08x\n", readl(vic_regs + VIC_IRQ_EN));
        pr_info("  VIC Route: 0x%08x\n", readl(vic_regs + 0x10));

        // Wait a bit and check if we get any interrupts
        udelay(1000);
        pr_info("IRQ Check after 1ms:\n");
        pr_info("  VIC Status: 0x%08x\n", readl(vic_regs + VIC_STATUS));
		// Check the VIC interrupt controller routing
		pr_info("VIC Route Config:\n");
		pr_info("  Route: 0x%08x\n", readl(vic_regs + 0x10));  // Assuming route reg offset
        verify_isp_state(dev);
        dump_csi_reg(dev);

    } else {

        // Disable VIC first
        writel(0, vic_regs + VIC_DMA_CTRL);  // Stop DMA
        writel(0, vic_regs + VIC_CTRL);      // Disable VIC core
        wmb();

        // Then disable ISP streaming
        writel(0x0, isp_regs + ISP_STREAM_CTRL);
        writel(0x0, isp_regs + ISP_STREAM_START);
        wmb();
    }

    return ret;
}


int tx_isp_video_s_stream(struct IMPISPDev *dev, int enable)
{
    struct isp_framesource_state *fs;
    int ret = 0;

    pr_info("Setting video stream state: %d\n", enable);

    if (!dev) {
        pr_err("Invalid device in stream control\n");
        return -EINVAL;
    }

    // Get frame source channel 0 as seen in enable_isp_streaming
    fs = &dev->frame_sources[0];
    if (!fs || !fs->is_open) {
        pr_err("Frame source not initialized\n");
        return -EINVAL;
    }

    if (enable) {
        // Perform stream enable sequence
        ret = enable_isp_streaming(dev, NULL, 0, true);
        if (ret)
            return ret;

        // Update state seen in decompiled enable paths
        fs->state = 2;
    } else {
        // Stop streaming
        ret = enable_isp_streaming(dev, NULL, 0, false);
        if (ret)
            return ret;

        // Reset state
        fs->state = 1;
    }

    pr_info("Stream state changed to: %d\n", enable);
    return 0;
}
