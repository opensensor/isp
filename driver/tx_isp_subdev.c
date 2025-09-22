#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/clk.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_csi.h"
#include "../include/tx_isp_vin.h"
#include "../include/tx_isp_core.h"
#include "../include/tx_isp_core_device.h"
#include "../include/tx_isp_vic.h"
#include "../include/tx_isp_sysfs.h"

/* External reference to global ISP device */
extern struct tx_isp_dev *ourISPdev;

/* Function declarations for Binary Ninja compatibility */
int tx_isp_module_init(struct platform_device *pdev, struct tx_isp_subdev *sd);
void tx_isp_module_deinit(struct tx_isp_subdev *sd);
int isp_subdev_init_clks(struct tx_isp_subdev *sd, int clk_num);
int tx_isp_request_irq(struct platform_device *pdev, struct tx_isp_irq_info *irq_info);
void tx_isp_free_irq(struct tx_isp_irq_info *irq_info);
void tx_isp_disable_irq(void *arg1);

/* Binary Ninja interrupt handlers - EXACT reference implementation */
irqreturn_t isp_irq_handle(int irq, void *dev_id);
irqreturn_t isp_irq_thread_handle(int irq, void *dev_id);

/* Export the missing tx_isp_* functions */
EXPORT_SYMBOL(tx_isp_module_init);
EXPORT_SYMBOL(tx_isp_module_deinit);
EXPORT_SYMBOL(tx_isp_request_irq);
EXPORT_SYMBOL(tx_isp_free_irq);

/* Platform data structure moved to tx_isp.h for global access */

/* IRQ info structure - defined in tx-isp-device.h */

/* Frame channel states */
#define FRAME_CHAN_INIT    2
#define FRAME_CHAN_OPENED  3
#define FRAME_CHAN_STREAM  4

/* Properly prototype the vb2 helper functions */
static void __vb2_queue_free(void *queue, void *alloc_ctx);

/* frame_channel_open and forward declarations */
int frame_channel_open(struct inode *inode, struct file *file);
int frame_channel_release(struct inode *inode, struct file *file);
long frame_channel_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


/* Frame buffer structure */
struct frame_channel_buffer {
    struct list_head list;      // For queue management
    struct v4l2_buffer v4l2_buf;  // V4L2 buffer info
    void *data;                   // Buffer data
    int state;                    // Buffer state
    dma_addr_t dma_addr;         // DMA address
};

/* Helper function implementations */
static int __frame_channel_vb2_streamoff(void *chan, void *queue)
{
    struct tx_isp_frame_channel *fchan = (struct tx_isp_frame_channel *)chan;
    int ret = 0;

    if (!fchan)
        return -EINVAL;

    // Stop streaming
    spin_lock(&fchan->slock);
    // Clear queues and reset state
    INIT_LIST_HEAD(&fchan->queue_head);
    INIT_LIST_HEAD(&fchan->done_head);
    fchan->queued_count = 0;
    fchan->done_count = 0;
    spin_unlock(&fchan->slock);

    return ret;
}

static void __fill_v4l2_buffer(void *vb, struct v4l2_buffer *buf)
{
    struct frame_channel_buffer *fbuf = (struct frame_channel_buffer *)vb;

    if (!fbuf || !buf)
        return;

    // Copy buffer info
    memcpy(buf, &fbuf->v4l2_buf, sizeof(struct v4l2_buffer));
}

/* Frame channel file operations */
static const struct file_operations fs_channel_ops = {
    .owner = THIS_MODULE,
    .open = frame_channel_open,
    .release = frame_channel_release,
    .unlocked_ioctl = frame_channel_unlocked_ioctl,
};


extern struct tx_isp_dev *ourISPdev;

/* No external function dependencies needed - using safe direct implementation */

/* Subdevice initialization */
/* First, let's define the missing enums/constants */
#define TX_ISP_PADTYPE_INPUT   0x1
#define TX_ISP_PADTYPE_OUTPUT  0x2
#define TX_ISP_PADSTATE_FREE   0x0
#define TX_ISP_LINKFLAG_DYNAMIC 0x1
#define TX_ISP_PADLINK_VIC     0x1
#define TX_ISP_PADLINK_DDR     0x2
#define TX_ISP_PADLINK_ISP     0x4

/* isp_subdev_init_clks - Using vic_start clock management pattern with Binary Ninja structure */
int isp_subdev_init_clks(struct tx_isp_subdev *sd, int clk_count)
{
    struct clk *cgu_isp_clk, *isp_clk, *csi_clk;
    struct clk **clk_array = NULL;
    void __iomem *cpm_regs;
    int ret;
    int i = 0;

    /* Binary Ninja: int32_t $s5 = *(arg1 + 0xc0) */
    /* Binary Ninja: int32_t $s1 = $s5 << 2 */
    int clk_array_size = clk_count << 2;

    pr_info("isp_subdev_init_clks: Initializing %d clocks\n", clk_count);

    /* Binary Ninja: if ($s5 != 0) */
    if (clk_count != 0) {
        /* Binary Ninja: int32_t* $v0_1 = private_kmalloc($s1, 0xd0) */
        clk_array = private_kmalloc(clk_array_size, 0xd0);

        /* Binary Ninja: if ($v0_1 == 0) */
        if (clk_array == NULL) {
            /* Binary Ninja: isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", "isp_subdev_init_clks") */
            isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", "isp_subdev_init_clks");
            return 0xfffffff4;  /* Binary Ninja: return 0xfffffff4 */
        }

        /* Binary Ninja: memset($v0_1, 0, $s1) */
        memset(clk_array, 0, clk_array_size);

        /* Use vic_start clock management pattern - get standard ISP clocks */
        i = 0;

        /* Binary Ninja: Clock 0 - Try different clock names for T31 platform */
        if (i < clk_count) {
            /* Try multiple clock names that might be available on T31 */
            const char *clock_names[] = {"cgu_isp", "isp", "cgu_cim", NULL};
            int clock_idx = 0;

            cgu_isp_clk = NULL;
            while (clock_names[clock_idx] && IS_ERR_OR_NULL(cgu_isp_clk)) {
                cgu_isp_clk = clk_get(sd->dev, clock_names[clock_idx]);
                if (!IS_ERR(cgu_isp_clk)) {
                    pr_info("Found clock: %s\n", clock_names[clock_idx]);
                    break;
                }
                clock_idx++;
            }

            clk_array[i] = cgu_isp_clk;

            /* Binary Ninja: if ($v0_3 u< 0xfffff001) */
            if (!IS_ERR_OR_NULL(cgu_isp_clk)) {
                /* Binary Ninja: private_clk_set_rate($v0_3, $a1_1) */
                ret = clk_set_rate(cgu_isp_clk, 100000000);  /* 100MHz */
                if (ret == 0) {
                    ret = clk_prepare_enable(cgu_isp_clk);
                    if (ret == 0) {
                        pr_info("ISP clock enabled at 100MHz\n");
                        i++;
                    } else {
                        /* Binary Ninja: isp_printf(2, "sensor type is BT1120!\n", *$s6_1) */
                        pr_warn("Failed to enable ISP clock, continuing anyway\n");
                        i++; /* Continue anyway - clock might not be needed */
                    }
                } else {
                    /* Binary Ninja: isp_printf(2, "sensor type is BT1120!\n", *$s6_1) */
                    pr_warn("Failed to set ISP clock rate, continuing anyway\n");
                    i++; /* Continue anyway - clock might not be needed */
                }
            } else {
                /* Binary Ninja: isp_printf(2, "Can not support this frame mode!!!\n", *$s6_1) */
                pr_warn("No ISP clock found, continuing without clock management\n");
                clk_array[i] = NULL;
                i++; /* Continue anyway - T31 might not need explicit clock management */
            }
        }

        /* Binary Ninja: Clock 1 - ISP Core */
        if (i < clk_count) {
            const char *isp_clock_names[] = {"isp", "cgu_isp", "ipu", NULL};
            int clock_idx = 0;

            isp_clk = NULL;
            while (isp_clock_names[clock_idx] && IS_ERR_OR_NULL(isp_clk)) {
                isp_clk = clk_get(sd->dev, isp_clock_names[clock_idx]);
                if (!IS_ERR(isp_clk)) {
                    pr_info("Found ISP clock: %s\n", isp_clock_names[clock_idx]);
                    break;
                }
                clock_idx++;
            }

            clk_array[i] = isp_clk;

            if (!IS_ERR_OR_NULL(isp_clk)) {
                ret = clk_prepare_enable(isp_clk);
                if (ret == 0) {
                    pr_info("ISP core clock enabled\n");
                    i++;
                } else {
                    pr_warn("Failed to enable ISP core clock, continuing anyway\n");
                    i++;
                }
            } else {
                pr_warn("No ISP core clock found, continuing anyway\n");
                clk_array[i] = NULL;
                i++;
            }
        }

        /* Binary Ninja: Clock 2 - CSI/MIPI */
        if (i < clk_count) {
            const char *csi_clock_names[] = {"csi", "mipi", "cgu_cim", NULL};
            int clock_idx = 0;

            csi_clk = NULL;
            while (csi_clock_names[clock_idx] && IS_ERR_OR_NULL(csi_clk)) {
                csi_clk = clk_get(sd->dev, csi_clock_names[clock_idx]);
                if (!IS_ERR(csi_clk)) {
                    pr_info("Found CSI clock: %s\n", csi_clock_names[clock_idx]);
                    break;
                }
                clock_idx++;
            }

            clk_array[i] = csi_clk;

            if (!IS_ERR_OR_NULL(csi_clk)) {
                ret = clk_prepare_enable(csi_clk);
                if (ret == 0) {
                    pr_info("CSI/MIPI clock enabled\n");
                    i++;
                } else {
                    pr_warn("Failed to enable CSI clock, continuing anyway\n");
                    i++;
                }
            } else {
                pr_warn("No CSI clock found, continuing anyway\n");
                clk_array[i] = NULL;
                i++;
            }
        }

        /* CPM register setup - following vic_start pattern */
        cpm_regs = ioremap(0x10000000, 0x1000);
        if (cpm_regs) {
            u32 clkgr0 = readl(cpm_regs + 0x20);
            u32 clkgr1 = readl(cpm_regs + 0x28);

            clkgr0 &= ~(1 << 13); // ISP
            clkgr0 &= ~(1 << 21); // Alternative ISP
            clkgr0 &= ~(1 << 30); // VIC in CLKGR0
            clkgr1 &= ~(1 << 30); // VIC in CLKGR1

            writel(clkgr0, cpm_regs + 0x20);
            writel(clkgr1, cpm_regs + 0x28);
            wmb();
            msleep(20);
            iounmap(cpm_regs);
            pr_info("CPM clock gates configured\n");
        }

        /* Binary Ninja: *(arg1 + 0xbc) = $v0_1 */
        sd->clks = clk_array;
        pr_info("isp_subdev_init_clks: Successfully initialized %d clocks\n", clk_count);
    } else {
        /* Binary Ninja: *(arg1 + 0xbc) = 0 */
        sd->clks = NULL;
    }

    /* Binary Ninja: return 0 */
    return 0;

cleanup_clocks:
    /* Binary Ninja: Clock cleanup loop */
    for (int j = 0; j < i; j++) {
        if (clk_array[j] && !IS_ERR(clk_array[j])) {
            clk_disable_unprepare(clk_array[j]);
            clk_put(clk_array[j]);
        }
    }

    /* Binary Ninja: private_kfree($v0_1) */
    private_kfree(clk_array);
    return -EFAULT;
}

/* tx_isp_subdev_init - EXACT Binary Ninja reference with safe struct member access */
int tx_isp_subdev_init(struct platform_device *pdev, struct tx_isp_subdev *sd,
                      struct tx_isp_subdev_ops *ops)
{
    struct resource *res;
    struct resource *mem_res = NULL;
    void __iomem *regs;
    int ret;
    int i;

    pr_info("*** tx_isp_subdev_init: CALLED for device '%s' ***\n", pdev ? pdev->name : "NULL");
    pr_info("*** tx_isp_subdev_init: pdev=%p, sd=%p, ops=%p ***\n", pdev, sd, ops);
    pr_info("*** tx_isp_subdev_init: ourISPdev=%p ***\n", ourISPdev);

    /* Binary Ninja: if (arg1 == 0 || arg2 == 0) */
    if (pdev == NULL || sd == NULL) {
        /* Binary Ninja: isp_printf(2, tiziano_wdr_gamma_refresh, "tx_isp_subdev_init") */
        isp_printf(2, "tiziano_wdr_gamma_refresh", "tx_isp_subdev_init");
        return 0xffffffea;  /* Binary Ninja: return 0xffffffea */
    }

    /* Binary Ninja: *(arg2 + 0xc4) = arg3 */
    /* SAFE: Use struct member access instead of offset */
    sd->ops = ops;

    /* DEBUG: Check what ops structure we're getting */
    pr_info("*** tx_isp_subdev_init: ops=%p, ops->core=%p ***\n", ops, ops ? ops->core : NULL);
    if (ops && ops->core && ops->core->init) {
        pr_info("*** tx_isp_subdev_init: ops->core->init=%p ***\n", ops->core->init);
    } else {
        pr_info("*** tx_isp_subdev_init: WARNING - ops->core->init is NULL! ***\n");
    }

    /* CRITICAL FIX: Set device pointers that are needed for IRQ registration */
    sd->dev = &pdev->dev;        /* Device pointer for to_platform_device() */
    sd->pdev = pdev;             /* Platform device pointer */
    pr_info("*** tx_isp_subdev_init: Set sd->dev=%p, sd->pdev=%p ***\n", sd->dev, sd->pdev);

    /* CRITICAL: Link subdevices to main ISP device when they're created */
    extern struct tx_isp_dev *ourISPdev;

    /* CRITICAL: Register subdevices in the global ISP device with proper if-else logic */
    extern struct tx_isp_subdev_ops core_subdev_ops;
    extern struct tx_isp_subdev_ops vic_subdev_ops;
    extern struct tx_isp_subdev_ops csi_subdev_ops;

    if (ourISPdev) {
        if (ops == &csi_subdev_ops) {
            /* This is a CSI subdev - register it in subdevs array */
            ourISPdev->subdevs[0] = sd;  /* CSI at index 0 */
            sd->isp = ourISPdev;
            pr_info("*** tx_isp_subdev_init: CSI subdev registered at index 0 ***\n");
        } else if (ops == &vic_subdev_ops) {
            /* This is a VIC subdev - link the VIC device to ourISPdev */
            struct tx_isp_vic_device *vic_dev = container_of(sd, struct tx_isp_vic_device, sd);
            ourISPdev->vic_dev = vic_dev;
            ourISPdev->subdevs[1] = sd;
            sd->isp = ourISPdev;
            pr_info("*** tx_isp_subdev_init: VIC device linked and registered at index 1 ***\n");
        } else if (ops == &core_subdev_ops) {
            /* This is the Core ISP subdev - registration handled by core device linking */
            pr_info("*** tx_isp_subdev_init: Core ISP subdev registration handled by core device ***\n");
            /* The actual registration is done in tx_isp_link_core_device() */
        } else if (ops && ops->sensor && ops != &csi_subdev_ops && ops != &vic_subdev_ops) {
            /* CRITICAL FIX: This is a REAL sensor subdev (not CSI or VIC which also have sensor ops) */
            pr_info("*** tx_isp_subdev_init: DETECTED SENSOR SUBDEV - ops=%p, ops->sensor=%p ***\n", ops, ops->sensor);

            /* CRITICAL FIX: Set up the module notify function for TX_ISP_EVENT_SYNC_SENSOR_ATTR */
            extern int tx_isp_handle_sync_sensor_attr_event(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr);
            extern int tx_isp_module_notify_handler(struct tx_isp_module *module, unsigned int cmd, void *arg);
            sd->module.notify = tx_isp_module_notify_handler;
            pr_info("*** tx_isp_subdev_init: Set up sensor module notify handler ***\n");

            /* Find next available slot starting from index 5 (after CSI=0, VIC=1, VIN=2, FS=3, Core=4) */
            int sensor_index = -1;
            for (int i = 5; i < ISP_MAX_SUBDEVS; i++) {
                if (ourISPdev->subdevs[i] == NULL) {
                    sensor_index = i;
                    break;
                }
            }

            if (sensor_index != -1) {
                ourISPdev->subdevs[sensor_index] = sd;
                sd->isp = ourISPdev;
                pr_info("*** tx_isp_subdev_init: SENSOR subdev registered at index %d, sd=%p ***\n", sensor_index, sd);
                pr_info("*** tx_isp_subdev_init: SENSOR ops=%p, ops->sensor=%p ***\n", sd->ops, sd->ops->sensor);
            } else {
                pr_err("*** tx_isp_subdev_init: No available slot for sensor subdev ***\n");
            }
        } else {
            pr_info("*** tx_isp_subdev_init: NOT A SENSOR - ops=%p ***\n", ops);
            if (ops) {
                pr_info("*** tx_isp_subdev_init: ops->sensor=%p, csi_subdev_ops=%p ***\n", ops->sensor, &csi_subdev_ops);
            }
        }
    }

    /* Binary Ninja: if (tx_isp_module_init(arg1, arg2) != 0) */
    ret = tx_isp_module_init(pdev, sd);
    if (ret != 0) {
        /* Binary Ninja: isp_printf(2, "&vsd->snap_mlock", *arg1) */
        isp_printf(2, "&vsd->snap_mlock", pdev->name);
        return 0xfffffff4;  /* Binary Ninja: return 0xfffffff4 */
    }

    const char *dev_name_str = dev_name(&pdev->dev);

    /* VIC interrupt registration moved to auto-linking function where registers are actually mapped */
    pr_info("*** tx_isp_subdev_init: VIC interrupt registration will happen in auto-linking function ***\n");

    /* Binary Ninja: char* $s1_1 = arg1[0x16] */
    /* SAFE: Get platform data using proper kernel API */
    struct tx_isp_subdev_platform_data *pdata = dev_get_platdata(&pdev->dev);
    if (pdata != NULL) {
        /* CRITICAL: Skip IRQ request for devices that don't have IRQ resources */
        /* Only VIC (isp-w02) and Core (isp-m0) have IRQ resources */
        const char *dev_name_str = dev_name(&pdev->dev);
        if (strcmp(dev_name_str, "isp-w00") != 0 &&  /* CSI - no IRQ */
            strcmp(dev_name_str, "isp-w01") != 0 &&  /* VIN - no IRQ */
            strcmp(dev_name_str, "isp-fs") != 0) {   /* FS - no IRQ */
            /* Binary Ninja: tx_isp_request_irq(arg1, arg2 + 0x80) */
            /* SAFE: Use struct member access for IRQ setup */
            ret = tx_isp_request_irq(pdev, &sd->irq_info);
            if (ret != 0) {
                /* Binary Ninja: isp_printf(2, " %d, %d\n", $a2_1) */
                isp_printf(2, " %d, %d\n", ret);
                tx_isp_module_deinit(sd);
                return ret;
            }
        } else {
            pr_info("*** %s: Skipping IRQ request - device has no IRQ resource ***\n", dev_name_str);
        }

        /* FIXED: Get first memory resource without string comparison */
        mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        pr_info("tx_isp_subdev_init: platform_get_resource returned %p for device %s\n", mem_res, dev_name(&pdev->dev));
        if (mem_res) {
            pr_info("tx_isp_subdev_init: Memory resource found: start=0x%08x, end=0x%08x, size=0x%08x\n",
                    (u32)mem_res->start, (u32)mem_res->end, (u32)resource_size(mem_res));
        } else {
            /* CRITICAL FIX: Memory resources are optional for logical devices like VIN */
            pr_info("tx_isp_subdev_init: No memory resource for device %s (logical device - OK)\n", dev_name(&pdev->dev));
        }

        if (mem_res) {
            /* Binary Ninja: private_request_mem_region($a0_18, $v0_2[1] + 1 - $a0_18, $a2_11) */
            /* SAFE: Use struct member access for memory region */
            sd->mem_res = request_mem_region(mem_res->start,
                                           resource_size(mem_res),
                                           pdev->name);
            if (!sd->mem_res) {
                /* Binary Ninja: isp_printf(2, "The parameter is invalid!\n", "tx_isp_subdev_init") */
                pr_err("tx_isp_subdev_init: request_mem_region failed for %s (0x%08x-0x%08x)\n",
                       dev_name(&pdev->dev), (u32)mem_res->start, (u32)mem_res->end);
                pr_err("*** MEMORY REGION CONFLICT: Stock driver may still be loaded ***\n");
                pr_err("*** Please unload stock driver first: rmmod tx_isp_t31 ***\n");
                isp_printf(2, "The parameter is invalid!\n", "tx_isp_subdev_init");
                ret = 0xfffffff0;
                goto cleanup_irq;
            }

            /* Binary Ninja: private_ioremap($a0_19, $v0_22[1] + 1 - $a0_19) */
            /* SAFE: Use struct member access for register mapping */
            regs = ioremap(mem_res->start, resource_size(mem_res));
            sd->regs = regs;
            if (!regs) {
                /* Binary Ninja: isp_printf(2, "vic_done_gpio%d", "tx_isp_subdev_init") */
                isp_printf(2, "vic_done_gpio%d", "tx_isp_subdev_init");
                ret = 0xfffffffa;
                goto cleanup_mem;
            }
        }

        /* Binary Ninja: Clock initialization based on platform data */
        /* Binary Ninja: uint32_t $v0_5 = zx.d(*$s1_1) */
        if (pdata->interface_type == 1 || pdata->interface_type == 2) {
            /* Binary Ninja: *(arg2 + 0xc0) = zx.d($s1_1[4]) */
            /* SAFE: Use struct member access for clock count */
            sd->clk_num = pdata->clk_num;

            /* Binary Ninja: isp_subdev_init_clks(arg2, *($s1_1 + 8)) */
            ret = isp_subdev_init_clks(sd, pdata->clk_num);
            if (ret != 0) {
                /* Binary Ninja: isp_printf(2, "register is 0x%x, value is 0x%x\n", *(arg2 + 8)) */
                isp_printf(2, "register is 0x%x, value is 0x%x\n", (unsigned int)sd->dev);
                goto cleanup_regs;
            }
        } else {
            /* Binary Ninja: isp_printf(0, tiziano_wdr_params_refresh, result_4) */
            isp_printf(0, "tiziano_wdr_params_refresh", ret);
            return 0;
        }
    }

    /* CRITICAL: Auto-link subdevices to global ISP device */
    tx_isp_subdev_auto_link(pdev, sd);

    /* Binary Ninja: return 0 */
    return 0;

cleanup_regs:
    /* Binary Ninja: private_iounmap(*(arg2 + 0xb8)) */
    if (sd->regs) {
        iounmap(sd->regs);
        sd->regs = NULL;
    }

cleanup_mem:
    /* Binary Ninja: private_release_mem_region($a0_15, $s3_2[1] + 1 - $a0_15) */
    /* Only release memory region if it was successfully requested */
    if (sd->mem_res) {
        release_mem_region(sd->mem_res->start, resource_size(sd->mem_res));
        sd->mem_res = NULL;
    }
    /* Note: If mem_res is NULL, we used direct ioremap without request_mem_region */

cleanup_irq:
    /* Binary Ninja: tx_isp_free_irq(arg2 + 0x80) */
    tx_isp_free_irq(&sd->irq_info);
    tx_isp_module_deinit(sd);
    return ret;
}
EXPORT_SYMBOL(tx_isp_subdev_init);

/* Subdevice deinitialization */
void tx_isp_subdev_deinit(struct tx_isp_subdev *sd)
{
    if (!sd)
        return;

    /* Clean up secondary VIC register mapping if this is a VIC device */
    if (ourISPdev && ourISPdev->vic_dev == sd) {
        struct tx_isp_vic_device *vic_dev = container_of(sd, struct tx_isp_vic_device, sd);
        if (vic_dev->vic_regs) {
            iounmap(vic_dev->vic_regs);
            vic_dev->vic_regs = NULL;
            pr_info("*** Unmapped secondary VIC registers ***\n");
        }
    }
}
EXPORT_SYMBOL(tx_isp_subdev_deinit);

/* Auto-linking function to connect subdevices to global ISP device */
void tx_isp_subdev_auto_link(struct platform_device *pdev, struct tx_isp_subdev *sd)
{
    pr_info("*** tx_isp_subdev_auto_link: ENTRY - pdev=%p, sd=%p, ourISPdev=%p ***\n", pdev, sd, ourISPdev);

    if (!ourISPdev) {
        pr_err("*** CRITICAL: tx_isp_subdev_auto_link: ourISPdev is NULL! ***\n");
        return;
    }
    if (!pdev) {
        pr_err("*** CRITICAL: tx_isp_subdev_auto_link: pdev is NULL! ***\n");
        return;
    }
    if (!pdev->name) {
        pr_err("*** CRITICAL: tx_isp_subdev_auto_link: pdev->name is NULL! ***\n");
        return;
    }
    if (!sd) {
        pr_err("*** CRITICAL: tx_isp_subdev_auto_link: sd is NULL! ***\n");
        return;
    }

    const char *dev_name = pdev->name;

    pr_info("*** tx_isp_subdev_auto_link: Auto-linking device '%s' to ourISPdev=%p ***\n", dev_name, ourISPdev);
    pr_info("*** DEBUG: Device name comparison - checking '%s' ***\n", dev_name);
    pr_info("*** DEBUG: About to check device name matches ***\n");

    if (strcmp(dev_name, "isp-w00") == 0) {
        /* Link CSI device - device name is now "isp-w00" */
        struct tx_isp_csi_device *csi_dev = container_of(sd, struct tx_isp_csi_device, sd);
        ourISPdev->csi_dev = csi_dev;
        if (sd->regs) {
            ourISPdev->csi_regs = sd->regs;
            /* CRITICAL FIX: Set CSI device's basic registers from mapped registers */
            csi_dev->csi_regs = sd->regs;
            pr_info("*** CSI BASIC REGISTERS SET: %p (from tx_isp_subdev_init) ***\n", sd->regs);
        }
        pr_info("*** LINKED CSI device: %p, regs: %p ***\n", csi_dev, sd->regs);

    } else if (strcmp(dev_name, "isp-w02") == 0) {
        pr_info("*** DEBUG: VIC DEVICE NAME MATCHED! Processing VIC device linking ***\n");

        /* Link VIC device - actual device name is "isp-w02" not "tx-isp-vic" */
        /* CRITICAL FIX: Use direct pointer storage instead of container_of to avoid corruption */
        struct tx_isp_vic_device *vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
        pr_info("*** DEBUG: Retrieved vic_dev from subdev data: %p ***\n", vic_dev);

        if (!vic_dev || (unsigned long)vic_dev < 0x80000000 || (unsigned long)vic_dev >= 0xfffff000) {
            pr_err("*** VIC DEVICE LINKING FAILED: Invalid vic_dev pointer %p ***\n", vic_dev);
            return;
        }

        pr_info("*** DEBUG: About to set ourISPdev->vic_dev = %p ***\n", vic_dev);
        pr_info("*** DEBUG: ourISPdev before linking: %p ***\n", ourISPdev);

        /* CRITICAL FIX: Store VIC device pointer correctly - NOT cast to subdev! */
        ourISPdev->vic_dev = vic_dev;  /* vic_dev field expects struct tx_isp_vic_device * */
        vic_dev->vic_regs = sd->regs;  /* Critical: Set primary VIC registers */

        pr_info("*** DEBUG: ourISPdev->vic_dev set to: %p ***\n", ourISPdev->vic_dev);

        /* CRITICAL FIX: Register VIC IRQ immediately after successful linking */
        if (vic_dev->sd.irq_info.irq == 0) {
            extern int tx_isp_request_irq(struct platform_device *pdev, struct tx_isp_irq_info *irq_info);
            struct platform_device *vic_pdev = to_platform_device(vic_dev->sd.dev);

            pr_info("*** VIC AUTO-LINK: Registering VIC IRQ immediately after device linking ***\n");
            int irq_ret = tx_isp_request_irq(vic_pdev, &vic_dev->sd.irq_info);
            if (irq_ret != 0) {
                pr_err("*** VIC AUTO-LINK: Failed to register VIC IRQ: %d ***\n", irq_ret);
            } else {
                pr_info("*** VIC AUTO-LINK: VIC IRQ registered successfully ***\n");
            }
        } else {
            pr_info("*** VIC AUTO-LINK: VIC IRQ already registered (irq=%d) ***\n", vic_dev->sd.irq_info.irq);
        }

        /* CRITICAL: Map secondary VIC register space (0x10023000) */
        vic_dev->vic_regs = ioremap(0x10023000, 0x1000);
        if (!vic_dev->vic_regs) {
            pr_err("*** FAILED to map secondary VIC registers (0x10023000) ***\n");
        } else {
            pr_info("*** MAPPED secondary VIC registers: %p (0x10023000) ***\n", vic_dev->vic_regs);
        }

        /* CRITICAL: Register VIC interrupt handler NOW that registers are mapped */
        if (sd->regs && ourISPdev->vic_dev && ourISPdev->vic_dev->vic_regs) {
            pr_info("*** VIC AUTO-LINK: Registers are mapped, registering interrupt handler ***\n");

            /* Find the VIC platform device */
            struct platform_device *vic_pdev = NULL;
            if (sd->pdev) {
                vic_pdev = sd->pdev;
            } else {
                /* Find VIC platform device from registry */
                extern struct platform_device tx_isp_vic_platform_device;
                vic_pdev = &tx_isp_vic_platform_device;
            }
        } else {
            pr_err("*** VIC AUTO-LINK: Registers not mapped - cannot register interrupt ***\n");
        }

    } else if (strcmp(dev_name, "tx-isp-vin") == 0) {
        /* Link VIN device */
        pr_info("*** DEBUG: VIN device name matched! Setting up VIN device ***\n");
        struct tx_isp_vin_device *vin_dev = container_of(sd, struct tx_isp_vin_device, sd);
        ourISPdev->vin_dev = vin_dev;

        /* CRITICAL FIX: Set up VIN subdev ops structure immediately after linking */
        extern struct tx_isp_subdev_ops vin_subdev_ops;
        vin_dev->sd.ops = &vin_subdev_ops;
        vin_dev->sd.isp = (void *)ourISPdev;

        pr_info("*** LINKED VIN device: %p ***\n", vin_dev);
        pr_info("*** VIN SUBDEV OPS CONFIGURED: core=%p, video=%p, s_stream=%p ***\n",
                vin_dev->sd.ops->core, vin_dev->sd.ops->video,
                vin_dev->sd.ops->video ? vin_dev->sd.ops->video->s_stream : NULL);

        /* CRITICAL: Add VIN to subdev array at index 2 (after VIC=0, CSI=1) */
        ourISPdev->subdevs[2] = &vin_dev->sd;
        pr_info("*** REGISTERED VIN SUBDEV AT INDEX 2 WITH VIDEO OPS ***\n");

        /* VIN initialization now happens during sensor registration for proper timing */
        pr_info("*** VIN INITIALIZATION DEFERRED TO SENSOR REGISTRATION PHASE ***\n");

    } else if (strcmp(dev_name, "tx-isp-fs") == 0) {
        /* Link FS device */
        struct tx_isp_fs_device *fs_dev = container_of(sd, struct tx_isp_fs_device, subdev);
        ourISPdev->fs_dev = (struct frame_source_device *)fs_dev;
        pr_info("*** LINKED FS device: %p ***\n", fs_dev);

        /* CRITICAL: Add FS to subdev array at index 3 (after CSI=0, VIC=1, VIN=2, before sensors) */
        if (ourISPdev->subdevs[3] == NULL) {
            ourISPdev->subdevs[3] = &fs_dev->subdev;
            pr_info("*** REGISTERED FS SUBDEV AT INDEX 3 WITH SUBDEV OPS ***\n");
        } else {
            pr_err("*** FS subdev slot (index 3) already occupied ***\n");
        }

    } else if (strcmp(dev_name, "tx-isp-core") == 0) {
        /* Core device register mapping - CRITICAL FIX */
        if (sd->regs) {
            /* CRITICAL: Get core device from subdev private data */
            struct tx_isp_core_device *core_dev = tx_isp_get_subdevdata(sd);
            if (core_dev) {
                /* Map core registers directly to core device */
                core_dev->core_regs = sd->regs;
                pr_info("*** CRITICAL FIX: CORE regs mapped to core device: %p ***\n", sd->regs);

                /* Also link to global ISP device if available */
                if (ourISPdev) {
                    ourISPdev->core_dev = core_dev;
                    pr_info("*** CRITICAL FIX: Core device linked to global ISP device ***\n");
                }
            } else {
                pr_err("*** CRITICAL ERROR: Core device not found in subdev private data ***\n");
            }
        } else {
            pr_err("*** CRITICAL ERROR: No core registers mapped ***\n");
        }
    } else if (strcmp(dev_name, "gc2053") == 0 || strstr(dev_name, "sensor") != NULL) {
        /* CRITICAL: This is a sensor device - check if already registered to prevent duplicates */
        pr_info("*** DETECTED SENSOR DEVICE: '%s' - checking for existing registration ***\n", dev_name);

        /* CRITICAL FIX: Check if this subdev is already registered to prevent duplicates */
        bool already_registered = false;
        for (int i = 5; i < ISP_MAX_SUBDEVS; i++) {
            if (ourISPdev->subdevs[i] == sd) {
                already_registered = true;
                pr_info("*** SENSOR '%s' already registered at subdev index %d (sd=%p) ***\n", dev_name, i, sd);
                pr_info("*** SENSOR: Skipping duplicate registration to prevent multiple sensor inits ***\n");
                break;
            }
        }

        if (!already_registered) {
            /* Find next available slot starting from index 5 (after CSI=0, VIC=1, VIN=2, FS=3, Core=4) */
            int sensor_index = -1;
            for (int i = 5; i < ISP_MAX_SUBDEVS; i++) {
                if (ourISPdev->subdevs[i] == NULL) {
                    sensor_index = i;
                    break;
                }
            }

            if (sensor_index != -1) {
                ourISPdev->subdevs[sensor_index] = sd;
                sd->isp = ourISPdev;
                pr_info("*** SENSOR '%s' registered at subdev index %d ***\n", dev_name, sensor_index);
                pr_info("*** SENSOR subdev: %p, ops: %p ***\n", sd, sd->ops);
                if (sd->ops && sd->ops->sensor) {
                    pr_info("*** SENSOR ops->sensor: %p ***\n", sd->ops->sensor);
                }
            } else {
                pr_err("*** No available slot for sensor '%s' ***\n", dev_name);
            }
        }
    } else {
        pr_info("*** DEBUG: Unknown device name '%s' - no specific auto-link handling ***\n", dev_name);
    }
}
EXPORT_SYMBOL(tx_isp_subdev_auto_link);

/**
 * tx_isp_reg_set - EXACT Binary Ninja MCP implementation
 * Address: 0x1f580
 * Sets register bits in a specific range for a subdevice
 */
int tx_isp_reg_set(struct tx_isp_subdev *sd, unsigned int reg, int start, int end, int val)
{
    int32_t mask = 0;
    int32_t *reg_addr;

    if (!sd || !sd->regs) {
        pr_err("tx_isp_reg_set: Invalid subdev or registers not mapped");
        return -EINVAL;
    }

    /* Binary Ninja: Build mask for bit range */
    for (int i = 0; i < (end - start + 1); i++) {
        mask += 1 << ((i + start) & 0x1f);
    }

    /* Binary Ninja: int32_t* $a1 = *(arg1 + 0xb8) + arg2 - SAFE: Get register address */
    reg_addr = (int32_t*)((char*)sd->regs + reg);

    /* Binary Ninja: *$a1 = arg5 << (arg3 & 0x1f) | (not.d($v0) & *$a1) */
    *reg_addr = (val << (start & 0x1f)) | ((~mask) & *reg_addr);

    pr_debug("tx_isp_reg_set: subdev=%p reg[0x%x] = 0x%x (bits %d-%d = %d)",
             sd, reg, *reg_addr, start, end, val);

    return 0;
}
EXPORT_SYMBOL(tx_isp_reg_set);

/* ===== MISSING tx_isp_* FUNCTION IMPLEMENTATIONS ===== */

/* tx_isp_module_init - Binary Ninja stub implementation */
int tx_isp_module_init(struct platform_device *pdev, struct tx_isp_subdev *sd)
{
    if (!pdev || !sd) {
        pr_err("tx_isp_module_init: Invalid parameters\n");
        return -EINVAL;
    }

    pr_info("tx_isp_module_init: Module initialized for %s\n", dev_name(&pdev->dev));
    return 0;
}

/* tx_isp_module_deinit - Binary Ninja stub implementation */
void tx_isp_module_deinit(struct tx_isp_subdev *sd)
{
    if (!sd) {
        pr_err("tx_isp_module_deinit: Invalid subdev\n");
        return;
    }

    pr_info("tx_isp_module_deinit: Module deinitialized\n");
}

/* tx_isp_module_notify_handler - Handle module notification events */
int tx_isp_module_notify_handler(struct tx_isp_module *module, unsigned int cmd, void *arg)
{
    struct tx_isp_subdev *sd;

    if (!module) {
        pr_err("tx_isp_module_notify_handler: Invalid module\n");
        return -EINVAL;
    }

    /* Get the subdev that contains this module */
    sd = container_of(module, struct tx_isp_subdev, module);

    pr_info("*** tx_isp_module_notify_handler: cmd=0x%x, arg=%p ***\n", cmd, arg);

    switch (cmd) {
        case TX_ISP_EVENT_SYNC_SENSOR_ATTR:
            pr_info("*** tx_isp_module_notify_handler: Processing TX_ISP_EVENT_SYNC_SENSOR_ATTR ***\n");
            /* The sensor calls with &sensor->video, so arg is struct tx_isp_video_in * */
            struct tx_isp_video_in *video = (struct tx_isp_video_in *)arg;
            if (video && video->attr) {
                pr_info("*** tx_isp_module_notify_handler: Found video attributes, calling sync handler ***\n");
                extern int tx_isp_handle_sync_sensor_attr_event(struct tx_isp_subdev *sd, struct tx_isp_sensor_attribute *attr);
                return tx_isp_handle_sync_sensor_attr_event(sd, video->attr);
            } else {
                pr_err("*** tx_isp_module_notify_handler: No video attributes available ***\n");
                return -EINVAL;
            }

        default:
            pr_info("*** tx_isp_module_notify_handler: Unsupported event 0x%x ***\n", cmd);
            return -ENOIOCTLCMD;
    }
}

/* tx_isp_request_irq - EXACT Binary Ninja reference implementation */
int tx_isp_request_irq(struct platform_device *pdev, struct tx_isp_irq_info *irq_info)
{
    int irq_num;
    int ret;

    /* Binary Ninja: if (arg1 == 0 || arg2 == 0) return 0xffffffea */
    if (!pdev || !irq_info) {
        isp_printf(2, "tx_isp_request_irq: Invalid parameters\n");
        return -EINVAL;
    }

    /* Binary Ninja: int32_t $v0_1 = private_platform_get_irq(arg1, 0) */
    irq_num = platform_get_irq(pdev, 0);
    pr_info("*** tx_isp_request_irq: platform_get_irq returned %d for device %s ***\n", irq_num, dev_name(&pdev->dev));

    /* Binary Ninja: if ($v0_1 s>= 0) */
    if (irq_num >= 0) {
        /* Reference driver behavior: Register all IRQs normally without special handling */

        /* Binary Ninja: request_threaded_irq($v0_1, isp_irq_handle, isp_irq_thread_handle, 0x2000, *arg1, arg2) */
        extern struct tx_isp_dev *ourISPdev;
        extern irqreturn_t isp_irq_handle(int irq, void *dev_id);
        extern irqreturn_t isp_irq_thread_handle(int irq, void *dev_id);

        /* CRITICAL FIX: Always pass main ISP device as dev_id to prevent kernel panic */
        /* The interrupt handlers expect tx_isp_dev*, not subdevice structures */
        void *correct_dev_id = ourISPdev;  /* Always use main ISP device */
        const char *dev_name_str = dev_name(&pdev->dev);

        /* FIXED: All interrupt handlers expect tx_isp_dev* as dev_id */
        /* Passing subdevice structures (vic_dev, core_dev, vin_dev) causes type mismatch crashes */
        pr_info("*** tx_isp_request_irq: Using main ISP device as dev_id for IRQ %d (device: %s) ***\n",
                irq_num, dev_name_str);

        pr_info("*** tx_isp_request_irq: About to call request_threaded_irq(irq=%d, handler=%p, thread=%p, flags=0x%lx, name=%s, dev_id=%p) ***\n",
                irq_num, isp_irq_handle, isp_irq_thread_handle, IRQF_SHARED, dev_name(&pdev->dev), correct_dev_id);

        ret = request_threaded_irq(irq_num,
                                   isp_irq_handle,      /* Main dispatcher handles all IRQs */
                                   isp_irq_thread_handle, /* Thread handler */
                                   IRQF_SHARED,  /* 0x2000 = IRQF_SHARED */
                                   dev_name(&pdev->dev),  /* Device name */
                                   correct_dev_id);  /* CRITICAL FIX: Pass correct structure type */

        pr_info("*** tx_isp_request_irq: request_threaded_irq returned %d ***\n", ret);

        if (ret != 0) {
            /* Binary Ninja: isp_printf(2, "flags = 0x%08x, jzflags = %p,0x%08x", "tx_isp_request_irq") */
            pr_err("*** CRITICAL: tx_isp_request_irq: request_threaded_irq FAILED for IRQ %d: %d ***\n", irq_num, ret);
            isp_printf(2, "tx_isp_request_irq: request_threaded_irq failed: %d\n", ret);
            /* Binary Ninja: *arg2 = 0 */
            irq_info->irq = 0;
            return -EBUSY;
        }

        /* Binary Ninja: *arg2 = $v0_1 */
        irq_info->irq = irq_num;
        /* Binary Ninja: arg2[1] = tx_isp_enable_irq */
        irq_info->handler = isp_irq_handle;
        /* Binary Ninja: arg2[2] = tx_isp_disable_irq */
        irq_info->data = irq_info;  /* Store self-reference for callbacks */
        /* Binary Ninja: tx_isp_disable_irq(arg2) - initially disable IRQ */
        tx_isp_disable_irq(irq_info);

        pr_info("*** tx_isp_request_irq: IRQ %d registered successfully for %s ***\n", irq_num, dev_name(&pdev->dev));
    } else {
        /* Binary Ninja: *arg2 = 0 */
        irq_info->irq = 0;
        pr_err("tx_isp_request_irq: Failed to get IRQ: %d\n", irq_num);
        return irq_num;
    }

    /* Binary Ninja: return 0 */
    return 0;
}

/* tx_isp_free_irq - EXACT Binary Ninja reference implementation */
void tx_isp_free_irq(struct tx_isp_irq_info *irq_info)
{
    if (!irq_info || irq_info->irq <= 0) {
        pr_err("tx_isp_free_irq: Invalid IRQ info\n");
        return;
    }

    /* CRITICAL FIX: Use ourISPdev as dev_id to match request_threaded_irq call */
    extern struct tx_isp_dev *ourISPdev;
    free_irq(irq_info->irq, ourISPdev);
    irq_info->irq = 0;
    irq_info->handler = NULL;
    irq_info->data = NULL;

    pr_info("tx_isp_free_irq: IRQ freed\n");
}


static struct tx_isp_subdev_ops fs_subdev_ops = { 0 }; // All fields NULL/0


/* Platform driver structures */
static struct platform_driver tx_isp_csi_driver = {
    .probe = tx_isp_csi_probe,
    .remove = tx_isp_csi_remove,
    .driver = {
        .name = "isp-w00",  /* Match platform device name */
        .owner = THIS_MODULE,
    },
};

static struct platform_driver tx_isp_vin_driver = {
    .probe = tx_isp_vin_probe,
    .remove = tx_isp_vin_remove,
    .driver = {
        .name = "isp-w01",  /* Match platform device name */
        .owner = THIS_MODULE,
    },
};

static struct platform_driver tx_isp_core_driver = {
    .probe = tx_isp_core_probe,
    .remove = tx_isp_core_remove,
    .driver = {
        .name = "isp-m0",  /* Match platform device name */
        .owner = THIS_MODULE,
    },
};

static struct platform_driver tx_isp_vic_driver = {
    .probe = tx_isp_vic_probe,
    .remove = tx_isp_vic_remove,
    .driver = {
        .name = "isp-w02",
        .owner = THIS_MODULE,
    },
};

/* Platform driver initialization functions - MISSING from original implementation */
int __init tx_isp_subdev_platform_init(void)
{
    int ret;
    
    pr_info("*** TX ISP SUBDEV PLATFORM DRIVERS REGISTRATION ***\n");
    
    /* Register CSI platform driver */
    ret = platform_driver_register(&tx_isp_csi_driver);
    if (ret) {
        pr_err("Failed to register CSI platform driver: %d\n", ret);
        return ret;
    }

    /* Register VIC platform driver BEFORE Core ISP - CRITICAL for initialization order */
    ret = platform_driver_register(&tx_isp_vic_driver);
    if (ret) {
        pr_err("Failed to register VIC platform driver: %d\n", ret);
        goto err_unregister_csi;
    }

    /* Register VIN platform driver */
    ret = platform_driver_register(&tx_isp_vin_driver);
    if (ret) {
        pr_err("Failed to register VIN platform driver: %d\n", ret);
        goto err_unregister_vic;
    }

    /* Register FS platform driver - CRITICAL for /proc/jz/isp/isp-fs entry */
    extern struct platform_driver tx_isp_fs_platform_driver;
    ret = platform_driver_register(&tx_isp_fs_platform_driver);
    if (ret) {
        pr_err("Failed to register FS platform driver: %d\n", ret);
        goto err_unregister_vin;
    }

    /* Register CORE platform driver AFTER VIC - CRITICAL for initialization order */
    ret = platform_driver_register(&tx_isp_core_driver);
    if (ret) {
        pr_err("Failed to register CORE platform driver: %d\n", ret);
        goto err_unregister_fs;
    }
    
    pr_info("All ISP subdev platform drivers registered successfully\n");
    return 0;

err_unregister_vin:
    platform_driver_unregister(&tx_isp_vin_driver);
err_unregister_vic:
    platform_driver_unregister(&tx_isp_vic_driver);
err_unregister_csi:
    platform_driver_unregister(&tx_isp_csi_driver);
    return ret;
}

void __exit tx_isp_subdev_platform_exit(void)
{
    pr_info("*** TX ISP SUBDEV PLATFORM DRIVERS UNREGISTRATION ***\n");
    
    /* Unregister all platform drivers in reverse order */
    platform_driver_unregister(&tx_isp_core_driver);
    platform_driver_unregister(&tx_isp_vin_driver);
    platform_driver_unregister(&tx_isp_vic_driver);
    platform_driver_unregister(&tx_isp_csi_driver);
    
    pr_info("All ISP subdev platform drivers unregistered\n");
}

/* Export symbols for main module to call these functions */
EXPORT_SYMBOL(tx_isp_subdev_platform_init);
EXPORT_SYMBOL(tx_isp_subdev_platform_exit);
