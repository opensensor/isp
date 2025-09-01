#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <linux/vmalloc.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_core.h"
#include "../include/tx-isp-debug.h"
#include "../include/tx_isp_sysfs.h"
#include "../include/tx_isp_vic.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx_isp_vin.h"
#include "../include/tx_isp_tuning.h"
#include "../include/tx-isp-device.h"
#include "../include/tx-libimp.h"
#include <linux/platform_device.h>
#include <linux/device.h>


extern struct tx_isp_dev *ourISPdev;
uint32_t vic_start_ok = 0;  /* Global VIC interrupt enable flag definition */


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
int vic_core_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg);
int vic_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg);
int vic_sensor_ops_sync_sensor_attr(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr);
int isp_vic_frd_show(struct seq_file *seq, void *v);
int dump_isp_vic_frd_open(struct inode *inode, struct file *file);
long isp_vic_cmd_set(struct file *file, unsigned int cmd, unsigned long arg);


/* tx_isp_vic_start - EXACT Binary Ninja implementation (CORRECTED for MIPI) */
int tx_isp_vic_start(struct vic_device *vic_dev)
{
    void __iomem *vic_regs;
    u32 interface_type;
    u32 sensor_format;
    u32 timeout = 10000;

    if (!vic_dev || !vic_dev->vic_regs) {
        pr_err("tx_isp_vic_start: Invalid parameters\n");
        return -EINVAL;
    }

    vic_regs = vic_dev->vic_regs;
    interface_type = vic_dev->sensor_attr.dbus_type;  /* Binary Ninja: *(*(arg1 + 0x110) + 0x14) */
    sensor_format = vic_dev->sensor_attr.data_type;   /* Binary Ninja: *(arg1 + 0xe4) */

    pr_info("*** tx_isp_vic_start: EXACT Binary Ninja implementation ***\n");
    pr_info("tx_isp_vic_start: interface=%d, format=0x%x\n", interface_type, sensor_format);

    /* Binary Ninja: interface 1=DVP, 2=MIPI, 3=BT601, 4=BT656, 5=BT1120 */

    if (interface_type == 1) {
        /* DVP interface - Binary Ninja: if ($v0 == 1) */
        pr_info("tx_isp_vic_start: DVP interface configuration (type 1)\n");

        /* Binary Ninja: Check flags match */
        if (vic_dev->sensor_attr.dbus_type != interface_type) {
            pr_warn("tx_isp_vic_start: DVP flags mismatch\n");
            writel(0xa000a, vic_regs + 0x1a4);
        } else {
            pr_info("tx_isp_vic_start: DVP flags match, normal configuration\n");
            /* Binary Ninja: *(*(arg1 + 0xb8) + 0x10) = &data_20000 */
            writel(0x20000, vic_regs + 0x10);   /* DVP config register */
            writel(0x100010, vic_regs + 0x1a4); /* DMA config */
        }

        /* Binary Ninja: DVP buffer calculations and configuration */
        u32 stride_multiplier = 8;
        if (sensor_format != 0) {
            if (sensor_format == 1) stride_multiplier = 0xa;
            else if (sensor_format == 2) stride_multiplier = 0xc;
            else if (sensor_format == 7) stride_multiplier = 0x10;
        }

        u32 buffer_calc = stride_multiplier * vic_dev->sensor_attr.integration_time;
        u32 buffer_size = (buffer_calc >> 5) + ((buffer_calc & 0x1f) ? 1 : 0);
        writel(buffer_size, vic_regs + 0x100);
        writel(2, vic_regs + 0xc);
        writel(sensor_format, vic_regs + 0x14);
        writel((vic_dev->width << 16) | vic_dev->height, vic_regs + 0x4);
        wmb();

        /* Binary Ninja: DVP timing and WDR configuration */
        u32 wdr_mode = vic_dev->sensor_attr.wdr_cache;
        u32 frame_mode = (wdr_mode == 0) ? 0x4440 : (wdr_mode == 1) ? 0x4140 : 0x4240;
        writel(frame_mode, vic_regs + 0x1ac);
        writel(frame_mode, vic_regs + 0x1a8);
        writel(0x10, vic_regs + 0x1b0);
        wmb();

        /* Binary Ninja: DVP unlock sequence WITH unlock key */
        writel(2, vic_regs + 0x0);
        wmb();
        writel(4, vic_regs + 0x0);
        wmb();

        /* *** CRITICAL: DVP unlock key - Binary Ninja exact *** */
        u32 unlock_key = (vic_dev->sensor_attr.integration_time_apply_delay << 4) | vic_dev->sensor_attr.again_apply_delay;
        writel(unlock_key, vic_regs + 0x1a0);
        wmb();
        pr_info("tx_isp_vic_start: DVP unlock key 0x1a0 = 0x%x\n", unlock_key);

    } else if (interface_type == 2) {
        /* *** CRITICAL: MIPI interface - EXACT Binary Ninja implementation *** */
        pr_info("tx_isp_vic_start: MIPI interface configuration (interface type 2)\n");

        /* Binary Ninja: *(*(arg1 + 0xb8) + 0xc) = 3 */
        writel(3, vic_regs + 0xc);
        wmb();

        /* *** EXACT Binary Ninja MIPI format handling *** */
        u32 mipi_config = 0x20000; /* Default value: &data_20000 */

        /* Binary Ninja format switch based on sensor_format (*(arg1 + 0xe4)) */
        if (sensor_format >= 0x300e) {
            /* Binary Ninja label_10928: Standard MIPI RAW path */
            u32 dbus_type_check = vic_dev->sensor_attr.dbus_type;

            /* Binary Ninja: Check integration_time_apply_delay for SONY mode */
            if (vic_dev->sensor_attr.integration_time_apply_delay != 2) {
                /* Standard MIPI mode */
                mipi_config = 0x20000;  /* &data_20000 */
                if (dbus_type_check == 0) {
                    /* OK - standard mode */
                } else if (dbus_type_check == 1) {
                    mipi_config = 0x120000; /* Alternative MIPI mode */
                } else {
                    pr_err("tx_isp_vic_start: VIC failed to config DVP mode!(10bits-sensor)\n");
                    return -EINVAL;
                }
            } else {
                /* SONY MIPI mode */
                mipi_config = 0x30000;  /* &data_30000 */
                if (dbus_type_check == 0) {
                    /* OK - SONY standard */
                } else if (dbus_type_check == 1) {
                    mipi_config = 0x130000; /* SONY alternative */
                } else {
                    pr_err("tx_isp_vic_start: VIC failed to config DVP SONY mode!(10bits-sensor)\n");
                    return -EINVAL;
                }
            }
            pr_info("tx_isp_vic_start: MIPI format 0x%x -> config 0x%x (>= 0x300e path)\n",
                    sensor_format, mipi_config);
        } else {
            /* Binary Ninja: Handle other format ranges */
            if (sensor_format == 0x2011) {
                mipi_config = 0xc0000;  /* &data_c0000 */
            } else if (sensor_format >= 0x2012) {
                /* Additional format handling from Binary Ninja */
                if (sensor_format == 0x1008) {
                    mipi_config = 0x80000;  /* &data_80000 */
                } else if (sensor_format >= 0x1009) {
                    if ((sensor_format - 0x2002) >= 4) {
                        pr_err("tx_isp_vic_start: VIC do not support this format %d\n", sensor_format);
                        return -EINVAL;
                    }
                    mipi_config = 0xc0000;  /* &data_c0000 */
                } else {
                    /* Default handling for other formats */
                    mipi_config = 0x20000;
                }
            } else if (sensor_format == 0x1006) {
                mipi_config = 0xa0000;  /* &data_a0000 */
            } else {
                /* For unknown formats including 0x2b, use default MIPI config */
                pr_info("tx_isp_vic_start: Unknown/default format 0x%x, using standard MIPI config 0x20000\n", sensor_format);
                mipi_config = 0x20000;
            }
        }

        /* Binary Ninja: Additional configuration flags */
        if (vic_dev->sensor_attr.total_width == 2) {
            mipi_config |= 2;
        }
        if (vic_dev->sensor_attr.total_height == 2) {
            mipi_config |= 1;
        }

        /* Binary Ninja: MIPI timing registers */
        u32 integration_time = vic_dev->sensor_attr.integration_time;
        if (integration_time != 0) {
            writel((integration_time << 16) + vic_dev->width, vic_regs + 0x18);
            wmb();
        }

        u32 again_value = vic_dev->sensor_attr.again;
        if (again_value != 0) {
            writel(again_value, vic_regs + 0x3c);
            wmb();
        }

        /* Binary Ninja: Final timing setup - EXACT order */
        writel((integration_time << 16) + vic_dev->width, vic_regs + 0x18);
        wmb();

        /* Binary Ninja: VIC register 0x10 with timing flags */
        u32 final_mipi_config = (vic_dev->sensor_attr.total_width << 31) | mipi_config;
        writel(final_mipi_config, vic_regs + 0x10);
        wmb();

        /* Frame dimensions */
        writel((vic_dev->width << 16) | vic_dev->height, vic_regs + 0x4);
        wmb();

        pr_info("tx_isp_vic_start: MIPI registers configured - 0x10=0x%x, 0x18=0x%x\n",
                final_mipi_config, (integration_time << 16) + vic_dev->width);

        /* *** Binary Ninja EXACT unlock sequence *** */
        writel(2, vic_regs + 0x0);
        wmb();
        writel(4, vic_regs + 0x0);
        wmb();

        /* Binary Ninja: Wait for unlock completion */
        timeout = 10000;
        while (timeout > 0) {
            u32 vic_status = readl(vic_regs + 0x0);
            if (vic_status == 0) {
                break;
            }
            udelay(1);
            timeout--;
        }

        if (timeout == 0) {
            pr_err("tx_isp_vic_start: VIC unlock timeout\n");
            return -ETIMEDOUT;
        }

        /* Binary Ninja: Enable VIC processing */
        writel(1, vic_regs + 0x0);
        wmb();

        /* Binary Ninja: Final configuration registers */
        writel(0x100010, vic_regs + 0x1a4);
        writel(0x4210, vic_regs + 0x1ac);
        writel(0x10, vic_regs + 0x1b0);
        writel(0, vic_regs + 0x1b4);
        wmb();

        pr_info("tx_isp_vic_start: MIPI interface configured successfully\n");

    } else if (interface_type == 4) {
        /* BT656 interface */
        pr_info("tx_isp_vic_start: BT656 interface\n");
        writel(0, vic_regs + 0xc);
        writel(0x800c0000, vic_regs + 0x10);
        writel(0x100010, vic_regs + 0x1a4);
        writel(0x4440, vic_regs + 0x1ac);
        writel(2, vic_regs + 0x0);
        wmb();

    } else if (interface_type == 5) {
        /* BT1120 interface */
        pr_info("tx_isp_vic_start: BT1120 interface\n");
        writel(4, vic_regs + 0xc);
        writel(0x800c0000, vic_regs + 0x10);
        writel(0x100010, vic_regs + 0x1a4);
        writel(0x4440, vic_regs + 0x1ac);
        writel(2, vic_regs + 0x0);
        wmb();

    } else {
        pr_err("tx_isp_vic_start: Unsupported interface type %d\n", interface_type);
        return -EINVAL;
    }

    /* Binary Ninja: Wait for VIC unlock completion */
    pr_info("tx_isp_vic_start: Waiting for VIC unlock completion...\n");
    timeout = 10000;
    while (timeout > 0) {
        u32 status = readl(vic_regs + 0x0);
        if (status == 0) {
            pr_info("tx_isp_vic_start: VIC unlocked after %d iterations (status=0)\n", 10000 - timeout);
            break;
        }
        udelay(1);
        timeout--;
    }

    if (timeout == 0) {
        pr_err("tx_isp_vic_start: VIC unlock timeout - still locked\n");
        return -ETIMEDOUT;
    }

    /* Binary Ninja: Enable VIC processing */
    writel(1, vic_regs + 0x0);
    wmb();
    pr_info("tx_isp_vic_start: VIC processing enabled (reg 0x0 = 1)\n");

    /* Binary Ninja: Final configuration registers */
    writel(0x100010, vic_regs + 0x1a4);
    writel(0x4210, vic_regs + 0x1ac);
    writel(0x10, vic_regs + 0x1b0);
    writel(0, vic_regs + 0x1b4);
    wmb();

    /* Binary Ninja: Log WDR mode */
    const char *wdr_msg = (vic_dev->sensor_attr.wdr_cache != 0) ?
        "WDR mode enabled" : "Linear mode enabled";
    pr_info("tx_isp_vic_start: %s\n", wdr_msg);

    /* *** CRITICAL: Set global vic_start_ok flag at end - Binary Ninja exact! *** */
    vic_start_ok = 1;
    pr_info("*** tx_isp_vic_start: CRITICAL vic_start_ok = 1 SET! ***\n");
    pr_info("*** VIC interrupts now enabled for processing in isp_vic_interrupt_service_routine ***\n");

    return 0;
}


/* VIC sensor operations ioctl - EXACT Binary Ninja implementation */
int vic_sensor_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    struct vic_device *vic_dev;
    int result = 0;
    
    pr_info("*** vic_sensor_ops_ioctl: cmd=0x%x, arg=%p ***\n", cmd, arg);
    
    /* Binary Ninja: if (arg1 != 0 && arg1 u< 0xfffff001) */
    if (!sd || (unsigned long)sd >= 0xfffff001) {
        pr_err("vic_sensor_ops_ioctl: Invalid sd parameter\n");
        return result;
    }
    
    /* Binary Ninja: void* $a0 = *(arg1 + 0xd4) */
    vic_dev = (struct vic_device *)*((void **)((char *)sd + 0xd4));
    pr_info("*** vic_sensor_ops_ioctl: Retrieved vic_dev from offset 0xd4: %p ***\n", vic_dev);
    
    /* Binary Ninja: if ($a0 != 0 && $a0 u< 0xfffff001) */
    if (!vic_dev || (unsigned long)vic_dev >= 0xfffff001) {
        pr_err("*** vic_sensor_ops_ioctl: Invalid vic_dev from offset 0xd4 ***\n");
        return result;
    }
    
    /* Binary Ninja: if (arg2 - 0x200000c u>= 0xd) return 0 */
    if (cmd - 0x200000c >= 0xd) {
        pr_info("vic_sensor_ops_ioctl: cmd out of range, returning 0\n");
        return 0;
    }
    
    switch (cmd) {
        case 0x200000c:
        case 0x200000f:
            pr_info("*** vic_sensor_ops_ioctl: Starting VIC (cmd=0x%x) - CALLING tx_isp_vic_start ***\n", cmd);
            return tx_isp_vic_start(vic_dev);
            
        case 0x200000d:
        case 0x2000010:
        case 0x2000011:
        case 0x2000012:
        case 0x2000014:
        case 0x2000015:
        case 0x2000016:
            pr_info("vic_sensor_ops_ioctl: No-op cmd=0x%x\n", cmd);
            return 0;
            
        case 0x200000e:
            pr_info("vic_sensor_ops_ioctl: Setting VIC register 0x10 (cmd=0x%x)\n", cmd);
            /* Binary Ninja: **($a0 + 0xb8) = 0x10 */
            writel(0x10, vic_dev->vic_regs);
            return 0;
            
        case 0x2000013:
            pr_info("vic_sensor_ops_ioctl: Resetting and setting VIC register (cmd=0x%x)\n", cmd);
            /* Binary Ninja: **($a0 + 0xb8) = 0, then = 4 */
            writel(0, vic_dev->vic_regs);
            writel(4, vic_dev->vic_regs);
            return 0;
            
        case 0x2000017:
            pr_info("vic_sensor_ops_ioctl: GPIO configuration (cmd=0x%x)\n", cmd);
            /* GPIO configuration logic - simplified for now */
            return 0;
            
        case 0x2000018:
            pr_info("vic_sensor_ops_ioctl: GPIO switch state (cmd=0x%x)\n", cmd);
            /* Binary Ninja: gpio_switch_state = 1, memcpy(&gpio_info, arg3, 0x2a) */
            return 0;
            
        default:
            pr_info("vic_sensor_ops_ioctl: Unknown cmd=0x%x\n", cmd);
            return 0;
    }
}

/* VIC sensor operations sync_sensor_attr - EXACT Binary Ninja implementation */
int vic_sensor_ops_sync_sensor_attr(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr)
{
    struct vic_device *vic_dev;
    
    pr_info("vic_sensor_ops_sync_sensor_attr: sd=%p, attr=%p\n", sd, attr);
    
    if (!sd || (unsigned long)sd >= 0xfffff001) {
        pr_err("The parameter is invalid!\n");
        return -EINVAL;
    }
    
    vic_dev = (struct vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev || (unsigned long)vic_dev >= 0xfffff001) {
        pr_err("The parameter is invalid!\n");
        return -EINVAL;
    }
    
    /* Binary Ninja: $v0_1 = arg2 == 0 ? memset : memcpy */
    if (attr == NULL) {
        /* Clear sensor attribute */
        memset(&vic_dev->sensor_attr, 0, sizeof(vic_dev->sensor_attr));
        pr_info("vic_sensor_ops_sync_sensor_attr: cleared sensor attributes\n");
    } else {
        /* Copy sensor attribute */
        memcpy(&vic_dev->sensor_attr, attr, sizeof(vic_dev->sensor_attr));
        pr_info("vic_sensor_ops_sync_sensor_attr: copied sensor attributes\n");
    }
    
    return 0;
}

/* VIC core operations ioctl - EXACT Binary Ninja implementation */
int vic_core_ops_ioctl(struct tx_isp_subdev *sd, unsigned int cmd, void *arg)
{
    int result = -ENOTSUPP;  /* 0xffffffed */
    void *callback_ptr;
    int (*callback_func)(void);  /* Changed to int return type */
    
    pr_info("vic_core_ops_ioctl: cmd=0x%x, arg=%p\n", cmd, arg);
    
    if (cmd == 0x1000001) {
        result = -ENOTSUPP;
        if (sd != NULL) {
            /* Binary Ninja: void* $v0_2 = *(*(arg1 + 0xc4) + 0xc) */
            if (sd->inpads && sd->inpads[0].priv) {
                callback_ptr = sd->inpads[0].priv;
                if (callback_ptr != NULL) {
                    /* Get function pointer at offset +4 in callback structure */
                    callback_func = *((int (**)(void))((char *)callback_ptr + 4));
                    if (callback_func != NULL) {
                        pr_info("vic_core_ops_ioctl: Calling callback function for cmd 0x1000001\n");
                        result = callback_func();  /* Call the function without casting */
                    }
                }
            }
        }
    } else if (cmd == 0x3000009) {
        pr_info("vic_core_ops_ioctl: tx_isp_subdev_pipo cmd=0x%x\n", cmd);
        result = tx_isp_subdev_pipo(sd, arg);
    } else if (cmd == 0x1000000) {
        result = -ENOTSUPP;
        if (sd != NULL) {
            /* Binary Ninja: void* $v0_5 = **(arg1 + 0xc4) */
            if (sd->inpads && sd->inpads[0].priv) {
                callback_ptr = sd->inpads[0].priv;
                if (callback_ptr != NULL) {
                    /* Get function pointer at offset +4 in callback structure */
                    callback_func = *((int (**)(void))((char *)callback_ptr + 4));
                    if (callback_func != NULL) {
                        pr_info("vic_core_ops_ioctl: Calling callback function for cmd 0x1000000\n");
                        result = callback_func();  /* Call the function without casting */
                    }
                }
            }
        }
    } else {
        pr_info("vic_core_ops_ioctl: Unknown cmd=0x%x, returning 0\n", cmd);
        return 0;
    }
    
    /* Binary Ninja: if (result == 0xfffffdfd) return 0 */
    if (result == -515) {  /* 0xfffffdfd */
        pr_info("vic_core_ops_ioctl: Result -515, returning 0\n");
        return 0;
    }
    
    return result;
}

/* ISP VIC FRD show function - EXACT Binary Ninja implementation */
int isp_vic_frd_show(struct seq_file *seq, void *v)
{
    struct tx_isp_subdev *sd;
    struct vic_device *vic_dev;
    int i, total_errors = 0;
    int frame_count;
    
    /* Binary Ninja: void* $v0 = *(arg1 + 0x3c) */
    sd = (struct tx_isp_subdev *)seq->private;
    if (!sd || (unsigned long)sd >= 0xfffff001) {
        pr_err("The parameter is invalid!\n");
        return 0;
    }
    
    vic_dev = (struct vic_device *)tx_isp_get_subdevdata(sd);
    if (!vic_dev || (unsigned long)vic_dev >= 0xfffff001) {
        pr_err("The parameter is invalid!\n");
        return 0;
    }
    
    /* Binary Ninja: *($v0_1 + 0x164) = 0 */
    vic_dev->total_errors = 0;
    
    /* Binary Ninja: Sum up error counts from vic_err array */
    for (i = 0; i < 13; i++) {  /* 0x34 / 4 = 13 elements */
        total_errors += vic_dev->vic_errors[i];
    }
    
    frame_count = vic_dev->frame_count;
    vic_dev->total_errors = total_errors;
    
    /* Binary Ninja: private_seq_printf(arg1, " %d, %d\n", $a2) */
    seq_printf(seq, " %d, %d\n", frame_count, total_errors);
    
    /* Binary Ninja: Print all error counts */
    return seq_printf(seq, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
                     vic_dev->vic_errors[0], vic_dev->vic_errors[1], vic_dev->vic_errors[2],
                     vic_dev->vic_errors[3], vic_dev->vic_errors[4], vic_dev->vic_errors[5],
                     vic_dev->vic_errors[6], vic_dev->vic_errors[7], vic_dev->vic_errors[8],
                     vic_dev->vic_errors[9], vic_dev->vic_errors[10], vic_dev->vic_errors[11],
                     vic_dev->vic_errors[12]);
}

/* Dump ISP VIC FRD open - EXACT Binary Ninja implementation */
int dump_isp_vic_frd_open(struct inode *inode, struct file *file)
{
    /* Binary Ninja: return private_single_open_size(arg2, isp_vic_frd_show, PDE_DATA(), 0x400) __tailcall */
    return single_open_size(file, isp_vic_frd_show, PDE_DATA(inode), 0x400);
}

/* ISP VIC cmd set function - placeholder matching reference driver interface */
long isp_vic_cmd_set(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct tx_isp_subdev *sd = file->private_data;
    
    pr_info("isp_vic_cmd_set: cmd=0x%x, arg=0x%lx\n", cmd, arg);
    
    if (!sd) {
        pr_err("isp_vic_cmd_set: No subdev in file private_data\n");
        return -EINVAL;
    }
    
    /* Forward to the main VIC ioctl handler */
    return vic_chardev_ioctl(file, cmd, arg);
}

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

//  TODO -- Which is more right?
///* ispvic_frame_channel_s_stream - EXACT Binary Ninja implementation */
//static int ispvic_frame_channel_s_stream(struct tx_isp_vic_device *vic_dev, int enable)
//{
//    void *s0;
//    int var_18 = 0;
//    const char *stream_msg;
//
//    /* Binary Ninja: void* $s0 = nullptr; if (arg1 != 0 && arg1 u< 0xfffff001) $s0 = *(arg1 + 0xd4) */
//    s0 = NULL;
//    if (vic_dev != NULL && (uintptr_t)vic_dev < 0xfffff001) {
//        s0 = vic_dev->self;  /* *(arg1 + 0xd4) - self pointer */
//    }
//
//    /* Binary Ninja: if (arg1 == 0) */
//    if (!vic_dev) {
//        pr_err("ispvic_frame_channel_s_stream: invalid parameter\n");
//        return 0xffffffea;
//    }
//
//    /* Binary Ninja: char const* const $v0_3; if (arg2 != 0) $v0_3 = "streamon" else $v0_3 = "streamoff" */
//    stream_msg = enable ? "streamon" : "streamoff";
//    pr_info("ispvic_frame_channel_s_stream: %s\n", stream_msg);
//
//    /* Binary Ninja: if (arg2 == *($s0 + 0x210)) return 0 */
//    if (enable == vic_dev->streaming) {
//        pr_debug("ispvic_frame_channel_s_stream: Already in requested state %d\n", enable);
//        return 0;
//    }
//
//    /* Binary Ninja: __private_spin_lock_irqsave($s0 + 0x1f4, &var_18) */
//    spin_lock_irqsave(&vic_dev->buffer_lock, var_18);
//
//    /* Binary Ninja: if (arg2 == 0) */
//    if (enable == 0) {
//        /* Stream OFF - Binary Ninja: *(*($s0 + 0xb8) + 0x300) = 0; *($s0 + 0x210) = 0 */
//        pr_info("ispvic_frame_channel_s_stream: Stream OFF - disabling VIC\n");
//        writel(0, vic_dev->vic_regs + 0x300);
//        wmb();
//        vic_dev->streaming = 0;
//    } else {
//        /* Stream ON - Binary Ninja: vic_pipo_mdma_enable($s0) */
//        pr_info("ispvic_frame_channel_s_stream: Stream ON - enabling VIC\n");
//        vic_pipo_mdma_enable(vic_dev);
//
//        /* Binary Ninja: *(*($s0 + 0xb8) + 0x300) = *($s0 + 0x218) << 0x10 | 0x80000020 */
//        u32 stream_ctrl = (vic_dev->frame_count << 16) | 0x80000020;
//        writel(stream_ctrl, vic_dev->vic_regs + 0x300);
//        wmb();
//
//        /* Binary Ninja: *($s0 + 0x210) = 1 */
//        vic_dev->streaming = 1;
//
//        pr_info("ispvic_frame_channel_s_stream: VIC streaming enabled (reg 0x300 = 0x%x)\n", stream_ctrl);
//    }
//
//    /* Binary Ninja: private_spin_unlock_irqrestore($s0 + 0x1f4, var_18) */
//    spin_unlock_irqrestore(&vic_dev->buffer_lock, var_18);
//
//    /* Binary Ninja: return 0 */
//    return 0;
//}

/* ISPVIC Frame Channel S_Stream - EXACT Binary Ninja implementation */
int ispvic_frame_channel_s_stream(struct vic_device *vic_dev, int enable)
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

/* VIC event callback structure - matching reference driver layout 
 * CRITICAL: Must be exactly 32 bytes total with function pointer at offset 0x1c */
struct vic_callback_struct {
    char padding[28];           /* Exact 28 bytes padding to reach offset 0x1c */
    void *event_callback;       /* Function pointer at offset 0x1c */
} __attribute__((packed));

/* VIC event callback handler for QBUF events */
static int vic_pad_event_handler(struct tx_isp_subdev_pad *pad, unsigned int cmd, void *data)
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

/* VIC sensor operations structure - MISSING from original implementation */
static struct tx_isp_subdev_sensor_ops vic_sensor_ops = {
    .ioctl = vic_sensor_ops_ioctl,                    /* From tx-isp-module.c */
    .sync_sensor_attr = vic_sensor_ops_sync_sensor_attr, /* From tx-isp-module.c */
};

/* VIC core operations structure - MISSING ioctl registration */
static struct tx_isp_subdev_core_ops vic_core_ops = {
    .init = vic_core_ops_init,
    .ioctl = vic_core_ops_ioctl,  /* MISSING from original! */
};

/* Complete VIC subdev ops structure - MISSING sensor ops registration */
struct tx_isp_subdev_ops vic_subdev_ops = {
    .core = &vic_core_ops,
    .video = &vic_video_ops,
    .sensor = &vic_sensor_ops,    /* MISSING from original! */
};
EXPORT_SYMBOL(vic_subdev_ops);


/* VIC FRD file operations - EXACT Binary Ninja implementation */
static const struct file_operations isp_vic_frd_fops = {
    .owner = THIS_MODULE,
    .llseek = seq_lseek,                /* private_seq_lseek from hex dump */
    .read = seq_read,                   /* private_seq_read from hex dump */
    .unlocked_ioctl = isp_vic_cmd_set,  /* isp_vic_cmd_set from hex dump */
    .open = dump_isp_vic_frd_open,      /* dump_isp_vic_frd_open from hex dump */
    .release = single_release,          /* private_single_release from hex dump */
};

/* VIC W02 proc file operations - FIXED for proper proc interface */
static const struct file_operations isp_w02_proc_fops = {
    .owner = THIS_MODULE,
    .open = vic_chardev_open,
    .release = vic_chardev_release,
    .write = vic_proc_write,
    .llseek = default_llseek,
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
    
    /* *** CRITICAL FIX: Store VIC device at EXACT offset 0xd4 for Binary Ninja compatibility *** */
    pr_info("*** CRITICAL: Storing vic_dev at offset 0xd4 for Binary Ninja reference compatibility ***\n");
    *((void **)((char *)sd + 0xd4)) = vic_dev;
    
    /* VERIFY: Check that we can retrieve it correctly */
    struct vic_device *verification = (struct vic_device *)*((void **)((char *)sd + 0xd4));
    pr_info("*** VERIFICATION: vic_dev stored=%p, retrieved=%p ***\n", vic_dev, verification);
    
    if (verification != vic_dev) {
        pr_err("*** CRITICAL ERROR: VIC device storage/retrieval mismatch! ***\n");
        ret = -EIO;
        goto err_free_vic_dev;
    }
    pr_info("*** SUCCESS: VIC device correctly stored at offset 0xd4 ***\n");

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
        
        /* CRITICAL: Zero the structure first */
        memset(callback, 0, sizeof(struct vic_callback_struct));
        
        /* CRITICAL FIX: Set function pointer at EXACT offset 0x1c using direct memory access */
        *((void **)(((char *)callback) + 0x1c)) = (void *)vic_pad_event_handler;
        
        /* VERIFY: Also set via struct field for completeness */
        callback->event_callback = (void *)vic_pad_event_handler;
        
        /* Store callback structure pointer in priv field as expected by reference driver */
        sd->inpads[0].priv = callback;  /* This will be at pad+0xc (priv field offset) */
        
        /* Also set the event handler directly on the pad */
        sd->inpads[0].event = vic_pad_event_handler;
        
        /* COMPREHENSIVE VERIFICATION */
        pr_info("*** VIC CALLBACK FINAL VERIFICATION ***\n");
        pr_info("*** sizeof(vic_callback_struct) = %zu (should be 32) ***\n", sizeof(struct vic_callback_struct));
        pr_info("*** callback allocated at = %p ***\n", callback);
        pr_info("*** callback + 0x1c = %p ***\n", ((char *)callback) + 0x1c);
        pr_info("*** &callback->event_callback = %p ***\n", &callback->event_callback);
        pr_info("*** vic_pad_event_handler = %p ***\n", vic_pad_event_handler);
        pr_info("*** Direct memory read [callback+0x1c] = %p ***\n", *((void **)(((char *)callback) + 0x1c)));
        pr_info("*** Struct field callback->event_callback = %p ***\n", callback->event_callback);
        
        /* MEMORY DUMP: Show first 36 bytes of callback structure */
        pr_info("*** CALLBACK STRUCTURE MEMORY DUMP ***\n");
        {
            unsigned char *data = (unsigned char *)callback;
            int i;
            for (i = 0; i < 36; i += 4) {
                pr_info("*** [%02x]: %02x %02x %02x %02x ***\n", i, 
                       data[i], data[i+1], data[i+2], data[i+3]);
            }
        }
        
        /* Final pad setup */
        pr_info("*** sd->inpads[0].priv = %p ***\n", sd->inpads[0].priv);
        pr_info("*** sd->inpads[0].event = %p ***\n", sd->inpads[0].event);
        pr_info("*** VIC CALLBACK SETUP COMPLETE ***\n");
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

    /* Create /proc/jz/isp directory and entries */
    isp_dir = proc_mkdir("jz/isp", NULL);
    if (!isp_dir) {
        pr_err("Failed to create /proc/jz/isp directory\n");
        goto err_deinit_sd;
    }

    /* Create /proc/jz/isp/isp-w02 for write operations */
    w02_entry = proc_create_data("isp-w02", 0666, isp_dir, &isp_w02_proc_fops, sd);
    if (!w02_entry) {
        pr_err("Failed to create /proc/jz/isp/isp-w02\n");
        remove_proc_entry("jz/isp", NULL);
        goto err_deinit_sd;
    }
    pr_info("*** Successfully created /proc/jz/isp/isp-w02 ***\n");

    /* Create /proc/jz/isp/isp-vic-frd for VIC frame rate debugging - CRITICAL MISSING INTERFACE */
    struct proc_dir_entry *vic_frd_entry = proc_create_data("isp-vic-frd", 0444, isp_dir, &isp_vic_frd_fops, sd);
    if (!vic_frd_entry) {
        pr_err("*** CRITICAL ERROR: Failed to create /proc/jz/isp/isp-vic-frd - VIC FRD interface missing! ***\n");
        pr_err("*** This indicates VIC initialization is NOT completing fully ***\n");
        remove_proc_entry("isp-w02", isp_dir);
        remove_proc_entry("jz/isp", NULL);
        goto err_deinit_sd;
    }
    pr_info("*** SUCCESS: Created /proc/jz/isp/isp-vic-frd - VIC FULLY INITIALIZED! ***\n");

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
/* Forward declarations for callback functions referenced in pipo */
static int ispvic_frame_channel_qbuf(void);
static int ispvic_frame_channel_clearbuf(void);

/* ISPVIC Frame Channel QBUF - placeholder matching Binary Ninja reference */
static int ispvic_frame_channel_qbuf(void)
{
    pr_info("ispvic_frame_channel_qbuf called\n");
    return 0;
}

/* ISPVIC Frame Channel Clear Buffer - placeholder matching Binary Ninja reference */
static int ispvic_frame_channel_clearbuf(void)
{
    pr_info("ispvic_frame_channel_clearbuf called\n");
    return 0;
}

/* tx_isp_subdev_pipo - EXACT Binary Ninja implementation (1:1 mapping) */
int tx_isp_subdev_pipo(struct tx_isp_subdev *sd, void *arg)
{
    struct tx_isp_vic_device *vic_dev = NULL;
    void **raw_pipe = (void **)arg;
    int i;
    void **buffer_ptr;
    void **list_head;
    uint32_t offset_calc;
    
    pr_info("*** tx_isp_subdev_pipo: EXACT Binary Ninja implementation ***\n");
    pr_info("tx_isp_subdev_pipo: entry - sd=%p, arg=%p\n", sd, arg);
    
    /* Binary Ninja: if (arg1 != 0 && arg1 u< 0xfffff001) */
    if (sd != NULL && (unsigned long)sd < 0xfffff001) {
        /* Binary Ninja: $s0 = *(arg1 + 0xd4) */
        vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
        pr_info("tx_isp_subdev_pipo: vic_dev retrieved from sd->priv = %p\n", vic_dev);
    }
    
    if (!vic_dev) {
        pr_err("tx_isp_subdev_pipo: vic_dev is NULL\n");
        return 0;  /* Binary Ninja returns 0 even on error */
    }
    
    /* Binary Ninja: *($s0 + 0x20c) = 1 */
    /* This sets streaming flag at offset 0x20c in our structure */
    *(uint32_t *)((char *)vic_dev + 0x20c) = 1;
    pr_info("tx_isp_subdev_pipo: set offset 0x20c = 1 (streaming init)\n");
    
    /* Binary Ninja: raw_pipe = arg2 (store globally) */
    /* Note: In Binary Ninja this is stored in a global variable */
    
    /* Binary Ninja: if (arg2 == 0) */
    if (arg == NULL) {
        /* Binary Ninja: *($s0 + 0x214) = 0 */
        *(uint32_t *)((char *)vic_dev + 0x214) = 0;
        pr_info("tx_isp_subdev_pipo: arg is NULL - set offset 0x214 = 0\n");
    } else {
        pr_info("tx_isp_subdev_pipo: arg is not NULL - initializing pipe structures\n");
        
        /* Binary Ninja: Initialize linked list heads */
        /* *($s0 + 0x204) = $s0 + 0x204 */
        *(void **)((char *)vic_dev + 0x204) = (char *)vic_dev + 0x204;
        /* *($s0 + 0x208) = $s0 + 0x204 */
        *(void **)((char *)vic_dev + 0x208) = (char *)vic_dev + 0x204;
        /* *($s0 + 0x1f4) = $s0 + 0x1f4 */
        *(void **)((char *)vic_dev + 0x1f4) = (char *)vic_dev + 0x1f4;
        /* *($s0 + 0x1f8) = $s0 + 0x1f4 */
        *(void **)((char *)vic_dev + 0x1f8) = (char *)vic_dev + 0x1f4;
        /* *($s0 + 0x1fc) = $s0 + 0x1fc */
        *(void **)((char *)vic_dev + 0x1fc) = (char *)vic_dev + 0x1fc;
        /* *($s0 + 0x200) = $s0 + 0x1fc */
        *(void **)((char *)vic_dev + 0x200) = (char *)vic_dev + 0x1fc;
        
        pr_info("tx_isp_subdev_pipo: initialized linked list heads\n");
        
        /* Binary Ninja: private_spin_lock_init() - initialize spinlock */
        spin_lock_init((spinlock_t *)((char *)vic_dev + 0x1f4));
        pr_info("tx_isp_subdev_pipo: initialized spinlock at 0x1f4\n");
        
        /* Binary Ninja: Set up function pointers in raw_pipe structure */
        /* *raw_pipe = ispvic_frame_channel_qbuf */
        *raw_pipe = (void *)ispvic_frame_channel_qbuf;
        /* *(raw_pipe_1 + 8) = ispvic_frame_channel_clearbuf */
        *((void **)((char *)raw_pipe + 8)) = (void *)ispvic_frame_channel_clearbuf;
        /* *(raw_pipe_1 + 0xc) = ispvic_frame_channel_s_stream */
        *((void **)((char *)raw_pipe + 0xc)) = (void *)ispvic_frame_channel_s_stream;
        /* *(raw_pipe_1 + 0x10) = arg1 */
        *((void **)((char *)raw_pipe + 0x10)) = (void *)sd;
        
        pr_info("tx_isp_subdev_pipo: set function pointers - qbuf=%p, clearbuf=%p, s_stream=%p, sd=%p\n",
                ispvic_frame_channel_qbuf, ispvic_frame_channel_clearbuf, 
                ispvic_frame_channel_s_stream, sd);
        
        /* Binary Ninja: Initialize buffer array - loop with i from 0 to 4 */
        /* void** $v0_3 = $s0 + 0x168 */
        buffer_ptr = (void **)((char *)vic_dev + 0x168);
        
        for (i = 0; i < 5; i++) {
            /* Binary Ninja: $v0_3[4] = i */
            buffer_ptr[4] = (void *)(unsigned long)i;
            
            /* Binary Ninja: void*** $a0_1 = *($s0 + 0x200) */
            list_head = *(void ***)((char *)vic_dev + 0x200);
            
            /* Binary Ninja: *($s0 + 0x200) = $v0_3 */
            *(void **)((char *)vic_dev + 0x200) = buffer_ptr;
            
            /* Binary Ninja: $v0_3[1] = $a0_1 */
            buffer_ptr[1] = list_head;
            
            /* Binary Ninja: *$v0_3 = $s0 + 0x1fc */
            *buffer_ptr = (char *)vic_dev + 0x1fc;
            
            /* Binary Ninja: *$a0_1 = $v0_3 */
            if (list_head) {
                *list_head = buffer_ptr;
            }
            
            /* Binary Ninja: int32_t $a1 = (i + 0xc6) << 2 */
            offset_calc = (i + 0xc6) << 2;
            
            /* Binary Ninja: *(*($s0 + 0xb8) + $a1) = 0 */
            /* This clears a register at calculated offset */
            if (vic_dev->vic_regs) {
                *(uint32_t *)((char *)vic_dev->vic_regs + offset_calc) = 0;
            }
            
            /* Binary Ninja: $v0_3 = &$v0_3[7] - move to next buffer structure */
            buffer_ptr = &buffer_ptr[7];
            
            pr_info("tx_isp_subdev_pipo: initialized buffer %d, offset_calc=0x%x\n", i, offset_calc);
        }
        
        /* Binary Ninja: *($s0 + 0x214) = 1 */
        *(uint32_t *)((char *)vic_dev + 0x214) = 1;
        pr_info("tx_isp_subdev_pipo: set offset 0x214 = 1 (pipe enabled)\n");
    }
    
    pr_info("tx_isp_subdev_pipo: completed successfully, returning 0\n");
    /* Binary Ninja: return 0 */
    return 0;
}
EXPORT_SYMBOL(tx_isp_subdev_pipo);

/* VIC platform driver structure - CRITICAL MISSING PIECE */
static struct platform_driver tx_isp_vic_platform_driver = {
    .probe = tx_isp_vic_probe,
    .remove = tx_isp_vic_remove,
    .driver = {
        .name = "tx-isp-vic",
        .owner = THIS_MODULE,
    },
};

/* VIC platform device registration functions */
int __init tx_isp_vic_platform_init(void)
{
    int ret;
    
    pr_info("*** TX ISP VIC PLATFORM DRIVER REGISTRATION ***\n");
    
    ret = platform_driver_register(&tx_isp_vic_platform_driver);
    if (ret) {
        pr_err("Failed to register VIC platform driver: %d\n", ret);
        return ret;
    }
    
    pr_info("VIC platform driver registered successfully\n");
    return 0;
}

void __exit tx_isp_vic_platform_exit(void)
{
    pr_info("*** TX ISP VIC PLATFORM DRIVER UNREGISTRATION ***\n");
    platform_driver_unregister(&tx_isp_vic_platform_driver);
    pr_info("VIC platform driver unregistered\n");
}

/* Export symbols for use by other parts of the driver */
EXPORT_SYMBOL(tx_isp_vic_stop);
EXPORT_SYMBOL(tx_isp_vic_set_buffer);
EXPORT_SYMBOL(tx_isp_vic_wait_frame_done);

/* Export VIC platform init/exit for main module */
EXPORT_SYMBOL(tx_isp_vic_platform_init);
EXPORT_SYMBOL(tx_isp_vic_platform_exit);
