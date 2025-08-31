#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_vic.h"

extern struct tx_isp_dev *ourISPdev;

/* VIC frame completion handler */
static void tx_isp_vic_frame_done(struct tx_isp_subdev *sd, int channel)
{
    if (!sd || channel >= VIC_MAX_CHAN)
        return;

    complete(&sd->vic_frame_end_completion[channel]);
}

/* VIC interrupt handler */
static irqreturn_t tx_isp_vic_irq_handler(int irq, void *dev_id)
{
    struct tx_isp_subdev *sd = dev_id;
    struct vic_device *vic_dev;
    void __iomem *vic_base;
    u32 status, isr, isr1;
    unsigned long flags;
    irqreturn_t ret = IRQ_NONE;
    int i;

    if (!sd)
        return IRQ_NONE;

    vic_dev = (struct vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev)
        return IRQ_NONE;

    spin_lock_irqsave(&vic_dev->lock, flags);

    /* Map VIC registers directly for interrupt handling */
    vic_base = ioremap(0x10023000, 0x1000);
    if (!vic_base) {
        spin_unlock_irqrestore(&vic_dev->lock, flags);
        return IRQ_NONE;
    }

    /* Read interrupt status registers */
    isr = readl(vic_base + 0x00);   /* ISR register */
    isr1 = readl(vic_base + 0x20);  /* ISR1 register */

    if (isr || isr1) {
        ret = IRQ_HANDLED;
        
        /* Handle main ISR interrupts */
        if (isr) {
            /* Clear handled interrupts */
            writel(isr, vic_base + 0x00);
            wmb();
            
            /* Handle frame done interrupts */
            if (isr & 0x1) {
                /* Frame processing complete */
                complete(&vic_dev->frame_complete);
                tx_isp_vic_frame_done(sd, 0);
            }
            
            /* Handle DMA complete interrupts */
            if (isr & 0x2) {
                /* DMA transfer complete */
                pr_debug("VIC DMA transfer complete\n");
            }
            
            /* Handle buffer overflow */
            if (isr & 0x4) {
                ISP_ERROR("VIC buffer overflow interrupt\n");
            }
            
            /* Handle processing errors */
            if (isr & 0x8) {
                ISP_ERROR("VIC processing error interrupt\n");
            }
        }
        
        /* Handle secondary ISR1 interrupts */
        if (isr1) {
            /* Clear handled interrupts */
            writel(isr1, vic_base + 0x20);
            wmb();
            
            /* Handle channel-specific interrupts */
            for (i = 0; i < VIC_MAX_CHAN; i++) {
                if (isr1 & (1 << i)) {
                    tx_isp_vic_frame_done(sd, i);
                }
            }
            
            /* Handle error conditions */
            if (isr1 & 0x80000000) {
                ISP_ERROR("VIC critical error interrupt\n");
            }
        }
        
        /* Update VIC state based on interrupts */
        if ((isr & 0xC) || (isr1 & 0x80000000)) {
            /* Error condition - may need recovery */
            vic_dev->processing = false;
        } else if (isr & 0x3) {
            /* Normal completion */
            vic_dev->processing = false;
        }
    }

    iounmap(vic_base);
    spin_unlock_irqrestore(&vic_dev->lock, flags);
    return ret;
}

/* Initialize VIC hardware */
static int tx_isp_vic_hw_init(struct tx_isp_subdev *sd)
{
    u32 ctrl;
    void __iomem *vic_base;

//    /* Reset VIC */
//    vic_write32(VIC_CTRL, VIC_CTRL_RST);
//    udelay(10);
//    vic_write32(VIC_CTRL, 0);
//
//    /* Configure default settings */
//    ctrl = VIC_CTRL_EN;  /* Enable VIC */
//    vic_write32(VIC_CTRL, ctrl);
//
//    /* Clear and enable interrupts */
//    vic_write32(VIC_INT_STATUS, 0xFFFFFFFF);
//    vic_write32(VIC_INT_MASK, ~(INT_FRAME_DONE | INT_ERROR));


    // Initialize VIC hardware
    vic_base = ioremap(0x10023000, 0x1000);  // Direct map VIC

    // Clear any pending interrupts first
    writel(0, vic_base + 0x00);  // Clear ISR
    writel(0, vic_base + 0x20);  // Clear ISR1
    wmb();

    // Set up interrupt masks to match OEM
    writel(0x00000001, vic_base + 0x04);  // IMR
    wmb();
    writel(0x00000000, vic_base + 0x24);  // IMR1
    wmb();

    // Configure ISP control interrupts
    writel(0x07800438, vic_base + 0x04);  // IMR
    wmb();
    writel(0xb5742249, vic_base + 0x0c);  // IMCR
    wmb();

    return 0;
}

/* Stop VIC processing */
int tx_isp_vic_stop(struct tx_isp_subdev *sd)
{
    u32 ctrl;

    if (!sd || !sd->isp)
        return -EINVAL;

    mutex_lock(&sd->vic_frame_end_lock);

    /* Stop processing */
    ctrl = vic_read32(VIC_CTRL);
    ctrl &= ~VIC_CTRL_START;
    ctrl |= VIC_CTRL_STOP;
    vic_write32(VIC_CTRL, ctrl);

    /* Wait for stop to complete */
    while (vic_read32(VIC_STATUS) & STATUS_BUSY) {
        udelay(10);
    }

    mutex_unlock(&sd->vic_frame_end_lock);
    return 0;
}

/* Configure VIC frame buffer */
int tx_isp_vic_set_buffer(struct tx_isp_subdev *sd, dma_addr_t addr, u32 size)
{
    if (!sd || !sd->isp)
        return -EINVAL;

    mutex_lock(&sd->vic_frame_end_lock);

    /* Set frame buffer address and size */
    vic_write32(VIC_BUFFER_ADDR, addr);
    vic_write32(VIC_FRAME_SIZE, size);

    mutex_unlock(&sd->vic_frame_end_lock);
    return 0;
}

/* Wait for frame completion */
int tx_isp_vic_wait_frame_done(struct tx_isp_subdev *sd, int channel, int timeout_ms)
{
    int ret;

    if (!sd || channel >= VIC_MAX_CHAN)
        return -EINVAL;

    ret = wait_for_completion_timeout(
        &sd->vic_frame_end_completion[channel],
        msecs_to_jiffies(timeout_ms)
    );

    return ret ? 0 : -ETIMEDOUT;
}

int vic_saveraw(struct tx_isp_subdev *sd, unsigned int savenum)
{
    uint32_t vic_ctrl, vic_status, vic_intr, vic_addr;
    uint32_t width, height, frame_size, buf_size;
    struct tx_isp_dev *isp_dev = ourISPdev;
    void __iomem *vic_base;
    void *capture_buf;
    dma_addr_t dma_addr;
    unsigned long timeout;
    int i, ret = 0;
    struct file *fp;
    char filename[64];
    loff_t pos = 0;
    mm_segment_t old_fs;

    if (!sd || !isp_dev) {
        pr_err("No VIC or ISP device\n");
        return -EINVAL;
    }

    width = isp_dev->sensor_width;
    height = isp_dev->sensor_height;

    if (width >= 0xa81) {
        pr_err("Can't output the width(%d)!\n", width);
        return -EINVAL;
    }

    frame_size = width * height * 2;
    buf_size = frame_size * savenum;

    pr_info("width=%d height=%d frame_size=%d total_size=%d savenum=%d\n",
            width, height, frame_size, buf_size, savenum);

    vic_base = ioremap(0x10023000, 0x1000);
    if (!vic_base) {
        pr_err("Failed to map VIC registers\n");
        return -ENOMEM;
    }

    // Always allocate fresh buffer
    capture_buf = dma_alloc_coherent(sd->dev, buf_size, &dma_addr, GFP_KERNEL);
    if (!capture_buf) {
        pr_err("Failed to allocate DMA buffer\n");
        iounmap(vic_base);
        return -ENOMEM;
    }
    // Read original register values
    vic_ctrl = readl(vic_base + 0x7810);
    vic_status = readl(vic_base + 0x7814);
    vic_intr = readl(vic_base + 0x7804);
    vic_addr = readl(vic_base + 0x7820);

    // Different register configuration for saveraw
    writel(vic_ctrl & 0x11111111, vic_base + 0x7810);
    writel(0, vic_base + 0x7814);
    writel(vic_intr | 1, vic_base + 0x7804);

    writel(dma_addr, vic_base + 0x7820);
    writel(width * 2, vic_base + 0x7824);
    writel(height, vic_base + 0x7828);

    // Start capture
    writel(1, vic_base + 0x7800);

    timeout = jiffies + msecs_to_jiffies(600);
    while (time_before(jiffies, timeout)) {
        if (!(readl(vic_base + 0x7800) & 1)) {
            break;
        }
        usleep_range(1000, 2000);
    }

    if (readl(vic_base + 0x7800) & 1) {
        pr_err("VIC capture timeout!\n");
        ret = -ETIMEDOUT;
        goto cleanup;
    }

    // Save frames to files
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    for (i = 0; i < savenum; i++) {
        // Different filename format for saveraw
        snprintf(filename, sizeof(filename), "/tmp/vic_save_%d.raw", i);
        fp = filp_open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (IS_ERR(fp)) {
            pr_err("Failed to open file %s\n", filename);
            continue;
        }

        ret = vfs_write(fp, capture_buf + (i * frame_size), frame_size, &pos);
        if (ret != frame_size) {
            pr_err("Failed to write frame %d\n", i);
        }

        filp_close(fp, NULL);
        pos = 0;
    }

    set_fs(old_fs);

    // Restore registers
    writel(vic_ctrl & 0x11111111, vic_base + 0x7810);
    writel(vic_status, vic_base + 0x7814);
    writel(vic_intr | 1, vic_base + 0x7804);

cleanup:
    // Don't free the buffer - it stays allocated for future use
    iounmap(vic_base);
    return ret;
}

int vic_snapraw(struct tx_isp_subdev *sd, unsigned int savenum)
{
    uint32_t vic_ctrl, vic_status, vic_intr, vic_addr;
    uint32_t width, height, frame_size, buf_size;
    struct tx_isp_dev *isp_dev = ourISPdev;
    void __iomem *vic_base;
    void *capture_buf;
    dma_addr_t dma_addr;
    unsigned long timeout;
    int i, ret = 0;
    struct file *fp;
    char filename[64];
    loff_t pos = 0;
    mm_segment_t old_fs;

    if (!sd) {
        pr_err("No VIC or sensor device\n");
        return -EINVAL;
    }

    width = isp_dev->sensor_width;
    height = isp_dev->sensor_height;

    if (width >= 0xa81) {
        pr_err("Can't output the width(%d)!\n", width);
        return -EINVAL;
    }

    frame_size = width * height * 2;
    buf_size = frame_size * savenum;

    pr_info("width=%d height=%d frame_size=%d total_size=%d savenum=%d\n",
            width, height, frame_size, buf_size, savenum);

    // Map VIC registers
    vic_base = ioremap(0x10023000, 0x1000);
    if (!vic_base) {
        pr_err("Failed to map VIC registers\n");
        return -ENOMEM;
    }

    // Allocate DMA-able buffer
    capture_buf = dma_alloc_coherent(sd->dev, buf_size, &dma_addr, GFP_KERNEL);
    if (!capture_buf) {
        pr_err("Failed to allocate DMA buffer\n");
        iounmap(vic_base);
        return -ENOMEM;
    }

    // Read original register values
    vic_ctrl = readl(vic_base + 0x7810);
    vic_status = readl(vic_base + 0x7814);
    vic_intr = readl(vic_base + 0x7804);
    vic_addr = readl(vic_base + 0x7820);

    // Configure VIC registers for capture
    writel(vic_ctrl & 0x11110111, vic_base + 0x7810);
    writel(0, vic_base + 0x7814);
    writel(vic_intr | 1, vic_base + 0x7804);

    // Setup DMA
    writel(dma_addr, vic_base + 0x7820);  // DMA target address
    writel(width * 2, vic_base + 0x7824);  // Stride
    writel(height, vic_base + 0x7828);     // Height

    // Start capture
    writel(1, vic_base + 0x7800);  // Start DMA

    // Wait for completion with timeout
    timeout = jiffies + msecs_to_jiffies(600); // 600ms timeout
    while (time_before(jiffies, timeout)) {
        if (!(readl(vic_base + 0x7800) & 1)) {
            // DMA complete
            break;
        }
        usleep_range(1000, 2000);
    }

    if (readl(vic_base + 0x7800) & 1) {
        pr_err("VIC capture timeout!\n");
        ret = -ETIMEDOUT;
        goto cleanup;
    }

    // Save frames to files
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    for (i = 0; i < savenum; i++) {
        snprintf(filename, sizeof(filename), "/tmp/vic_frame_%d.raw", i);
        fp = filp_open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (IS_ERR(fp)) {
            pr_err("Failed to open file %s\n", filename);
            continue;
        }

        ret = vfs_write(fp, capture_buf + (i * frame_size), frame_size, &pos);
        if (ret != frame_size) {
            pr_err("Failed to write frame %d\n", i);
        }

        filp_close(fp, NULL);
        pos = 0;
    }

    set_fs(old_fs);

    // Restore original register values
    writel(vic_ctrl & 0x11111111, vic_base + 0x7810);
    writel(vic_status, vic_base + 0x7814);
    writel(vic_intr | 1, vic_base + 0x7804);

cleanup:
    dma_free_coherent(sd->dev, buf_size, capture_buf, dma_addr);
    iounmap(vic_base);
    return ret;
}

static ssize_t vic_proc_write(struct file *file, const char __user *buf,
                             size_t count, loff_t *ppos)
{
    struct tx_isp_subdev *sd = PDE_DATA(file->f_inode);
    char cmd[32];
    unsigned int savenum = 0;
    int ret;

    if (count >= sizeof(cmd))
        return -EINVAL;

    if (copy_from_user(cmd, buf, count))
        return -EFAULT;

    cmd[count] = '\0';
    cmd[count-1] = '\0'; // Remove trailing newline

    // Parse command format: "<cmd> <savenum>"
    ret = sscanf(cmd, "%s %u", cmd, &savenum);
    if (ret != 2) {
        pr_info("\t\t\t please use this cmd: \n");
        pr_info("\t\"echo snapraw savenum > /proc/jz/isp/isp-w02\"\n");
        pr_info("\t\"echo saveraw savenum > /proc/jz/isp/isp-w02\"\n");
        return count;
    }

    if (strcmp(cmd, "snapraw") == 0) {
        if (savenum < 2)
            savenum = 1;

        // Save raw frames
        ret = vic_snapraw(sd, savenum);
    }
    else if (strcmp(cmd, "saveraw") == 0) {
        if (savenum < 2)
            savenum = 1;

        // Save processed frames
        ret = vic_saveraw(sd, savenum);
    }
    else {
        pr_info("help:\n");
        pr_info("\t cmd:\n");
        pr_info("\t\t snapraw\n");
        pr_info("\t\t saveraw\n");
        pr_info("\t\t\t please use this cmd: \n");
        pr_info("\t\"echo cmd savenum > /proc/jz/isp/isp-w02\"\n");
    }

    return count;
}

/* Forward declarations */
int vic_core_ops_init(struct tx_isp_subdev *sd, int enable);

/* VIC activation function - matching reference driver */
int tx_isp_vic_activate_subdev(struct tx_isp_subdev *sd)
{
    struct vic_device *vic_dev;
    
    if (!sd)
        return -EINVAL;
    
    vic_dev = (struct vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev) {
        pr_err("VIC device is NULL\n");
        return -EINVAL;
    }
    
    mutex_lock(&vic_dev->state_lock);
    
    if (vic_dev->state == 1) {
        vic_dev->state = 2; /* INIT -> READY */
        pr_info("VIC activated: state %d -> 2 (READY)\n", 1);
    }
    
    mutex_unlock(&vic_dev->state_lock);
    return 0;
}

/* VIC core operations initialization - matching reference driver */
int vic_core_ops_init(struct tx_isp_subdev *sd, int enable)
{
    struct vic_device *vic_dev;
    int old_state;
    
    if (!sd)
        return -EINVAL;
    
    vic_dev = (struct vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev) {
        pr_err("VIC device is NULL\n");
        return -EINVAL;
    }
    
    old_state = vic_dev->state;
    
    if (enable) {
        /* Enable VIC processing */
        if (old_state != 3) {
            /* Enable VIC interrupts - placeholder register write */
            pr_info("VIC: Enabling interrupts (enable=%d)\n", enable);
            vic_dev->state = 3; /* READY -> ACTIVE */
        }
    } else {
        /* Disable VIC processing */
        if (old_state != 2) {
            /* Disable VIC interrupts - placeholder register write */
            pr_info("VIC: Disabling interrupts (enable=%d)\n", enable);
            vic_dev->state = 2; /* ACTIVE -> READY */
        }
    }
    
    pr_info("VIC core ops init: enable=%d, state %d -> %d\n",
            enable, old_state, vic_dev->state);
    
    return 0;
}

/* VIC slake function - matching reference driver */
int tx_isp_vic_slake_subdev(struct tx_isp_subdev *sd)
{
    struct vic_device *vic_dev;
    
    if (!sd)
        return -EINVAL;
        
    vic_dev = (struct vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev) {
        pr_err("VIC device is NULL\n");
        return -EINVAL;
    }
    
    mutex_lock(&vic_dev->state_lock);
    
    if (vic_dev->state > 1) {
        vic_dev->state = 1; /* Back to INIT state */
        pr_info("VIC slaked: state -> 1 (INIT)\n");
    }
    
    mutex_unlock(&vic_dev->state_lock);
    return 0;
}

/* VIC PIPO MDMA Enable function - EXACT Binary Ninja implementation */
static void vic_pipo_mdma_enable(struct vic_device *vic_dev)
{
    void __iomem *vic_base = vic_dev->vic_regs;
    u32 width = vic_dev->width;
    u32 height = vic_dev->height;
    u32 stride = width << 1;  /* width * 2 for stride */
    
    pr_info("*** VIC PIPO MDMA ENABLE - Binary Ninja exact sequence ***\n");
    pr_info("vic_pipo_mdma_enable: width=%d, height=%d, stride=%d\n", width, height, stride);
    
    /* Step 1: Enable MDMA - reg 0x308 = 1 */
    writel(1, vic_base + 0x308);
    wmb();
    pr_info("vic_pipo_mdma_enable: reg 0x308 = 1 (MDMA enable)\n");
    
    /* Step 2: Set dimensions - reg 0x304 = (width << 16) | height */
    writel((width << 16) | height, vic_base + 0x304);
    wmb();
    pr_info("vic_pipo_mdma_enable: reg 0x304 = 0x%x (dimensions %dx%d)\n", 
            (width << 16) | height, width, height);
    
    /* Step 3: Set stride - reg 0x310 = stride */
    writel(stride, vic_base + 0x310);
    wmb();
    pr_info("vic_pipo_mdma_enable: reg 0x310 = %d (stride)\n", stride);
    
    /* Step 4: Set stride again - reg 0x314 = stride */
    writel(stride, vic_base + 0x314);
    wmb();
    pr_info("vic_pipo_mdma_enable: reg 0x314 = %d (stride)\n", stride);
    
    pr_info("*** VIC PIPO MDMA ENABLE COMPLETE ***\n");
}

/* ISPVIC Frame Channel S_Stream - EXACT Binary Ninja implementation */
static int ispvic_frame_channel_s_stream(struct vic_device *vic_dev, int enable)
{
    void __iomem *vic_base = vic_dev->vic_regs;
    unsigned long flags;
    u32 stream_ctrl;
    
    pr_info("ispvic_frame_channel_s_stream: enable=%d\n", enable);
    
    if (enable == vic_dev->streaming) {
        pr_info("ispvic_frame_channel_s_stream: already in state %d\n", enable);
        return 0;
    }
    
    spin_lock_irqsave(&vic_dev->lock, flags);
    
    if (enable == 0) {
        /* Stream OFF - Binary Ninja: *(*($s0 + 0xb8) + 0x300) = 0 */
        pr_info("*** STREAM OFF: Setting reg 0x300 = 0 ***\n");
        writel(0, vic_base + 0x300);
        wmb();
        vic_dev->streaming = 0;
    } else {
        /* Stream ON - Binary Ninja: vic_pipo_mdma_enable($s0) FIRST */
        pr_info("*** STREAM ON: Calling vic_pipo_mdma_enable() FIRST ***\n");
        vic_pipo_mdma_enable(vic_dev);
        
        /* Then set stream control register - Binary Ninja: 
         * *(*($s0 + 0xb8) + 0x300) = *($s0 + 0x218) << 0x10 | 0x80000020 */
        stream_ctrl = (vic_dev->buffer_count << 16) | 0x80000020;
        pr_info("*** STREAM ON: Setting reg 0x300 = 0x%x ***\n", stream_ctrl);
        writel(stream_ctrl, vic_base + 0x300);
        wmb();
        vic_dev->streaming = 1;
    }
    
    spin_unlock_irqrestore(&vic_dev->lock, flags);
    return 0;
}

/* VIC event callback structure - matching reference driver layout */
struct vic_callback_struct {
    void *reserved[7];          /* Reserved fields 0x0-0x18 */
    void *event_callback;       /* Function pointer at offset 0x1c */
};

/* VIC event callback handler for QBUF events */
static int vic_pad_event_callback(struct tx_isp_subdev_pad *pad, unsigned int cmd, void *data)
{
    struct tx_isp_subdev *sd;
    struct vic_device *vic_dev;
    int ret = 0;
    
    if (!pad || !pad->sd) {
        pr_err("VIC event callback: Invalid pad or subdev\n");
        return -EINVAL;
    }
    
    sd = pad->sd;
    vic_dev = (struct vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev) {
        pr_err("VIC event callback: No vic_dev\n");
        return -EINVAL;
    }
    
    pr_info("*** VIC EVENT CALLBACK: cmd=0x%x, data=%p ***\n", cmd, data);
    
    switch (cmd) {
        case 0x3000008: /* QBUF event */
            pr_info("*** VIC: Processing QBUF event 0x3000008 ***\n");
            
            /* Handle QBUF event - trigger frame processing */
            if (vic_dev->state == 4) { /* Streaming state */
                /* Signal frame completion to wake up waiting processes */
                complete(&vic_dev->frame_complete);
                pr_info("*** VIC: QBUF event processed - frame completion signaled ***\n");
                ret = 0;
            } else {
                pr_info("VIC: QBUF event received but not streaming (state=%d) - allowing anyway\n", vic_dev->state);
                complete(&vic_dev->frame_complete);
                ret = 0;
            }
            break;
            
        default:
            pr_info("VIC: Unknown event cmd=0x%x\n", cmd);
            ret = -ENOIOCTLCMD;
            break;
    }
    
    pr_info("*** VIC EVENT CALLBACK: returning %d ***\n", ret);
    return ret;
}

/* VIC video streaming operations - matching reference driver vic_core_s_stream */
static int vic_video_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct vic_device *vic_dev;
    int ret = 0;
    
    if (!sd) {
        pr_err("VIC s_stream: NULL subdev\n");
        return -EINVAL;
    }
    
    vic_dev = (struct vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev) {
        pr_err("VIC s_stream: NULL vic_dev\n");
        return -EINVAL;
    }
    
    pr_info("VIC s_stream: enable=%d, current_state=%d\n", enable, vic_dev->state);
    
    mutex_lock(&vic_dev->state_lock);
    
    if (enable) {
        /* Start VIC streaming - Call Binary Ninja exact sequence */
        if (vic_dev->state != 4) { /* Not already streaming */
            pr_info("VIC: Starting streaming - calling ispvic_frame_channel_s_stream(1)\n");
            ret = ispvic_frame_channel_s_stream(vic_dev, 1);
            if (ret == 0) {
                vic_dev->state = 4; /* STREAMING state */
                pr_info("VIC: Streaming started successfully, state -> 4\n");
            } else {
                pr_err("VIC: ispvic_frame_channel_s_stream failed: %d\n", ret);
            }
        } else {
            pr_info("VIC: Already streaming (state=%d)\n", vic_dev->state);
        }
    } else {
        /* Stop VIC streaming */
        if (vic_dev->state == 4) { /* Currently streaming */
            pr_info("VIC: Stopping streaming - calling ispvic_frame_channel_s_stream(0)\n");
            ret = ispvic_frame_channel_s_stream(vic_dev, 0);
            if (ret == 0) {
                vic_dev->state = 3; /* ACTIVE but not streaming */
                pr_info("VIC: Streaming stopped, state -> 3\n");
            }
        } else {
            pr_info("VIC: Not streaming (state=%d)\n", vic_dev->state);
        }
    }
    
    mutex_unlock(&vic_dev->state_lock);
    return ret;
}

/* Define VIC video operations */
static struct tx_isp_subdev_video_ops vic_video_ops = {
    .s_stream = vic_video_s_stream,
};

/* Define the core operations */
static struct tx_isp_subdev_core_ops vic_core_ops = {
    .init = vic_core_ops_init,
};

/* Initialize the subdev ops structure with video operations */
struct tx_isp_subdev_ops vic_subdev_ops = {
    .core = &vic_core_ops,
    .video = &vic_video_ops,    /* NOW VIC HAS VIDEO STREAMING! */
    .sensor = NULL,             /* No sensor ops for VIC */
};
EXPORT_SYMBOL(vic_subdev_ops);


static const struct file_operations isp_w02_proc_fops = {
    .owner = THIS_MODULE,
    .open = vic_chardev_open,
    .release = vic_chardev_release,
    .write = vic_proc_write
//    .unlocked_ioctl = vic_chardev_ioctl,
//    .compat_ioctl = vic_chardev_ioctl  // For 32-bit compatibility
};

/* Implementation of the open/release functions */
/* Implementation of the open/release functions */
int vic_chardev_open(struct inode *inode, struct file *file)
{
    struct tx_isp_subdev *sd = PDE_DATA(inode);  // Get data set during proc_create_data

    pr_info("VIC device open called from pid %d\n", current->pid);
    file->private_data = sd;

    return 0;
}
EXPORT_SYMBOL(vic_chardev_open);

int vic_chardev_release(struct inode *inode, struct file *file)
{
    struct tx_isp_subdev *sd = file->private_data;

    if (!sd) {
        return -EINVAL;
    }

    file->private_data = NULL;
    pr_info("VIC device released\n");
    return 0;
}
EXPORT_SYMBOL(vic_chardev_release);


long vic_chardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct tx_isp_subdev *sd = file->private_data;
    int ret = 0;

    pr_info("VIC IOCTL called: cmd=0x%x arg=0x%lx\n", cmd, arg);

    if (!sd) {
        pr_err("VIC: no private data in file\n");
        return -EINVAL;
    }

//    switch (cmd) {
//        case 0x1000000: { // VIC_IOCTL_CMD1:
//            if (!sd->ops || !sd->ops->core_ops) {
//                return 0;
//            }
//            ret = sd->ops->core_ops->ioctl(sd, cmd);
//            if (ret == -515) { // 0xfffffdfd
//                return 0;
//            }
//            break;
//		}
//        case 0x1000001: { // VIC_IOCTL_CMD2:
//            if (!sd->ops || !sd->ops->core_ops) {
//                return -EINVAL;
//            }
//            ret = sd->ops->core_ops->ioctl(sd, cmd);
//            if (ret == -515) { // 0xfffffdfd
//                return 0;
//            }
//            break;
//		}
//        case 0x3000009: { // 0x3000009
//            ret = tx_isp_subdev_pipo(sd, (void __user *)arg);
//            if (ret == -515) { // 0xfffffdfd
//                return 0;
//            }
//            break;
//		}
//        default:
//            return 0;
//    }

    return ret;
}
EXPORT_SYMBOL(vic_chardev_ioctl);

int tx_isp_vic_probe(struct platform_device *pdev)
{
    struct tx_isp_subdev *sd = NULL;
    struct tx_isp_subdev_ops *ops = &vic_subdev_ops;
    struct resource *res = NULL;
    struct proc_dir_entry *isp_dir;
    struct proc_dir_entry *w02_entry;
    int ret = 0;
    int i;

    pr_info("tx_isp_vic_probe\n");

    if (!pdev) {
        pr_err("NULL platform device\n");
        return -EINVAL;
    }

    // Allocate subdev first
    sd = kzalloc(sizeof(struct tx_isp_subdev), GFP_KERNEL);
    if (!sd) {
        pr_err("Failed to allocate VIC subdev\n");
        return -ENOMEM;
    }

    // Get memory resource before subdev init
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        pr_err("No memory resource for VIC\n");
        ret = -ENODEV;
        goto err_free_sd;
    }

    /* Request and map VIC memory region */
    if (!request_mem_region(res->start, resource_size(res), dev_name(&pdev->dev))) {
        pr_err("Failed to request VIC memory region\n");
        ret = -EBUSY;
        goto err_free_sd;
    }

    /* Map VIC registers */
    sd->base = ioremap(res->start, resource_size(res));
    if (!sd->base) {
        pr_err("Failed to map VIC registers\n");
        ret = -ENOMEM;
        goto err_release_mem;
    }

    /* Allocate and initialize VIC device structure */
    struct vic_device *vic_dev = kzalloc(sizeof(struct vic_device), GFP_KERNEL);
    if (!vic_dev) {
        pr_err("Failed to allocate VIC device structure\n");
        ret = -ENOMEM;
        goto err_unmap;
    }

    /* Initialize VIC device structure - Binary Ninja compatible */
    vic_dev->state = 1;              /* INIT state */
    vic_dev->vic_regs = sd->base;    /* VIC register base address */
    vic_dev->width = 1920;           /* Default width (will be updated by sensor config) */
    vic_dev->height = 1080;          /* Default height (will be updated by sensor config) */
    vic_dev->buffer_count = 4;       /* Number of frame buffers */
    vic_dev->streaming = 0;          /* Not streaming initially */
    vic_dev->processing = false;     /* Not processing initially */
    
    /* Initialize synchronization objects */
    mutex_init(&vic_dev->state_lock);
    spin_lock_init(&vic_dev->lock);
    init_completion(&vic_dev->frame_complete);
    
    pr_info("VIC device initialized: width=%d, height=%d, buffers=%d, regs=%p\n",
            vic_dev->width, vic_dev->height, vic_dev->buffer_count, vic_dev->vic_regs);
    
    /* Link VIC device to subdev */
    tx_isp_set_subdevdata(sd, vic_dev);

    /* Initialize VIC specific locks */
    spin_lock_init(&sd->vic_lock);
    mutex_init(&sd->vic_frame_end_lock);
    for (i = 0; i < VIC_MAX_CHAN; i++) {
        init_completion(&sd->vic_frame_end_completion[i]);
    }

    // Store initial platform data
    platform_set_drvdata(pdev, sd);

    /* Now initialize the subdev */
    ret = tx_isp_subdev_init(pdev, sd, ops);
    if (ret < 0) {
        pr_err("Failed to initialize VIC subdev\n");
        goto err_free_vic_dev;
    }

    /* Set up VIC event callback structure matching reference driver */
    pr_info("*** VIC: Setting up event callback structure for input pad ***\n");
    if (sd->num_inpads > 0 && sd->inpads) {
        /* Allocate callback structure matching reference driver layout */
        struct vic_callback_struct *callback = kzalloc(sizeof(struct vic_callback_struct), GFP_KERNEL);
        if (!callback) {
            pr_err("VIC: Failed to allocate callback structure\n");
            ret = -ENOMEM;
            goto err_deinit_sd;
        }
        
        /* Set function pointer at offset 0x1c (callback_struct + 0x1c) */
        callback->event_callback = vic_pad_event_callback;
        
        /* Store callback structure pointer at pad+0xc as expected by reference driver */
        sd->inpads[0].callback_data = callback;  /* This will be at pad+0xc */
        
        pr_info("*** VIC: Event callback structure set up - callback=%p, function=%p ***\n", 
                callback, vic_pad_event_callback);
        pr_info("*** VIC: Pad callback_data at %p (pad+0xc equivalent) ***\n", 
                &sd->inpads[0].callback_data);
    } else {
        pr_err("VIC: No input pads available for event callback\n");
        ret = -EINVAL;
        goto err_deinit_sd;
    }

    /* Initialize hardware after subdev init */
    ret = tx_isp_vic_hw_init(sd);
    if (ret < 0) {
        pr_err("Failed to initialize VIC hardware\n");
        goto err_deinit_sd;
    }

    /* Request interrupt */
    ret = platform_get_irq(pdev, 0);
    if (ret < 0) {
        pr_err("No IRQ specified for VIC\n");
        goto err_deinit_sd;
    }

    ret = request_irq(ret, tx_isp_vic_irq_handler, IRQF_SHARED,
                     dev_name(&pdev->dev), sd);
    if (ret) {
        pr_err("Failed to request VIC IRQ\n");
        goto err_deinit_sd;
    }

    /* Create /proc/jz/isp/isp-w02 */
    isp_dir = proc_mkdir("jz/isp", NULL);
    if (!isp_dir) {
        pr_err("Failed to create /proc/jz/isp directory\n");
        goto err_deinit_sd;
    }

    w02_entry = proc_create_data("isp-w02", 0666, isp_dir, &isp_w02_proc_fops, sd);
    if (!w02_entry) {
        pr_err("Failed to create /proc/jz/isp/isp-w02\n");
        remove_proc_entry("jz/isp", NULL);
        goto err_deinit_sd;
    }

    pr_info("VIC probe completed successfully\n");
    return 0;

err_deinit_sd:
    tx_isp_subdev_deinit(sd);
err_free_vic_dev:
    kfree(vic_dev);
err_unmap:
    iounmap(sd->base);
err_release_mem:
    release_mem_region(res->start, resource_size(res));
err_free_sd:
    kfree(sd);
    return ret;
}

/* VIC remove function */
int tx_isp_vic_remove(struct platform_device *pdev)
{
    struct tx_isp_subdev *sd = platform_get_drvdata(pdev);
    struct resource *res;

    if (!sd)
        return -EINVAL;

    /* Stop VIC */
    tx_isp_vic_stop(sd);

    /* Free interrupt */
    free_irq(platform_get_irq(pdev, 0), sd);

    remove_proc_entry("isp-w02", NULL);
    remove_proc_entry("jz/isp", NULL);

    /* Unmap and release memory */
    iounmap(sd->base);
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (res)
        release_mem_region(res->start, resource_size(res));

    /* Clean up subdev */
    tx_isp_subdev_deinit(sd);
    kfree(sd);

    return 0;
}
/* Export symbols for use by other parts of the driver */
EXPORT_SYMBOL(tx_isp_vic_stop);
EXPORT_SYMBOL(tx_isp_vic_set_buffer);
EXPORT_SYMBOL(tx_isp_vic_wait_frame_done);
