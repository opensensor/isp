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
    u32 status;
    unsigned long flags;
    int i;

    if (!sd)
        return IRQ_NONE;

    spin_lock_irqsave(&sd->vic_lock, flags);

    /* Read and clear interrupt status */
    status = vic_read32(VIC_INT_STATUS);
    vic_write32(VIC_INT_STATUS, status);

    /* Handle frame completion interrupts */
    for (i = 0; i < VIC_MAX_CHAN; i++) {
        if (status & (INT_FRAME_DONE << i)) {
            tx_isp_vic_frame_done(sd, i);
        }
    }

    /* Handle error conditions */
    if (status & INT_ERROR) {
        ISP_ERROR("VIC error interrupt received\n");
    }

    spin_unlock_irqrestore(&sd->vic_lock, flags);
    return IRQ_HANDLED;
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

/* Start VIC processing */
int tx_isp_vic_start(struct tx_isp_subdev *sd)
{
    u32 ctrl;

    if (!sd || !sd->isp)
        return -EINVAL;

    mutex_lock(&sd->vic_frame_end_lock);

    /* Enable VIC and start processing */
    ctrl = vic_read32(VIC_CTRL);
    ctrl |= (VIC_CTRL_EN | VIC_CTRL_START);
    vic_write32(VIC_CTRL, ctrl);

    mutex_unlock(&sd->vic_frame_end_lock);
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

static struct tx_isp_subdev_ops vic_subdev_ops = { 0 }; // All fields NULL/0


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
        goto err_release_mem;
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
    return -ENOENT;
}

w02_entry = proc_create_data("isp-w02", 0666, isp_dir, &isp_w02_proc_fops, sd);
if (!w02_entry) {
    pr_err("Failed to create /proc/jz/isp/isp-w02\n");
    remove_proc_entry("jz/isp", NULL);
    return -ENOMEM;
}

    pr_info("VIC probe completed successfully\n");
    return 0;

err_deinit_sd:
    tx_isp_subdev_deinit(sd);
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
EXPORT_SYMBOL(tx_isp_vic_start);
EXPORT_SYMBOL(tx_isp_vic_stop);
EXPORT_SYMBOL(tx_isp_vic_set_buffer);
EXPORT_SYMBOL(tx_isp_vic_wait_frame_done);
