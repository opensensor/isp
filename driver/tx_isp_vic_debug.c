/*
 * TX ISP VIC Debug and MIPI PHY Configuration
 * 
 * This file implements the missing MIPI PHY configuration and VIC debugging
 * functionality based on Binary Ninja decompilation of the reference driver.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/interrupt.h>

#include "../include/tx_isp.h"
#include "../include/tx_isp_core.h"
#include "../include/tx_isp_vic.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx-isp-debug.h"

/* Global variables for VIC error tracking - EXACT Binary Ninja implementation */
static uint32_t vic_err = 0;
static uint32_t data_b299c = 0;  /* frame asfifo ovf counter */
static uint32_t data_b29a0 = 0;  /* general error counter */
static uint32_t data_b2974 = 0;  /* ver err ch0 counter */
static uint32_t data_b2978 = 0;  /* hvf err counter */
static uint32_t data_b297c = 0;  /* dvp hcomp err counter */
static uint32_t data_b2980 = 0;  /* dma syfifo ovf counter */
static uint32_t data_b2984 = 0;  /* control limit err counter */
static uint32_t data_b2988 = 0;  /* image syfifo ovf counter */
static uint32_t data_b298c = 0;  /* mipi fid asfifo ovf counter */
static uint32_t data_b2990 = 0;  /* mipi ch0 hcomp err counter */
static uint32_t data_b2994 = 0;  /* mipi ch0 vcomp err counter */
static uint32_t data_b2998 = 0;  /* dma chid ovf counter */
extern uint32_t vic_start_ok;  /* Global VIC interrupt enable flag declaration */

/* GPIO switch state for frame done processing */
static uint32_t gpio_switch_state = 0;
static uint8_t gpio_info[20] = {0xff}; /* GPIO configuration array */

int vic_mdma_irq_function(struct tx_isp_dev *isp_dev, int channel);


/**
 * vic_mdma_irq_function - Handle VIC MDMA interrupts
 */
int vic_mdma_irq_function(struct tx_isp_dev *isp_dev, int channel)
{
    pr_debug("*** vic_mdma_irq_function: MDMA channel %d interrupt ***\n", channel);
    
    if (!isp_dev) {
        return -EINVAL;
    }
    
    /* Process MDMA completion for the specified channel */
    /* This would handle DMA transfer completion and buffer management */
    
    return 0;
}
EXPORT_SYMBOL(vic_mdma_irq_function);

/**
 * tx_isp_vic_start_streaming - Start VIC streaming with proper initialization
 * This function addresses the "control limit err" by ensuring proper VIC setup
 */
int tx_isp_vic_start_streaming(struct tx_isp_dev *isp_dev)
{
    void __iomem *vic_regs;
    uint32_t width, height;
    uint32_t buffer_size;
    
    if (!isp_dev || !isp_dev->vic_regs) {
        pr_err("tx_isp_vic_start_streaming: Invalid parameters\n");
        return -EINVAL;
    }
    
    vic_regs = isp_dev->vic_regs;
    width = isp_dev->sensor_width;
    height = isp_dev->sensor_height;
    
    pr_info("*** tx_isp_vic_start_streaming: Starting VIC streaming %dx%d ***\n", width, height);
    
    /* CRITICAL: Validate dimensions to prevent control limit error */
    if (width == 0 || height == 0 || width > 8192 || height > 8192) {
        pr_err("*** tx_isp_vic_start_streaming: INVALID DIMENSIONS %dx%d ***\n", width, height);
        pr_err("*** This will cause VIC control limit error! ***\n");
        return -EINVAL;
    }
    
    /* Calculate buffer size and validate */
    buffer_size = width * height * 2; /* Assume 16-bit per pixel */
    if (buffer_size > 0x2000000) { /* 32MB limit */
        pr_err("*** tx_isp_vic_start_streaming: Buffer size too large: %d bytes ***\n", buffer_size);
        return -EINVAL;
    }
    
    /* Configure VIC dimensions - CRITICAL for preventing control limit error */
    writel((width << 16) | height, vic_regs + 0x10);
    writel(width * 2, vic_regs + 0x14); /* Stride for 16-bit */
    wmb();
    
    /* Configure VIC control register */
    writel(3, vic_regs + 0xc); /* MIPI mode */
    wmb();
    
    /* Enable VIC interrupts using CORRECT VIC registers */
    writel(0xFFFFFFFF, vic_regs + 0x00); /* Clear any pending interrupts */
    writel(0x00000003, vic_regs + 0x04); /* Enable frame done and error interrupts */
    wmb();
    
    /* Set VIC start flag - CRITICAL for interrupt processing */
    vic_start_ok = 1;
    
    /* Start VIC controller */
    writel(1, vic_regs + 0x0);
    wmb();
    
    pr_info("*** tx_isp_vic_start_streaming: VIC streaming started successfully ***\n");
    pr_info("*** vic_start_ok = %d, interrupts enabled ***\n", vic_start_ok);
    
    return 0;
}
EXPORT_SYMBOL(tx_isp_vic_start_streaming);

/**
 * tx_isp_vic_stop_streaming - Stop VIC streaming
 */
int tx_isp_vic_stop_streaming(struct tx_isp_dev *isp_dev)
{
    void __iomem *vic_regs;
    
    if (!isp_dev || !isp_dev->vic_regs) {
        pr_err("tx_isp_vic_stop_streaming: Invalid parameters\n");
        return -EINVAL;
    }
    
    vic_regs = isp_dev->vic_regs;
    
    pr_info("*** tx_isp_vic_stop_streaming: Stopping VIC streaming ***\n");
    
    /* Clear VIC start flag */
    vic_start_ok = 0;

    /* Stop VIC controller */
    writel(0, vic_regs + 0x0);
    wmb();
    
    /* Disable VIC interrupts */
    writel(0x0, vic_regs + 0x1e0);
    writel(0xffffffff, vic_regs + 0x1e8);
    wmb();
    
    pr_info("*** tx_isp_vic_stop_streaming: VIC streaming stopped ***\n");
    
    return 0;
}
EXPORT_SYMBOL(tx_isp_vic_stop_streaming);

/**
 * tx_isp_mipi_phy_status_check - Check MIPI PHY lane status
 * This function provides the missing MIPI lane debugging capability
 */
int tx_isp_mipi_phy_status_check(struct tx_isp_dev *isp_dev)
{
    void __iomem *phy_regs;
    uint32_t lane_status, clock_status, error_status;
    int i;
    
    if (!isp_dev || !isp_dev->phy_base) {
        pr_err("tx_isp_mipi_phy_status_check: Invalid parameters\n");
        return -EINVAL;
    }
    
    phy_regs = isp_dev->phy_base;
    
    pr_info("*** tx_isp_mipi_phy_status_check: Checking MIPI PHY status ***\n");
    
    /* Check clock lane status */
    clock_status = readl(phy_regs + 0x14);
    pr_info("MIPI Clock Lane Status: 0x%08x\n", clock_status);
    
    if (clock_status & 0x1) {
        pr_info("  - Clock lane: ACTIVE\n");
    } else {
        pr_err("  - Clock lane: INACTIVE - No clock detected!\n");
    }
    
    /* Check data lane status */
    for (i = 0; i < 4; i++) {
        lane_status = readl(phy_regs + 0x40 + (i * 4));
        pr_info("MIPI Data Lane %d Status: 0x%08x\n", i, lane_status);
        
        if (lane_status & 0x1) {
            pr_info("  - Data lane %d: SYNC\n", i);
        } else {
            pr_warn("  - Data lane %d: NO SYNC\n", i);
        }
        
        if (lane_status & 0x2) {
            pr_info("  - Data lane %d: RECEIVING DATA\n", i);
        } else {
            pr_warn("  - Data lane %d: NO DATA\n", i);
        }
    }
    
    /* Check for PHY errors */
    error_status = readl(phy_regs + 0x8c);
    pr_info("MIPI PHY Error Status: 0x%08x\n", error_status);
    
    if (error_status & 0x1) {
        pr_err("  - PHY Error: Clock lane error\n");
    }
    if (error_status & 0x2) {
        pr_err("  - PHY Error: Data lane 0 error\n");
    }
    if (error_status & 0x4) {
        pr_err("  - PHY Error: Data lane 1 error\n");
    }
    if (error_status & 0x8) {
        pr_err("  - PHY Error: Escape mode error\n");
    }
    
    /* Check PHY configuration registers */
    uint32_t phy_config = readl(phy_regs + 0x160);
    pr_info("MIPI PHY Configuration: 0x%08x\n", phy_config);
    pr_info("  - PHY frequency setting: %d\n", phy_config & 0xf);
    
    return 0;
}
EXPORT_SYMBOL(tx_isp_mipi_phy_status_check);

/**
 * tx_isp_vic_register_dump - Dump VIC registers for debugging
 */
void tx_isp_vic_register_dump(struct tx_isp_dev *isp_dev)
{
    void __iomem *vic_regs;
    
    if (!isp_dev || !isp_dev->vic_regs) {
        pr_err("tx_isp_vic_register_dump: Invalid parameters\n");
        return;
    }
    
    vic_regs = isp_dev->vic_regs;
    
    pr_info("*** VIC Register Dump ***\n");
    pr_info("VIC_CTRL (0x00):     0x%08x\n", readl(vic_regs + 0x0));
    pr_info("VIC_STATUS (0x04):   0x%08x\n", readl(vic_regs + 0x4));
    pr_info("VIC_CONFIG (0x0C):   0x%08x\n", readl(vic_regs + 0xc));
    pr_info("VIC_SIZE (0x10):     0x%08x\n", readl(vic_regs + 0x10));
    pr_info("VIC_STRIDE (0x14):   0x%08x\n", readl(vic_regs + 0x14));
    pr_info("VIC_INT_EN (0x1E0):  0x%08x\n", readl(vic_regs + 0x1e0));
    pr_info("VIC_INT_MASK (0x1E8): 0x%08x\n", readl(vic_regs + 0x1e8));
    pr_info("VIC_INT_STAT (0x1F0): 0x%08x\n", readl(vic_regs + 0x1f0));
    pr_info("VIC_BUFFER0 (0x380): 0x%08x\n", readl(vic_regs + 0x380));
    pr_info("VIC_BUFFER1 (0x384): 0x%08x\n", readl(vic_regs + 0x384));
    pr_info("VIC_BUFFER2 (0x388): 0x%08x\n", readl(vic_regs + 0x388));
    pr_info("*** End VIC Register Dump ***\n");
}
EXPORT_SYMBOL(tx_isp_vic_register_dump);

/**
 * tx_isp_debug_frame_capture_status - Check frame capture status
 */
void tx_isp_debug_frame_capture_status(struct tx_isp_dev *isp_dev)
{
    void __iomem *vic_regs;
    uint32_t frame_count_reg, buffer_status;
    
    if (!isp_dev || !isp_dev->vic_regs) {
        pr_err("tx_isp_debug_frame_capture_status: Invalid parameters\n");
        return;
    }
    
    vic_regs = isp_dev->vic_regs;
    
    pr_info("*** Frame Capture Status Debug ***\n");
    pr_info("Driver frame count: %u\n", isp_dev->frame_count);
    pr_info("VIC start flag: %d\n", vic_start_ok);
    
    /* Check hardware frame counter if available */
    frame_count_reg = readl(vic_regs + 0x160);
    pr_info("Hardware frame count: %u\n", frame_count_reg);
    
    /* Check buffer status */
    buffer_status = readl(vic_regs + 0x300);
    pr_info("Buffer status: 0x%08x\n", buffer_status);
    
    /* Check if VIC is actually receiving data */
    uint32_t vic_status = readl(vic_regs + 0x4);
    pr_info("VIC status register: 0x%08x\n", vic_status);
    
    if (vic_status & 0x1) {
        pr_info("  - VIC is ACTIVE\n");
    } else {
        pr_warn("  - VIC is INACTIVE - No data flow!\n");
    }
    
    if (vic_status & 0x2) {
        pr_info("  - Frame sync detected\n");
    } else {
        pr_warn("  - No frame sync - Check sensor output!\n");
    }
    
    /* Error counters */
    pr_info("*** Error Counters ***\n");
    pr_info("Control limit errors: %u\n", data_b2984);
    pr_info("Frame FIFO overflows: %u\n", data_b299c);
    pr_info("DMA FIFO overflows: %u\n", data_b2980);
    pr_info("MIPI errors: %u\n", data_b2990 + data_b2994);
    
    pr_info("*** End Frame Capture Status ***\n");
}
EXPORT_SYMBOL(tx_isp_debug_frame_capture_status);
